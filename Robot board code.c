/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Professional
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 5/3/2012
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

//////////////////////////////////////////RFM12 functions//////////////////////////////
#define SCK 5  // SPI clock
#define SDO 4 // SPI Data output (RFM12B side)
#define SDI 3 // SPI Data input (RFM12B side)
#define CS 2 // SPI SS (chip select)
#define NIRQ 3
#define DATA PORTD.4
#define HI(x) PORTB |= (1<<(x))
#define LO(x) PORTB &= ~(1<<(x))

#define MODULE_ON() PORTD|=(1<<7)
#define MODULE_OFF() PORTD&=~(1<<7)

#define WAIT_IRQ_LOW() while(PIND&(1<<NIRQ))
   
  char  recv_data_slave='o';

void LED_ON() {
PORTC.0=1;
 } 
void LED_OFF(){
PORTC.0=0;
 } 
void rf_portInit() {
HI(CS);
HI(SDI);
LO(SCK);
DDRB = (1<<CS) | (1<<SDI) | (1<<SCK);
DDRD.4=1; //fsk/data pulled up
DATA=1;
DDRD.3=0  ;
//PORTD.3=1;

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
rf_writeCmd_transmit(0x80D7); //EL,EF,433band,12.0pF
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
rf_writeCmd_recieve(0xC040); //1.66MHz,2.2V
}
void rfSend(unsigned char data)
{
 while(PIND.3==1) ;  //wait for previous TX over
rf_writeCmd_transmit(0xB800 + data);
}

