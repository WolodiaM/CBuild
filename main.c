#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "termios.h"
void getCursor(int* x, int* y) {
	struct termios orig = {0};
	tcgetattr(STDIN_FILENO, &orig);
	struct termios new = orig;
	new.c_lflag &= ~((unsigned int)ICANON | (unsigned int)ECHO);
	new.c_cc[VMIN] = 1;
	new.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &new);
	const char req[] = "\e[6n";
	write(STDOUT_FILENO, req, sizeof(req));
	char resp[9] = {0}; 
	if (read(STDIN_FILENO, resp, 8) < 0) abort();
	sscanf(resp, "\033[%d;%dR", x, y);
	tcsetattr(STDIN_FILENO, TCSANOW, &orig);
}
int main(void) {
	int t = printf("\t"); fflush(stdout);
	printf("|\n%d\n", t);
	return 0;
}
