#pragma GCC optimize("Os")

#define ROW 2
#define COL 16
#define SPLITTER_BLINK_DELAY 500

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

RTC_DS1307 rtc;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2) ;

char daysOfTheWeek[7][4] = {
	"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
} ;

char Months[12][4] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
} ;

void center(char *str, char *buffer, char ch, unsigned char num) {
	unsigned char len = strlen(str), i = 0, j = 0 ;

	// Clear buffer
	buffer[0] = '\0' ;

	// Copy the string to buffer if they are same
	if (len >= num) for(;i < len ; ++i) buffer[i] = str[i] ;

	// Calculate the padding left and right
	unsigned char p_left = ceil(num / 2.0 - len / 2.0) ;
	unsigned char p_right = floor(num / 2.0 - len / 2.0) ;

	// Copy the left padded string to buffer
	for(; i < p_left ; ++i) buffer[i] = ch ;

	// Copy the original string to buffer
	for(j = 0 ; j < len ; ++j) buffer[i++] = str[j] ;

	// Copy the right padded string to buffer
	for(j = 0 ; j < p_right ; ++j) buffer[i++] = ch ;

	buffer[i] = '\0' ;
}

unsigned short getNDigits(unsigned long num, char places) {
	unsigned short n = num ;
	unsigned int max = pow(10, places) - 1 ;
	while(n > max) n /= 10 ;
	return n ;
}

void setup() {
	Serial.begin(57600) ;
	lcd.begin(COL, ROW) ;

	while(!rtc.begin()) {
		char *message = (char *)"No RTC Found!" ;
		lcd.setCursor(4, 0) ;
		lcd.print(message) ;
	}

	while(!rtc.isrunning()) {
		char *message = (char *)"RTC Not Running" ;
		lcd.setCursor(4, 0) ;
		lcd.print(message) ;
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__))) ;
	}
}

char str[100] ;
char str2[100] ;

char *splitter ;
unsigned char secondNow, lastSecond, splitterUnset ;
unsigned long blinkSplitterDelay = millis() + SPLITTER_BLINK_DELAY ;
unsigned long currT ;
unsigned char today ;
unsigned char hr = 0 ;
char *suffix ;
unsigned short jiffy = 0 ;
unsigned long currentMS = 0 ;
char *am_pm ;
unsigned char formatted_hr ;

char *getSuffix(unsigned char num) {
	if (num > 4 && num < 21) {
		return (char *)"th" ;
	} else {
		unsigned char mod10 = num % 10 ;	

		if (mod10 == 1) {
			return (char *)"st" ;
		} else if (mod10 == 2) {
			return (char *)"nd" ;
		} else if (mod10 == 3) {
			return (char *)"rd" ;
		} else return (char *)"th" ;
	}
}

int counter ;
unsigned long counterDelay = 0 ;

void loop() {
	if (millis() > counterDelay) {
		++counter ;
		counterDelay = millis() + 100 ;
	}
	
	DateTime now = rtc.now() ;
	currT = millis() ;

	secondNow = now.second() ;
	today = now.day() ;

	if (lastSecond != secondNow) {
		lastSecond = secondNow ;
		splitter = (char *)":" ;
		splitterUnset = 1 ;
		currentMS = millis() ;
		blinkSplitterDelay = currT + SPLITTER_BLINK_DELAY ;
	}	

	if (splitterUnset && currT > blinkSplitterDelay) {
		splitter = (char *)" " ;
		splitterUnset = 0 ;
	}

	// Print Date
	sprintf(str2, "%3s, %3s %d%s", daysOfTheWeek[now.dayOfTheWeek() - 1], Months[now.month() - 1], today, getSuffix(today)) ;
	center(str2, str, ' ', COL) ;	
	lcd.setCursor(0, 0) ;
	lcd.print(str) ;
	
	jiffy = getNDigits((unsigned short)millis() - currentMS, 2) ;
	if (jiffy > 999) jiffy = 99 ;

	// Print time
	hr = now.hour() ;

	am_pm = hr < 12 ? (char *)"AM" : (char *)"PM" ;

	formatted_hr = hr == 12 ? 12 : hr % 12 ;
	
	sprintf(str2, "%02d%s%02d%s%02d%s%02d %s", formatted_hr, splitter, now.minute(), splitter, secondNow, splitter, jiffy, am_pm) ;
	center(str2, str, ' ', COL) ;
	lcd.setCursor(0, 1) ;
	lcd.print(str) ;
}
