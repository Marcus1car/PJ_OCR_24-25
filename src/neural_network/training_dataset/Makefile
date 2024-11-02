all: 
	mkdir -p ./build/ ; gcc gen_image.c -g -fsanitize=address -lm -lSDL2 -lSDL2_image -lSDL2_ttf -o ./build/gen

.PHONY : clean
clean:
	rm -rf ./build/