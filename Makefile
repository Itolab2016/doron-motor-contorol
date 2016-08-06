CXX ?= g++
NAVIO = ../../Navio
INCLUDES = -I ../..

all:
	$(CXX) -std=gnu++11 $(INCLUDES) dronemotor.cpp $(NAVIO)/PWM.cpp $(NAVIO)/Util.cpp -o dronemotor

clean:
	rm dronemotor
