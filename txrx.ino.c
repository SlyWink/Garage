#define IS_TX1
//#define IS_TX2
//#define IS_RX

#include <Manchester.h>
#include <xtea.h>
#include <util/crc16.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include "txrx.h"
#include "xteakey.h"


uint8_t Checksum(T_DATA *p_pdata) {
  uint8_t l_crc = 0;
  for (uint8_t i=0 ; i<sizeof(T_DATA) ; i++)
    l_crc = _crc8_ccitt_update(l_crc,((uint8_t*)p_pdata)[i]) ;
  return l_crc ;
}


EMPTY_INTERRUPT(WDT_vect) ;


//  15MS, 30MS, 60MS, 120MS, 250MS, 500MS, 1S, 2S, 4S, 8S
void Delay_Sleep(uint8_t p_wdto, uint8_t p_tics = 1) {
  if (p_wdto > 7) p_wdto = (p_wdto & 7) | _BV(WDP3) ;
  WDTCR = p_wdto | _BV(WDE) ;
  while(p_tics-- && !g_event) {
    WDTCR |= _BV(WDIE) ;
    sleep_mode() ;
  }
  WDTCR |= _BV(WDCE) | _BV(WDE) ;
  WDTCR = 0 ;
}


#ifdef IS_RX
void Display(uint8_t num) {
  noInterrupts() ;
  Serial_Debug_Send(num) ;
  interrupts() ;
}
#endif


#ifdef IS_TX
void Set_Message(T_MESSAGE *p_pmsg, uint8_t p_info, uint8_t p_count) {
  T_DATA* l_pdata = &(p_pmsg->data) ;

  *l_pdata = { DEVICE_ID, TX_ID, p_info, p_count, PADDING};
  xtea_enc(l_pdata,l_pdata,&k_xteakey) ;
  p_pmsg->length = sizeof(T_MESSAGE) ;
  p_pmsg->check = Checksum(l_pdata) ;
}
#endif


#ifdef IS_RX
void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN) ;
  pinMode(PIN_LED,OUTPUT);
  Serial_Debug_Init() ;
  man.setupReceive(PIN_RX,TX_SPEED);
  digitalWrite(PIN_LED,HIGH);
  Delay_Sleep(WDTO_1S) ;
  digitalWrite(PIN_LED,LOW);
  for (int8_t i=3 ; i>=0 ; --i) {
    Display(i) ;
    Delay_Sleep(WDTO_30MS,10);
  }
  man.beginReceiveArray(sizeof(T_MESSAGE),(uint8_t*)&g_msg);
}
#endif


#ifdef IS_TX
void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN) ;
  randomSeed(TX_ID) ;
  pinMode(PIN_LED,OUTPUT) ;
  man.setupTransmit(PIN_TX,TX_SPEED) ;
  digitalWrite(PIN_LED,HIGH) ;
  Delay_Sleep(WDTO_1S) ;
  digitalWrite(PIN_LED,LOW) ;
  Delay_Sleep(WDTO_500MS) ;
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
      if ( (l_pdata->count != s_prev) && (l_pdata->dev_id == DEVICE_ID) )
        Display(s_prev = l_pdata->count) ;
    }
    digitalWrite(PIN_LED,LOW) ;
    man.beginReceiveArray(sizeof(T_MESSAGE),(uint8_t*)&g_msg) ;
  }
}
#endif


#ifdef IS_TX
void loop() {
  static uint8_t s_cpt = 0 ;

  // FAIRE LECTURE TENSION PILE
  Set_Message(&g_msg,INFO_OPEN,s_cpt) ;
  for (uint8_t i=0 ; i<REPEAT_COUNT ; i++) {
    if (i) Delay_Sleep(REPEAT_DELAY,random(1,REPEAT_TIMES+1)) ;
    digitalWrite(PIN_LED,HIGH) ;
    man.transmitArray(sizeof(T_MESSAGE),(uint8_t*)&g_msg) ;
    digitalWrite(PIN_LED,LOW) ;
  }
  Delay_Sleep(WDTO_250MS,2) ;
  s_cpt++ ;

}
#endif
