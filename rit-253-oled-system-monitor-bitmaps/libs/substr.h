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
