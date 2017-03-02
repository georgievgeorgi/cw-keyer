#define __16f88
#include<pic14/pic16f88.h>

typedef unsigned int config;
config __at _CONFIG1 gCONFIG1 =
_WDT_OFF  &
_MCLR_OFF &
_BODEN_OFF  &
_LVP_OFF &
_INTRC_IO& // _FOSC_HS &
_CP_OFF;


//                         ┐     ┌ QRS
//                      ┐  │     │  ┌  QRQ
//           OSCOUT  ┐  │  │     │  │  ┌
//          OUT   ┐  │  │  │  +  │  │  │  ┌
//                ↑  ↑  ↓  ↓     ↓  ↓  ↓  ↓
//              ┌─┴──┴──┴──┴──┴──┴──┴──┴──┴─┐
//              │ 1  0  7  6  P  7  6  5  4 │
//              │             w             │
//              │⊃   PORTA    S    PORTB    │
//              │             u             │
//              │ 2  3  4  5  p  0  1  2  3 │
//              └─┬──┬──┬──┬──┬──┬──┬──┬──┬─┘
//                ↑  ↑  ↑  ↑     ↑  ↑  ↑  ↑
//           DIT  ┘  │  │  │  -  │  │  │  └
//              DAH  ┘  │  │     │  │  └
//                      ┘  │     │  └
//                         ┘     └
//


#define OSCOUT RA0
#define OUT RA1
#define PDIT RA2
#define PDAH RA3
#define PPULSE RA4


void QRQ(){ //increase speed
  if(PR2<10)return;
  PR2-=10;
  //PR2*=.916666;
}

void QRS(){ //decrease speed
  if(PR2>225)return;
  PR2+=10;
  //PR2*=1.090909;
}

void init(){
  //OSCCON=0b01101110;       // Fosc 4MHz
  OSCCON=0b01111110;       // Fosc 8MHz
  //OSCCON=0b00011110;       // Fosc 125kHz
  //OPTION_REG=0b11000111; //TMR0 presc 1:256
  OPTION_REG=0b11000001; //TMR0 presc 1:4
  ANSEL=0;
  PORTA=PORTB=0;
  TRISA=TRISB=0xff;
  TRISA0=0;
  TRISA1=0;
  TRISA4=0;PPULSE=0;
  //TRISB5=0;

  INTCON=0;
  GIE=1;
  PEIE=1;
  TMR0IE=0;
  //TMR0IE=1;
  RBIE=1;

  PIE1=0;
  TMR2IE=1;
  PIR1=0;
  PIE2=0;
  PIR2=0;


  PR2=68;
  T2CON=0b01111011;// 1:16,off, 1:16
}

void wait(){
  TMR2=0;
  TMR2ON=1;
  while(TMR2ON==1);
}

void waitdot(){
      wait();wait();wait();
      wait();wait();wait();
      wait();wait();wait();
      wait();wait();wait();
}

void waitnop(){
  long int i;
  for(i=0;i<100;++i);
}


void main(void){
  init();
  while(1){
    PPULSE=1; TRISA&=0b11110011; PDIT=PDAH=0; TRISA|=0b1100; PPULSE=0;
    if(PDIT) {
      OUT=1;
      waitdot();
      OUT=0;
      waitdot();
    }
    PPULSE=1; TRISA&=0b11110011; PDIT=PDAH=0; TRISA|=0b1100; PPULSE=0;
    if(PDAH) {
      OUT=1;
      waitdot();
      waitdot();
      waitdot();
      OUT=0;
      waitdot();
    }
  }
}
static void interruptf(void) __interrupt 0 {
  if(TMR0IF){
    TMR0IF=0;
    if(OUT==1){
      OSCOUT^=1;
    }else{
      OSCOUT=0;
    }
  }
  if(TMR2IF){
    TMR2IF=0;
    TMR2ON=0;
  }
  if(RBIF){
    RBIF=0;
    if(RB7){QRS();}
    if(RB6){QRQ();}
    waitnop();
  }
}
