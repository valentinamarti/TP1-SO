COMPILER=gcc
FLAGS=-Wall

all: appx childx

appx: ./application/app.c ./application/app.h
	$(COMPILER) $^ $(FLAGS) -o $@

childx: ./child/child.c ./child/child.h
	$(COMPILER) $^ $(FLAGS) -o $@

clean:
	rm -f appx childx

.PHONY: all clean appx childx