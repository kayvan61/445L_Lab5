#include "Sound.h"
#include <stdint.h>
#include "DAC.h"
#include "../inc/tm4c123gh6pm.h"

#define C_2 19111   // 65.406 Hz
#define DF_1 18039   // 69.296 Hz
#define D_1 17026   // 73.416 Hz
#define EF_1 16071   // 77.782 Hz
#define E_1 15169   // 82.407 Hz
#define F_1 14317   // 87.307 Hz
#define GF_1 13514   // 92.499 Hz
#define G_1 12755   // 97.999 Hz
#define AF_1 12039   // 103.826 Hz
#define A_1 11364   // 110.000 Hz
#define BF_1 10726   // 116.541 Hz
#define B_1 10124   // 123.471 Hz

#define C_1 9556   // 130.813 Hz
#define DF0 9019   // 138.591 Hz
#define D0 8513   // 146.832 Hz
#define EF0 8035   // 155.563 Hz
#define E0 7584   // 164.814 Hz
#define F0 7159   // 174.614 Hz
#define GF0 6757   // 184.997 Hz
#define G0 6378   // 195.998 Hz
#define AF0 6020   // 207.652 Hz
#define A0 5682   // 220.000 Hz
#define BF0 5363   // 233.082 Hz
#define B0 5062   // 246.942 Hz

#define C0 4778   // 261.626 Hz
#define DF 4510   // 277.183 Hz
#define D 4257   // 293.665 Hz
#define EF 4018   // 311.127 Hz
#define E 3792   // 329.628 Hz
#define F 3579   // 349.228 Hz
#define GF 3378   // 369.994 Hz
#define G 3189   // 391.995 Hz
#define AF 3010   // 415.305 Hz
#define A 2841   // 440.000 Hz
#define BF 2681   // 466.164 Hz
#define B 2531   // 493.883 Hz

#define C 2389   // 523.251 Hz
#define DF1 2255   // 554.365 Hz
#define D1 2128   // 587.330 Hz
#define EF1 2009   // 622.254 Hz
#define E1 1896   // 659.255 Hz
#define F1 1790   // 698.456 Hz
#define GF1 1689   // 739.989 Hz
#define G1 1594   // 783.991 Hz
#define AF1 1505   // 830.609 Hz
#define A1 1420   // 880.000 Hz
#define BF1 1341   // 932.328 Hz
#define B1 1265   // 987.767 Hz

#define C1 1194   // 1046.502 Hz
#define DF2 1127   // 1108.731 Hz
#define D2 1064   // 1174.659 Hz
#define EF2 1004   // 1244.508 Hz
#define E2 948   // 1318.510 Hz
#define F2 895   // 1396.913 Hz
#define GF2 845   // 1479.978 Hz
#define G2 797   // 1567.982 Hz
#define AF2 752   // 1661.219 Hz
#define A2 710   // 1760.000 Hz
#define BF2 670   // 1864.655 Hz
#define B2 633   // 1975.533 Hz

#define C2 597   // 2093.005 Hz

#define BPM 100
#define NUM_NOTES 300

typedef struct {
	uint16_t amp;
	uint16_t freq;
	uint8_t dur;
} Note;

void DisableInterrupts(void); // Disable interrupts

void (*timer2A_Func)(void);
void (*timer3A_Func)(void);

static const Note* currentNote;

//https://pianoletternotes.blogspot.com/2017/10/dragonborn-skyrim-theme.html

