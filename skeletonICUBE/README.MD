# skeletonICUBE
โครงการ Keil uVision ที่ปรับแต่งจากตัวอย่าง End node ของ I-CUBE-LRWAN สำหรับบอร์ด B-L072Z-LRWAN1

## ข้อมูลเกี่ยวกับโค้ด

### ขั้นตอนการเตรียมโครงการ
1. สร้างโครงการ Keil uVision ใหม่ โดยเลือก MCU เป็น STM32L072CZYx
2. ดาวน์โหลดและขยายไฟล์ I-CUBE-LRWAN รุ่น 1.1.5
3. สำเนาไฟล์โค้ดจาก End node และไดรเวอร์ต่างๆ
4. เพิ่มไฟล์ที่จำเป็นเข้าสู่โครงการ
5. เพิ่มส่วน Processor Symbols > Defines เป็น STM32L072xx,USE_B_L072Z_LRWAN1,USE_HAL_DRIVER, REGION_AS923, DEBUG, LOW_POWER_DISABLE
6. เพิ่มรายการไดเร็กทอรีของไฟล์ .h ต่างๆ
7. เลือก ST-Link Debugger
8. ลบส่วนโค้ดที่เกี่ยวข้องกับเซ็นเซอร์ต่างๆ

### ฮาร์ดแวร์สำเร็จรูปบนบอร์ด
#### ไฟล์ Drivers\B-L072Z-LRWAN1\**b-l072z-lrwan1.c**
1. LED ควบคุมด้วยฟังก์ชัน `BSP_LED_On()` `BSP_LED_Off()` และ `BSP_LED_Toggle()` โดยใช้ตัวเลือก
   - `LED1` หรือ `LED_GREEN`
   - `LED2` หรือ `LED_RED1`
   - `LED3` หรือ `LED_BLUE`
   - `LED4` หรือ `LED_RED2`
2. ปุ่มกด อ่านค่าด้วยฟังก์ชัน `BSP_PB_GetState()` โดยใช้ตัวเลือก `BUTTON_KEY` หรือ `BUTTON_USER`

#### ไฟล์ Drivers\MLM32L07X01\mlm32l0xx_hw.c
1. สถานะแบตเตอรี อ่านค่าด้วยฟังก์ชัน `HW_GetBatteryLevel()` โดยคืนค่าระหว่าง 1 (ต่ำมาก) หรือ 254 (ชาร์จเต็ม)
2. เซ็นเซอร์อุณหภูมิในตัวชิพ อ่านค่าด้วยฟังก์ชัน `HW_GetTemperatureLevel()` โดยคืนค่าเป็นอุณหภูมิเซลเซียสแบบจำนวนเต็ม 

