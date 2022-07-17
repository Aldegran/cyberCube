#include "PCF8575.h"
#include <avr/sleep.h>
#include <EEPROM.h>
#define HIDE_ALL() (PORTD = 0)

#define UP 11
#define DOWN 12
#define SENSOR 13
#define LEFT 3
#define RIGHT A2

#define POS_UP 1
#define POS_DOWN 2

#define USE_ALIEN true

byte glypf[35] = {
	// 76543210
		B11110110, //0
		B00100010, //1
		B10110101, //2
		B10100111, //3
		B01100011, //4
		B11000111, //5
		B11010111, //6
		B10100010, //7
		B11110111, //8
		B11100111, //9
		B00000001, //- 10
		B00010101, //c 11 
		B01010111, //b 12
		B00000000, //  13
		B00000000, //. 14
		B11110001, //P 15
		B11010101, //E 16
		B00010011, //n 17
		B01000000, //|lt 18
		B10000000, //_t 19
		B00100000, //|rt 20
		B00000010, //|rb 21
		B00000100, //_b 22
		B00010000, //|lb 23
		B11110011, //0 24
		B01000011, //1 25
		B10010111, //2 26
		B11110000, //3 27
		B11000110, //4 28
		B00110101, //5 29
		B10110001, //6 30
		B01010001, //7 31
		B11010110, //8 32
		B10100101, //9 33
		B10000101  //___ 34
};

byte secret[4] = { 1,2,3,4 };
byte user[4] = { 0,0,0,0 };
byte show[4] = { 13,13,13,13 };
byte flash[4] = { 0,0,0,0 };
byte cows[2] = { 0,0 };
unsigned long timers[] = { 0,0,0,0,0 };
bool flashNow = false;
byte delayNop = 0;
byte state = 2;
byte animate = 0;
byte currentFlash = 0;
bool newPressed1 = false;
bool newPressed2 = false;
bool moved = false;
byte position = 1;
byte hacked = 1;
bool USE_SERIAL = false;

PCF8575 PCF(0x20);

void setup() {
	position = EEPROM.read(10);
	hacked = EEPROM.read(11);
	secret[0] = EEPROM.read(12);
	secret[1] = EEPROM.read(13);
	secret[2] = EEPROM.read(14);
	secret[3] = EEPROM.read(15);
	if (position != POS_UP && position != POS_DOWN)updatePosition(1);
	if (hacked != 1 && hacked != 2)setHacked(false);
	DDRD = B11110111;
	DDRB = B00001111;
	PORTD = B00001000;
	PORTB = B00000000;
	PORTC = B00000000;

	pinMode(A0, OUTPUT);
	digitalWrite(A0, HIGH);
	pinMode(RIGHT, INPUT);
	digitalWrite(RIGHT, HIGH);
	pinMode(LEFT, INPUT);
	digitalWrite(LEFT, HIGH);
	checkHacked();
	if (digitalRead(UP) && !digitalRead(DOWN)) USE_SERIAL = true;
	pinMode(UP, OUTPUT);
	pinMode(DOWN, OUTPUT);
	digitalWrite(UP, LOW);
	digitalWrite(DOWN, LOW);
	pinMode(SENSOR, INPUT_PULLUP);
	if (USE_SERIAL) {
		Serial.begin(9600);
		Serial.print("---Start---\r\nPos: ");
		Serial.println(position == 1 ? "UP" : "DOWN");
	}
	Wire.begin();
	byte error = 1;
	while (error) {
		demo();
		Wire.beginTransmission(0x20);
		error = Wire.endTransmission();
		if (USE_SERIAL) {
			Serial.println(error);
		}
	}
	PCF.begin();
	PCF.write16(0x0000);
	delay(300);
	PCF.write16(0xFFFF);
	ADCSRA = 0;
	//setupNewSecret();
	if (position == POS_UP && checkSensor()) SERVO_DOWN();
	else if (position == POS_DOWN && !checkSensor()) SERVO_UP();
	clearSleepTimer();
	cls();
	showLoop();
	while (1) {
		loop();
	}
}

