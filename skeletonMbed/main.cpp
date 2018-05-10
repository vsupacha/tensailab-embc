#include "mbed.h"
#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
#include "events/EventQueue.h"
#include "lorawan/LoRaRadio.h"
#include "SX1276_LoRaRadio.h"

#define MAX_NUMBER_OF_EVENTS            10
#define CONFIRMED_MSG_RETRY_COUNTER     3
#define SX1276                          0xEE


SX1276_LoRaRadio radio(MBED_CONF_APP_LORA_SPI_MOSI,
                        MBED_CONF_APP_LORA_SPI_MISO,
                        MBED_CONF_APP_LORA_SPI_SCLK,
                        MBED_CONF_APP_LORA_CS,
                        MBED_CONF_APP_LORA_RESET,
                        MBED_CONF_APP_LORA_DIO0,
                        MBED_CONF_APP_LORA_DIO1,
                        MBED_CONF_APP_LORA_DIO2,
                        MBED_CONF_APP_LORA_DIO3,
                        MBED_CONF_APP_LORA_DIO4,
                        MBED_CONF_APP_LORA_DIO5,
                        MBED_CONF_APP_LORA_RF_SWITCH_CTL1,
                        MBED_CONF_APP_LORA_RF_SWITCH_CTL2,
                        MBED_CONF_APP_LORA_TXCTL,
                        MBED_CONF_APP_LORA_RXCTL,
                        MBED_CONF_APP_LORA_ANT_SWITCH,
                        MBED_CONF_APP_LORA_PWR_AMP_CTL,
                        MBED_CONF_APP_LORA_TCXO);

static LoRaWANInterface lorawan(radio);
static lorawan_app_callbacks_t callbacks;
static void lora_event_handler(lorawan_event_t event);

static EventQueue ev_queue(MAX_NUMBER_OF_EVENTS * EVENTS_EVENT_SIZE);

uint8_t tx_buffer[LORAMAC_PHY_MAXPAYLOAD];
uint8_t rx_buffer[LORAMAC_PHY_MAXPAYLOAD];

lorawan_channelplan_t channel_plan;
static const channel_params_t AS923_LC3 = { 923600000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 };
static const channel_params_t AS923_LC4 = { 923800000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 };
static const channel_params_t AS923_LC5 = { 924000000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 };
static const channel_params_t AS923_LC6 = { 924200000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 };
static const channel_params_t AS923_LC7 = { 924400000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 };
static const channel_params_t AS923_LC8 = { 924600000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 };

int count = 0;
InterruptIn mybtn(USER_BUTTON);
Ticker sender;

void countBtn()
{
    count++;
}

void send_message(void);

int main()
{
    mybtn.fall(&countBtn);
    lorawan_status_t retcode;
    
    // setup
    if (lorawan.initialize(&ev_queue) == LORAWAN_STATUS_OK) {
        printf("\r\n LoRaWAN stack initialization OK \r\n");
    } else {
        printf("\r\n LoRaWAN stack initialization failed! \r\n");
        return -1;
    }
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);
    if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER)
                                          == LORAWAN_STATUS_OK) {
        printf("\r\n set_confirmed_msg_retries OK \r\n");
    } else {
        printf("\r\n set_confirmed_msg_retries failed! \r\n");
        return -1;
    }
    if (lorawan.enable_adaptive_datarate() == LORAWAN_STATUS_OK) {
        printf("\r\n enable_adaptive_datarate OK \r\n");
    } else {
        printf("\r\n enable_adaptive_datarate failed! \r\n");
        return -1;
    }
    
    retcode = lorawan.connect();

    if ((retcode == LORAWAN_STATUS_OK) ||
        (retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS)) {
        printf("\r\n Connecting \r\n");
    } else {
        printf("\r\n Connection error, code = %d \r\n", retcode);
        return -1;
    }
    
    ev_queue.dispatch_forever();

    return 0;
}

static void lora_event_handler(lorawan_event_t event)
{    
    switch(event) {
        case CONNECTED:
            printf("\r\n Connected \r\n");
            //sender.attach(&send_message, 30.0);
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
                printf("\r\n Message in queue \r\n");
            } 
            break;
        case DISCONNECTED:
        case TX_DONE:
            printf("\r\n Message Sent to Network Server \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
                printf("\r\n Message in queue \r\n");
            }
            break;
        case RX_DONE:
            int sz = lorawan.receive(MBED_CONF_LORA_APP_PORT, rx_buffer,
                              LORAMAC_PHY_MAXPAYLOAD,
                              MSG_CONFIRMED_FLAG|MSG_UNCONFIRMED_FLAG);
            if (sz > 0) { 
                printf("\r\n Got: ");
                for (int i = 0; i < sz; i++) {
                    printf("%X ", rx_buffer[i]);
                }
            } else {
                
            }
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("\r\n Transmission Error - EventCode = %d \r\n", event);
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
        case JOIN_FAILURE:
            printf("\r\n Joining fails! \r\n");
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}

void send_message()
{
    const uint16_t id = 0xFF;

    uint16_t *pbuf;
    int retcode;
    
    pbuf = (uint16_t*)tx_buffer;
    pbuf[0] = id;
    pbuf[1] = count;
    retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, tx_buffer, 4, MSG_CONFIRMED_FLAG);
    if (retcode < 0) {
        printf("Sending error\n");
    }

    memset(tx_buffer, 0, LORAMAC_PHY_MAXPAYLOAD);
}
