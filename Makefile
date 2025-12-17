# These are found by running pkg-config, you better have it on your system
CFLAGS = `pkg-config --cflags gtk4`
LIBS = `pkg-config --libs gtk4`

SRC = ./src
VER = 0.1
EXE = WRText-v$(VER)

all: # Compiles the release version of the application
	gcc $(CFLAGS) -o $(EXE) src/*.c $(LIBS) -DVER=\"$(VER)\"
debug: # Compiles the debug version of the application
	gcc $(CFLAGS) -o $(EXE) src/*.c $(LIBS) -DVER=\"$(VER)\" -DDEBUG -g -Wall
test:
	gcc $(CFLAGS) -o $(EXE) test.c $(LIBS) -DDEBUG -g -Wall

auto-style:
	clang-format -i src/*.c -style=file
check-style:
	clang-format -i src/*.c -style=file --dry-run
update-docs:
	doxygen Doxyfile

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=vg.log --verbose ./WRText-v0.1


clean:
	rm -f ./$(EXE)