void checkHacked() {
	pinMode(UP, INPUT);
	pinMode(DOWN, INPUT);
	if (digitalRead(UP) && digitalRead(DOWN)) {
		setHacked(hacked == 1 ? true : false);
		cls();
		show[0] = hacked;
		show[1] = hacked;
		show[2] = hacked;
		show[3] = hacked;
		flash[0] = 1;
		flash[1] = 1;
		flash[2] = 1;
		flash[3] = 1;
		while (1) {
			showLoop();
			if (delayAsynk(0, 250))flashNow = !flashNow;
		}
	}
}

void setHacked(bool h) {
	hacked = h ? 2 : 1;
	EEPROM.write(11, hacked);
}

void updatePosition(byte newPosition) {
	position = newPosition;
	EEPROM.write(10, position);
}

bool checkSensor() {
	bool s = !digitalRead(SENSOR);
	if (USE_SERIAL) {
		Serial.println(s ? "Sensor OK" : "Sensor FAIL");
	}
	return s;
}

void SERVO_UP() {
	if (position == POS_UP || moved) return;
	if (USE_SERIAL) {
		Serial.println("UP");
	}
	digitalWrite(UP, HIGH);
	digitalWrite(DOWN, LOW);
	timers[3] = millis();
	updatePosition(POS_UP);
	moved = true;
}

void SERVO_DOWN() {
	if (position == POS_DOWN || moved) return;
	if (USE_SERIAL) {
		Serial.println("DOWN");
	}
	digitalWrite(UP, LOW);
	digitalWrite(DOWN, HIGH);
	timers[3] = millis();
	updatePosition(POS_DOWN);
	moved = true;
}

void stop() {
	if (USE_SERIAL) {
		Serial.println("STOP");
	}
	moved = false;
	digitalWrite(UP, LOW);
	digitalWrite(DOWN, LOW);
	PCF.write16(0xFFFF);
}

void SHOW_S1() { digitalWrite(8, HIGH); }
void HIDE_S1() { digitalWrite(8, LOW); }
void SHOW_S2() { digitalWrite(9, HIGH); }
void HIDE_S2() { digitalWrite(9, LOW); }
void SHOW_S3() { digitalWrite(A1, HIGH); }
void HIDE_S3() { digitalWrite(A1, LOW); }
void SHOW_S4() { digitalWrite(10, HIGH); }
void HIDE_S4() { digitalWrite(10, LOW); }

byte linePos = 0;
byte pos = 0;
void loop() {
	if (USE_SERIAL) {
		while (Serial.available()) {
			byte command = Serial.read();
			switch (command) {
			case 'u': SERVO_UP(); break;
			case 'd': SERVO_DOWN(); break;
			case 'h': setHacked(true); break;
			case 'r': setHacked(false); break;
			case 'c':
				delay(10);
				secret[0] = Serial.read() - 48;
				secret[1] = Serial.read() - 48;
				secret[2] = Serial.read() - 48;
				secret[3] = Serial.read() - 48;
				EEPROM.write(12, secret[0]);
				EEPROM.write(13, secret[1]);
				EEPROM.write(14, secret[2]);
				EEPROM.write(15, secret[3]);
				Serial.print("New code: ");
				Serial.print(secret[0]);
				Serial.print(secret[1]);
				Serial.print(secret[2]);
				Serial.println(secret[3]);
				break;
			default:
				break;
			}
		}
	}
	if (state == 1) {
		calculate();
	} else if (state == 10) {
		clearSleepTimer();
		if (delayAsynk(1, 5000)) {
			cls();
			showLoop();
			//sevroBack();
			state = 2;
		}
	} else {
		if (!digitalRead(LEFT)) {
			clearSleepTimer();
			if (!newPressed1)newPressed1Action();
		} else {
			newPressed1 = false;
		}
		if (!digitalRead(RIGHT)) {
			clearSleepTimer();
			if (!newPressed2)newPressed2Action();
		} else {
			newPressed2 = false;
		}
	}
	showLoop();
	if (delayAsynk(0, 250))flashNow = !flashNow;
	if (delayAsynk(2, 120000))sleepAction();
	if (moved && delayAsynk(3, 5000))stop();
	if (moved && delayAsynk(4, 300))moveLines();
}


