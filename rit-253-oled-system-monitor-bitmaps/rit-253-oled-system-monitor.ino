#pragma GCC optimize("Os")
// Libraries needed: Adafruit_BusIO, Adafruit VEML 6075, Adafruit GFX, Adafruit SSD1306

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C
#define DELAY 1000

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) ;

#include "Fonts/aldrich-regular-5.h"
#include "Fonts/aldrich-regular-6.h"
#include "libs/sysData.h"
#include "libs/arts.h"
#include "libs/drawBitmap.h"
#include "libs/substr.h"

#define FONT Aldrich_Regular5pt7b
#define FONTBIG Aldrich_Regular6pt7b

static struct sysData info ;

void setup() {
	Serial.begin(9600) ;

	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		while (1); // Don't proceed, loop forever
	}

	display.setTextSize(1) ;
	display.setTextWrap(true) ;
	display.setFont(&FONT) ;
	display.setTextColor(WHITE) ;
	display.clearDisplay() ;

	display.setCursor(0, 0) ;
	drawBitmap(memArt, 54, 28) ;
	display.display() ;

	Serial.setTimeout(5) ;
}

unsigned char getLen(char *str) {
	unsigned char lenCounter = 0 ;
	while (str[++lenCounter]) ;
	return lenCounter ;
}

char *getUnit(unsigned char unit) {
	if (unit == 0) return " B" ;
	else if (unit == 1) return "KB" ;
	else if (unit == 2) return "MB" ;
	else if (unit == 3) return "GB" ;
	else if (unit == 4) return "TB" ;
	else if (unit == 5) return "PB" ;
	else if (unit == 6) return "EB" ;
	else if (unit == 7) return "ZB" ;
	else return "YB" ;
}

char disp[10] ;
char recv[103] ;

float v ;
// unsigned char initialize = 0 ;;
unsigned long currTime ;
unsigned long delayTime = millis() + DELAY ;
unsigned char page = 0 ;

