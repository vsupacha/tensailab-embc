# skeletonCubeMX
โครงการ Keil uVision ที่สร้างด้วย STM32CubeMX และโค้ดตัวอย่าง End node จาก I-CUBE-LRWAN สำหรับบอร์ด B-L072Z-LRWAN1

## กรอบการพัฒนา
### แนวทาง
1. ใช้ STM32CubeMX เพื่อตั้งค่าเพอริเฟอรัลบนชิพและสร้าง project เป็น Keil MDK-ARM V5 ทำให้สามารถเพิ่มโค้ดจัดการฮาร์ดแวร์ง่ายขึ้น

### ข้อมูลการเรียงขา
| pin | CMWX1ZZABZ-091 | STM32L072CZY6 |
|---|---|---|
| - | RADIO_NSS | PA15 |
| - | RADIO_SCK | PB3 |
| - | RADIO_RESET | PC0 |
| - | RADIO_MISO | PA6 |
| - | RADIO_MOSI | PA7 |
| 1 | RADIO_TCXO_VCC | PA12/USB_DP | 
| 2 | | PA11/USB_DM |
| 3 | GND | |
| 4 | VDD_USB| |
| 5 | VDD_MCU | |
| 6 | VDD_RF | |
| 7 | GND | |
| 8 | DBG_SX1276_DIO2 | PB0 |
| 9 | DBG_SX1276_DIO3 | PC13 |
| 10 | DBG_SX1276_DIO4 / LED2 | PA5 |
| 11 | DBG_SX1276_DIO5 | PA4 |
| 12 | DBG_SX1276_DIO1 | PB1 |
| 13 | DBG_SX1276_DIO0 | PB4 |
| 14 | | PB15/SPI2_MOSI |
| 15 | | PB14/SPI2_MISO |
| 16 | | PB13/SPI2_SCK |
| 17 | | PB12/SPI2_NSS |
| 18 | | PA10/USART1_RX |
| 19 | | PA9/USART1_TX |
| 20 | | PA8/MCO |
| 21 | | PA5/ADC5/DAC2 |
| 22 | | PA4/ADC4/DAC1 |
| 23 | UART_RX | PA3/ADC3/LPUART1_RX |
| 24 | UART_TX | PA2/ADC2/LPUART1_TX |
| 25 | GND | |
| 26 | ANT | |
| 27 | GND | |
| 28 | DBG_CRF1 (ANT_SWITCH_RX) | PA1 |
| 29 | DBG_CRF3 (ANT_SWITCH_TX_BOOST) | PC1 |
| 30 | DBG_CRF2 (ANT_SWITCH_TX_RFO) | PC2 |
| 31 | STSAFE_NRST | |
| 32 | VREF+ | |
| 33 | | PA0/WKUP1 |
| 34 | | MCU_NRST |
| 35 | | PB8/I2C1_SCL |
| 36 | | PB9/I2C1_SDA |
| 37 | USR_BUTTON | PB2/LPTIM1_OUT |
| 38 | LED4 | PB7/LPTIM1_IN2 |
| 39 | LED3 | PB6/LPTIM1_ETR |
| 40 | LED1 | PB5/LPTIM1_IN1 |
| 41 | | PA13/SWDIO |
| 42 | | PA14/SWCLK |
| 43 | | BOOT0 |
| 44 | GND | |
| 45 | | PH1/OSC_OUT |
| 46 | | PH0/OSC_IN |
| 47 | TCXO_OUT | |
| 48 | VDD_TCXO | |
| 49-57 | GND | |

## รุ่นการพัฒนา
### รุ่น 0.1
สร้าง CubeMX project เพื่อตั้งค่าฮาร์ดแวร์ตามการเชื่อมต่อของโมดูล Murata CMWX1ZZABZ-091 จากนั้นสร้างโค้ดของ Keil MDK-ARM V5
- RCC เปิดใช้งาน LSE clock source แล้วไปตั้งใน clock เป็น 32.768 kHz และเลือก HCLK จาก HSI PLL (x6 /3) เป็น 32 MHz 
- SYS เปิดใช้งาน Serial Wire
- SPI1 เปิดใช้งานด้วยขา PB3 (SCLK), PA6 (MISO), PA7 (MOSI) และ disable hardware NSS แล้วไปตั้งค่า prescaler /4, full-duplex master, low polarity, 1 edge
- RTC เปิดใช้งาน active clock source, alarm A แล้วไปตั้งค่า sync prediv 1023 และ async prediv 31 และเปิดใช้งาน RTC interrupt
- USART2 เปิดใช้งาน async mode ด้วยขา PA2 (Tx), PA3 (Rx) แล้วตั้งค่า 115200,8,n,1 และเปิดใช้งาน DMA1 channel 4 เพื่อ Tx, DMA Tx interrupt และ USART Rx interrupt
- GPIO output ตั้งค่าแบบ push-pull, high speed สำหรับขา  PA12(TCXO VCC), PA1 (antenna switch RX), PC1 (antenna switch TX-BOOST), PC2 (antenna switch TX), PC0 (SX1276 reset) และ PA15 (SX1276 NSS)
- GPIO input ตั้งการทำงานแบบ EXTI แบบ rising, no pull สำหรับขา PB4 (DIO0), PB1 (DIO1), PB0 (DIO2) และ PC13 (DIO3) แล้วไปเปิดใช้งาน EXTI interrupt กลุ่ม 0-1 และกลุ่ม 4-15
- ADC เปิดใช้งานเพื่อวัด temperature และ vrefint
- NVIC เปิดใช้งาน interrupt สำหรับ RTC, EXTI และ EXTI