bool delayAsynk(unsigned int timerId, unsigned int maxTime) {
	if (millis() - timers[timerId] >= (long)maxTime) {
		timers[timerId] = millis();
		return true;
	}
	return false;
}

void moveLines() {
	PCF.write16(0xFFFF);
	if (moved) {
		PCF.write(linePos, 0);
		if (position == POS_UP) {
			if (linePos == 3) linePos = 0;
			else linePos++;
		}
		if (position == POS_DOWN) {
			if (linePos == 0) linePos = 3;
			else linePos--;
		}
	}
}

byte demoList[20][2] = {
	{3,10},
	{2,10},
	{1,10},
	{0,10},
	{0,18},
	{0,19},
	{1,19},
	{2,19},
	{3,19},
	{3,20},
	{3,21},
	{3,22},
	{2,22},
	{1,22},
	{0,22},
	{0,23},
	{0,10},
	{1,10},
	{2,10},
	{3,10},
};

void demo() {
	byte dd = 100;
	for (byte i = 0;i < 20;i++) {
		cls();
		show[demoList[i][0]] = demoList[i][1];
		showLoop();
		delay(dd);
	}
}

void showLoop() {
	byte add = USE_ALIEN ? 24 : 0;
	HIDE_S4();
	SHOW_S1();
	if (!flash[0] || flashNow)PORTD = glypf[show[0] + (show[0] < 10 ? add : 0)];
	else HIDE_ALL();
	myDelay();
	HIDE_S1();
	SHOW_S2();
	if (!flash[1] || flashNow)PORTD = glypf[show[1] + (show[1] < 10 ? add : 0)];
	else HIDE_ALL();
	myDelay();
	HIDE_S2();
	SHOW_S3();
	if (!flash[2] || flashNow)PORTD = glypf[show[2] + (show[2] < 10 ? add : 0)];
	else HIDE_ALL();
	myDelay();
	HIDE_S3();
	SHOW_S4();
	if (!flash[3] || flashNow)PORTD = glypf[show[3] + (show[3] < 10 ? add : 0)];
	else HIDE_ALL();
	myDelay();
}
void myDelay() {
	while (delayNop++)__asm__("nop");
}
void sevroBack() {
	SERVO_UP();
	delay(100);
	//servoLock.write(10);        
	//delay(1000);
	SERVO_DOWN();
}

void stopFlash() {
	flash[0] = 0;
	flash[1] = 0;
	flash[2] = 0;
	flash[3] = 0;
}


void cls() {
	show[0] = 13;
	show[1] = 13;
	show[2] = 13;
	show[3] = 13;
}

void clearAll() {
	show[0] = 10;
	show[1] = 10;
	show[2] = 10;
	show[3] = 10;
	user[0] = 0;
	user[1] = 0;
	user[2] = 0;
	user[3] = 0;
	cows[0] = 0;
	cows[1] = 0;
	stopFlash();
	flash[0] = 1;
	animate = 0;
	currentFlash = 0;
}

void setupNewSecret() {
	secret[0] = 0;
	secret[1] = 0;
	secret[2] = 0;
	secret[3] = 0;
	randomSeed(millis());
	for (byte i = 0;i < 4;i++) {
		//secret[i]=i+1;
		bool f = false;
		byte m = 0;
		while (!f) {
			m = byte(random(1, 10));
			f = true;
			if (m == secret[0] || m == secret[1] || m == secret[2] || m == secret[3])f = false;
		}
		secret[i] = m;
	}
}

