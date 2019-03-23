/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Professional
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 5/2/2012
Author  : 
Company : 
Comments: 


Chip type               : ATmega8L
Program type            : Application
AVR Core Clock frequency: 1.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/

#include <mega8.h>
#include <delay.h>
////////////////////////////////////////////////////
 /////////////////////////////uart code
char isCharAvailable()// indicate a char has been received?
{

if ( (UCSRA & (0x80)) ) return 1;
else return 0;
}/* Read and write functions */
unsigned char ReceiveByte( void )
{
    while ( !(USR & (1<<RXC)) )     /* Wait for incomming data */
        ;                            /* Return the data */
    return UDR;
}
void ReceiveCommand(char*a )
{
int index=0;
if(isCharAvailable())
{
a[index]=  ReceiveByte();
index++;
}
}
void TransmitByte( unsigned char data )
{
    while ( !(USR & (1<<UDRE)) )
        ;                             /* Wait for empty transmit buffer */
    UDR = data;                     /* Start transmittion */
}
void TransmitCommand (char* s)
 {  int c=0;                           
    while(s[c]!='\0')
    {
    TransmitByte(s[c]);
    c++;
    } 
    TransmitByte('\0');
 }
int counter=0;
  ////////////////////////////////rfm12 functions//////////////////////////////////////////////
#define SCK 5  // SPI clock
#define SDO 4 // SPI Data output (RFM12B side)
#define SDI 3 // SPI Data input (RFM12B side)
#define CS 2 // SPI SS (chip select)
#define NIRQ 3
#define HI(x) PORTB |= (1<<(x))
#define LO(x) PORTB &= ~(1<<(x))
#define WAIT_IRQ_LOW()  while(PIND.3==1 &&counter<80)
//#define WAIT_IRQ_LOW() while(PIND&(1<<NIRQ))
#define MODULE_ON() PORTD|=(1<<7)
#define MODULE_OFF() PORTD&=~(1<<7)
 
unsigned int rf_writeCmd(unsigned int cmd) {
unsigned char i;
unsigned int recv;
recv = 0;
LO(SCK);
LO(CS);
for(i=0; i<16; i++) 
{
recv<<=1;
if( PINB&(1<<SDO) ) 
{
recv|=0x0001;
}
LO(SCK);
if(cmd&0x8000) HI(SDI);
else LO(SDI);

HI(SCK);
cmd<<=1;
}
LO(SCK);
HI(CS);
return recv;
}

void rf_portInit() {
HI(CS);
HI(SDI);
LO(SCK);
DDRB = (1<<CS) | (1<<SDI) | (1<<SCK);
DDRD.4=1 ;  //data high fsk/DATA
PORTD.4=1;
DDRD.3=0;//input interrupt
}

void rfInit_transmitter() {

  
  rf_writeCmd(0x80D7); //EL,EF,868band,12.0pF
  rf_writeCmd(0x8239); //!er,!ebb,ET,ES,EX,!eb,!ew,DC
  rf_writeCmd(0xA640); //frequency select
  rf_writeCmd(0xC647); //4.8kbps
  rf_writeCmd(0x94A0); //VDI,FAST,134kHz,0dBm,-103dBm
  rf_writeCmd(0xC2AC); //AL,!ml,DIG,DQD4
  rf_writeCmd(0xCA81); //FIFO8,SYNC,!ff,DR

  rf_writeCmd(0xC483); //@PWR,NO RSTRIC,!st,!fi,OE,EN
  rf_writeCmd(0x9850); //!mp,90kHz,MAX OUT
  
  rf_writeCmd(0xE000); //NOT USE
  rf_writeCmd(0xC800); //NOT USE
  rf_writeCmd(0xC040); //1.66MHz,2.2V
}
void rfInit_reciver() 
{
rf_writeCmd(0x80D7); //EL,EF,868band,12.0pF
rf_writeCmd(0x82D9);; //enable reciver
rf_writeCmd(0xA640); //frequency select
rf_writeCmd(0xC647); //4.8kbps//31.348
rf_writeCmd(0x90A0); //VDI,FAST,134kHz,0dBm,-103dBm
rf_writeCmd(0xC2AC); //AL,!ml,DIG,DQD4
rf_writeCmd(0xCA81); //FIFO8,SYNC,!ff,DR       
rf_writeCmd(0xC483); //@PWR,NO RSTRIC,!st,!fi,OE,EN
rf_writeCmd(0x9850); //!mp,90kHz,MAX OUT

rf_writeCmd(0xE000); //NOT USED
rf_writeCmd(0xC800); //NOT USED
rf_writeCmd(0xC400); //1.66MHz,2.2V
}
void rfSend(unsigned char data)
{
 while(PIND.3==1) ;
rf_writeCmd(0xB800 + data);
}
unsigned char rfRecv() 
{

unsigned int data;
counter=0;
{
WAIT_IRQ_LOW();
counter=0;
rf_writeCmd(0x0000);
data=rf_writeCmd(0xB000);
return(data&0x00FF);
} 
}
void FIFOReset()
 {
rf_writeCmd(0xCA81);
rf_writeCmd(0xCA83);
}
  
