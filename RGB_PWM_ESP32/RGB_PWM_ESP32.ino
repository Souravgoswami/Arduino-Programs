#pragma GCC optimize("Os")

#define __SSID__ "Sourav's RGB Light! (SRGBL)"
#define __PASS__ "hexrgblight"

#define RED1 13
#define GREEN1 12
#define BLUE1 14

#define RED2 27
#define GREEN2 26
#define BLUE2 25

#define FREQ 5000
#define RES 8

// Set these to your desiRED3 cRED3entials.
const char *ssid = __SSID__;
const char *password = __PASS__;

# include <WiFi.h> 
#include <WiFiClient.h>
#include <WiFiAP.h>

WiFiServer server(80);

void setup() {
	ledcSetup(0, FREQ, RES);

	ledcAttachPin(RED1, 0);
	ledcAttachPin(GREEN1, 1);
	ledcAttachPin(BLUE1, 2);	

	ledcAttachPin(RED2, 3);
	ledcAttachPin(GREEN2, 4);
	ledcAttachPin(BLUE2, 5);	

	// WiFi stuff
	WiFi.softAP(ssid, password);
	IPAddress myIP = WiFi.softAPIP();
	server.begin();

	Serial.println("Server started");
}

int dutyCycle ;

unsigned int colour = 0x888888;
unsigned char r = 255;
unsigned char g = 255;
unsigned char b = 255;

unsigned char decimal_r = 100, decimal_g = 100, decimal_b = 100;

int cycleB = 0;
char colourStr[7] ;

const unsigned int stopAPTime = millis() + 10000;
unsigned char stopped = 0;

void loop() {
	// WiFi stuff
	WiFiClient client = server.available(); 

	if (client) {
		String currentLine = "";
	
		while (client.connected()) {
			Serial.println("Hi");
			if (client.available()) {
				char c = client.read();

				if (c == '\n') {
					if (currentLine.length() == 0) {
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();

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
						);
	
						client.println();
						break;
					} else {
						if (currentLine.indexOf("GET") == 0) {
							char y[currentLine.length() + 1];
							strcpy(y, currentLine.c_str());
							sscanf(y, (char *)"%*s /?colour=%%23%6s", colourStr);
							if(strlen(colourStr) == 6) {
								colour = strtoul(colourStr, NULL, 16);

								r = colour >> 16;
								g = 255 & (colour >> 8);
								b = 255 & colour;

								// Convert value to percentage for PWM
								decimal_r = round((r * 100.0) / 255.0) ;
								decimal_g = round((g * 100.0) / 255.0) ;
								decimal_b = round((b * 100.0) / 255.0) ;
							
								if(decimal_r > 100) decimal_r = 100;
								if(decimal_g > 100) decimal_g = 100;
								if(decimal_b > 100) decimal_b = 100;
							}
						}
						
						currentLine = "";
					}
				} else if (c != '\r') {
					currentLine += c;
				}
			}
		}

		client.stop();
	}

	if (!stopped && stopAPTime > millis()) {
		Serial.println("He");
		// WiFi.softAPdisconnect(true);
	}

	// Light stuff
	ledcWrite(0, decimal_r);
	ledcWrite(1, decimal_g);
	ledcWrite(2, decimal_b);

	ledcWrite(3, decimal_r);
	ledcWrite(4, decimal_g);
	ledcWrite(5, decimal_b);

	Serial.println("Hel");
}
