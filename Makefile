COMPILER=gcc
FLAGS=-Wall

all: appx childx

appx: ./application/app.c ./application/app.h ./utils/utils.h ./utils/utils.c
	$(COMPILER) $^ $(FLAGS) -o $@

childx: ./child/child.c ./child/child.h ./utils/utils.h ./utils/utils.c
	$(COMPILER) $^ $(FLAGS) -o $@

clean:
	rm -f appx childx

.PHONY: all clean appx childx