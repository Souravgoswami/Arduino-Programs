#ifndef PARTICLES
#define PARTICLES 30
#endif

#ifndef PARTICLES_FALL
#define PARTICLES_FALL 5
#endif

// Paint the particles
unsigned char particles[PARTICLES][2] ;
unsigned char *particleFall[PARTICLES_FALL] ;
unsigned char *p ;

void initParticles() {
	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;
		p[0] = rand() & 127 ;
		p[1] = rand() & 63 ;
	}
}

void drawParticles() {
	for(unsigned char i = 0 ; i < PARTICLES_FALL ; ++i) {
		p = particleFall[i] ;

		if (p[1] & 63) {
			p[1] += 1 ;
		} else {
			p[0] = rand() & 127 ;
			p[1] = rand() & 63 ;
			particleFall[i] = particles[rand() % PARTICLES] ;
		}
	}

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;
		display.drawPixel(p[0], p[1], WHITE) ;
	}
}
