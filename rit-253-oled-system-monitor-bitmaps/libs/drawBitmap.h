#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) ;

void drawBitmap(char *art, int col_offset = 0, int row_offset = 0) {
	char ch[2] ;

	unsigned char len = strlen_P(art) ;
	unsigned char repeat = 0 ;
	unsigned char row = row_offset, col = col_offset ;

	for(byte i = 0 ; i < len ; ++i) {
		ch[0] = pgm_read_byte_near(art + i) ;
		ch[1] = '\0' ;
		repeat = atoi(ch) ;


		if (repeat > 0) {
			++i ;
		} else {
			repeat = 1 ;
		}

		ch[0] = pgm_read_byte_near(art + i) ;
		ch[1] = '\0' ;

		for(unsigned char r = 0 ; r < repeat ; ++r) {
			if (ch[0] == '`') {
				display.drawPixel(col, row, SSD1306_WHITE) ;
				col++ ;
			} else if (ch[0] == ' ') {
				col++ ;
			} else if (ch[0] == 'x') {
				row++ ;
				col = col_offset ;
			}
		}
	}
}
