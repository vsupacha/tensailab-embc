# Rapid IoT prototyping with mbed and LoRaWAN

โครงการนี้จะสาธิตขั้นตอนในการพัฒนาต้นแบบอย่างเร็วโดยใช้แพลตฟอร์ม mbed ร่วมกับ CAT LoRaWAN และ Google Firebase   ฮาร์ดแวร์ที่ใช้คือ บอร์ด B-L072Z-LRWAN1 ที่มีโมดูล CMWX1ZZABZ-091 (ไมโครคอนโทรลเลอร์ STM32L073 + Semtech SX1276) ของ Murata เพื่อเชื่อมต่อกับโครงข่าย CAT LoRaWAN ที่เป็น low-power wide area network (LPWAN)   ส่วน Google Firebase จะใช้บริการฐานข้อมูล Realtime Database เนื่องจากโมเดลการคิดค่าบริการจาก bandwidth และ storage จะถูกกว่าในกรณี IoT ที่รับส่งข้อมูลขนาดเล็ก

## Step 1: configure database/web services on Google Firebase
1. ลงทะเบียนกับเว็บไซต์ https://firebase.google.com/
2. สร้าง project ใหม่โดยเลือกใช้ฐานข้อมูลแบบ Realtime Database
   - เช็ค Project ID และ Web API Key จาก Project Settings
   - กำหนด Rules ให้รองรับการสืบค้นข้อมูล
   ```
   {
       "rules": {
           ".read": true,
           ".write": true,
           "test": {
               "v01": {
                   ".indexOn":"DevEUI_uplink/DevEUI"  
               } 
           }    
       }
   }
   ```

## Step 2: configure LoRaWAN network on CAT LoRaWAN portal
1. ล็อกอิน
2. ลงทะเบียน routing profile ให้ชี้ไปที่ Google Firebase
   
## Step 3: Make Things with mbed 
### Use mbed online compiler
1. ลงทะเบียนกับเว็บไซต์ https://os.mbed.com
2. เพิ่มฮาร์ดแวร์เข้า mbed IDE
   - เลือก Add Board แล้วเลือก DISCO-L072CZ-LRWAN1 จากปุ่มที่มุมขวาของหน้าต่าง IDE
   - หรือเลือกเพิ่มเข้า IDE จากเว็บไซต์ https://os.mbed.com/platforms/ST-Discovery-LRWAN1/
3. สร้าง Program ใหม่สำหรับบอร์ด DISCO-L072CZ-LRWAN1 โดยเลือก template "Blink LED example"
   - ลบไลบรารี mbed
   - เพื่มไลบรารี mbed-os โดยคลิก Import > Libraries จาก https://github.com/ARMmbed/mbed-os

### Use mbed CLI
1. ดาวน์โหลดและติดตั้ง Python จาก https://www.python.org/
   - คลิกให้ ADD PATH ในช่วงการติดตั้ง
2. ดาวน์โหลดและติดตั้ง Git for Windows จาก https://git-scm.com/download/win
3. ดาวน์โหลดและติดตั้ง GCC ARM 6 จาก https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
4. สร้างโฟลเดอร์สำหรับทำงาน เช่น mbed_test
   - ในหน้าต่าง Explorer ให้คลิกขวาเลือก Git Bash Here
5. พิมพ์คำสั่งเพื่อติดตั้ง mbed CLI และตั้งค่าชุดคอมไพเลอร์ GCC ARM
   ```
   $ python -m pip install --upgrade pip
   $ pip install mbed-cli
   $ mbed config -G GCC_ARM_PATH "C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q2-update\bin"
   ```
6. ทดสอบด้วยการ clone และสร้างโค้ดตัวอย่างจากอินเตอร์เน็ตโดยพิมพ์คำสั่ง
   ```
   $ mbed import https://github.com/ARMmbed/mbed-os-example-lorawan
   $ cd mbed-os-example-lorawan/
   $ mbed compile -m disco_l072cz_lrwan1 -t GCC_ARM
   ```
7. แก้ไขไฟล์ mbed_app.json เพื่อปรับแต่งโค้ดให้ทำงานกับเซิร์ฟเวอร์ CAT LoRaWAN (ย่านความถี่ 923 MHz และใช้ APP EUI + APP KEY)
   ```
   "lora.over-the-air-activation": true,
   "lora.duty-cycle-on": true,
   "lora.phy": "AS923",
   "lora.device-eui": "{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }",
   "lora.application-eui": "{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }",
   "lora.application-key": "{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }"
   ```
8. สร้างโค้ดตัวอย่างใหม่และติดตั้งลงบอร์ด 

## Reference
1. [DISCO-L072CZ-LRWAN1](https://os.mbed.com/platforms/ST-Discovery-LRWAN1/)
2. [Firebase REST API](https://firebase.google.com/docs/database/rest/start)
