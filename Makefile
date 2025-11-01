BINDIR = bin
OBJDIR = obj
LIBDIR = lib
TARGET = $(BINDIR)/invaders
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
FRAMEWORKS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL 
LIBS = $(LIBDIR)/libraylib.a

run: default
	./$(TARGET)

default: $(TARGET)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/*

# clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a my_app.c -o my_app
$(TARGET): $(OBJ) | $(BINDIR)
	clang $(FRAMEWORKS) $(LIBS) -o $@ $?

$(OBJDIR)/%.o : src/%.c | $(OBJDIR)
	cc -c $< -o $@ -Iinclude

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