### การปรับแก้โค้ด
#### การทดสอบกับเครือข่าย CAT Telecom ที่ ICIC ตรงอาคาร NIA (923MHz, ABP)
1. ตั้ง processor symbols เป็น REGION_AS923 
2. ขอ account จากทีมงาน CAT Telecom แล้วลงทะเบียน device กับ [network server ทดสอบ](https://loraiot.cattelecom.com/portal/login)
3. ในกรณี ABP แก้ไขค่าตั้งของเครือข่ายในไฟล์ inc/**commissioning.h** โดยค่าของ DevAddr, NwkSKey, AppSKey ให้ตั้งค่าจาก web UI ของ CAT Telecom
```
#define OVER_THE_AIR_ACTIVATION                     0
#define STATIC_DEVICE_ADDRESS                       1
#define LORAWAN_DEVICE_ADDRESS                      ( uint32_t )0x89EF1155
#define LORAWAN_NWKSKEY                             { 0x28, 0xAE, 0xD2, 0x2B, 0x7E, 0x15, 0x16, 0xA6, 0x09, 0xCF, 0xAB, 0xF7, 0x15, 0x88, 0x4F, 0x3C }
#define LORAWAN_APPSKEY                             { 0x17, 0x28, 0xAE, 0x2B, 0x7E, 0x15, 0xD2, 0xA6, 0xAB, 0xF7, 0xCF, 0x4F, 0x3C, 0x15, 0x88, 0x09 }
```
4. ในกรณี OTAA แก้ไขค่าตั้งของเครือข่ายในไฟล์ inc/**commissioning.h** โดยค่าของ DevEUI, AppEUI ให้ตั้งค่าจาก web UI ของ CAT Telecom
```
#define OVER_THE_AIR_ACTIVATION                     1
#define LORAWAN_PUBLIC_NETWORK                      true
#define STATIC_DEVICE_EUI                           1
#define LORAWAN_DEVICE_EUI                          { 0xAA, 0x00, 0xDB, 0xCA, 0x89, 0xEF, 0x11, 0x55 }
#define LORAWAN_APPLICATION_EUI                     { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 }
#define LORAWAN_APPLICATION_KEY                     { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }
```
5. ข้อมูลจาก network server จะส่งต่อไปที่ app server ที่กำหนดใน routing profile ซึ่งในกรณี REST จะเป็นการ POST ด้วย JSON ตามตัวอย่างที่ได้จาก log นี้
```
2018-05-25T04:56:40.320883+00:00 app[web.1]: {'DevEUI_uplink': {'Time': '2018-05-25T11:56:39.081+07:00', 'DevEUI': 'AA00DBCA89EF1155', 'DevAddr': '89EF1155', 'FPort': '2', 'FCntUp': '4', 'ADRbit': '1', 'MType': '2', 'FCntDn': '4', 'payload_hex': 'fe000020ff', 'mic_hex': '2153b4cf', 'Lrcid': '00000231', 'LrrRSSI': '-102.000000', 'LrrSNR': '-3.000000', 'SpFact': '9', 'SubBand': 'G1', 'Channel': 'LC4', 'DevLrrCnt': '1', 'Lrrid': '00002E82', 'Late': '0', 'LrrLAT': '13.765070', 'LrrLON': '100.528679', 'Lrrs': {'Lrr': [{'Lrrid': '00002E82', 'Chain': '0', 'LrrRSSI': '-102.000000', 'LrrSNR': '-3.000000', 'LrrESP': '-106.764351'}]}, 'CustomerID': '1100000195', 'CustomerData': {'alr': {'pro': 'LORA/Generic', 'ver': '1'}}, 'ModelCfg': '0', 'InstantPER': '0.000000', 'MeanPER': '0.000000'}}
```

#### การปรับแก้อัตราในการส่งข้อมูล (class A)
1. แก้ไขตัวเลขในไฟล์ src/**main.c** โดยตัวเลขจะอยู่ในหน่วยมิลลิวินาที
```
#define APP_TX_DUTYCYCLE                            10000
```

#### การเขียนโค้ดสำหรับเซ็นเซอร์เฉพาะทาง
1. แก้ไขนิยามของข้อมูลในไฟล์ inc/**bsp.h**
```
typedef struct{
  uint32_t dummy;
  /**more may be added*/
} sensor_t;
```
2. เพิ่มส่วนโค้ดสำหรับตั้งค่าฮาร์ดแวร์ของเซ็นเซอร์ในไฟล์ inc/**bsp.c**
```
void  BSP_sensor_Init( void  )
{
  /*init code here*/
}
```
3. เพิ่มส่วนโค้ดสำหรับอ่านค่าจากเซ็นเซอร์ในไฟล์ src/**bsp.c**
```
void BSP_sensor_Read( sensor_t *sensor_data)
{
  /*read code here*/
}
```
4. แก้ไขการเพิ่มข้อมูลลงในบัฟเฟอร์ของฟังก์ชัน `Send()` ในไฟล์ src/**main.c**
```
  AppData.Buff[i++] = /*data here*/;
```

#### การเขียนโค้ดสำหรับรับข้อมูล
1. เพิ่ม case สำหรับ app port ในฟังก์ชัน `LORA_RxData()` ในไฟล์ src/**main.c**
```
static void LORA_RxData( lora_AppData_t *AppData )
{
  switch (AppData->Port)
  {
    case NEW_PORT_HERE:
    /*get data*/
    break;
  }
}
```
2. ตรวจสอบขนาดข้อมูลจาก `AppData->BuffSize`
3. ดึงข้อมูลจาก `AppData->Buff[/*index here*/]` 

## อ้างอิง
- [LoRaWAN software expansion for STM32Cube (UM2073)](http://www.st.com/en/embedded-software/i-cube-lrwan.html)
- [เอกสารเกี่ยวกับซอฟต์แวร์สำหรับ STM32L0](http://www.st.com/en/embedded-software/stm32cubel0.html)