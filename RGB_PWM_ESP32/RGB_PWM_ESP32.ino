#pragma GCC optimize("Os")

#define __SSID__ "Sourav's RGB Light! (SRGBL)"
#define __PASS__ "hexrgblight"

#define RED 13
#define GREEN 12
#define BLUE 14

#define R_FULL 40
#define G_FULL 100
#define B_FULL 100

#define FREQ 5000
#define RES 8

// Set these to your desiRED3 cRED3entials.
const char *ssid = __SSID__ ;
const char *password = __PASS__ ;

# include <WiFi.h> 
#include <WiFiClient.h>
#include <WiFiAP.h>

void hex2RGBPercent(unsigned int colour, unsigned char decimals[3]) {
	unsigned char r = colour >> 16 ;
	unsigned char g = 255 & (colour >> 8) ;
	unsigned char b = 255 & colour ;

	// Convert value to percentage for PWM
	decimals[0] = round((r * R_FULL) / 255.0) ;
	decimals[1] = round((g * G_FULL) / 255.0) ;
	decimals[2] = round((b * B_FULL) / 255.0) ;

	if(decimals[0] > 100) decimals[0] = 100 ;
	if(decimals[1] > 100) decimals[1] = 100 ;
	if(decimals[2] > 100) decimals[2] = 100 ;
}

WiFiServer server(80) ;

void setup() {
	ledcSetup(0, FREQ, RES) ;

	ledcAttachPin(RED, 0) ;
	ledcAttachPin(GREEN, 1) ;
	ledcAttachPin(BLUE, 2) ;

	// WiFi stuff
	WiFi.softAP(ssid, password) ;
	IPAddress myIP = WiFi.softAPIP() ; 
	WiFi.setTxPower(WIFI_POWER_2dBm) ;
	server.begin() ;
}

unsigned int colour = 0x00aaaa ;
unsigned char decimals[3] ;

char colourStr[7] ;

void loop() {
	hex2RGBPercent(colour, decimals) ;

	// WiFi stuff
	WiFiClient client = server.available() ; 

	if (client) {
		String currentLine = "" ;
	
		while (client.connected()) {
			if (client.available()) {
				char c = client.read() ;

				if (c == '\n') {
					if (currentLine.length() == 0) {
						client.println("HTTP/1.1 200 OK") ;
						client.println("Content-type:text/html") ;
						client.println() ;

						client.print(
							"<!Doctype HTML>"
							"<html>"

							"<head>"
								"<meta charset=\"utf-8\">"
								"<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
								"<title>RGB LED Controller</title>"
								"<style>"
								"</style>"
							"</head>"

							"<body>"
								"<form method=\"get\" action=\"\">"
									"<label style=\"\" for=\"colour\">LED Colour:&nbsp;</label>"
									"<input name=\"colour\" id=\"colour\" type=\"color\" value=\"#ffffff\">"
									"<br>"
									"<input type=\"submit\" value=\"Change!\">"
								"</form>"
							"</body>"

							"</html>"
						) ;
	
						client.println() ;
						break ;
					} else {
						if (currentLine.indexOf("GET") == 0) {
							char y[currentLine.length() + 1] ;
							strcpy(y, currentLine.c_str()) ;
							sscanf(y, (char *)"%*s /?colour=%%23%6s", colourStr) ;
							if(strlen(colourStr) == 6) {
								colour = strtoul(colourStr, NULL, 16) ;
								hex2RGBPercent(colour, decimals) ;
							}
						}
						
						currentLine = "" ;
					}
				} else if (c != '\r') {
					currentLine += c ; 
				}
			}
		}

		client.stop() ;
	}

	// Light stuff
	ledcWrite(0, decimals[0]) ;
	ledcWrite(1, decimals[1]) ;
	ledcWrite(2, decimals[2]) ;
}