unsigned char rfRecv() 
{
unsigned int data;
{
WAIT_IRQ_LOW();
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
/////////////////////////////////////////////////////////////////////
///////////////////////////////////TWI functions///////////////////////

char write_i2c(char slaveadd,char Data)
{
  
TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // Send START condition
while (!(TWCR &(1<<TWINT))); // Wait for TWINT flag set. This indicates that the START condition has been transmitted.

 //delay_ms(300);

if (TWSR == 0x08)// Check value of TWI Status Register. A START condition has been transmitted ACK has been received.
{
 
 // delay_ms(300);

TWDR = 0x0c; //Load Slave Address + write (SLA_W) into TWDR Register.
TWCR = (1<<TWINT) | (1<<TWEN); // Clear TWINT bit in TWCR to start transmission of address.
while (!(TWCR & (1<<TWINT)));

//  delay_ms(300);
//Wait for TWINT flag set. This indicates that the SLA+W has been transmitted.
if (TWSR == 0x18) //Check value of TWI Status Register. SLA+W has been transmitted, ACK has been received.
{

// delay_ms(300);
  
TWDR = Data; //Load DATA into TWDR Register.
 
 //delay_ms(300);

TWCR = (1<<TWINT) | (1<<TWEN); //Clear TWINT bit in TWCR to start transmission of data.
while (!(TWCR & (1<<TWINT)));

 // delay_ms(300);
  //Wait for TWINT flag set. This indicates that the DATA has been transmitted.
if (TWSR == 0x28) //Check value of TWI Status Register. Data byte has been transmitted, ACK has been received
{  

//delay_ms(200);

 //Wait for TWINT flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
//if (TWSR == 0x28) PORTA.7=1; //Check value of TWI Status Register. Data byte has been transmitted ACK has been received
//if (TWSR == 0x30) PORTB.6=1; //Data byte has been transmitted; NOT ACK has been received.
//if (TWSR == 0x38) PORTB.7=1; //Arbitration lost in SLA+W or data bytes.


TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
//while(!(TWCR & (1<<TWSTO)));  // Wait till stop condition is transmitted

//A STOP condition is generated and interrupt enabled.
//Switched to the not addressed Slave mode and own SLA will be recognized.
return 0;
}
}
}

}
char read_i2c(char slaveadd)     // will be called when the interrup is triggeried
{     char    recv_data;
 TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // Send START condition
while (!(TWCR &(1<<TWINT))); // Wait for TWINT flag set. This indicates that the START condition has been transmitted.
 while(TWSR != 0x08);//Start Has been Transmitted and Acknowledgement has been recieved
 
 //////////////////////////////////////////////////////////// 
TWDR = 0x0d;// slave addressis 6 loading slave address

TWCR = (1<<TWINT) | (1<<TWEN)|(1<<TWEA); //Load Slave Address + write (SLA_W) into TWDR Register
while (!(TWCR &(1<<TWINT))); // Wait for TWINT flag set. This indicates that the address  has been transmitted.
  
while (TWSR != 0x40);//n2bl el address wl read // address has been acknowledgmed 

 TWCR = (1<<TWINT) | (1<<TWEN); // pulling the interrupt   
 while (!(TWCR &(1<<TWINT))); 
   ////////////////////////////////////////////////////////////////////////////////////////    
 while((TWSR != 0x58)); // data was recieved but no acknowedgement was resent
  recv_data=TWDR;  /// putting the data on the register
  TWCR = (1<<TWINT) | (1<<TWEN)| (1<<TWEA);  // 
   while (!(TWCR &(1<<TWINT)));// Wait for TWINT flag set. This indicates that the data  has been recieved.
 // Check for the acknoledgement
   
TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
//while(!(TWCR & (1<<TWSTO)));  // Wait till stop condition is transmitted
 //while (!(TWCR &(1<<TWINT)));
return recv_data;;

}
void i2c_init()
{
// TWI initialization
// Bit Rate: 50.000 kHz
TWBR=0x00;
// Two Wire Bus Slave Address: 0x0
// General Call Recognition: On
TWAR=0x00;
// Generate Acknowledge Pulse: On
// TWI Interrupt: On
TWCR=0x45;
TWSR=0x00;
}
 int read_adc(unsigned char adc_input)
{
//ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}


// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
//recv_data_slave=read_i2c(6); 

char  recv_data_slave;

    recv_data_slave=read_i2c(6); 
    rf_writeCmd_transmit(0x0000);//read status register
    rf_writeCmd_transmit(0x8239);//!er,!ebb,ET,ES,EX,!eb,!ew,DC
   
    LED_ON();  
   // while(counter<50)
   {
    rfSend(0xAA);//PREAMBLE
    rfSend(0xAA);//PREAMBLE
    rfSend(0xAA);//PREAMBLE
    rfSend(0x2D);//SYNC HI BYTE
    rfSend(0xD4);//SYNC LOW BYTE     
    rfSend(recv_data_slave);//DATA BYTE 0      
    rfSend(0xAA);//DUMMY BYTE
    rfSend(0xAA);//DUMMY BYTE
    rfSend(0xAA);//DUMMY BYTE
    rf_writeCmd_transmit(0x8201);
delay_us(50);
}
LED_OFF();
rf_portInit();
 rf_writeCmd_recieve(0x82d9); //to return reciver
 
}

 void switch_to_Tranmitter(char )
  {
 
     MODULE_OFF(); //for reset the rfm12
     delay_us(50);                               
     MODULE_ON(); 
     rf_portInit();
     rf_writeCmd_transmit(0x0000);//read status register
    rf_writeCmd_transmit(0x8239);//!er,!ebb,ET,ES,EX,!eb,!ew,DC
    LED_ON();  
   // while(counter<50)
   {
    rfSend(0xAA);//PREAMBLE
    rfSend(0xAA);//PREAMBLE
    rfSend(0xAA);//PREAMBLE
    rfSend(0x2D);//SYNC HI BYTE
    rfSend(0xD4);//SYNC LOW BYTE     
    rfSend(recv_data_slave);//DATA BYTE 0
          
    rfSend(0xAA);//DUMMY BYTE
    rfSend(0xAA);//DUMMY BYTE
    rfSend(0xAA);//DUMMY BYTE
    rf_writeCmd_transmit(0x8201); 
    rf_portInit();
 rf_writeCmd_recieve(0x82d9); 

  
  }


void main(void)
{
// Declare your local variables here
        char rec='0';
        int i;
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
// INT0: On
// INT0 Mode: Low level
// INT1: Off
GICR|=0x40;
MCUCR=0x00;
GIFR=0x40;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// USART initialization
// USART disabled
UCSRB=0x00;

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
// Bit Rate: 50.000 kHz
TWBR=0x00;
// Two Wire Bus Slave Address: 0x0
// General Call Recognition: On
TWAR=0x00;
// Generate Acknowledge Pulse: On
// TWI Interrupt: On
TWCR=0x45;
TWSR=0x00;

// Global enable interrupts


DDRC.0=1;//for the ledPORTD.2=1;//////////////for the interrupt from  the i2c 
DDRD.2=0;
 //for reset 
 MODULE_OFF();
 LED_ON();
delay_ms(2000);
LED_OFF();
delay_ms(2000);
MODULE_OFF();
LED_ON();
delay_ms(2000);
LED_OFF();
delay_ms(2000);
  MODULE_ON();
rf_portInit();
 //rf_writeCmd_recieve(0xCA81);  //enable fifo
rfInit_reciver();
 
   // read from i2c
   // rf_portInit();
   // rfInit_tranmitter();   
 #asm("sei")
while (1)
      { 
          LED_ON();
    rf_writeCmd_transmit(0x0000);//read status register
    rf_writeCmd_transmit(0x8239);//!er,!ebb,ET,ES,EX,!eb,!ew,DC
    rfSend(0xAA);//PREAMBLE
    rfSend(0xAA);//PREAMBLE
    rfSend(0xAA);//PREAMBLE
    rfSend(0x2D);//SYNC HI BYTE
    rfSend(0xD4);//SYNC LOW BYTE 
    
    rfSend('a');//DATA BYTE 0
     rfSend('Y');//DATA BYTE 0
      rfSend('Z');//DATA BYTE 0
      
    rfSend(0xAA);//DUMMY BYTE
    rfSend(0xAA);//DUMMY BYTE
    rfSend(0xAA);//DUMMY BYTE
    rf_writeCmd_transmit(0x8201);
    LED_OFF();
delay_ms(50);// delay_ms(100)
     
      rf_writeCmd_recieve(0xCA83);         
      rec=rfRecv();
      if(rec=='f'||rec=='b'||rec=='r'||rec=='l'||rec=='s')
      {    
  LED_ON(); 
    write_i2c(6,rec);
    LED_OFF();    
    
    //rf_portInit();
  //  rf_writeCmd_recieve(0x82d9); 
      rf_writeCmd_recieve(0xCA81);           
      
      
      
   }
}
}
