COMPILER=gcc
FLAGS=-Wall -fsanitize=address

all: appx childx viewx

appx: ./application/app.c ./application/app.h ./utils/utils.h ./utils/utils.c ./sharedMemoryInfoADT/sharedMemoryInfoADT.c ./sharedMemoryInfoADT/sharedMemoryInfoADT.h
	$(COMPILER) $^ $(FLAGS) -o $@

childx: ./child/child.c ./child/child.h ./utils/utils.h ./utils/utils.c
	$(COMPILER) $^ $(FLAGS) -o $@

viewx: ./view/view.c ./view/view.h ./utils/utils.h ./utils/utils.c ./sharedMemoryInfoADT/sharedMemoryInfoADT.c ./sharedMemoryInfoADT/sharedMemoryInfoADT.h
	$(COMPILER) $^ $(FLAGS) -o $@

clean:
	rm -f appx childx viewx shm_content.txt

.PHONY: all clean appx childx viewx