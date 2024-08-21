.PHONY: start debug clean finish
# @$(CC) /LD $(CFLAGS_COMMON) $(CFLAGS_DEBUG) $(SRC_DIR)/win32.c \
#  	   /Fe:$(DEBUG_DIR)/win32.dll /Fo:$(DEBUG_DIR)/win32.obj
# @cd ../..

SRC_DIR := src
SOURCES := $(SRC_DIR)/main.c
BIN := viz
BIN_DIR := bin
EXE := $(BIN).exe
OBJ := $(BIN).obj
CFLAGS_COMMON := /nologo /Iinclude /FC /DPLT_WIN
LFLAGS_COMMON := /nologo /LIBPATH:E:\vendors\raylib-5.0_win64_msvc16\lib /LIBPATH:bin\debug \
				 /NODEFAULTLIB:libcmt gdi32.lib kernel32.lib msvcrt.lib opengl32.lib \
				 shell32.lib winmm.lib raylib.lib user32.lib win32.lib
CC := cl
CL := link

RELEASE_DIR := $(BIN_DIR)$/release

DEBUG_DIR := $(BIN_DIR)/debug
DEBUG_OBJ := $(DEBUG_DIR)/$(OBJ)
DEBUG_EXE := $(DEBUG_DIR)/$(EXE)
CFLAGS_DEBUG := /Od /Zo /Z7
LFLAGS_DEBUG := /DEBUG 

debug: start $(DEBUG_DIR) $(DEBUG_OBJ) $(DEBUG_EXE) finish

$(DEBUG_DIR):
	@mkdir -p $@

$(DEBUG_OBJ): $(SOURCES)
	@echo "[Debug] Building win32.lib..."
	@$(CC) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) /Fo:$(DEBUG_DIR)/win32.obj /c $(SRC_DIR)/win32.c
	@lib /nologo /OUT:$(DEBUG_DIR)/win32.lib $(DEBUG_DIR)/win32.obj
	@echo "[Debug] Compiling..."
	@$(CC) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) /Fo$@ /c $<

$(DEBUG_EXE): $(DEBUG_OBJ)
	@echo "[Debug] Linking..."
	@$(CL) $^ /OUT:$@ $(LFLAGS_COMMON) $(LFLAGS_DEBUG)

start:
finish: $(DEBUG_EXE)
	@echo "[Debug] Done."

clean: $(BIN_DIR)
	@rm -rf $^
