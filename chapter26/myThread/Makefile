CC			= g++
CFLAGS			= -c -Wall
LDFLAGS			= -lpthread
SOURCES			= main.cpp Thread.cpp
INCLUDES		= -I
OBJECTS			= $(SOURCES:.cpp=.o)
TARGET			= thread

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)
