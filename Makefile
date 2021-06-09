# NOTE: 3D renderer build

INCLUDE_PATH = D:/thirdparty/mingw32/include
LIB_PATH = D:/thirdparty/mingw32/lib

build:
	gcc -Wall -std=c99 src/*.c -o renderer -I$(INCLUDE_PATH) -L$(LIB_PATH) -lmingw32 -lSDL2main -lSDL2 -lm

run:
	renderer.exe

clean:
	del renderer.exe
