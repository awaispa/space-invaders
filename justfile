build:
	cc src/main.c -o build/space-invaders -I /opt/homebrew/include -L /opt/homebrew/lib -l raylib -lm -lpthread -ldl
run:
	just build
	./build/space-invaders
