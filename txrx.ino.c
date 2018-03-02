#include <Manchester.h>
#include <xtea.h>
#include "xteakey.h"
#include <util/crc16.h>

//#define IS_TX1
//#define IS_TX2
#define IS_RX

#include "txrx.h"

uint8_t Checksum(T_DATA *p_pdata) {
  uint8_t l_crc = 0;
  for (uint8_t i=0 ; i<sizeof(T_DATA) ; i++)
    l_crc = _crc8_ccitt_update(l_crc,((uint8_t*)p_pdata)[i]) ;
  return l_crc ;
}


#ifdef IS_RX
  void Display(uint8_t num) {
    cli() ;
    Serial_Debug_Send(num) ;
    sei() ;
  }
#endif


#ifdef IS_TX
void Set_Message(T_MESSAGE *p_pmsg, uint8_t p_count) {
  T_DATA* l_pdata = &(p_pmsg->data) ;

  *l_pdata = { {LETTER1, LETTER2, LETTER3}, TX_ID, p_count, PADDING};
  xtea_enc(l_pdata,l_pdata,&k_xteakey) ;
  p_pmsg->length = sizeof(T_MESSAGE) ;
  p_pmsg->check = Checksum(l_pdata) ;
}
#endif


#ifdef IS_RX
  void setup() {
    pinMode(PIN_LED,OUTPUT);
    Serial_Debug_Init() ;
    man.setupReceive(PIN_RX,TX_SPEED);
    digitalWrite(PIN_LED,HIGH);
    delay(1000);
    digitalWrite(PIN_LED,LOW);
    for (int8_t i=3 ; i>=0 ; --i) {
      Display(i) ;
      delay(300);
    }
    man.beginReceiveArray(sizeof(T_MESSAGE),(uint8_t*)&g_msg);
  }
#endif


#ifdef IS_TX
  void setup() {
    pinMode(PIN_LED,OUTPUT) ;
    man.setupTransmit(PIN_TX,TX_SPEED) ;
    digitalWrite(PIN_LED,HIGH) ;
    delay(1000) ;
    digitalWrite(PIN_LED,LOW) ;
    randomSeed(TX_ID) ;
  }
#endif


#ifdef IS_RX
void loop() {
  static uint8_t s_prev = 255 ;
  T_DATA* l_pdata ;

  if (man.receiveComplete()) {
    digitalWrite(PIN_LED,HIGH) ;
    l_pdata = &(g_msg.data) ;
    if (Checksum(l_pdata) == g_msg.check) {
      xtea_dec(l_pdata,l_pdata,&k_xteakey) ;
      if ( (l_pdata->count != s_prev) && (l_pdata->name[0] == LETTER1) &&
        (l_pdata->name[1] == LETTER2) && (l_pdata->name[2] == LETTER3) ) {
        Display(l_pdata->count) ;
        s_prev = l_pdata->count ;
      }
    }
    digitalWrite(PIN_LED,LOW) ;
    man.beginReceiveArray(sizeof(T_MESSAGE),(uint8_t*)&g_msg) ;
  }
}
#endif


#ifdef IS_TX
void loop() {
  static uint8_t s_cpt = 0 ;

  Set_Message(&g_msg,s_cpt) ;
  digitalWrite(PIN_LED,HIGH);
  for (uint8_t i=0 ; i<REPEAT_COUNT ; i++) {
    if (i) delay(random(1,REPEAT_TIMES+1) * REPEAT_DELAY);
    man.transmitArray(sizeof(T_MESSAGE),(uint8_t*)&g_msg);
  }
  digitalWrite(PIN_LED,LOW);
  delay(500);
  s_cpt++;
}
#endif
