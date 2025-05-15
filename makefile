GLAD := -I src/lib/glad/include src/lib/glad/src/glad.c
SDL := -I src/lib/SDL/include -lSDL2main -lSDL2

NAME := gravsim

ifeq ($(OS),Windows_NT)
NAME := gravsim.exe
# adds windows tags to SDL
WIN_SDL := -lmingw32
# WIN_NET := -lwsock32 -lws2_32
endif

TAGS := $(GLAD) $(WIN_SDL) $(SDL)

default:
	g++ -o bin/$(NAME) src/*.cpp $(TAGS)
run:
	g++ -o bin/$(NAME) src/*.cpp $(TAGS)
	bin/$(NAME)