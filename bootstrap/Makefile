.PHONY: q3plugshim

all: bootstrap q3plugshim

%.o: %.c
	gcc -g -c -o"$@" "$<"  `pkg-config --cflags gtk+-2.0`

bootstrap: main.o msgpipe.o
	gcc -g $^ -o bootstrap `pkg-config --libs gtk+-2.0,glib-2.0`

q3plugshim:
	$(MAKE) -C q3plugshim