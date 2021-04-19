#pragma GCC optimize("O3")
// Libraries needed: Adafruit_BusIO, Adafruit VEML 6075, Adafruit GFX, Adafruit SSD1306

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Fonts/Picopixel.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C

#define ROW_OFFSET 16
#define WIDTH 6
#define SIZE 1
#define FONT Picopixel

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) ;

void setup() {
	Serial.begin(9600) ;
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		while (true) {} ; // Don't proceed, loop forever
	}

	display.setTextSize(1) ;
	display.setTextWrap(false) ;
	display.setFont(&FONT) ;
	display.setTextColor(WHITE) ;
	Serial.setTimeout(10) ;
}

unsigned char getLen(char *str) {
	unsigned char lenCounter = 0 ;
	while	(str[lenCounter]) {
		++lenCounter ;
	}
	return lenCounter ;
}

void substr(char *str, char *buf, unsigned char start, unsigned char stop) {
	char ch[2] ;
	buf[0] = '\0' ;
	++stop ;
	unsigned char i ;

	for(i = start ; i < stop ; ++i) {
		ch[0] = str[i] ;
		ch[1] = '\0' ;
		strncat(buf, ch, 1) ;
	}

	buf[++i] = '\0' ;
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

char disp[12] ;
char recv[60] ;

float cpu_usage = 0 ;
float mem_usage = 0 ;
float mem_percent = 0 ;
float swap_usage = 0 ;
float swap_percent = 0 ;
float io_active = 0 ;
float net_upload = 0 ;
float net_download = 0 ;
float disk_total = 0 ;
float disk_used = 0 ;
float disk_used_percent ;

char *mem_unit = "B" ;
char *swap_unit = "B" ;
char *net_upload_unit = "B" ;
char *net_download_unit = "B" ;
char *disk_total_unit = "B" ;
char *disk_used_unit = "B" ;
float v ;

void loop() {
	display.clearDisplay() ;
	display.setCursor(0, 8) ;

	// cpu(01234) memUsed(999993) swapUsed(999992) io_active(0)
	// netUpload(999991) netDownload(999990)
	// disktotal(999990) diskused(999990)
	// memPercent(01234) swapPercent(01234) diskPercent(01234)
	
	if (Serial.available() > 0) {
		recv[0] = '\0' ;
		Serial.readBytes(recv, 57) ;
		recv[58] = '\0' ;

		if (getLen(recv) == 57)  {
			// CPU
			substr(recv, disp, 0, 2) ;
			cpu_usage = atoi(disp) ;
			substr(recv, disp, 3, 4) ;
			v = atoi(disp) ;
			cpu_usage += v / 100.0 ;
	
			// Mem
			substr(recv, disp, 5, 7) ;
			mem_usage = atoi(disp) ;
			substr(recv, disp, 8, 9) ;
			v = atoi(disp) ;
			mem_usage += v / 100.0 ;
			substr(recv, disp, 10, 10) ;
			mem_unit = getUnit(atoi(disp)) ;
	
			// Swap
			substr(recv, disp, 11, 13) ;
			swap_usage = atoi(disp) ;
			substr(recv, disp, 14, 15) ;
			v = atoi(disp) ;
			swap_usage += v / 100.0 ;
			substr(recv, disp, 16, 16) ;
			swap_unit = getUnit(atoi(disp)) ;
			
			// IO Read
			substr(recv, disp, 17, 17) ;
			io_active = atoi(disp) == 0 ? 0 : 1 ;
	
			// Net Upload
			substr(recv, disp, 18, 20) ;
			net_upload = atoi(disp) ;
			substr(recv, disp, 21, 22) ;
			v = atoi(disp) ;
			net_upload += v / 100.0 ;
			substr(recv, disp, 23, 23) ;
			net_upload_unit = getUnit(atoi(disp)) ;

			// Net Download
			substr(recv, disp, 24, 26) ;
			net_download = atoi(disp) ;
			substr(recv, disp, 27, 28) ;
			v = atoi(disp) ;
			net_download += v / 100.0 ;
			substr(recv, disp, 29, 29) ;
			net_download_unit = getUnit(atoi(disp)) ;

			// Disk total
			substr(recv, disp, 30, 32) ;
			disk_total = atoi(disp) ;
			substr(recv, disp, 33, 34) ;
			v = atoi(disp) ;
			disk_total += v / 100.0 ;
			substr(recv, disp, 35, 35) ;
			disk_total_unit = getUnit(atoi(disp)) ;

			// Disk Used
			substr(recv, disp, 36, 38) ;
			disk_used = atoi(disp) ;
			substr(recv, disp, 39, 40) ;
			v = atoi(disp) ;
			disk_used += v / 100.0 ;
			substr(recv, disp, 41, 41) ;
			disk_used_unit = getUnit(atoi(disp)) ;

			// Mem Percent
			substr(recv, disp, 42, 44) ;
			mem_percent = atoi(disp) ;
			substr(recv, disp, 45, 46) ;
			v = atoi(disp) ;
			mem_percent += v / 100.0 ;

			// Swap Percent
			substr(recv, disp, 47, 49) ;
			swap_percent = atoi(disp) ;
			substr(recv, disp, 50, 51) ;
			v = atoi(disp) ;
			swap_percent += v / 100.0 ;

			// Disk Used Percent
			substr(recv, disp, 52, 54) ;
			disk_used_percent = atoi(disp) ;
			substr(recv, disp, 55, 56) ;
			v = atoi(disp) ;
			disk_used_percent += v / 100.0 ;
		}
	}

	// CPU Usage
	dtostrf(cpu_usage, 5, 2, disp) ;
	sprintf(disp, "%s%%", disp) ;
	display.print("CPU:               ") ;
	display.println(disp) ;
	disp[0] = '\0' ;

	// Memory Usage
	dtostrf(mem_usage, 5, 2, disp) ;
	sprintf(disp, "%s %s", disp, mem_unit) ;
	display.print("MEM:             ") ;
	display.print(disp) ;
	disp[0] = '\0' ;
	dtostrf(mem_percent, 5, 2, disp) ;
	sprintf(disp, "%s %%", disp) ;
	display.print(" (") ;
	display.print(disp) ;
	display.println(")") ;
	disp[0] = '\0' ;

	// Swap Usage
	dtostrf(swap_usage, 5, 2, disp) ;
	sprintf(disp, "%s %s", disp, swap_unit) ;
	display.print("SWAP:            ") ;
	display.print(disp) ;
	disp[0] = '\0' ;
	dtostrf(swap_percent, 5, 2, disp) ;
	sprintf(disp, "%s %%", disp) ;
	display.print(" (") ;
	display.print(disp) ;
	display.println(")") ;
	disp[0] = '\0' ;

	// IO Usage
	sprintf(disp, io_active ? "0" : "1", disp) ;
	display.print("IO:                   ") ;
	display.println(disp) ;
	disp[0] = '\0' ;

	// Network Usage
	dtostrf(net_upload, 5, 2, disp) ;
	sprintf(disp, "%s %s/S", disp, net_upload_unit) ;
	display.print("Net UL:          ") ;
	display.println(disp) ;
	disp[0] = '\0' ;

	dtostrf(net_download, 5, 2, disp) ;
	sprintf(disp, "%s %s/S", disp, net_download_unit) ;
	display.print("Net DL:          ") ;
	display.println(disp) ;
	disp[0] = '\0' ;

	// Disk Total
	dtostrf(disk_total, 5, 2, disp) ;
	sprintf(disp, "%s %s", disp, disk_total_unit) ;
	display.print("Disk Tot:        ") ;
	display.println(disp) ;
	disp[0] = '\0' ;

	// Disk Used
	dtostrf(disk_used, 5, 2, disp) ;
	sprintf(disp, "%s %s", disp, disk_used_unit) ;
	display.print("Disk Used:     ") ;
	display.print(disp) ;
	disp[0] = '\0' ;
	dtostrf(disk_used_percent, 5, 2, disp) ;
	sprintf(disp, "%s %%", disp) ;
	display.print(" (") ;
	display.print(disp) ;
	display.println(")") ;
	disp[0] = '\0' ;

	display.display() ;
}
