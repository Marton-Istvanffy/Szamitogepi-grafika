all:
	gcc -Iinclude/ src/camera.c src/cube.c src/main.c src/render.c -Llib -lglfw3dll -lopengl32 -lglu32 -lgdi32 -luser32 -lshell32 -lm -o rubik_sanctuary.exe -Wall -Wextra -Wpedantic

linux:
	gcc -Iinclude/ src/camera.c src/cube.c src/main.c src/render.c -lglfw -lGL -lGLU -lm -o rubik_sanctuary -Wall -Wextra -Wpedantic