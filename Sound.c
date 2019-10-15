#include "Sound.h"
#include <stdint.h>
#include "DAC.h"
#include "../inc/tm4c123gh6pm.h"

#define C2 9556   // 130.813 Hz
#define DF2 9019   // 138.591 Hz
#define D2 8513   // 146.832 Hz
#define EF2 8035   // 155.563 Hz
#define E2 7584   // 164.814 Hz
#define F2 7159   // 174.614 Hz
#define GF2 6757   // 184.997 Hz
#define G2 6378   // 195.998 Hz
#define AF2 6020   // 207.652 Hz
#define A2 5682   // 220.000 Hz
#define BF2 5363   // 233.082 Hz
#define B2 5062   // 246.942 Hz

#define C3 4778   // 261.626 Hz
#define DF3 4510   // 277.183 Hz
#define D3 4257   // 293.665 Hz
#define EF3 4018   // 311.127 Hz
#define E3 3792   // 329.628 Hz
#define F3 3579   // 349.228 Hz
#define GF3 3378   // 369.994 Hz
#define G3 3189   // 391.995 Hz
#define AF3 3010   // 415.305 Hz
#define A3 2841   // 440.000 Hz
#define BF3 2681   // 466.164 Hz
#define B3 2531   // 493.883 Hz

#define C4 2389   // 523.251 Hz
#define DF4 2255   // 554.365 Hz
#define D4 2128   // 587.330 Hz
#define EF4 2009   // 622.254 Hz
#define E4 1896   // 659.255 Hz
#define F4 1790   // 698.456 Hz
#define GF4 1689   // 739.989 Hz
#define G4 1594   // 783.991 Hz
#define AF4 1505   // 830.609 Hz
#define A4 1420   // 880.000 Hz
#define BF4 1341   // 932.328 Hz
#define B4 1265   // 987.767 Hz

#define C5 1194   // 1046.502 Hz
#define DF5 1127   // 1108.731 Hz
#define D5 1064   // 1174.659 Hz
#define EF5 1004   // 1244.508 Hz
#define E5 948   // 1318.510 Hz
#define F5 895   // 1396.913 Hz
#define GF5 845   // 1479.978 Hz
#define G5 797   // 1567.982 Hz
#define AF5 752   // 1661.219 Hz
#define A5 710   // 1760.000 Hz
#define BF5 670   // 1864.655 Hz
#define B5 633   // 1975.533 Hz

#define C6 597   // 2093.005 Hz

