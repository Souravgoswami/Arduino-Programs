#pragma GCC optimize("O3")

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define DISPLAYROTATION 0
#define TEXTWRAP false
#define BAUDRATE 115200
#define SPLITTER_DELAY 250
#define PARTICLES 55

#define BUZZER 11
#define BUZZ_DELAY 50

#define SMALL_FONT DMMono_Medium9pt7b
#define MEDIUM_FONT DMMono_Medium10pt7b
#define BIG_FONT DMMono_Medium11pt7b

#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "fonts/DMMono9.h"
#include "fonts/DMMono10.h"
#include "fonts/DMMono11.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4) ;
RTC_DS3231 rtc ;

#include "libs/particles.h"

const char SUN[] PROGMEM = "Sun" ;
const char MON[] PROGMEM = "Mon" ;
const char TUE[] PROGMEM = "Tue" ;
const char WED[] PROGMEM = "Wed" ;
const char THU[] PROGMEM = "Thu" ;
const char FRI[] PROGMEM = "Fri" ;
const char SAT[] PROGMEM = "Sat" ;

const char *const dayTable[] PROGMEM = {
	SUN, MON, TUE, WED, THU, FRI, SAT
} ;

const unsigned short buzzDelay4 = BUZZ_DELAY * 4 ;

unsigned char jiffies = 0 ;
unsigned long pastJiffies = 0 ;

char tempStr[16] ;

char splitter = ':' ;
unsigned long splitterChangeDelay = millis() + SPLITTER_DELAY ;
static unsigned long milliSec ;

static unsigned char hour, minute, second, pastSecond ;
unsigned char buzz = 0 ;
unsigned long buzzed = 0 ;
unsigned char lastHour = 0 ;
unsigned char doubleBuzz = 0 ;

static unsigned char weekDay ;
char dayOfWeek[4] ;

void setup () {
	if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		while(1) ;
	}

	display.setRotation(DISPLAYROTATION) ;
	display.setTextSize(1) ;
	display.setTextWrap(TEXTWRAP) ;
	display.setTextColor(SSD1306_WHITE) ;
	display.clearDisplay() ;
	display.setFont(&SMALL_FONT) ;

	if (!rtc.begin()) {
		display.setCursor(20, 32) ;
		display.print("No RTC!") ;
		display.display() ;
		
		while(1) ;
	}

	if (rtc.lostPower()) {
		display.setCursor(8, 32) ;
		display.print("Power Loss!") ;
		display.display() ;

		rtc.adjust(DateTime(F(__DATE__), F(__TIME__))) ;

		while(1) ;
	}

	initParticles() ;

	pinMode(BUZZER, OUTPUT) ;
	digitalWrite(BUZZER, LOW) ;

	DateTime now = rtc.now() ;
	lastHour = now.hour() ;
	weekDay = now.dayOfTheWeek();

	strcpy_P(
		dayOfWeek,
		(char *)pgm_read_word(&(dayTable[weekDay]))
	) ;
}

void loop () {
	display.clearDisplay() ;
	milliSec = millis() ;

	DateTime now = rtc.now() ;
	second = now.second() ;
	hour = now.hour() ;
	minute = now.minute() ;

	if (hour != lastHour) {
		buzz = 1 ;
		digitalWrite(BUZZER, HIGH) ;
		buzzed = milliSec + BUZZ_DELAY ;
		lastHour = hour ;
		
		weekDay = now.dayOfTheWeek() ;
		strcpy_P(
			dayOfWeek,
			(char *)pgm_read_word(&(dayTable[weekDay]))
		) ;
	}

	doubleBuzz = (hour == 0 || hour == 12) ? 1 : 0 ;

	if (doubleBuzz) {
		if (milliSec > buzzed + buzzDelay4) {
			digitalWrite(BUZZER, LOW) ;
		} else if (milliSec > buzzed + BUZZ_DELAY * 3) {
			digitalWrite(BUZZER, HIGH) ;
		} else if (milliSec > buzzed + BUZZ_DELAY * 2) {
			digitalWrite(BUZZER, LOW) ;
		} else if (milliSec > buzzed + BUZZ_DELAY) {
			digitalWrite(BUZZER, HIGH) ;
		} else if (milliSec > buzzed) {
			digitalWrite(BUZZER, LOW) ;
		}
	} else {
		if (milliSec > buzzed) {
			digitalWrite(BUZZER, LOW) ;
		}
	}

	if(second != pastSecond) {
		pastSecond = second ;
		pastJiffies = milliSec ;	

		splitterChangeDelay = milliSec + SPLITTER_DELAY ;
		splitter = ':' ;
	}

	jiffies = (milliSec - pastJiffies) / 10 ;

	if (milliSec > splitterChangeDelay + SPLITTER_DELAY) {
		splitter = ':' ;
		splitterChangeDelay = milliSec + SPLITTER_DELAY ;
	} else if(milliSec > splitterChangeDelay) {
		splitter = ' ' ;
	}

	drawParticles() ;

	// Show day of the week
	display.setFont(&BIG_FONT) ;
	display.setCursor(44, 14) ;
	display.print(dayOfWeek) ;

	// Show date
	sprintf(
		tempStr,
		"%02d/%02d/%02d",
		now.day(), now.month(), now.year()
	) ;
	
	display.setFont(&MEDIUM_FONT) ;
	display.setCursor(2, 34) ;
	display.print(tempStr) ;

	sprintf(
		tempStr,
		"%02d%c%02d%c%02d%c%02d",
		hour, splitter, minute, splitter, second, splitter, jiffies
	) ;

	display.setFont(&SMALL_FONT) ;
	display.setCursor(2, 54) ;
	display.print(tempStr) ;

	display.display() ;
}