### รุ่น 0.2
เพิ่มไฟล์จาก I-CUBE-LRWAN ที่จำเป็น โดยปรับแก้บางส่วนในกลุ่มไฟล์ hw ที่ abstract การทำงานของ STM32 เพื่อให้คอมไพล์ผ่าน
1. เพิ่มไฟล์เข้ากลุ่ม Middlewares/LoRaWAN โดยสำเนาจากโฟลเดอร์ Middlewares\Third_Party\LoRaWAN
2. เพิ่มไฟล์เข้ากลุ่ม Drivers/BSP โดยสำเนาจากโฟลเดอร์ B-L072Z-LRWAN1, CMWX1ZZABZ-0xx, sx1276 ที่อยู่ภายในโฟลเดอร์ Drivers/BSP
3. เพิ่มไฟล์เข้ากลุ่ม Applications/User โดยสำเนาจากโฟลเดอร์ Projects\B-L072Z-LRWAN1\Applications\LoRa\End_Node\LoRaWAN\App\src
4. เพิ่มรายการของโฟลเดอร์ที่มีไฟล์ header
5. ตั้งค่า Defines ในแท็บ C/C++ คือ USE_B_L072Z_LRWAN1, REGION_AS923
6. ลบส่วนโค้ด Error_Handler() ใน debug.c และ debug.h
7. ลบส่วนโค้ด SystemClockConfig() HAL_x_MspInit() HAL_xxx_MspDeInit และ handler ของ interrupt ในไฟล์ mlm32l0xx_hw.c mlm32l0xx_hal_msp.c และ mlm32l0xx_it.c

### รุ่น 0.3
เพิ่มส่วนโค้ดตัวอย่าง EndNode เข้าไปใน main.c แล้วทดสอบการคอมไพล์
1. ประกาศค่าคงที่ต่างๆ
```
#define APP_TX_DUTYCYCLE                            10000
#define LORAWAN_ADR_STATE                           LORAWAN_ADR_ON
#define LORAWAN_DEFAULT_DATA_RATE                   DR_0
#define LORAWAN_APP_PORT                            2
#define LORAWAN_DEFAULT_CLASS                       CLASS_A
#define LORAWAN_DEFAULT_CONFIRM_MSG_STATE           LORAWAN_UNCONFIRMED_MSG
#define LORAWAN_APP_DATA_BUFF_SIZE                  64
```
2. ประกาศฟังก์ชันต้นแบบเป็น callback สำหรับ LoRaWAN stack
```
static void LORA_RxData( lora_AppData_t *AppData);
static void LORA_HasJoined( void );
static void LORA_ConfirmClass ( DeviceClass_t Class );
static void LORA_TxNeeded ( void );
static void LoraStartTx(TxEventType_t EventType);
static void OnTxTimerEvent( void );
```
3. ประกาศตัวแปร persistence สำหรับสถานะการทำงาน
```
static uint8_t AppDataBuff[LORAWAN_APP_DATA_BUFF_SIZE];
lora_AppData_t AppData={ AppDataBuff,  0 ,0 };
static TimerEvent_t TxTimer;
static LoRaMainCallback_t LoRaMainCallbacks = { HW_GetBatteryLevel,
                                                HW_GetTemperatureLevel,
                                                HW_GetUniqueId,
                                                HW_GetRandomSeed,
                                                LORA_RxData,
                                                LORA_HasJoined,
                                                LORA_ConfirmClass,
                                                LORA_TxNeeded};
static LoRaParam_t LoRaParamInit = {LORAWAN_ADR_STATE,
                                    LORAWAN_DEFAULT_DATA_RATE,  
                                    LORAWAN_PUBLIC_NETWORK};
```
4. เขียนส่วนโค้ดสำหรับก่อน main loop
```
DBG_Init();  
TraceInit();
Radio.IoInit( );
LPM_SetOffMode(LPM_APPLI_Id , LPM_Disable );

PRINTF("Start program");
LORA_Init( &LoRaMainCallbacks, &LoRaParamInit);
LORA_Join();
LoraStartTx(TX_ON_TIMER) ;  
```
5. เขียนส่วนโค้ดสำหรับภายใน main loop
```
LoRaMacProcess( );
DISABLE_IRQ( );
#ifndef LOW_POWER_DISABLE
LPM_EnterLowPower( );
#endif
ENABLE_IRQ();
```
6. ปรับแก้ส่วนโค้ดของไฟล์ vcom.c โดยการอ้างถึงตัวแปร global ที่จัดการ UART2 ที่ประกาศใน main.c
```
extern UART_HandleTypeDef huart2;
```
7. ปรับแก้ส่วนโค้ดของไฟล์ main.c ด้วยการย้ายจากไฟล์ hw_rtc.c
```
static void MX_RTC_Init(void) {
  HAL_RTCEx_EnableBypassShadow(&hrtc);
}
```
8. ปรับแก้ส่วนโค้ดของไฟล์ hw_rtc.c โดยการอ้างถึงตัวแปร global ที่จัดการ RTC ที่ประกาศใน main.c
```
extern RTC_HandleTypeDef hrtc;
```
9. ปรับแก้ส่วนโค้ดของไฟล์ hw_spi.c โดยการอ้างถึงตัวแปร global ที่จัดการ SPI1 ที่ประกาศใน main.c
```
extern SPI_HandleTypeDef hspi1;
```
10. ปรับแก้ส่วนโค้ดของไฟล์  stm32l0xx_hal_msp.c ด้วยการสำเนาจากไฟล์ mlm32l0xx_hal_msp.c
11. ปรับแก้ส่วนโค้ดของไฟล์ stm32l0xx_it.c ด้วยการสำเนาจากไฟล์ mlm32l0xx_it.c
 