#define BPM 250
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
	{1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2},
	{1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, 
	{1, DF5, 1}, {1, D5, 1}, 				//Cd
	{1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2},
	{1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2},
	{1, DF5, 1}, {1, D5, 1}, 				//Cd
	{1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2},
	{1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2},
	{1, DF5, 1}, {1, D5, 1}, 				//Cd
	{1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, 
	{1, B4, 2}, {1, B4, 2}, {1, B4, 2}, {1, B4, 2}, 
	{1, DF5, 1}, {1, D5, 1}, 				//Cd
	{1, B4, 2}, 
	{1, DF5, 1}, {1, D5, 1}, 				//Cd
	{1, B3, 12}, {1, E4, 12}, {1, B4, 12}, {1, A4, 12}, {1, B3, 12},
	{1, E4, 12}, {1, B4, 12}, {1, C5, 12},
	{1, D5, 4}, 
	{1, D5, 1}, {1, DF5, 1}, {1, D5, 4},
	
	{1, C5, 1}, {1, D5, 1}, {1, E5, 2}, {1, D5, 2},
	{1,DF5,2}, 
	{1,B4,4}, {1,B4,1}, {1,A4,1}, {1,B4,4},{1,B4,1}, {1,A4,1}, {1,B4,4}, {1,A4,1}, {1,B4,1},
	{1,DF5,2}, {1,D5,2}, 
	{1,A4,2},
	//
	{1,B4,6},
	{1,D5,2}, {1,D5,2}, {1,D5,2}, {1,D5,4}, {1,DF5,1}, {1,D5,1}, {1,E5,2}, {1,D5,2}, {1,DF5,2}, 
  {1,B4,4},
	//
	{1,B4,1}, {1,A4,1}, {1,B4,4}, {1,B4,1}, {1,A4,1}, {1,B4,4}, {1, A4,1}, {1,B4,1}, 
	{1,DF5,2}, {1,D5,2}, 
	{1, A4, 2}, {1,B4,6},
	//
	{1,D4,6}, {1,DF4,6},
	{1,B3,12},
	//
	{1,A3,24},
	{1,D4,12},
	//
	{1,E4,12}, {1,GF4,6},
	//
	{1,DF4,6}, {1,D4,12}, {1,E4,6},
	//
	{1,GF4,6}, {1,G4,12}, {1,A4,12},
	//
	{1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2},{1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2},
	// this line I started to include every octave
	{1,DF4,1}, {1,D4,1},
	{1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2},{1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, 
	{1,DF4,1}, {1,D4,1},
	//
	{1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2},{1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, 
	{1,DF4,1}, {1,D4,1},
	{1,B4,2},
	//
	{1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,2}, {1,B4,10},
	{1,B3,2},
	{1,DF4,2}, {1,D4,8},
	//
	{1,D4,2}, {1,E4,2}, {1,GF4,8}, {1,GF4,2}, {1,A4,2}, {1,E4,8},
	//
	{1,D4,2}, {1,DF4,2},
	{1,B4,12},
	{1,DF4,2}, {1,D4,8},
	//
	{1,D4,2}, {1,E4,2}, {1,GF4,8}, {1,GF4,2}, {1,A4,2}, {1,B4,8}, {1,A4,2},
	//
	{1,DF5,2},
	{1,B4,10},
	{1,DF5,2}, {1,D5,4}, {1,DF5,4}, {1,B4,4},
	//
	{1,A4,4}, {1,G4,4}, {1,GF4,4}, {1,E4,8}, {1,D4,2}, {1,GF4,2}, {1,E4,8},
	//
	{1,D5,1}, {1,DF5,3}, {1,D5,4}, {1,D5,1}, {1,DF5,1}, {1,D5,4}, {1,DF5,1}, {1,D5,1}, {1,E5,2}, {1,D5,2},
	//
	{1,DF5,2},
	{1,B4,4}, {1,B4,1}, {1,A4,1}, {1,B4,4}, {1,B4,1}, {1,A4,1}, {1,B4,4}, {1,A4,1}, {1,B4,1},
	{1,DF5,2}, {1,D5,2},
	{1,A4,2},
	//
	//5|------d-d-d-d.....
	//4|b------------.....
	
};

static uint16_t sustain = 0x0FFF;
static uint8_t isPlay = 0;
static uint8_t isDacOut = 0;
static uint8_t waveIndex = 0;
static uint8_t instrumentVec = 0x01;
static uint8_t envIndex = 0;
static uint16_t envCnt = 0;
static uint8_t tempMul = 1;

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

const uint16_t envalope[16] = {
1,
1,
2,
4,
7,
12,
20,
33,
54,
90,
148,
244,
403,
665,
1096,
1808
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
  TIMER2_TAILR_R = C4;         // start value for 10kHz interrupts
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
	isDacOut = isPlay;
}

void rewind(void) {
	currentNote = songLeft;
}

void songWorker(void) {
	//UserTask();
	if(!isPlay) {
		isDacOut=0;
		return;
	}
	envIndex = 0;
	envCnt = 0;
	sustain = 0x3FF;
	if(currentNote->freq == 0) {
		isDacOut = 0;
	} else {
		isDacOut = 1;
		TIMER2_TAILR_R = currentNote->freq;
	}
	
	TIMER3_TAILR_R = 80000000/((BPM)) * currentNote->dur * 30 / tempMul;
	
	currentNote++;
	//outData(0);
}

void instUpd(void){
	instrumentVec++;
	if(instrumentVec > 0x1F) {
		instrumentVec = 0x01;
	}
}

void toggleTempo(void){
	if(tempMul == 2){
		tempMul = 1;
	} else {
		tempMul = 2;
	}
}

void soundWorker(void) {
	//UserTask();
	uint16_t res = Flute[waveIndex] * (instrumentVec & 0x01) + \
								 Horn[waveIndex] * (instrumentVec >> 1 & 0x01) + \
								 Wave[waveIndex] * (instrumentVec >> 2 & 0x01) + \
								 Trumpet[waveIndex] * (instrumentVec >> 3 & 0x01) + \
								 Guitar[waveIndex] * (instrumentVec >> 4 & 0x01);
	if(isDacOut){
		outData(res/envalope[envIndex]);
	} else {
		outData(0);
		return;
	}
	if(envCnt > sustain){
		envIndex++;
		envCnt = 0;
	} else {
		envCnt++;
	}
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

