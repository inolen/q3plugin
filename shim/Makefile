all: libq3plugshim.so

%.o: %.c
	gcc -g -c -o"$(@F)" "$<" `pkg-config --cflags sdl`

%.o: ../lib/%.c
	gcc -g -c -o"$(@F)" "$<" `pkg-config --cflags sdl`

libq3plugshim.so: main.o hooks_sdl.o msgpipe.o
	gcc -shared -g $^ -o libq3plugshim.so -ldl `pkg-config --libs sdl,glib-2.0`
