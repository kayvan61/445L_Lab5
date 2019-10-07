#include <stdint.h>
#include "DAC.h"
#include "../inc/tm4c123gh6pm.h"

void initDAC(void){
  SYSCTL_RCGCSSI_R |= 0x02;       // activate SSI0
  SYSCTL_RCGCGPIO_R |= 0x08;      // activate port A
  while((SYSCTL_PRGPIO_R&0x08) == 0){};// ready?
  GPIO_PORTD_AFSEL_R |= 0x0B;     // enable alt funct on PD0,1,3
  GPIO_PORTD_DEN_R |= 0x0B;       // configure PA0,1,3 as SSI
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0F00)+0x00002022;
  GPIO_PORTD_AMSEL_R = 0;         // disable analog functionality on PD
  SSI1_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI1_CPSR_R = 0x02;             // 8 MHz SSIClk 
  SSI1_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 0 Freescale
  SSI1_CR0_R |= 0x0F;             // DSS = 16-bit data
  SSI1_DR_R = 0;               // load 'data' into transmit FIFO
  SSI1_CR1_R |= 0x00000002;       // enable SSI
}

void outData(uint16_t d){
	while((SSI1_SR_R&0x00000002)==0){};// wait until room in FIFO
  SSI1_DR_R = d; // data out
}
