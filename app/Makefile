all:
	gcc -Iinclude/ src/main.c src/app.c src/input.c src/camera.c src/cube.c src/math3d.c src/particles.c src/render.c src/ui.c src/texture.c -Llib -lobj -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lm -o rubik_sanctuary.exe -Wall -Wextra -Wpedantic

linux:
	gcc -Iinclude/ src/main.c src/app.c src/input.c src/camera.c src/cube.c src/math3d.c src/particles.c src/render.c src/ui.c src/texture.c -Llib -lobj -lSDL2 -lSDL2_image -lGL -lm -o rubik_sanctuary -Wall -Wextra -Wpedantic