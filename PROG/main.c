/*
 By Liyanboy74
 https://github.com/liyanboy74
*/

#include <mega32.h>
#include <i2c.h>
#include <ds1307.h>
#include <alcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <delay.h>

#define MAX_ALARM_LENGTH_S  10800

//Variable for Time and Date
unsigned char h,m,s;
unsigned char yy,mm,dd,ww;

//LCD Buffer
char lcd_b[32];

//Alarm Start
eeprom char my_h=0,my_m=0;

//Alarm Length
eeprom long int time=1;

// External Interrupt 1 service routine
interrupt [EXT_INT1] void ext_int1_isr(void)
{
 //Start Config Time Or Alarm
 lcd_clear();
 lcd_puts("Loading..");
 delay_ms(2000);

 //if Key is Released - before 2 Second
 //Config RTC
 if(PIND.3==0)
 {
 lcd_clear();
 lcd_puts("set RTC time:");
 rtc_get_time(&h,&m,&s);
 rtc_get_date(&ww,&dd,&mm,&yy);
 while(PIND.4==1)
  {
  sprintf(lcd_b,"%02d:%02d          H",h,m);
  lcd_gotoxy(0,1);
  lcd_puts(lcd_b);   
  if(PIND.5==0){h++;delay_ms(200);} 
  if(PIND.6==0){h--;delay_ms(200);} 
  if(h<=0)h=0;
  if(h>23)h=23;
  }
 while(PIND.4==0); 
 delay_ms(50); 
 while(PIND.4==1)
  {
  sprintf(lcd_b,"%02d:%02d          M",h,m);
  lcd_gotoxy(0,1);
  lcd_puts(lcd_b);   
  if(PIND.5==0){m++;delay_ms(200);} 
  if(PIND.6==0){m--;delay_ms(200);} 
  if(m<=0)m=0;
  if(m>59)m=59;
  } 
 while(PIND.4==0); 
 delay_ms(50);
 rtc_set_time(h,m,0);  
 lcd_clear(); 
 lcd_puts("set day:");
 while(PIND.4==1)
  {
  lcd_gotoxy(0,1);
  switch(ww)
   {
   case 1:lcd_puts("saturday  ");break; 
   case 2:lcd_puts("sunday    ");break;
   case 3:lcd_puts("monday    ");break;
   case 4:lcd_puts("tuesday   ");break;
   case 5:lcd_puts("wednesday ");break;
   case 6:lcd_puts("thursday  ");break;
   case 7:lcd_puts("friday    ");break;
   }
  if(PIND.5==0){ww++;delay_ms(200);} 
  if(PIND.6==0){ww--;delay_ms(200);} 
  if(ww<=0)ww=7;
  if(ww>7)ww=1;
  }
 while(PIND.4==0); 
 delay_ms(50);    
 rtc_set_date(ww,dd,mm,yy);
 lcd_clear();
 }
 //if Key Released after 2 Second
 //Config Alarm
 else
 {
 int a1,a2,a3,temp; 
 lcd_clear();
 lcd_puts("Time to start:");
 while(PIND.4==1)
  {
  sprintf(lcd_b,"%02d:%02d          H",my_h,my_m);
  lcd_gotoxy(0,1);
  lcd_puts(lcd_b);   
  if(PIND.5==0){my_h++;delay_ms(200);} 
  if(PIND.6==0){my_h--;delay_ms(200);} 
  if(my_h<=0)my_h=0;
  if(my_h>23)my_h=23;
  }
 while(PIND.4==0); 
 delay_ms(50); 
 while(PIND.4==1)
  {
  sprintf(lcd_b,"%02d:%02d          M",my_h,my_m);
  lcd_gotoxy(0,1);
  lcd_puts(lcd_b);   
  if(PIND.5==0){my_m++;delay_ms(200);} 
  if(PIND.6==0){my_m--;delay_ms(200);} 
  if(my_m<=0)my_m=0;
  if(my_m>59)my_m=59;
  } 
 while(PIND.4==0); 
 delay_ms(50); 
 lcd_clear();
 lcd_puts("Enter ON time:"); 
 delay_ms(50);
 while(PIND.4==1)
  {       
  a1=time/3600;
  temp=time%3600;
  a2=temp/60;
  a3=temp%60;
  sprintf(lcd_b,"%02d:%02d:%02d       S",a1,a2,a3);
  lcd_gotoxy(0,1);
  lcd_puts(lcd_b);   
  if(PIND.5==0){if(time<MAX_ALARM_LENGTH_S)time=time+1;delay_ms(200);} 
  if(PIND.6==0){if(time>1)time=time-1;delay_ms(200);} 
  }
 while(PIND.4==0); 
 delay_ms(50); 
 while(PIND.4==1)
  {       
  a1=time/3600;
  temp=time%3600;
  a2=temp/60;
  a3=temp%60;
  sprintf(lcd_b,"%02d:%02d:%02d       M",a1,a2,a3);
  lcd_gotoxy(0,1);
  lcd_puts(lcd_b);   
  if(PIND.5==0){if(time<=(MAX_ALARM_LENGTH_S-60))time=time+60;delay_ms(200);} 
  if(PIND.6==0){if(time>=60)time=time-60;delay_ms(200);} 
  }
 while(PIND.4==0); 
 while(PIND.4==1)
  {       
  a1=time/3600;
  temp=time%3600;
  a2=temp/60;
  a3=temp%60;
  sprintf(lcd_b,"%02d:%02d:%02d       H",a1,a2,a3);
  lcd_gotoxy(0,1);
  lcd_puts(lcd_b);   
  if(PIND.5==0){if(time<=(MAX_ALARM_LENGTH_S-3600))time=time+3600;delay_ms(200);} 
  if(PIND.6==0){if(time>=3600)time=time-3600;delay_ms(200);} 
  }
 while(PIND.4==0); 
 if((time>MAX_ALARM_LENGTH_S)||time<=0)time=1;
 lcd_clear();
 }
 //Clear External Interupt Flag
 GIFR|=(1<<INTF1);
}

