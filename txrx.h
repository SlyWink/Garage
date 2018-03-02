#define TX1 0b00110011
#define TX2 0b11001100

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
  #define REPEAT_DELAY 50
  #define REPEAT_TIMES 4
#endif

typedef struct {
  uint8_t name[3];
  uint8_t ident;
  uint8_t count;
} T_DATA;

typedef struct {
  uint8_t length; // Needed by Manchester library
  T_DATA data ;
  uint8_t check;
} T_MESSAGE;

T_MESSAGE g_msg ;

#define PROBE_NAME1 'G'
#define PROBE_NAME2 'A'
#define PROBE_NAME3 'R'

#define TX_SPEED MAN_600
