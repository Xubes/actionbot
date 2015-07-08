CC=g++


all: actionbot

actionbot: RoboteqDevice.o actionbot.o
	$(CC) -o $@ $^

%.o: %.cpp
	$(CC) -c $<

clean:
	rm *.o