//////////////////////////////////////////////////////////////////////////

void rf_portInit() {
HI(CS);
HI(SDI);
LO(SCK);
DDRB = (1<<CS) | (1<<SDI) | (1<<SCK);
DDRD.4=1 ;  //data high fsk/DATA
PORTD.4=1;
DDRD.3=0;//input interrupt
}
unsigned int rf_writeCmd_recieve(unsigned int cmd)  //spi for reciever
 {
unsigned char i;
unsigned int recv;
recv = 0;
LO(SCK);
LO(CS);
for(i=0; i<16; i++) 
{


if(cmd&0x8000) HI(SDI);
else LO(SDI);

HI(SCK);
recv<<=1;
 if( PINB&(1<<SDO) ) 
{
recv|=0x0001;
}
LO(SCK);
cmd<<=1;
}
HI(CS);
return recv;
}

unsigned int rf_writeCmd_transmit(unsigned int cmd) //spi for tranmitter
{
unsigned char i;
unsigned int recv;
recv = 0;
LO(SCK);
LO(CS);
for(i=0; i<16; i++) 
{
recv<<=1;
if( PINB&(1<<SDO) ) 
{
recv|=0x0001;
}
LO(SCK);
if(cmd&0x8000) HI(SDI);
else LO(SDI);

HI(SCK);
cmd<<=1;
}
LO(SCK);
HI(CS);
return recv;
}
 

void rfInit_tranmitter() {
rf_writeCmd_transmit(0x80D7); //EL,EF,868band,12.0pF
rf_writeCmd_transmit(0x8239); //!er,!ebb,ET,ES,EX,!eb,!ew,DC
rf_writeCmd_transmit(0xA640); //frequency select
rf_writeCmd_transmit(0xC647); // 4.8
rf_writeCmd_transmit(0x94A0); //VDI,FAST,134kHz,0dBm,-103dBm
rf_writeCmd_transmit(0xC2AC); //AL,!ml,DIG,DQD4
rf_writeCmd_transmit(0xCA81); //FIFO8,SYNC,!ff,DR        
rf_writeCmd_transmit(0xC483); //@PWR,NO RSTRIC,!st,!fi,OE,EN
rf_writeCmd_transmit(0x9850); //!mp,90kHz,MAX OUT
rf_writeCmd_transmit(0xE000); //NOT USED
rf_writeCmd_transmit(0xC800); //NOT USED
rf_writeCmd_transmit(0xC040); //1.66MHz,2.2V
}
void rfInit_reciver() {
rf_writeCmd_recieve(0x80D7); //EL,EF,868band,12.0pF
rf_writeCmd_recieve(0x82D9);; //enable reciver
rf_writeCmd_recieve(0xA640); //frequency select
rf_writeCmd_recieve(0xC647); 
rf_writeCmd_recieve(0x90A0); //VDI,FAST,134kHz,0dBm,-103dBm
rf_writeCmd_recieve(0xC2AC); //AL,!ml,DIG,DQD4
rf_writeCmd_recieve(0xCA81); //FIFO8,SYNC,!ff,DR
      
rf_writeCmd_recieve(0xC483); //@PWR,NO RSTRIC,!st,!fi,OE,EN
rf_writeCmd_recieve(0x9850); //!mp,90kHz,MAX OUT

rf_writeCmd_recieve(0xE000); //NOT USED
rf_writeCmd_recieve(0xC800); //NOT USED
rf_writeCmd_recieve(0xC400); //1.66MHz,2.2V
}


void rfSend(unsigned char data)
{
 while(PIND.3==1) ;  //wait for previous TX over
rf_writeCmd_transmit(0xB800 + data);
}


