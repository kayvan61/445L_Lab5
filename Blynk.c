// -------------------------------------------------------------------
// File name: Blynk.c
// Description: This code is used to bridge the TM4C123 board and the Blynk Application
//              via the ESP8266 WiFi board
// Author: Mark McDermott and Andrew Lynch (Arduino source)
// Converted to EE445L style Jonathan Valvano
// Orig gen date: May 21, 2018
// Last update: Sept 20, 2018
//
// Download latest Blynk library here:
//   https://github.com/blynkkk/blynk-library/releases/latest
//
//  Blynk is a platform with iOS and Android apps to control
//  Arduino, Raspberry Pi and the likes over the Internet.
//  You can easily build graphic interfaces for all your
//  projects by simply dragging and dropping widgets.
//
//   Downloads, docs, tutorials: http://www.blynk.cc
//   Sketch generator:           http://examples.blynk.cc
//   Blynk community:            http://community.blynk.cc
//
//------------------------------------------------------------------------------

// TM4C123       ESP8266-ESP01 (2 by 4 header)
// PE5 (U5TX) to Pin 1 (Rx)
// PE4 (U5RX) to Pin 5 (TX)
// PE3 output debugging
// PE2 nc
// PE1 output    Pin 7 Reset
// PE0 input     Pin 3 Rdy IO2
//               Pin 2 IO0, 10k pullup to 3.3V  
//               Pin 8 Vcc, 3.3V (separate supply from LaunchPad 
// Gnd           Pin 4 Gnd  
// Place a 4.7uF tantalum and 0.1 ceramic next to ESP8266 3.3V power pin
// Use LM2937-3.3 and two 4.7 uF capacitors to convert USB +5V to 3.3V for the ESP8266
// http://www.ti.com/lit/ds/symlink/lm2937-3.3.pdf
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "UART.h"
#include "PortF.h"
#include "esp8266.h"
#include "timers.h"
#include "buttonDriver.h"
#include "speaker.h"
#include "Clock.h"

void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

uint32_t LED;      // VP1
uint32_t LastF;    // VP74
// These 6 variables contain the most recent Blynk to TM4C123 message
// Blynk to TM4C123 uses VP0 to VP15
char serial_buf[64];
char Pin_Number[2]   = "99";       // Initialize to invalid pin number
char Pin_Integer[8]  = "0000";     //
char Pin_Float[8]    = "0.0000";   //
char Pin_String[128] = "";
uint32_t pin_num; 
uint32_t pin_int;
int lastp = -1, lastv = -1;
int lastpp = -1, lastpv = -1;
 
// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin,uint32_t value){
  if((pin < 70)||(pin > 99) || (lastp == pin && lastv == value)){
		UART_OutString("Illegal String");
    return; // ignore illegal requests
  }
	lastp = pin;
	lastv = value;
	// your account will be temporarily halted 
  // if you send too much data
  ESP8266_OutUDec(pin);       // Send the Virtual Pin #
  ESP8266_OutChar(',');
  ESP8266_OutUDec(value);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
	UART_OutUDec(pin);       // Send the Virtual Pin #
  UART_OutChar(',');
  UART_OutUDec(value);      // Send the current value
  UART_OutChar(',');
  UART_OutString("0.0\n\r");  // Null value not used in this example
}
 
 
// -------------------------   Blynk_to_TM4C  -----------------------------------
// This routine receives the Blynk Virtual Pin data via the ESP8266 and parses the
// data and feeds the commands to the TM4C.
void Blynk_to_TM4C(void){int j; char data;
// Check to see if a there is data in the RXD buffer
  if(ESP8266_GetMessage(serial_buf)){  // returns false if no message
    // Read the data from the UART5
#ifdef DEBUG1
    j = 0;
    do{
      data = serial_buf[j];
      UART_OutChar(data);        // Debug only
      j++;
    }while(data != '\n');
    UART_OutChar('\r');        
#endif
           
// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);  
  // ---------------------------- VP #1 ----------------------------------------
  // This VP is the LED select button
    if(pin_num == 0x01)  {  
      LED = pin_int;
      PortF_Output(LED<<2); // Blue LED
#ifdef DEBUG3
      Output_Color(ST7735_CYAN);
      ST7735_OutString("Rcv VP1 data=");
      ST7735_OutUDec(LED);
      ST7735_OutChar('\n');
#endif
    }                               // Parse incoming data        
#ifdef DEBUG1
    UART_OutString(" Pin_Number = ");
    UART_OutString(Pin_Number);
    UART_OutString("   Pin_Integer = ");
    UART_OutString(Pin_Integer);
    UART_OutString("   Pin_Float = ");
    UART_OutString(Pin_Float);
    UART_OutString("\n\r");
#endif
  }  
}

void delay(){
	volatile uint32_t d = 0;
	while(d < 80000000){
		d++;
	}
}

uint8_t off = 0;

