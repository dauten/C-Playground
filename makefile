all: window.c
	 gcc `pkg-config --cflags gtk+-3.0` -o out window.c `pkg-config --libs gtk+-3.0`


