all:
	g++ src/*.cpp -o bin/Splind -mwindows -Ilib -lfreeglut -lglu32 -lopengl32
