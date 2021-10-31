SOURCE_FILES ?= src/main.c
RAYLIB_PATH ?= :C/raylib/raylib
CFLAGS ?= -Os -std=c99 -Wall -Isrc/include -Lsrc/lib -DPLATFORM_DESKTOP
LDFLAGS ?= -lraylib -lopengl32 -lgdi32 -lwinmm
#Fix this makefile in the future

default:
	gcc -static $(SOURCE_FILES) -o game.exe $(CFLAGS) $(LDFLAGS)