unsigned char rfRecv() 
{ unsigned int data;
counter=0;

{
WAIT_IRQ_LOW();
counter=0;
rf_writeCmd_recieve(0x0000);
data=rf_writeCmd_recieve(0xB000);
return(data&0x00FF);
} 
}

void FIFOReset()
 {
rf_writeCmd_recieve(0xCA81);
rf_writeCmd_recieve(0xCA83);
}

void LED_ON()
 {
PORTD.5=1;
 } 
void LED_OFF()
 {
PORTD.5=0;
 }

char switch_to_reciever()
  {
  char recieved_RF;  
     MODULE_OFF(); //for reset the rfm12
     delay_us(50);                               
     MODULE_ON(); 
     rf_portInit();
     rf_writeCmd_recieve(0xCA81);  //enable fifo
     rfInit_reciver();
     rf_writeCmd_recieve(0xCA83);
     recieved_RF=rfRecv(); 
    // TransmitByte(rec);
     //TransmitByte('\0');    
   /*   rf_writeCmd_recieve(0xCA81);
      delay_us(50);     
      MODULE_OFF(); //for reset the rfm12
delay_us(50);                              
  MODULE_ON(); 
     rf_portInit();
   rfInit_tranmitter();*/
   return recieved_RF; 
  }

interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
  counter++;
}

// Standard Input/Output functions

#include <stdio.h>

// Declare your global variables here
    int rec;
void main(void)
{
// Declare your local variables here
char cmd;
// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;
// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 1000.000 kHz
TCCR0=0x01;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x01;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 2400  //changed to 4800
UCSRA=0x00;
UCSRB=0x18;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x0C;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;
//#asm("cli") 
PORTD.3=1;
DDRD.3=0;
DDRD.5=1;//for the led to be output
delay_ms(80);
TransmitCommand("hello");
MODULE_OFF(); //for reset the rfm12
 LED_ON();
delay_ms(2000);
LED_OFF();
delay_ms(2000);
LED_ON();
delay_ms(2000);
LED_OFF();                                
  MODULE_ON();  
//rf_portInit();
//rfInit_tranmitter();
 //rf_portInit();
// rf_writeCmd_recieve(0xCA81);  //enable fifo
///rfInit_reciver();
LED_ON();
delay_ms(70);
LED_OFF();
rf_portInit();
rfInit_tranmitter();   
rf_writeCmd_transmit(0x82f9);        
 #asm("sei") ;    
while (1)
      {  
     
      // Place your code here 
       if(isCharAvailable())
      { 
         LED_ON();
        cmd=ReceiveByte(); 
        
         
        rf_writeCmd_transmit(0x0000);//read status register
        rf_writeCmd_transmit(0x8239);//!er,!ebb,ET,ES,EX,!eb,!ew,DC  
        rfSend(0xAA);//PREAMBLE
        rfSend(0xAA);//PREAMBLE
        rfSend(0xAA);//PREAMBLE
        rfSend(0x2D);//SYNC HI BYTE
        rfSend(0xD4);//SYNC LOW BYTE 
     
        if(cmd=='s')
      {
       rfSend('s');
      } 
       else if(cmd=='f')
      { 
      rfSend('f');
      }
      else if(cmd=='b')
      { 
      rfSend('b');
      }
        else if(cmd=='l')
      {   
      rfSend('l');
      }
       else if(cmd=='r')
      { 
      rfSend('r');
      }
       else if(cmd=='p')// for positioning 
      {  
      rfSend('p');
      } 
         rfSend(0xAA);//PREAMBLE
         rfSend(0xAA);//PREAMBLE
         rfSend(0xAA);//PREAMBLE
         LED_OFF(); 
      }
 
    rec= switch_to_reciever();
    if(rec!='o'&& rec!='')
     {   
      //  TransmitCommand("Right Bummber hit the robot stopped");
      TransmitByte(rec);
      TransmitByte('\0');
     }
   
  /* 
    rf_writeCmd_recieve(0xCA83);
      rec=rfRecv(); 
       TransmitByte(rec);
       TransmitByte('\0');    
       
         rec=rfRecv(); 
       TransmitByte(rec);
       TransmitByte('\0');
       
         rec=rfRecv(); 
       TransmitByte(rec);
       TransmitByte('\0');
      rf_writeCmd_recieve(0xCA81);
      delay_us(50);  
      
      */ 
      }
}
