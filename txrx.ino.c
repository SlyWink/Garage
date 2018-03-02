#include <Manchester.h>
#include <util/crc16.h>

//#define IS_TX1
//#define IS_TX2
#define IS_RX
#include "txrx.h"

uint8_t Checksum(uint8_t p_chkidx,uint8_t *p_data) {
  uint8_t l_crc = 0;
  for (uint8_t i=0 ; i<p_chkidx ; i++)
    l_crc = _crc8_ccitt_update(l_crc,p_data[i]);
  return l_crc ;
}


#ifdef IS_RX
  void Display(uint8_t num) {
    cli();
    Serial_Debug_Send(num) ;
    sei();
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
    pinMode(PIN_LED,OUTPUT);
    man.setupTransmit(PIN_TX,TX_SPEED);
    digitalWrite(PIN_LED,HIGH);
    delay(1000);
    digitalWrite(PIN_LED,LOW);
    g_msg = {sizeof(T_MESSAGE),PROBE_NAME1,PROBE_NAME2,PROBE_NAME3,TX_ID,0,0};
    randomSeed(TX_ID);
  }
#endif


#ifdef IS_RX
void loop() {
  static uint8_t s_prev = 255;
  uint8_t l_crc ;
  T_DATA* l_pdata ;

  if (man.receiveComplete()) {
    digitalWrite(PIN_LED,HIGH);
    l_pdata = &(g_msg.data);
    l_crc = Checksum(sizeof(T_DATA),(uint8_t*)l_pdata);
    if ( (l_crc == g_msg.check) && ((*l_pdata).count != s_prev) &&
      ((*l_pdata).name[0]==PROBE_NAME1) && ((*l_pdata).name[1]==PROBE_NAME2) &&
        ((*l_pdata).name[2]==PROBE_NAME3) ) {
      Display((*l_pdata).count) ;
      s_prev = (*l_pdata).count;
    }
    digitalWrite(PIN_LED,LOW);
    man.beginReceiveArray(sizeof(T_MESSAGE),(uint8_t*)&g_msg);
  }
}
#endif


#ifdef IS_TX
void loop() {
  static uint8_t s_cpt = 0;

  g_msg.data.count = s_cpt;
  g_msg.check = Checksum(sizeof(T_DATA),(uint8_t*)&(g_msg.data));
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
