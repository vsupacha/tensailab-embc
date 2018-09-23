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
