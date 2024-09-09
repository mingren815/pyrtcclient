BIN = mediainout
all:$(BIN)


mediainout:mediainout.cpp 
	g++ -o $(BIN) mediainout.cpp  -std=gnu++11 -DLINUX -L./ -lrtc_avd_so -lpthread -I./
clean:
	rm -rf *.o $(BIN)
