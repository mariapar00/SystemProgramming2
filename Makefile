INCLUDE = include
MODULES = modules 
SRC = src 
CLASSES = classes

CC = g++

CPPFLAGS = -std=c++11 -Wall -I$(INCLUDE)

all: dataServer remoteClient

dataServer: dataServer.o communication.o worker.o execQueue.o mutexDict.o
	$(CC) -o $@ dataServer.o communication.o worker.o execQueue.o mutexDict.o -pthread

remoteClient: remoteClient.o folderCreation.o
	$(CC) -o $@ remoteClient.o folderCreation.o

dataServer.o: dataServer.cpp
	$(CC) $(CPPFLAGS) -c dataServer.cpp

remoteClient.o: remoteClient.cpp
	$(CC) $(CPPFLAGS) -c remoteClient.cpp

communication.o: modules/communication.cpp
	$(CC) $(CPPFLAGS) -c modules/communication.cpp

worker.o: modules/worker.cpp
	$(CC) $(CPPFLAGS) -c modules/worker.cpp
	
folderCreation.o: modules/folderCreation.cpp
	$(CC) $(CPPFLAGS) -c modules/folderCreation.cpp

execQueue.o: classes/execQueue.cpp
	$(CC) $(CPPFLAGS) -c classes/execQueue.cpp

mutexDict.o: classes/mutexDict.cpp
	$(CC) $(CPPFLAGS) -c classes/mutexDict.cpp 


clean:
	rm -f *.o dataServer remoteClient
	rm -r ClientsFileSystems/*