void loop() {
	display.clearDisplay() ;
	display.setCursor(0, 8) ;

	// cpu(01234) memUsed(999993) swapUsed(999992) io_active(0)
	// netUpload(999991) netDownload(999990)
	// disktotal(999990) diskused(999990) diskAvail(999990) totalProcess(12345678)
	// memPercent(01234) swapPercent(01234) diskPercent(01234) diskAvailPercent(01234)
	// psR(65536) psS(65536) psI(65536) psT(65536) psZ(65536)

	if (Serial.available() > 0) {
		recv[0] = '\0' ;
		Serial.readBytes(recv, 101) ;
		recv[102] = '\0' ;
	
		if (getLen(recv) != 101) return ;
		
		// CPU
		substr(recv, disp, 0, 2) ;
		info.cpu_usage = atoi(disp) ;
		substr(recv, disp, 3, 4) ;
		v = atoi(disp) ;
		info.cpu_usage += v / 100.0 ;
	
		// Mem
		substr(recv, disp, 5, 7) ;
		info.mem_usage = atoi(disp) ;
		substr(recv, disp, 8, 9) ;
		v = atoi(disp) ;
		info.mem_usage += v / 100.0 ;
		substr(recv, disp, 10, 10) ;
		info.mem_unit = getUnit(atoi(disp)) ;
	
		// Swap
		substr(recv, disp, 11, 13) ;
		info.swap_usage = atoi(disp) ;
		substr(recv, disp, 14, 15) ;
		v = atoi(disp) ;
		info.swap_usage += v / 100.0 ;
		substr(recv, disp, 16, 16) ;
		info.swap_unit = getUnit(atoi(disp)) ;
	
		// IO Read
		substr(recv, disp, 17, 17) ;
		info.io_active = atoi(disp) > 0 ? 1 : 0 ;

		// Net Upload
		substr(recv, disp, 18, 20) ;
		info.net_upload = atoi(disp) ;
		substr(recv, disp, 21, 22) ;
		v = atoi(disp) ;
		info.net_upload += v / 100.0 ;
		substr(recv, disp, 23, 23) ;
		info.net_upload_unit = getUnit(atoi(disp)) ;
	
		// Net Download
		substr(recv, disp, 24, 26) ;
		info.net_download = atoi(disp) ;
		substr(recv, disp, 27, 28) ;
		v = atoi(disp) ;
		info.net_download += v / 100.0 ;
		substr(recv, disp, 29, 29) ;
		info.net_download_unit = getUnit(atoi(disp)) ;
	
		// Disk total
		substr(recv, disp, 30, 32) ;
		info.disk_total = atoi(disp) ;
		substr(recv, disp, 33, 34) ;
		v = atoi(disp) ;
		info.disk_total += v / 100.0 ;
		substr(recv, disp, 35, 35) ;
		info.disk_total_unit = getUnit(atoi(disp)) ;
	
		// Disk Used
		substr(recv, disp, 36, 38) ;
		info.disk_used = atoi(disp) ;
		substr(recv, disp, 39, 40) ;
		v = atoi(disp) ;
		info.disk_used += v / 100.0 ;
		substr(recv, disp, 41, 41) ;
		info.disk_used_unit = getUnit(atoi(disp)) ;

		// Disk Available
		substr(recv, disp, 42, 44) ;
		info.disk_avail = atoi(disp) ;
		substr(recv, disp, 45, 46) ;
		v = atoi(disp) ;
		info.disk_avail += v / 100.0 ;
		substr(recv, disp, 47, 47) ;
		info.disk_avail_unit = getUnit(atoi(disp)) ;

		// Total Process 
		substr(recv, disp, 48, 55) ;
		info.ps_count = strtoul(disp, NULL, 10) ;
	
		// Mem Percent
		substr(recv, disp, 56, 58) ;
		info.mem_percent = atoi(disp) ;
		substr(recv, disp, 59, 60) ;
		v = atoi(disp) ;
		info.mem_percent += v / 100.0 ;

		// Swap Percent
		substr(recv, disp, 61, 63) ;
		info.swap_percent = atoi(disp) ;
		substr(recv, disp, 64, 65) ;
		v = atoi(disp) ;
		info.swap_percent += v / 100.0 ;

		// Disk Used Percent
		substr(recv, disp, 66, 68) ;
		info.disk_used_percent = atoi(disp) ;
		substr(recv, disp, 69, 70) ;
		v = atoi(disp) ;
		info.disk_used_percent += v / 100.0 ;

		// Disk Avail Percent
		substr(recv, disp, 71, 73) ;
		info.disk_used_percent = atoi(disp) ;
		substr(recv, disp, 74, 75) ;
		v = atoi(disp) ;
		info.disk_used_percent += v / 100.0 ;

		substr(recv, disp, 76, 80) ;
		info.process_running = (unsigned int)strtoul(disp, NULL, 10) ;
		Serial.println(disp) ;

		substr(recv, disp, 81, 85) ;
		info.process_sleeping = (unsigned int)strtoul(disp, NULL, 10) ;

		substr(recv, disp, 86, 90) ;
		info.process_idle = (unsigned int)strtoul(disp, NULL, 10) ;

		substr(recv, disp, 91, 95) ;
		info.process_stopped = (unsigned int)strtoul(disp, NULL, 10) ;

		substr(recv, disp, 96, 100) ;
		info.process_zombie = (unsigned int)strtoul(disp, NULL, 10) ;
	}

	// Display Usages
	currTime = millis() ;

	// CPU Usage
	if (currTime < delayTime) {
		dtostrf(info.cpu_usage, 5, 2, disp) ;
		sprintf(disp, "%s%%", disp) ;
		drawBitmap(cpuArt) ;
		display.setCursor(16, 8) ;
		display.setFont(&FONTBIG) ;
		display.println(F("CPU")) ;
		display.setFont(&FONT) ;
		display.println(disp) ;
		disp[0] = '\0' ;

		display.println(F("")) ;

		// Process Count
		drawBitmap(processArt, 0, 32) ;
		display.setFont(&FONTBIG) ;
		display.print(F("   PS (")) ;
		display.print(info.ps_count) ;
		display.println(F(")")) ;
		display.setFont(&FONT) ;

		// Process states
		sprintf(disp, "R: %u | ", info.process_running) ;
		display.print(disp) ;
		disp[0] = '\0' ;

		sprintf(disp, "S: %u | ", info.process_sleeping) ;
		display.print(disp) ;
		disp[0] = '\0' ;

		sprintf(disp, "I: %u", info.process_idle) ;
		display.println(disp) ;
		disp[0] = '\0' ;

		sprintf(disp, "St: %u | ", info.process_stopped) ;
		display.print(disp) ;
		disp[0] = '\0' ;

		sprintf(disp, "Z: %u", info.process_zombie) ;
		display.println(disp) ;
		disp[0] = '\0' ;

		display.println(F("")) ;
	} else if (currTime < delayTime + DELAY) {
		// Memory Usage
		drawBitmap(memArt) ;
		display.setCursor(20, 8) ;

		dtostrf((info.mem_usage), 5, 2, disp) ;
		sprintf(disp, "%s %s", disp, info.mem_unit) ;
		display.setFont(&FONTBIG) ;
		display.println(F("MEMORY")) ;
		display.setFont(&FONT) ;
		display.print(disp) ;
		disp[0] = '\0' ;
		dtostrf(info.mem_percent, 5, 2, disp) ;
		sprintf(disp, "%s %%", disp) ;
		display.print(F(" (")) ;
		display.print(disp) ;
		display.println(")") ;
		disp[0] = '\0' ;

		display.println(F("")) ;

		// Swap Usage
		drawBitmap(swapArt, 0, 30) ;
		dtostrf(info.swap_usage, 5, 2, disp) ;
		sprintf(disp, "%s %s", disp, info.swap_unit) ;
		display.setFont(&FONTBIG) ;
		display.println(F("     SWAP")) ;
		display.setFont(&FONT) ;
		display.print(disp) ;
		disp[0] = '\0' ;
		dtostrf(info.swap_percent, 5, 2, disp) ;
		sprintf(disp, "%s %%", disp) ;
		display.print(" (") ;
		display.print(disp) ;
		display.println(")") ;
		disp[0] = '\0' ;

		display.println(F("")) ;
	} else if (currTime < delayTime + DELAY * 2) {
		// IO Usage
		drawBitmap(ioArt) ;
		display.setCursor(18, 8) ;

		if(info.io_active == 0) sprintf(disp, "Idle", disp) ;
		if(info.io_active == 1) sprintf(disp, "Busy", disp) ;	
		display.setFont(&FONTBIG) ;
		display.println(F("IO")) ;
		display.setFont(&FONT) ;
		display.println(disp) ;
		disp[0] = '\0' ;

		display.println(F("")) ;
		
		// Network Usage
		dtostrf(info.net_upload, 5, 2, disp) ;
		sprintf(disp, "%s %s/S", disp, info.net_upload_unit) ;
		drawBitmap(netArt, 0, 34) ;
		display.setFont(&FONTBIG) ;
		display.println(F("    NET")) ;
		display.setFont(&FONT) ;
		display.print(F("Up: ")) ;
		display.println(disp) ;
		disp[0] = '\0' ;
		dtostrf(info.net_download, 5, 2, disp) ;
		sprintf(disp, "%s %s/S", disp, info.net_download_unit) ;
		display.print(F("Dl: ")) ;
		display.println(disp) ;
		disp[0] = '\0' ;

		display.println(F("")) ;
	} else if (currTime < delayTime + DELAY * 3) {
		// Disk
		drawBitmap(diskArt) ;
		display.setCursor(16, 9) ;
		display.setFont(&FONTBIG) ;
		display.println(F("DISK")) ;
		display.setFont(&FONT) ;
		dtostrf(info.disk_total, 5, 2, disp) ;
		sprintf(disp, "%s %s", disp, info.disk_total_unit) ;
		display.print(F("Tot: ")) ;
		display.print(disp) ;
		disp[0] = '\0' ;
		display.println(F("")) ;

		dtostrf(info.disk_used, 5, 2, disp) ;
		sprintf(disp, "%s %s", disp, info.disk_used_unit) ;
		display.print(F("Use: ")) ;
		display.print(disp) ;
		disp[0] = '\0' ;

		// Disk Used Percent
		dtostrf(info.disk_used_percent, 5, 2, disp) ;
		sprintf(disp, "%s %%", disp) ;
		display.print(F(" (")) ;
		display.print(disp) ;
		display.println(F(")")) ;
		disp[0] = '\0' ;

		dtostrf(info.disk_avail, 5, 2, disp) ;
		sprintf(disp, "%s %s", disp, info.disk_avail_unit) ;
		display.print(F("Avl: ")) ;
		display.print(disp) ;
		disp[0] = '\0' ;
	} else {
		// drawBitmap() ;

		delayTime = millis() + DELAY ;
	}
	
	display.display() ;
}
