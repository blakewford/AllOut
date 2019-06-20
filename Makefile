CC=g++
SRC=main.cpp
EXTRA=engine/engine.cpp -lSDL2 -DPROGMEM= -Dpgm_read_byte=*
GAME=allout
TARGET=atmega32u4

game: $(SRC)
	$(CC) -I/usr/include/SDL2 -gdwarf-4 -O3 -flto -std=c++11 $(SRC) $(EXTRA) -o $(GAME) -DPHASE2 -I. -I./engine -I./model -I./model/eigen -include port.h -Wno-narrowing -fpermissive -w -DNDEBUG -fdata-sections -ffunction-sections -Wl,--gc-sections

clean:
	-@rm $(GAME)
