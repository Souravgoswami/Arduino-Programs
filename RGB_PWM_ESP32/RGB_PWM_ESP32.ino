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
#include "libs/img_base64.h"

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
	// WiFi stuff
	Serial.begin(115200) ;
	WiFi.softAP(ssid, password) ;
	IPAddress myIP = WiFi.softAPIP() ; 
	WiFi.setTxPower(WIFI_POWER_2dBm) ;
	server.begin() ;

	// LED Stuff
	ledcSetup(0, FREQ, RES) ;

	ledcAttachPin(RED, 0) ;
	ledcAttachPin(GREEN, 1) ;
	ledcAttachPin(BLUE, 2) ;
}

unsigned int colour = 0x00aaaa ;
unsigned char decimals[3] ;

char colourStr[7] ;
String currentLine = "" ;
String content = "" ;
char c ;
String col = "#ffffff" ;

WiFiClient client ;

void loop() {
	hex2RGBPercent(colour, decimals) ;

	// WiFi stuff
	client = server.available() ; 

	if (client) {
		currentLine = "" ;
		content = "" ;
	
		while (client.connected()) {
			if (client.available()) {
				c = client.read() ;
				content += c ;

				if (c == '\n') {
					if (currentLine.length() == 0) {
						client.println("HTTP/1.1 200 OK") ;
						client.println("Content-type:text/html") ;
						client.println("Connection: close") ;
						client.println() ;

						col = (String)(strlen(colourStr) > 0 ? colourStr : "#ffffff") ;

						String x = (
							"<!Doctype HTML>"
							"<html>"

							"<head>"
								"<meta charset=\"utf-8\">"
								"<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
								"<title>RGB LED Controller</title>"

								"<style>"
									"body,html{padding:0;margin:0;height:100%}body{transition:.25s ease}"
									"#background{width:100%;height:100%;position:fixed;top:0;left:0;z-index:-1}"
 									"#background img{object-fit:cover;width:100%;height:100%;background-repeat:no-repeat;position:fixed;top:0;left:0;z-index:-1}"
									"main{display:flex;height:100vh;width:100vw;align-items:center;justify-content:center}"
									"h2{color:#fff}"
									".card{display:inline-block;border-radius:8px;background-color:#0009;backdrop-filter:blur(8px);box-shadow:4px 4px 10px #0004;padding:50px 50px;border:6px solid #" + col + "}"
									"input[type=submit]{background-color:transparent;border:1px solid #fff;color:#fff;padding:8px 12px;border-radius:6px;text-decoration:none;cursor:pointer;transition:.25s}"
									"input[type=submit]:hover{background-color:#fff;color:#000}.ta-center{display:block;text-align:center}.mb-1rem{margin-bottom:1rem}.w-80p{width:80%}"
									"body::before { background: url}"
								"</style>"
							"</head>"

							"<body>"
								"<div id=\"background\">"
									"<img src=\"" + bodyBG + "\">"
								"</div>"

								"<form method=\"get\" action=\"\">"
									"<main>"
										"<div id=\"card\" class=\"card\">"
											"<div class=\"ta-center\">"
												"<label for=\"colour\">"
												"" + bulbBG + ""
												"<h2>BULB COLOUR</h2>"
												"</label>"
											"</div>"
					
											"<div>"
												"<div class=\"ta-center\">"
													"<input class=\"mb-1rem w-80p\" name=\"colour\" id=\"colour\" type=\"color\" value=\"#" + col + "\">"
												"</div>"
					
												"<div class=\"ta-center\">"
													"<input type=\"submit\" id=\"colourSubmit\" value=\"Change!\">"
												"</div>"
											"</div>"
										"</div>"
									"</main>"
								"</form>"

								"<script>"
									"const colourInput = document.getElementById('colour');"
									"const card = document.getElementById('card');"
									"const bulbBG = Array.from(document.getElementsByClassName('bulb-background'));"
									
									"let v = colourInput.value || '#fffffff';"

									"card.style.borderColor = v;"
									"for(let i of bulbBG) { i.style.fill = v };"

									"colourInput.onchange = () => {"
										"v = colourInput.value || '#fffffff';"
										"card.style.borderColor = v;"
										"for(let i of bulbBG) { i.style.fill = v };"
									"};"
								"</script>"
							"</body>"

							"</html>"
						) ;

						client.println(x.c_str()) ;

						client.println() ;
						break ;
					} else {
						if (currentLine.indexOf("GET") == 0) {
							char y[currentLine.length() + 1] ;
							strcpy(y, currentLine.c_str()) ;
							unsigned char status = sscanf(y, (char *)"%*s /?colour=%%23%6s", colourStr) ;

							if(status == 1) {
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