void main(void)
{

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=P Bit6=P Bit5=P Bit4=P Bit3=P Bit2=P Bit1=P Bit0=P 
PORTA=(1<<PORTA7) | (1<<PORTA6) | (1<<PORTA5) | (1<<PORTA4) | (1<<PORTA3) | (1<<PORTA2) | (1<<PORTA1) | (1<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=Out Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(1<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=0 Bit6=P Bit5=P Bit4=P Bit3=P Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (1<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) | (1<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: On
// INT1 Mode: Falling Edge
// INT2: Off
GICR|=(1<<INT1) | (0<<INT0) | (0<<INT2);
MCUCR=(1<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);
GIFR=(1<<INTF1) | (0<<INTF0) | (0<<INTF2);

// Bit-Banged I2C Bus initialization
// I2C Port: PORTC
// I2C SDA bit: 1
// I2C SCL bit: 0
// Bit Rate: 100 kHz
// Note: I2C settings are specified in the
// Project|Configure|C Compiler|Libraries|I2C menu.
i2c_init();

// DS1307 Real Time Clock initialization
// Square wave output on pin SQW/OUT: Off
// SQW/OUT pin state: 0
rtc_init(0,0,0);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
lcd_init(16);

lcd_clear();

// Global Enable interrupts
#asm("sei")

while (1)
      {
      //Get Time and Date from I2C and Display
      rtc_get_time(&h,&m,&s); 
      rtc_get_date(&ww,&dd,&mm,&yy);
      sprintf(lcd_b,"%02d:%02d:%02d   %02d:%02d",h,m,s,my_h,my_m);
      lcd_gotoxy(0,0);
      lcd_puts(lcd_b);  
      lcd_gotoxy(0,1);  
       
      //Display day of week
      switch(ww)
       {
       case 1:lcd_puts("saturday  ");break; 
       case 2:lcd_puts("sunday    ");break;
       case 3:lcd_puts("monday    ");break;
       case 4:lcd_puts("tuesday   ");break;
       case 5:lcd_puts("wednesday ");break;
       case 6:lcd_puts("thursday  ");break;
       case 7:lcd_puts("friday    ");break;
       }
       
      //Print Relay State
      switch(PIND.7) 
       {
       case 0:lcd_puts(" OFF");break;
       case 1:lcd_puts("  ON");break;
       }
       
      //Process and Triger Alarm if Enabled Specific Week day 
      if((ww==1&&PINA.0==0)||(ww==2&&PINA.1==0)||(ww==3&&PINA.2==0)||(ww==4&&PINA.3==0)||(ww==5&&PINA.4==0)||(ww==6&&PINA.5==0)||(ww==7&&PINA.6==0))
       { 
       if(m==my_m&&h==my_h&&s==0) 
        {
        PORTD.7=1;
        }
       }
       
      //Process End of Alarm to Turn OFF Relay
      //I dont like that :(      
      if(PIND.7==1)
       {
       long int temp1,temp2,delta; 
       temp1=(h*3600L)+(m*60L)+s;
       temp2=(my_h*3600L)+(my_m*60L);
       delta=temp1-temp2;
       if(delta>=time)PORTD.7=0;
       }
       
      //Wait Less of Second   
      delay_ms(980);
       
      //Turn OFF at (00:00) if Device is On! (You Can Change that ;) and line 283!)  
      if(h==23&&m==59&&s==59)PORTD.7=0;
       
      //Force Relay by buttons   
      if(PIND.5==0)     PORTD.7=1;
      else if(PIND.6==0)PORTD.7=0;
      }
}
