all: tetris

WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2
NAME = TETRIS


tetris:
	$(CC) $(WARNINGS) $(DEBUG) $(OPTIMIZE) src/gui.c src/tetris.c src/tetris.h -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf -o $(NAME)


clean:
	rm -f TETRIS

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
run:
	./$(NAME)

