/**
либа для китайского семисегментника на 5 ног из повербанков
ноги подключать в пины дуины 3,4,5,6,7
*/
#include "PCF8575.h"
#define MICRO_DELAY 100
#define BG_PIN 13
#define SCAN_PIN 12
#define OUT_VCC_PIN 17
#define IN1_PIN 14
#define IN2_PIN 15
#define IN3_PIN 16
PCF8575 PCF(0x20);
unsigned long timers[3] = { 0,0,0 };
uint8_t symbol[16][2] = {
	// 76543210
	// 54321
	{0b01100000, 0b00100000},	// 1	34
	{0b01010000, 0b00010000},	// 2	24
	{0b00110000, 0b00010000},	// 3	23
	{0b00110000, 0b00100000},	// 4	32
	{0b01100000, 0b01000000},	// 5	43
	{0b01010000, 0b01000000},	// 6	42
	{0b10010000, 0b10000000},	// 7	52
	{0b10100000, 0b10000000},	// 8	53
	{0b11000000, 0b10000000},	// 9	54
	{0b00011000, 0b00001000},	// 10	12
	{0b00011000, 0b00010000},	// 11	21
	{0b00101000, 0b00001000},	// 12	13
	{0b00101000, 0b00100000},	// 13	31
	{0b01001000, 0b00001000},	// 14	14
	{0b01001000, 0b01000000},	// 15	41
	{0b10001000, 0b10000000}	// 16	51
};

uint8_t digits[25][2] = {
	// 00000000	   11111110
	// 87654321    65432109
	//				
	{0b00000000, 0b01111110},	//0
	{0b00000000, 0b00001100},	//1
	{0b00000000, 0b10110110},	//2
	{0b00000000, 0b10011110},	//3
	{0b00000000, 0b11001100},	//4
	{0b00000000, 0b11011010},	//5
	{0b00000000, 0b11111010},	//6
	{0b00000000, 0b00001110},	//7
	{0b00000000, 0b11111110},	//8
	{0b00000000, 0b11011110},	//9
	{0b11111100, 0b00000000},	//10
	{0b00011000, 0b00000000},	//11
	{0b01101100, 0b00000001},	//12
	{0b00111100, 0b00000001},	//13
	{0b10011000, 0b00000001},	//14
	{0b10110100, 0b00000001},	//15
	{0b11110100, 0b00000001},	//16
	{0b00011100, 0b00000000},	//17
	{0b11111100, 0b00000001},	//18
	{0b10111100, 0b00000001},	//19
	{0b00000011, 0b00000000},	//20
	{0b00000000, 0b10000001}, //21 --
	{0b00000000, 0b00000001}, //22 -x
	{0b00000000, 0b00000000}, //23 пустота
	{0b11100100, 0b10100001}  //24 Er
};

uint16_t array10[11] = { 0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023 };

/*
 * 0-199 - номера
 * 200-209 - "-"+номер
 * 210 - "--"
 * 211 - "Er"
 * 212 - пустота
 */
uint8_t number = 210;
uint8_t c = 0;
bool showBG = false;
bool showSCAN = false;
bool onPower = false;
void setup() {
	PCF.begin();
	PCF.write16(0xFFF);
	delay(300);
}

void loop() {
	while (1) {
		if (number < 212) {
			showNumber(number);
		} else {
			draw(23);
		}
		if (delayAsynk(1, 100)) {
			demo();
		}
	}
}

void showNumber(uint8_t number) {
	if (number == 211) {
		draw(24);
		return;
	}
	if (number == 210) {
		draw(21);
		return;
	}
	if (number > 199 && number < 210) {
		draw(22);
		number = number - 200;
	}
	if (number > 199) number = 199;
	if (number > 99) {
		draw(20);
		number = number - 100;
		if (number < 10) {
			draw(10);
		}
	}
	if (number > 9) {
		draw(number / 10 + 10);
		number = number % 10;
	}
	draw(number);
}

void draw(uint8_t n) {
	for (c = 0; c < 16; c++) {
		if ((c < 8 && (((digits[n][0]) >> c) & 0x01)) || (c >= 8 && ((digits[n][1]) >> (c - 8)) & 0x01)) {
			DDRD = symbol[c][0];
			PORTD = symbol[c][1];
			delayMicroseconds(MICRO_DELAY);
		}
	}
}

bool delayAsynk(unsigned int timerId, unsigned int maxTime) {
	if (millis() - timers[timerId] >= (long)maxTime) {
		timers[timerId] = millis();
		return true;
	}
	return false;
}

uint8_t demoState = 0;
uint8_t linePosition = 0;
uint8_t counter = 0;
uint8_t flash = 0;

void demo() {
	int led = 0x3FF;
	uint8_t tmp = 3;
	switch (demoState) {
	case 0: {
		showSCAN = true;
		showBG = false;
		if (delayAsynk(2, 5000)) {
			showSCAN = false;
			showBG = true;
			onPower = true;
			number = counter;
			demoState = 1;
			timers[0] = millis();
		}
	} break;
	case 1:
		//led = random(0, 0x3FF);
		if (random(0, 70) == 50) showBG = false;
		else showBG = true;
		onPower = true;
		led = array10[10 - linePosition];
		if (delayAsynk(0, 1600)) {
			linePosition++;
			if (linePosition == 11) {
				linePosition = 0;
				counter++;
				if (counter == 100) {
					counter--;
					demoState = 3;
				} else {
					//number = counter;
					//check();
					//if(!check()) {
					//	demoState = 100;
					//} else {
					timers[2] = millis();
					demoState = 2;
					//}
				}
			}
		}
		break;
	case 2:
		showSCAN = random(0, 5) > 2;
		if (delayAsynk(2, 2000)) {
			showSCAN = false;
			onPower = true;
			number = counter;
			demoState = 1;
			timers[0] = millis();
		}
		break;
	case 3:
		onPower = false;
		showBG = true;
		showSCAN = false;
		if (delayAsynk(2, 1000)) {
			showBG = false;
			onPower = true;
			demoState = 4;
			timers[0] = millis();
		}
		number = 210;
		break;
	case 4:
		flash++;
		number = flash < 4 ? 100 : 210;
		if (flash == 8)flash = 0;
		break;

	default:
		showSCAN = false;
		showBG = false;
		onPower = false;
		number = 211;
		break;
	}
	if (demoState == 3) {
		PCF.write16(0x3FF);
	} else if (demoState == 4) {
		PCF.write16(0xFFFF);
	} else {
		if (showSCAN) tmp &= ~(1 << SCAN_PIN - 12);
		if (showBG) tmp &= ~(1 << BG_PIN - 12);
		if (onPower) {
			tmp |= (1 << OUT_VCC_PIN - 12);
			tmp |= (1 << IN2_PIN - 12);
		}
		//number = tmp;
		int bg = tmp << 10;

		PCF.write16(led + bg);
	}
}

bool check() {
	delay(100);
	int data = PCF.read16() >> 12;
	number = (data & (1 << 1)) ? 0 : 1;
	return !(data & (1 << 1));
}