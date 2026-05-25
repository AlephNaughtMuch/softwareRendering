build:
	gcc -Wall -std=c99 -O2 -D_POSIX_C_SOURCE=199309L ./src/*.c -lSDL2 -lm -o renderer
run:
	./renderer

clean:
	rm renderer
