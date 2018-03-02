#include <xtea.h>

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
  uint8_t name[3] ;
  uint8_t ident ;
  uint8_t count ;
  uint8_t padding[3] ; // 64 bits needed for XTEA encryption
} T_DATA;

typedef struct {
  uint8_t length ; // Needed by Manchester library
  T_DATA data ;
  uint8_t check ;
} T_MESSAGE;

T_MESSAGE g_msg ;

#define LETTER1 'G'
#define LETTER2 'A'
#define LETTER3 'R'

#define PADDING    {0,0,0}

#define TX_SPEED MAN_600
