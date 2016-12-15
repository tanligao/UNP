CC			= g++
CFLAGS			= -c -Wall -std=c++11
LDFLAGS			= -lpthread
SOURCES			= main.cpp ../Mutex.cpp ../Thread.cpp Threadpool.cpp
INCLUDES		= -I. -g
OBJECTS			= $(SOURCES:.cpp=.o)
TARGET			= threadpool

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)