static const Note songLeft[NUM_NOTES] = {
	{1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2},
	{1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, 
	{1, 0, 1}, {1, 0, 1}, 
	{1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2},
	{1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2},
	{1, 0, 1}, {1, 0, 1}, 
	{1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2},
	{1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2},
	{1, 0, 1}, {1, 0, 1}, 
	{1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, 
	{1, B1, 2}, {1, B1, 2}, {1, B1, 2}, {1, B1, 2}, 
	{1, 0, 1}, {1, 0, 1}, 
	{1, B1, 2}, 
	{1, 0, 1}, {1, 0, 1}, 
	{1, B1, 12}, {1, E1, 12}, {1, B1, 12}, {1, A1, 12}, {1, B1, 12},
	{1, E1, 12}, {1, B1, 12}, {1, C1, 12},
	//d---dCd---Cde-d-
	{1, D1, 4}, {1, D1, 1}, {1, C1, 1}, {1, D1, 4},
	{1, C1, 1}, {1, D1, 1}, {1, E1, 2}, {1, D1, 2}
};

static const Note songRight[NUM_NOTES] = {
	{1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, 
	{1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, 
	{1, C1, 1}, {1, D1, 1}, 
	{1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, 
	{1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, 
	{1, C1, 1}, {1, D1, 1}, 
	{1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, 
	{1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, 
	{1, C1, 1}, {1, D1, 1}, 
	{1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2},
	{1, 0, 2}, {1, 0, 2}, {1, 0, 2}, {1, 0, 2}, 
	{1, C1, 1}, {1, D1, 1}, 
	{1, 0, 2}, 
	{1, C1, 1}, {1, D1, 1},
	{1, 0, 12}, {1, 0, 12}, {1, 0, 12}, {1, 0, 12}, {1, 0, 12},
	{1, 0, 12}, {1, 0, 12}, {1, 0, 12},
	//d---dCd---Cde-d-
	{1, 0, 4}, {1, 0, 1}, {1, 0, 1}, {1, 0, 4},
	{1, 0, 1}, {1, 0, 1}, {1, 0, 2}, {1, 0, 2}
};




static uint8_t isPlay = 1;
static uint8_t isDacOut = 1;
static uint8_t waveIndex = 0;
static uint8_t instrumentVec = 0x1F;

const uint16_t Flute[64] = {  
	  1007,1252,1374,1548,1698,1797,1825,1797,1675,1562,1383,
	  1219,1092,1007,913,890,833,847,810,777,744,674,
	  598,551,509,476,495,533,589,659,758,876,
	  1007,1252,1374,1548,1698,1797,1825,1797,1675,1562,1383,
	  1219,1092,1007,913,890,833,847,810,777,744,674,
	  598,551,509,476,495,533,589,659,758,876
};

const uint16_t Horn[64] = {  
  1063,1082,1119,1275,1678,1748,1275,755,661,661,703,
  731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,
  1370,1086,708,519,448,490,566,684,802,992,
	1063,1082,1119,1275,1678,1748,1275,755,661,661,703,
  731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,
  1370,1086,708,519,448,490,566,684,802,992
};  

const uint16_t Wave[64] = {  
  1024,1122,1215,1302,1378,1440,1486,1514,1524,1514,1486,
  1440,1378,1302,1215,1122,1024,926,833,746,670,608,
  562,534,524,534,562,608,670,746,833,926,
	1024,1122,1215,1302,1378,1440,1486,1514,1524,1514,1486,
  1440,1378,1302,1215,1122,1024,926,833,746,670,608,
  562,534,524,534,562,608,670,746,833,926
};  

const uint16_t Trumpet[64] = {  
  1007,1088,1156,1194,1067,789,303,99,789,1510,1476,
  1173,1067,1037,1084,1062,1011,1015,1045,1062,1011,1011,
  1058,1113,1084,1075,1079,1105,1088,1049,1015,1045,
	1007,1088,1156,1194,1067,789,303,99,789,1510,1476,
  1173,1067,1037,1084,1062,1011,1015,1045,1062,1011,1011,
  1058,1113,1084,1075,1079,1105,1088,1049,1015,1045
};  

const uint16_t Guitar[64] = {  
 1024, 1024, 1026, 1001, 935, 833, 730, 647, 619, 666, 782, 
 964, 1172, 1337, 1461, 1536, 1558, 1503, 1396, 1235, 1040, 864, 
 755, 758, 877, 1087, 1358, 1613, 1789, 1846, 1772, 1625, 1454, 
 1285, 1173, 1096, 1052, 1034, 1027, 1036, 1079, 1137, 1184, 1247, 
 1264, 1250, 1182, 1081, 974, 865, 801, 779, 810, 844, 859, 
 859, 851, 849, 875, 922, 977, 1024, 1024, 1024
};



void initSound(void) {
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x08;      // activate timer2
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER3_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER3_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER3_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER3_TAILR_R = (80000000/BPM);         // start value for 10kHz interrupts
  TIMER3_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// clear timer2A timeout flag
  TIMER3_CTL_R |= TIMER_CTL_TAEN;  // enable timer2A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer2A=priority 1
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x20000000; // top 3 bits
  NVIC_EN1_R |= 1<<(3);              // enable interrupt 23 in NVIC
	
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x04;      // activate timer2
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER2_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER2_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER2_TAILR_R = 8000;         // start value for 10kHz interrupts
  TIMER2_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// clear timer2A timeout flag
  TIMER2_CTL_R |= TIMER_CTL_TAEN;  // enable timer2A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer2A=priority 1
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // top 3 bits
  NVIC_EN0_R |= 1<<23;              // enable interrupt 23 in NVIC
	
	timer3A_Func = &songWorker;
	timer2A_Func = &soundWorker;
	currentNote = songLeft;
}

void togglePlay(void) {
	isPlay = !isPlay;
}

void rewind(void) {
	currentNote = songLeft;
}

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define WHEELSIZE 8           // must be an integer power of 2
                              //    red, yellow,    green, light blue, blue, purple,   white,          dark
const long COLORWHEEL[WHEELSIZE] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};

void UserTask(void){
  static int i = 0;
  LEDS = COLORWHEEL[i&(WHEELSIZE-1)];
  i = i + 1;
}

void songWorker(void) {
	//UserTask();
	if(!isPlay) {
		return;
	}
	if(currentNote->freq == 0) {
		isDacOut = 0;
	} else {
		isDacOut = 1;
		TIMER2_TAILR_R = currentNote->freq;
	}
	
	TIMER3_TAILR_R = 80000000/((BPM * currentNote->dur)) * 60;
	
	currentNote++;
}

void soundWorker(void) {
	UserTask();
	uint16_t res = Flute[waveIndex] * (instrumentVec & 0x01) + \
								 Horn[waveIndex] * (instrumentVec >> 1 & 0x01) + \
								 Wave[waveIndex] * (instrumentVec >> 2 & 0x01) + \
								 Trumpet[waveIndex] * (instrumentVec >> 3 & 0x01) + \
								 Guitar[waveIndex] * (instrumentVec >> 4 & 0x01);
	outData(res);
	waveIndex = (waveIndex+1)%64;
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
  (*timer2A_Func)();                // execute user task
}

void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
  (*timer3A_Func)();                // execute user task
}

