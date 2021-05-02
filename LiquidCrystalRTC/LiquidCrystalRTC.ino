#pragma GCC optimize("Ofast")

#define ROW 2
#define COL 16
#define SPLITTER_BLINK_DELAY 250

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <string.h>
#include <math.h>

RTC_DS1307 rtc;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2) ;

void center(char *str, char *buffer, char ch, unsigned char num) {
	buffer[0] = '\0' ;

	unsigned char len = strlen(str), i, j ;

	// Copy the string to buffer if they are same
	if (len >= num) {
		for(i = 0 ; i < len ; ++i) buffer[i] = str[i] ;
		return ;
	}

	// Calculate the padding left and right
	unsigned char p_left = ceil(num / 2.0 - len / 2.0) ;
	unsigned char p_right = floor(num / 2.0 - len / 2.0) ;

	// Copy the left padded string to buffer
	for(i = 0 ; i < p_left ; ++i) buffer[i] = ch ;

	// Copy the original string to buffer
	for(j = 0 ; j < len ; ++j) buffer[i++] = str[j] ;

	// Copy the right padded string to buffer
	for(j = 0 ; j < p_right ; ++j) buffer[i++] = ch ;

	buffer[i] = '\0' ;
}

void setup() {
	Serial.begin(57600) ;
	lcd.begin(COL, ROW) ;
	lcd.setCursor(0, 0) ;

	while(!rtc.begin()) {
		char message[14] = "No RTC Found!", str[14] ;
		center(message, str, ' ', COL) ;
		lcd.print(str) ;
	}

	if(!rtc.isrunning()) {
		char message[17] = "RTC Not Running!", str[17] ;
		center(message, str, ' ', COL) ;
		lcd.print(str) ;

		rtc.adjust(DateTime(F(__DATE__), F(__TIME__))) ;
		while(millis() < 2500) ;
	}
}

char *getSuffix(unsigned char num) {
	if (num > 3 && num < 21) return (char *)"th" ;

	switch(num % 10) {
		case 1: return (char *)"st" ;
		case 2: return (char *)"nd" ;
		case 3: return (char *)"rd" ;
		default: return (char *)"th" ;
	}
}

const char daysOfTheWeek[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" } ;
const char Months[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" } ;

char splitter = ':' ;
char str[20], str2[20] ;
char *am_pm ;

unsigned char secondNow, lastSecond = 0 ;
unsigned char today, hr = 0, formatted_hr ;
unsigned short jiffy ;

unsigned long blinkSplitterDelay = millis() + SPLITTER_BLINK_DELAY ;
unsigned long currentMS = 0, currT ;

void loop() {
	DateTime now = rtc.now() ;

	hr = now.hour() ;
	secondNow = now.second() ;
	today = now.day() ;
	currT = millis() ;

	if (lastSecond != secondNow) {
		// Set the millisecond
		lastSecond = secondNow ;
		currentMS = currT ;

		// Reset the splitter
		splitter = ':' ;
		blinkSplitterDelay = currT + SPLITTER_BLINK_DELAY ;
	}

	// Blink the splitter when timeout is reached

	// Set the jiffy to avoid delays
	jiffy = (currT - currentMS) / 10 ;

	if (currT > blinkSplitterDelay + SPLITTER_BLINK_DELAY) {	
		splitter = ':' ;
		blinkSplitterDelay = currT + SPLITTER_BLINK_DELAY ;
	} else if (currT > blinkSplitterDelay) {
		splitter = ' ' ;
	}

	// Print Date
	sprintf(
		str2, "%s, %s %d%s",
		daysOfTheWeek[now.dayOfTheWeek()], Months[now.month() - 1], today, getSuffix(today)
	) ;

	center(str2, str, ' ', COL) ;	
	lcd.setCursor(0, 0) ;
	lcd.print(str) ;

	am_pm = hr < 12 ? (char *)"AM" : (char *)"PM" ;
	formatted_hr = hr == 12 ? 12 : hr % 12 ;

	// Print time
	sprintf(
		str2, "%02d%c%02d%c%02d%c%02d %s",
		formatted_hr, splitter, now.minute(), splitter, secondNow, splitter, jiffy, am_pm
	) ;
		
	center(str2, str, ' ', COL) ;
	lcd.setCursor(0, 1) ;
	lcd.print(str) ;
}
