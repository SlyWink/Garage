#ifdef IS_TX1
  #define TX_ID 0b00110011
  #define IS_TX
#elif defined IS_TX2
  #define TX_ID 0b11001100
  #define IS_TX
#endif

#ifdef IS_RX
  #define SERIAL_DEBUG_DDR DDRB
  #define SERIAL_DEBUG_PORT PORTB
  #define SERIAL_DEBUG_PIN PB0
  #include "dbginclude.c"
#endif

#ifdef IS_RX
  #define PIN_RX  PB4
  #define PIN_LED PB1
#endif

#ifdef IS_TX
  #define PIN_TX  PB3
  #define PIN_LED PB4
  #define REPEAT_COUNT 10
//  #define REPEAT_DELAY 50
  #define REPEAT_DELAY WDTO_30MS
  #define REPEAT_TIMES 4
#endif

typedef struct {
  uint8_t dev_id ;
  uint8_t dev_num ;
  uint8_t info ;
  uint8_t count ;
  uint8_t padding[4] ; // 8 bytes needed for XTEA encryption
} T_DATA;

typedef struct {
  uint8_t length ; // Needed by Manchester library
  T_DATA data ;
  uint8_t check ;
} T_MESSAGE;

T_MESSAGE g_msg ;

#define DEVICE_ID 'G'

#define PADDING    {0, 0, 0, 0}

#define TX_SPEED MAN_600

#define INFO_CLOSE    0
#define INFO_OPEN     1
#define INFO_PAUSE    2
#define INFO_LOW_BATT 3

#ifdef IS_TX
#define EVT_CLOSE  0x01
#define EVT_OPEN   0x02
#define EVT_BUTTON 0x04
#endif

#ifdef IS_RX
#define EVT_CLOSE1  0x01
#define EVT_OPEN1   0x02
#define EVT_BUTTON1 0x04
#define EVT_CLOSE2  0x10
#define EVT_OPEN2   0x20
#define EVT_BUTTON2 0x40
#endif

static volatile uint8_t g_event = 0 ;

#define STAT_OPEN     0x01
#define STAT_PAUSE    0x02
#define STAT_LOW_BATT 0x04

uint8_t g_status = 0 ;

#define PAUSE_MN 30
#define PAUSE_BY_4S (PAUSE_MN * 15)
#define PAUSE_BY_1S (PAUSE_MN * 60)
