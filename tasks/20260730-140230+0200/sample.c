	CBUILDDEF unsigned short __cbuild_flag_term_width(void) {
		#if defined(TIOCGWINSZ)
			cbuild_fd_t fd = cbuild_fd_open_read("/dev/tty");
			if (fd == CBUILD_INVALID_FD) return 80;
			struct winsize w;
			if (ioctl(fd, TIOCGWINSZ, &w) == -1) {
				cbuild_fd_close(fd);
				return 80;
			}
			cbuild_fd_close(fd);
			return w.ws_col;
		#else
			return 80;
		#endif // Extension check
	}
	void __cbuild_flag_term_get_cursor(int* x, int* y) {
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
		cbuild_assert(read(STDIN_FILENO, resp, 8) >= 0,
			"Can not read from stdin.\n");
		sscanf(resp, "\033[%d;%dR", x, y);
		tcsetattr(STDIN_FILENO, TCSANOW, &orig);
	}
