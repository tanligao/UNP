CC			= g++
CFLAGS			= -c -Wall -std=c++11
LDFLAGS			= -lpthread
SOURCES			= main.cpp Thread.cpp Mutex.cpp
INCLUDES		= -I.
OBJECTS			= $(SOURCES:.cpp=.o)
TARGET			= thread

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)
