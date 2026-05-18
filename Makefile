build:
	gcc -Wall -std=c99 -O2 ./src/*.c -lSDL2 -lm -o renderer

run:
	./renderer

clean:
	rm renderer
