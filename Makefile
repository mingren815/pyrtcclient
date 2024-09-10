BIN = mediainout
all:$(BIN)


mediainout:main.cpp 
	g++ -o $(BIN) main.cpp rtcclient.cpp -std=gnu++11 -DLINUX -L./ -lrtc_avd_so -lpthread -I./
	
clean:
	rm -rf *.o $(BIN)
