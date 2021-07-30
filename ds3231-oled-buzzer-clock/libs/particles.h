#ifndef PARTICLES
#define PARTICLES 55
#endif

#ifndef PARTICLES_FALL
#define PARTICLES_FALL 10
#endif

// Paint the particles
unsigned char particles[PARTICLES][2] ;
unsigned char fallen = 0 ;
unsigned char *particleFall[PARTICLES_FALL] ;
unsigned char *p ;
const unsigned char particles_minus_1 = PARTICLES - 1 ;

void initParticles() {
	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;

		p[0] = rand() & 127 ;
		p[1] = rand() & 63 ;
	}

	for(unsigned char i = 0 ; i < PARTICLES_FALL ; ++i) {
		fallen = (fallen < PARTICLES) ? fallen + 1 : 0 ;
		particleFall[i] = particles[fallen] ;
	}
}

void drawParticles() {
	for(unsigned char i = 0 ; i < PARTICLES_FALL ; ++i) {
		p = particleFall[i] ;

		if (p[1] < 63) {
			p[1] += (p[0] & 1) + 1 ;
		} else {
			p[0] = rand() & 127 ;
			p[1] = rand() & 63 ;

			if (++fallen > particles_minus_1) fallen = 0 ;
			particleFall[i] = particles[fallen] ;
		}
	}

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;
		display.drawPixel(p[0], p[1], WHITE) ;
	}
}
