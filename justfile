SRC := "src/main.c"
OUT_DESKTOP := "build/space-invaders"
OUT_WEB := "docs/space-invaders.html"

RAYLIB_WEB := "raylib/src"
RAYLIB_WEB_LIB := "{{RAYLIB_WEB}}/libraylib.web.a"
RAYLIB_WEB_SHELL := "{{RAYLIB_WEB}}/minshell.html"


desktop:
    cc {{SRC}} -o {{OUT_DESKTOP}} -I /opt/homebrew/include -L /opt/homebrew/lib -l raylib -lm -lpthread -ldl
    echo "Built desktop version {{OUT_DESKTOP}}"

web:
    emcc {{SRC}} \
        -o {{OUT_WEB}} \
        -s USE_GLFW=3 \
        -s ASYNCIFY \
        -s WASM=1 \
        -DPLATFORM_WEB \
        -I raylib/src \
        raylib/src/libraylib.web.a \
        --shell-file raylib/src/minshell.html
    echo "Built web version {{OUT_WEB}}"

serve:
    cd build/web && python3 -m http.server 8000

run:
	just build
	./build/space-invaders