void getSunRise(void){int j; char data;
	ESP8266_OutUDec(16);       // Send the Virtual Pin #
	ESP8266_OutChar(',');
  ESP8266_OutUDec(1);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
	
	delay();
	
	//UART_OutString("sent pin data for sunrise time");

// Check to see if a there is data in the RXD buffer
	volatile int d = 0;
	while(!ESP8266_GetMessage(serial_buf)){
		d = 1;
	}
	PortF_Output(1<<2);

    // Read the data from the UART5
	DisableInterrupts();
#ifdef DEBUG1
    j = 0;
    do{
      data = serial_buf[j];
      UART_OutChar(data);        // Debug only
      j++;
    }while(data != '\n');
    UART_OutChar('\r');        
#endif
           
// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
		strtok(serial_buf, " \"");
    strcpy(Pin_String, strtok(NULL, " \""));       // Integer value that is determined by the Blynk App
		
		PortF_Output(0<<2);
  // ---------------------------- VP #1 ----------------------------------------
                           // Parse incoming data        
#ifdef DEBUG1
    UART_OutString("   Pin_String = ");
    UART_OutString(Pin_String);
    UART_OutString("\n\r");
#endif
		EnableInterrupts();
}


void SendInformation(int pin, int data){
  //uint32_t thisF;
  //thisF = PortF_Input();
// your account will be temporarily halted if you send too much data
  //if(thisF != LastF){
  TM4C_to_Blynk(pin, data);  // VP74
#ifdef DEBUG3
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 74 data=");
    //ST7735_OutUDec(thisF);
    ST7735_OutChar('\n');
#endif
  //}
  //LastF = thisF;
}

void buttonLogic(void);

int isAInput = 1;
int isCInput = 0;
int hour = 0;
int min = 0;
  
int main(void){       
  PLL_Init(Bus80MHz);   // Bus clock at 80 MHz
  DisableInterrupts();  // Disable interrupts until finished with inits
  PortF_Init();
  ST7735_InitR(INITR_REDTAB);
	UART_Init(7);
  LastF = PortF_Input();
#ifdef DEBUG3
  ST7735_InitR(INITR_REDTAB);        // initialize ST7735
  ST7735_OutString("EE445L Lab 4D\nBlynk example\n");
#endif
#ifdef DEBUG1
  UART_Init(5);         // Enable Debug Serial Port
  UART_OutString("\n\rEE445L Lab 4D\n\rBlynk example");
#endif
  ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
	
	displayAlarm(0, 0);
	updateClock(0, 0);
	displayClock();
	toggleAlarm(!isAInput);
	
	EnableInterrupts();
	
	getSunRise();
	
	Pin_String[off+2] = '\0';
	Pin_String[off+5] = '\0';
	Pin_String[off+8] = '\0';
	uint8_t h, m, s;
	h = atoi(Pin_String+off);
	m = atoi(Pin_String+off+3);
	s = atoi(Pin_String+off+6);
	
	UART_OutUDec(h);
	UART_OutUDec(m);
	UART_OutUDec(s);
	
	DisableInterrupts();
	
	buttonInit(0x00F0, buttonLogic);
	clockInit();
	speakerInit();
	
	clockSetState(h, m, s);
  
	EnableInterrupts();
	
  while(1) {   
    WaitForInterrupt(); // low power mode
  }
}

// V0 = hour toggle
// V1 = minute toggle
// V2 = Arm/Change Alarm toggle 
// V3 = Change Alarm toggle
void buttonLogic(void) {
	lastpp = pin_num;
	lastpv = pin_int;
	Blynk_to_TM4C();
	int isDirt = 0;
	if(!isAInput && !isCInput) {
		hour = 0;
		min = 0;
		clearSet();
	}
	if(getSubStale()) {
		TM4C_to_Blynk(76, getClockState());
	}
	if(getStale()) {
		displayClock();
	}
	if(!pin_int || (lastpp == pin_num && lastpv == pin_int)){
		return;
	}
	switch(pin_num) {
		case 0:
			hour++;
			isDirt = 1;
			break;
		case 1:
			min++;
			isDirt = 1;
			break;
		case 2:
			isAInput = !isAInput;
			toggleAlarm(!isAInput);
			TM4C_to_Blynk(74, (!isArmed()) * 255);
			break;
		case 3:
			isCInput = !isCInput;
			TM4C_to_Blynk(75, isCInput * 255);
			break;
		default:
			return;
	}
	
	if(isDirt){			
		if (min >= 60){
			hour += 1;
			min = 0;
		}
		
		if (hour >= 24) {
			hour = 0;
		}
		
		if(isAInput){
			displayAlarm(min, hour);
		}
		if(isCInput){
			updateClock(min, hour);
		}
		
		UART_OutUDec(hour);
		UART_OutChar(':');
		UART_OutUDec(min);
		UART_OutString("\n\r");
	}
}



