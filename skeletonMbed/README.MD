# skeletonMbed
โครงการ Keil uVision ที่ export ออกจาก online compiler ของ mbed.org สำหรับบอร์ด B-L072Z-LRWAN1

## ข้อมูลเกี่ยวกับโค้ด
### ขั้นตอนการเตรียมโครงการ
1. ลงทะเบียนกับเว็บไซต์ https://os.mbed.com/
2. ใช้งาน mbed online compiler
3. คลิกเลือกติดตั้งบอร์ด DISCO-L072Z-LRWAN1
4. สร้าง Program ใหม่ โดยเลือกจาก template คือ Empty Program
5. เลือก Import Library จาก URL สำหรับ [mbed-os](https://github.com/ARMmbed/mbed-os) และ [mbed Semtech driver](https://github.com/ARMmbed/mbed-semtech-lora-rf-drivers)
6. เพิ่มไฟล์ main.cpp mbed_app.json และ mbedtls_lora_config.h
7. เลือก Export เป็น uVision

### การปรับแก้โค้ด
#### การทดสอบกับเครือข่าย CAT Telecom ที่ ICIC ตรงอาคาร NIA (923MHz, ABP)
1. แก้ไขแบนด์ความถี่เป็น 923MHz ในไฟล์ **mbed_config.h**
```
#define MBED_CONF_LORA_PHY                                1 
```
2. แก้ไขค่าตั้งของเครือข่ายในไฟล์ **mbed_config.h**
```
#define MBED_CONF_LORA_OVER_THE_AIR_ACTIVATION            0 
#define MBED_CONF_LORA_DEVICE_ADDRESS                     0x00000000
#define MBED_CONF_LORA_NWKSKEY                            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MBED_CONF_LORA_APPSKEY                            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } 
```

## อ้างอิง
- [Building your own private LoRa network](https://os.mbed.com/docs/v5.8/reference/building-your-own-private-lora-network.html)
- [Simple LoRaWAN example application for mbed OS](https://github.com/ARMmbed/mbed-os-example-lorawan)