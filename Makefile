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
LDFLAGS = -L. -L$(RAYLIB_LIB_PATH)

LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo

# Determine PLATFORM_OS in case PLATFORM_DESKTOP selected
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    # No uname.exe on MinGW!, but OS=Windows_NT on Windows!
    # ifeq ($(UNAME),Msys) -> Windows
    ifeq ($(OS),Windows_NT)
        PLATFORM_OS = WINDOWS
        export PATH := $(COMPILER_PATH):$(PATH)
        ifndef PLATFORM_SHELL
            PLATFORM_SHELL = cmd
        endif
    else
        UNAMEOS = $(shell uname)
        ifeq ($(UNAMEOS),Linux)
            PLATFORM_OS = LINUX
        endif
        ifeq ($(UNAMEOS),FreeBSD)
            PLATFORM_OS = BSD
        endif
        ifeq ($(UNAMEOS),OpenBSD)
            PLATFORM_OS = BSD
        endif
        ifeq ($(UNAMEOS),NetBSD)
            PLATFORM_OS = BSD
        endif
        ifeq ($(UNAMEOS),DragonFly)
            PLATFORM_OS = BSD
        endif
        ifeq ($(UNAMEOS),Darwin)
            PLATFORM_OS = OSX
        endif
        ifndef PLATFORM_SHELL
            PLATFORM_SHELL = sh
        endif
    endif
endif

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
    TARGET = web/invaders.js
    ifeq ($(BUILD_WEB_RESOURCES),TRUE)
        LDFLAGS += --preload-file $(BUILD_WEB_RESOURCES_PATH)
    endif
else
    CFLAGS += -O2 -std=c99
endif

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        # Libraries for Windows desktop compilation
        # NOTE: WinMM library required to set high-res timer resolution
        LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32 -lole32
        # Required for physac examples
        LDLIBS += -static -lpthread
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        # Libraries for Debian GNU/Linux desktop compiling
        # NOTE: Required packages: libegl1-mesa-dev
        LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt

        # On Wayland windowing system, additional libraries requires
        ifeq ($(USE_WAYLAND_DISPLAY),TRUE)
            LDLIBS += -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
        else
            # On X11 requires also below libraries
            LDLIBS += -lX11
            # NOTE: It seems additional libraries are not required any more, latest GLFW just dlopen them
            #LDLIBS += -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor
        endif
        # Explicit link to libc
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            LDLIBS += -lc
        endif
    endif
    ifeq ($(PLATFORM_OS),OSX)
        # Libraries for OSX 10.9 desktop compiling
        # NOTE: Required packages: libopenal-dev libegl1-mesa-dev
        LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
    endif
    ifeq ($(PLATFORM_OS),BSD)
        # Libraries for FreeBSD, OpenBSD, NetBSD, DragonFly desktop compiling
        # NOTE: Required packages: mesa-libs
        LDLIBS = -lraylib -lGL -lpthread -lm

        # On XWindow requires also below libraries
        LDLIBS += -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor
    endif
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
	$(CC) -c $< -o $@ $(CFLAGS) -Iinclude -D$(PLATFORM)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

