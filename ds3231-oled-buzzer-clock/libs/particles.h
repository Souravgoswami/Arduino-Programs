#ifndef PARTICLES
#define PARTICLES 30
#endif

// Paint the particles
unsigned char particles[PARTICLES][4] ;

void initParticles() {
	static unsigned char *p ;

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;

		p[0] = rand() & 127 ;
		p[1] = rand() & 63 ;
		p[2] = p[0] & 1 ? 1 : -1 ;
		p[3] = p[1] & 1 ? 1 : -1 ;
	}
}

void drawParticles() {
	static unsigned char *p ;

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;

		if ((p[0] & 127) && (p[1] & 63)) {
			p[0] += p[2] ;
			p[1] += p[3] ;
		} else {
			p[0] = rand() & 127 ;
			p[1] = rand() & 63 ;
			p[2] = p[0] & 1 ? 1 : -1 ;
			p[3] = p[1] & 1 ? 1 : -1 ;
		}

		display.drawPixel(p[0], p[1], WHITE) ;
	}
}