### รุ่น 0.4
ปรับโครงสร้างของ project เพื่อให้เชื่อมต่อระหว่าง I-CUBE-LRWAN และ CubeMX ได้
1. ปรับตัวเลือกการสร้างโค้ด
  * เลือก Generate peripheral initialization as a pair of .c/.h files per peripheral
  * เลือก Set all free pins as analog (to optimize the power consumption)
  * ปรับ visibility ของกลุ่มฟังก์ชัน `MX_` เป็นไม่ใช้ static
2. เปลี่ยนโครงสร้างของโฟลเดอร์
  * /Middlewares/LoRaWAN สำหรับไฟล์แสตก LORAWAN
  * /Drivers/BSP สำหรับไฟล์ส่วนโค้ดฮาร์ดแวร์
3. ปรับแก้ไฟล์/โค้ดที่ขัดแย้ง/ซ้ำซ้อนระหว่าง I-CUBE-LRWAN และ CubeMX
  * ลบไฟล์ mlm32l0xx_it.c และ mlm32l0xx_it.h ซึ่งซ้ำซ้อนกับ stm32l0xx_it.c/.h
  * ย้ายไฟล์ mlm32l0xx_hw.c ไปยัง /Drivers/BSP/CMWX1ZZABZ-0xx

  | ฟังก์ชัน | การปรับแก้ | 
  |---|---|
  | `HW_Init()` | ลบเพราะส่วนใหญ่ซ้ำซ้อนกับ CubeMX / ย้ายส่วนโค้ด `TraceInit()` ไปเรียกใน USER CODE 2 ของ main() |
  | `HW_DeInit()` | ลบเพราะไม่ถูกเรียกใช้ |
  | `HW_IoInit()` | เปลี่ยนไปเรียกฟังก์ชัน `HAL_SPI_MspInit()` (แทน `HW_SPI_IoInit()`) และ `HAL_UART_MspInit()` (แทน `vcom_IoInit()`) |
  | `HW_IoDeInit()` | เปลี่ยนไปเรียกฟังก์ชัน `HAL_SPI_MspDeInit()` และ `HAL_UART_MspDeInit()` (แทน `vcom_IoDeInit()`) |
  | `HW_GpioInit()` | ลบเพราะซ้ำซ้อนกับ `MX_GPIO_Init()` ของ CubeMX |
  | `SystemClock_Config()` | ลบเพราะซ้ำซ้อนกับ `SystemClock_Config` ของ CubeMX |
  | `HW_AdcInit()` | ลบเพราะซ้ำซ้อนกับ `MX_ADC_Init()` ของ CubeMX |
  | `HW_AdcDeInit()` | ลบเพราะไม่ถูกเรียกใช้ |
  | `HW_AdcReadChannel()` | ลบเงื่อนไขของตัวแปร `AdcInitialized` |
  
  * ย้ายไฟล์ mlm32l0xx_hal_msp.c ไปยัง /Drivers/BSP/CMWX1ZZABZ-0xx
  
  | ฟังก์ชัน | การปรับแก้ | 
  |---|---|
  | `HAL_MspInit()` | ย้ายส่วนโค้ดไป USER CODE MspInit 1 ในไฟล์ stm32l0xx_hal_msp.c  | 
  | `HAL_RTC_MspInit()` | ลบเพราะซ้ำซ้อนกับ `SystemClock_Config()` |
  | `HAL_RTC_MspDeInit()` | ลบเพราะซ้ำซ้อนกับ `HAL_RTC_MspDeInit()` ของ CubeMX |
  
  * ย้ายไฟล์ 

ขั้นตอนการติดตั้ง