void newPressed1Action() {
	newPressed1 = true;
	if (state != 0) {
		return;
	}
	PCF.write(currentFlash, 0);
	increaseShow(currentFlash);
}

void newPressed2Action() {
	newPressed2 = true;
	if (state == 2) {
		clearAll();
		state = 0;
		return;
	}
	if (show[currentFlash] == 10) return;
	stopFlash();
	currentFlash++;
	if (currentFlash > 3) {
		user[0] = show[0];
		user[1] = show[1];
		user[2] = show[2];
		user[3] = show[3];
		timers[1] = millis();
		state = 1;
		currentFlash = 0;
		PCF.write16(0xFFFF);
		return;
	}
	flash[currentFlash] = 1;
}

void increaseShow(byte i) {
	bool r = false;
	byte n = show[i];
	while (!r) {
		n++;
		if (n > 9)n = 1;
		if (show[0] != n && show[1] != n && show[2] != n && show[3] != n) r = true;
	}
	show[i] = n;
}

void calculate() {
	cls();
	if (!animate) {
		show[0] = 10;
	} else if (animate == 1) {
		show[1] = 10;
	} else if (animate == 2) {
		show[2] = 10;
	} else if (animate == 3) {
		show[3] = 10;
	} else {
		calculateCows();
		if (cows[1] == 4) {
			if (USE_SERIAL) {
				Serial.println("Open");
			}
			state = 10;
			cls();
			showLoop();
			SERVO_UP();
			timers[1] = millis();
			flash[0] = 1;
			flash[1] = 1;
			flash[2] = 1;
			flash[3] = 1;
			show[0] = 34;
			show[1] = 34;
			show[2] = 34;
			show[3] = 34;
			//setupNewSecret();
			return;
		}
		//show[0] = 11;
		//show[1] = cows[0];
		//show[2] = 12;
		//show[3] = cows[1];
		if (hacked) {
			cls();
			showLoop();
			PCF.write16(0xFFFF);
			for (byte i = 0; i < cows[0]; i++) {
				PCF.write(i, 0);
				delay(300);
			}
			delay(2000);
			PCF.write16(0xFFFF);
			delay(1000);
			for (byte i = 0; i < cows[1]; i++) {
				PCF.write(3 - i, 0);
				delay(300);
			}
			delay(3000);
			PCF.write16(0xFFFF);
		}
		state = 2;
	}
	showLoop();
	if (delayAsynk(1, 500)) animate++;
}

void calculateCows() {
	for (byte i = 0; i < 4; i++) {
		if (user[i] == secret[i])cows[1]++;
		else if (secret[0] == user[i] || secret[1] == user[i] || secret[2] == user[i] || secret[3] == user[i]) cows[0]++;
	}
}
void activate() {
	if (USE_SERIAL) {
		Serial.println("W");
	}
	clearSleepTimer();
	sleep_disable();
}
void clearSleepTimer() {
	timers[2] = millis();
}

//******************************
#define BOOT_SECTION_ADDR 0x00 //адресс начала загрузчика исходя из его размера для 328 меги (256 words - 0x3f00; 512 words - 0x3e00; 1024 words - 0x3c00; 2048  words - 0x3800;)
void (*BootLoader)(void);
void(GoTo_BOOT)(void) {
	BootLoader = (void(*)())BOOT_SECTION_ADDR;
	BootLoader();
}
void sleepAction() {
	if (USE_SERIAL) {
		Serial.println("S");
	}
	cls();
	showLoop();
	PCF.write16(0xFFFF);
	attachInterrupt(1, activate, FALLING); //назначение прерывания на USER_1 (D3), для ф-и обработки кнопок, на изм. с low на high
	interrupts();               //включение прерываний
	delay(10);
	digitalWrite(A0, LOW);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sei();
	sleep_mode();
	cli();
	clearAll();
	state = 0;

	GoTo_BOOT();/// ибо не хочет по нормальному на 644й
}
//******************************
