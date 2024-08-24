.PHONY: start debug clean finish
TARGET        := viz
BIN_DIR       := bin
SRC_DIR       := src
SOURCES       := $(SRC_DIR)/main.c
RELEASE_DIR   := $(BIN_DIR)$/release
DEBUG_DIR     := $(BIN_DIR)/debug
CC            :=
CL            :=
DEBUG_OBJ     :=
DEBUG_TARGET  :=
CFLAGS_DEBUG  :=
LFLAGS_DEBUG  :=
CFLAGS_COMMON :=
LFLAGS_COMMON :=

ifeq ($(OS),Windows_NT)
	PLATFORM = win32
else
	UNAME = $(shell uname -s)
	ifeq ($(UNAME),Linux)
		PLATFORM = linux
	else ifeq ($(UNAME),Darwin)
		PLATFORM = macos
	else
		$(error Platform not supported)
	endif
endif

# @$(CC) /LD $(CFLAGS_COMMON) $(CFLAGS_DEBUG) $(SRC_DIR)/win32.c \
#  	   /Fe:$(DEBUG_DIR)/win32.dll /Fo:$(DEBUG_DIR)/win32.obj
# @cd ../..

ifeq ($(PLATFORM),win32)
	CC            := cl
	CL            := link
	DEBUG_OBJ     := $(DEBUG_DIR)/$(TARGET).obj
    DEBUG_TARGET  := $(DEBUG_DIR)/$(TARGET).exe
	CFLAGS_DEBUG  := /Od /Zo /Z7
	LFLAGS_DEBUG  := /DEBUG 
	CFLAGS_COMMON := /nologo /Iinclude /FC /DPLT_WIN
	LFLAGS_COMMON := /nologo /LIBPATH:E:\vendors\raylib-5.0_win64_msvc16\lib /LIBPATH:bin\debug \
				     /NODEFAULTLIB:libcmt gdi32.lib kernel32.lib msvcrt.lib opengl32.lib \
				     shell32.lib winmm.lib raylib.lib user32.lib win32.lib
else ifeq ($(PLATFORM),linux)
	CC            := clang
	CL            := clang
	DEBUG_OBJ     := $(DEBUG_DIR)/$(TARGET).o
	DEBUG_TARGET  := $(DEBUG_DIR)/$(TARGET)
	CFLAGS_DEBUG  := -g3 -fdiagnostics-absolute-paths
	LFLAGS_DEBUG  := 
	CFLAGS_COMMON := -Iinclude -DPLT_LINUX -Wno-null-dereference
	LFLAGS_COMMON := -Llib -l:linux_libraylib.a -lm
else ifeq ($(PLATFORM),macos)
	$(error No. Ain't happening)
endif

debug: $(DEBUG_DIR) $(DEBUG_TARGET)

$(DEBUG_DIR):
	@mkdir -p $@

$(DEBUG_OBJ): $(SOURCES)
ifeq ($(PLATFORM),win32)
	@echo "[Debug] Building win32.lib..."
	@$(CC) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) /Fo:$(DEBUG_DIR)/win32.obj /c $(SRC_DIR)/win32.c
	@lib /nologo /OUT:$(DEBUG_DIR)/win32.lib $(DEBUG_DIR)/win32.obj
	@echo "[Debug] Compiling..."
	@$(CC) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) /Fo$@ /c $<
else ifeq ($(PLATFORM),linux)
	@echo "[Debug] Compiling..."
	@$(CC) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) -c $< -o $@
else
	$(error No.)
endif

$(DEBUG_TARGET): $(DEBUG_OBJ)
	@echo "[Debug] Linking..."
ifeq ($(PLATFORM),win32)
	@$(CL) $^ /OUT:$@ $(LFLAGS_COMMON) $(LFLAGS_DEBUG)
else ifeq ($(PLATFORM),linux)
	@$(CC) $^ -o $@ $(LFLAGS_COMMON) $(LFLAGS_DEBUG) 
else
	$(error We ain't doing that)
endif

start:
finish: $(DEBUG_EXE)
	@echo "[Debug] Done."

clean: $(BIN_DIR)
	@rm -rf $^
