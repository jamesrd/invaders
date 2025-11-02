PLATFORM ?= PLATFORM_DESKTOP
CC = cc
BINDIR = bin
OBJDIR = obj
LIBDIR = lib
TARGET = $(BINDIR)/invaders
CFLAGS = -Wall -D_DEFAULT_SOURCE 
RAYLIB_LIB_PATH = lib
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
# PLATFORM_WEB: Default properties
BUILD_WEB_ASYNCIFY    ?= FALSE
BUILD_WEB_SHELL       ?= minshell.html
BUILD_WEB_HEAP_SIZE   ?= 128MB
BUILD_WEB_STACK_SIZE  ?= 1MB
BUILD_WEB_ASYNCIFY_STACK_SIZE ?= 1048576
BUILD_WEB_RESOURCES   ?= TRUE
BUILD_WEB_RESOURCES_PATH  ?= resources
# FRAMEWORKS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL 
# LIBS = $(LIBDIR)/libraylib.a
LDFLAGS = -L. -L$(RAYLIB_LIB_PATH)

LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo

ifeq ($(PLATFORM),PLATFORM_WEB)
    # HTML5 emscripten compiler
    # WARNING: To compile to HTML5, code must be redesigned
    # to use emscripten.h and emscripten_set_main_loop()
    CC = emcc
    CFLAGS += -std=gnu99
    LDFLAGS += -sUSE_GLFW=3 -sTOTAL_MEMORY=$(BUILD_WEB_HEAP_SIZE) -sSTACK_SIZE=$(BUILD_WEB_STACK_SIZE) -sFORCE_FILESYSTEM=1 -sMINIFY_HTML=0
    LDLIBS = $(RAYLIB_LIB_PATH)/libraylib.web.a
    # LDFLAGS += --shell-file $(BUILD_WEB_SHELL)
    EXT = .html
    TARGET = invaders.html
    ifeq ($(BUILD_WEB_RESOURCES),TRUE)
        LDFLAGS += --preload-file $(BUILD_WEB_RESOURCES_PATH)
    endif
else
    CFLAGS += -std=c99
endif


run: clean default
	./$(TARGET)

default: $(TARGET)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/*
	# rm *.data *.js *.wasm

# clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a my_app.c -o my_app
$(TARGET): $(OBJ) | $(BINDIR)
	$(CC) -o $@ $? $(CFLAGS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

$(OBJDIR)/%.o : src/%.c | $(OBJDIR)
	$(CC) -c $< -o $@ -Iinclude -D$(PLATFORM)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

