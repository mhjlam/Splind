all:
	g++ src/*.cpp -o bin/wavy -mwindows -Ilib -lfreeglut -lglu32 -lopengl32
