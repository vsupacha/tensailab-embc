# Rapid IoT prototyping with mbed and Firebase

โครงการนี้จะสาธิตขั้นตอนในการพัฒนาต้นแบบอย่างเร็วโดยใช้แพลตฟอร์ม mbed ร่วมกับ Google Firebase  ฮาร์ดแวร์ที่ใช้คือ บอร์ด DISCO-L475VG-IOT01A ที่มีไมโครคอนโทรลเลอร์ STM32L475 และโมดูลสื่อสารไร้สาย WiFi รุ่น Inventek ISM43362   ส่วน Google Firebase จะใช้บริการฐานข้อมูล Realtime Database เนื่องจากโมเดลการคิดค่าบริการจาก bandwidth และ storage จะถูกกว่าในกรณี IoT ที่รับส่งข้อมูลขนาดเล็ก

## Required software tools
1. [ไดรเวอร์ USB CDC](https://www.st.com/en/development-tools/stsw-stm32102.html) สำหรับเชื่อมต่อแบบ serial port กับบอร์ด STM32 (ไม่จำเป็นสำหรับ Windows 10)
2. โปรแกรม serial terminal เช่น [YAT](https://sourceforge.net/projects/y-a-terminal/)
3. โปรแกรม [git](https://gitforwindows.org/)
4. โปรแกรม [node.js](https://nodejs.org/dist/latest-v8.x/)

## Making IoT services on Google Firebase
1. ลงทะเบียนกับเว็บไซต์ https://firebase.google.com/
2. สร้าง project ใหม่โดยเลือกใช้ฐานข้อมูลแบบ Realtime Database
   - เช็ค Project ID และ Web API Key จาก Project Settings
   - กำหนด Rules ให้รองรับการสืบค้นข้อมูลแบบ public
   ```
   {
       "rules": {
           ".read": true,
           ".write": true,
           "test": {
               "v01": {
                   ".indexOn":"ID"  
               } 
           }    
       }
   }
   ```

## Making Things with mbed 
1. ลงทะเบียนกับเว็บไซต์ https://os.mbed.com
2. เพิ่มฮาร์ดแวร์เข้า mbed IDE
   - เลือก Add Board จากปุ่มที่มุมขวาของหน้าต่าง IDE
   - หรือเลือกเพิ่มเข้า IDE จากเว็บไซต์ https://os.mbed.com/platforms/ST-Discovery-L475E-IOT01A/ 
3. สร้าง Program ใหม่สำหรับบอร์ด DISCO-L475VG-IOT01A โดยเลือก template "Blink LED example"
   - ลบไลบรารี mbed
   - เพิ่มไลบรารี mbed-os โดยคลิก Import > Libraries จาก https://github.com/ARMmbed/mbed-os
   - เพิ่มไดรเวอร์สำหรับโมดูล ISM43362 โดย Import จาก https://github.com/ARMmbed/wifi-ism43362
   - เพิ่มไลบรารี [mbed-http](https://os.mbed.com/teams/sandbox/code/http-example/) โดย Import จาก https://os.mbed.com/teams/sandbox/code/mbed-http/
   - เพิ่มไลบรารี [MbedJSONValue](https://os.mbed.com/users/samux/code/MbedJSONValue/docs/tip/classMbedJSONValue.html) โดย Import จาก https://os.mbed.com/users/samux/code/MbedJSONValue/   
4. เพิ่มไฟล์ mbed_app.json ที่เก็บค่า setting ต่างๆ
```
{
    "macros": [
        "MBEDTLS_MPI_MAX_SIZE=1024",
        "MBEDTLS_MPI_WINDOW_SIZE=1",
        "MBED_HEAP_STATS_ENABLED=1",
        "MBED_STACK_STATS_ENABLED=1",
        "MBED_MEM_TRACING_ENABLED=1"
    ],       
    "target_overrides": {
        "*": {
            "platform.stdio-baud-rate"          : 115200,
            "platform.stdio-convert-newlines"   : true,
            "mbed-trace.enable"                 : 1,
            "mbed-http.http-buffer-size"        : 2048,
            "nsapi.default-wifi-security"       : "WPA_WPA2",
            "nsapi.default-wifi-ssid"           : "\"SSID name\"",
            "nsapi.default-wifi-password"       : "\"password\""
        },
        "DISCO_L475VG_IOT01A": {
            "target.extra_labels_add"           : ["WIFI_ISM43362"],
            "target.network-default-interface-type" : "WIFI"
        }        
    }
}
```
5. เขียนโค้ดเพื่อใช้งาน WiFi เชื่อมต่อ Google Firebase
   - ประกาศ root CA certificate โดย export จาก certificate ของเว็บ Google Firebase
   ```
   const char SSL_CA_PEM[] = "-----BEGIN CERTIFICATE-----\n"
   "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n"
   "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n"
   "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n"
   "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n"
   "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n"
   "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n"
   "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n"
   "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n"
   "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n"
   "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n"
   "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n"
   "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n"
   "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n"
   "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n"
   "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n"
   "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n"
   "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n"
   "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n"
   "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n"
   "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n"
   "-----END CERTIFICATE-----\n";
   ```
   - เชื่อมต่อโครงข่าย WiFi
   ```
   NetworkInterface *net;
   
   net = NetworkInterface::get_default_instance();    
   nsapi_error_t net_status = -1;
   for (int tries = 0; tries < 3; tries++) {
       net_status = net->connect();
       if (net_status == NSAPI_ERROR_OK) {
           break;
       } else {
           printf("Unable to connect to network. Retrying...\n");
       }
   }
   if (net_status != NSAPI_ERROR_OK) {
       printf("ERROR: Connecting to the network failed (%d)!\n", net_status);
       return -1;
   }    
   printf("Connected to the network successfully. IP address: %s\n", net->get_ip_address());
   ```
   - เชื่อมต่อ Google Firebase ด้วย REST API แบบ POST เพื่อเพิ่มข้อมูลเข้าฐานข้อมูล
   ```
   MbedJSONValue body;
   std::string s;
   
   const char DBASE_URL[] = "https://{PROJECT-ID}.firebaseio.com/test/v1.json"; 
   HttpsRequest* post_req = new HttpsRequest(net, SSL_CA_PEM, HTTP_POST, DBASE_URL);
   post_req->set_header("Content-Type", "application/json");
   body["name"] = "Supachai";
   body["value"] = 10;
   s = body.serialize();
   HttpResponse* post_resp = post_req->send(s.c_str(), s.length());
   ```
   - ถอดข้อมูลออกจาก JSON ที่ตอบกลับจาก Firebase
   ```
   parse(body, post_req->get_body_as_string().c_str());
   std::string pushId = body["name"].get<std::string>();
   delete post_resp;
   ```
   - ล้างตัวแปรจากหน่วยความจำ   
   
6. สืบค้นข้อมูลโดยอาศัย [REST query API](https://firebase.google.com/docs/reference/rest/database/) เพื่อสืบค้นข้อมูลโดย **orderBy=***"index ที่กำหนดใน rules"*&**equalTo=***"เงื่อนไขสืบค้น"*   ข้อมูลจะตอบกลับในรูปแบบ JSON โดยสามารถเพิ่มเงื่อนไขการสืบค้น **limitToFirst=**, **limitToLast=** เพื่อกำหนดจำนวนข้อมูล หรือใช้เงื่อนไข **startAt=** และ/หรือ **endAt=** เพื่อสืบค้นแบบย่าน
   - เชื่อมต่อ Google Firebase ด้วย REST API แบบ GET เพื่อสืบค้นฐานข้อมูล
   ```
   char QUERY_URL[] = "https://{PROJECT-ID}.firebaseio.com/test/v1.json?orderBy=\"%s\"&equalTo=\"%s\"";
   char query_url[100];
   sprintf(query_url, QUERY_URL, "name", "Supachai");
   HttpsRequest* get_req = new HttpsRequest(net, SSL_CA_PEM, HTTP_GET, query_url);
   get_req->set_header("Content-Type", "application/json");
   HttpResponse* get_resp = get_req->send();   
   ```
7. การเชื่อมต่อแบบ socket reuse จะลดปัญหาเนื่องจากการจองหน่วยความจำได้
   ```
   TLSSocket* socket = new TLSSocket();
   if ((net_status = socket->open(net)) != NSAPI_ERROR_OK) {
      printf("TLS socket open failed (%d)\n", net_status);
      return 1;
   }
   if ((net_status = socket->set_root_ca_cert(SSL_CA_PEM)) != NSAPI_ERROR_OK) {
      printf("TLS socket set_root_ca_cert failed (%d)\n", net_status);
      return 1;
   }
   const char SERVER_URL[] = "mbed-test-c7e17.firebaseio.com"; 
   if ((net_status = socket->connect(SERVER_URL, 443)) != NSAPI_ERROR_OK) {
      printf("TLS socket connect failed (%d)\n", net_status);
      return 1;
   }
   while(1) {
      const char DBASE_URL[] = "https://mbed-test-c7e17.firebaseio.com/test/v2.json"; 
      post_req = new HttpsRequest(socket, HTTP_POST, DBASE_URL);
      post_req->set_header("Content-Type", "application/json");
      body["name"] = "Supachai";
      body["value"] = 20;
      std::string s = body.serialize();
      post_resp = post_req->send(s.c_str(), s.length());
      if (post_resp) {
         parse(body, post_resp->get_body_as_string().c_str());
         std::string pushId = body["name"].get<std::string>();
         printf("Push ID: %s\n", pushId.c_str());
      } else {
         printf("No response from Firebase\n");
         print_memory_info();
      }
      delete post_req;
      wait(10);
   }
   ```   
   
## Cloud functions
1. ติดตั้งซอฟต์แวร์สำหรับ cloud functions
   - ซอฟต์แวร์ [node.js รุ่น 8](https://nodejs.org/dist/latest-v8.x/) 
   - ใช้ npm เพื่อติดตั้ง firebase-tools โดยพิมพ์คำสั่ง ```npm install -g firebase-tools```
2. เริ่มใช้งาน cloud functions สำหรับ Firebase
   - ล็อกอินเข้า Firebase โดยพิมพ์คำสั่ง ```firebase login```
   - สร้างโฟลเดอร์สำหรับซอร์สโค้ด
   - สร้าง project และตั้งค่าต่างๆ โดยพิมพ์คำสั่ง ```firebase init functions```
   - ไฟล์ซอร์สโค้ด JavaScript คือ /functions/index.js   
3. เปลี่ยนไปใช้ node 8 โดยเพิ่มรายการ ```"engines": { "node": "8" }``` ลงในไฟล์ functions/package.json 
4. สร้างโค้ดสำหรับ Realtime Database Trigger  
   ```
   const functions = require('firebase-functions');    // import Cloud Functions for Firebase SDK 
   const admin = require('firebase-admin'); // import Firebase Admin SDK
   admin.initializeApp();
   exports.timeStamp = functions.database.ref('/test/v3/{pushId}/name').onCreate((snapshot, context) => {
      const name = snapshot.val();
      const key = snapshot.ref.parent.key;
      var now = new Date();
      console.log('Timestamp of ' + name + ':' + now.toLocaleDateString() + ', ' + now.toLocaleTimeString());
      admin.database().ref('/latest').set({name: name, key: key});
      return snapshot.ref.parent.child('timestamp').set(now.getTime());
   });
   ```
5. ติดตั้งโค้ดขึ้น Firebase โดยพิมพ์คำสั่ง ```firebase deploy --only functions```
6. สร้างโค้ดสำหรับ HTTP Trigger
   ```
   exports.queryBefore = functions.https.onRequest(async (req, res) => {
      const mins  = req.query.mins;
      console.log('Param: ' + mins);
      var tprev = new Date().getTime();
      tprev = tprev - (mins*60*1000);
      var listKeys = [];
      var query = admin.database().ref('/test/v3').orderByChild('timestamp').startAt(tprev);      
      await query.once('value').then((snapshots) => {
         snapshots.forEach((snapshot) => {
            listKeys.push(snapshot.key);
            console.log(snapshot.val());
         });
      });
      var json_resp = {keys: listKeys}
      res.send(json_resp);
   });
   ```
7. เรียกใช้บริการผ่านทาง HTTPS endpoint ที่ https://us-central1-{ชื่อ project}.cloudfunctions.net/queryBefore?mins={เวลาในหน่วยนาที}   

## Debugging
1. การเปิดใช้งาน trace เพื่อรายงานสถานะการทำงาน แบ่งเป็น 2 ส่วนคือ การตั้งค่าในไฟล์ mbed_app.json 
   ```
    "target_overrides": {
        "*": {
            "platform.stdio-baud-rate"          : 115200,
            "platform.stdio-convert-newlines"   : true,
            "mbed-trace.enable"                 : 1
        }
   ```
   และการเปิดใช้งานในส่วนโค้ด
   ```
   #include "mbed_trace.h"

    mbed_trace_init();   
   ```   
2. การรายงานสถานะหน่วยความจำ โดยเพิ่มไลบรารีจาก https://github.com/nuket/mbed-memory-status
   ```
   #include "mbed_memory_status.h"
   
   print_all_thread_info();
   print_heap_and_isr_stack_info();
   ```   
3. การทดสอบ Firebase ภายในเครื่อง  
   - ติดตั้ง emulator สำหรับ Realtime Database โดยพิมพ์คำสั่ง ```firebase setup:emulators:database```
   - เปิดบริการ local service ในเครื่อง โดยพิมพ์คำสั่ง ```firebase serve```

  
## Notes
1. ไลบรารี mbed-http ไม่รองรับ method PUT และ PATCH จึงไม่สามารถอัพเดทฐานข้อมูลแบบระบุ absolute path ได้
2. การเพิ่มข้อมูลลงใน collection โดยใช้การ push() จะมีการสร้าง unique ID ที่เรียงตามเวลาโดยอัตโนมัติ ซึ่ง ID จะมีส่วนของ random bit ทำให้ guess ไม่ได้
3. ฐานข้อมูล Cloud Firestore จะมีรูปแบบของ request ที่ซับซ้อนกว่า Realtime Database (จัดเก็บแบบ JSON tree) เนื่องจากจัดเก็บแบบ document ที่สามารถประกาศเป็นลำดับชั้น collection ภายใน document ได้   นอกจากนี้ การส่งข้อมูลจะต้องประกาศ type ของข้อมูลด้วย
   ```
   {
      "fields": {
         "firstName": {
            "stringValue": "Jason"
         },
         "lastName": {
            "stringValue": "Byrne"
         }, 
         "age": { 
            "integerValue": 36
         }
      }
   }
   ```


## Reference
1. [DISCO-L475VG-IOT01A](https://os.mbed.com/platforms/ST-Discovery-L475E-IOT01A/)
2. [Firebase REST API](https://firebase.google.com/docs/database/rest/start)
3. [Cloud Functions for Firebase](https://firebase.google.com/docs/functions/)