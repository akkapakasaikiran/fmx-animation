CC = g++
RM = rm
OPENGLLIB = -lGL
GLEWLIB = -lGLEW
GLFWLIB = -lglfw
LIBS = $(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS = -L /usr/local/lib 
CPPFLAGS := -I ./include

RIDER = ./bin/a2-model-rider
BIKE = ./bin/a2-model-bike 
TRACK = ./bin/a2-model-track 
COMMON_SRCS = ./src/shader_util.cpp ./src/hnode.cpp ./src/camera.cpp 
RIDER_SRCS = ./src/rider_gl_framework.cpp ./src/rider.cpp
BIKE_SRCS = ./src/bike_gl_framework.cpp ./src/bike.cpp
TRACK_SRCS = ./src/track_gl_framework.cpp ./src/track.cpp

OUT_DIR = bin

.PHONY: clean all

all: $(OUT_DIR) $(RIDER) $(BIKE) $(TRACK)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(RIDER): $(COMMON_SRCS) $(RIDER_SRCS) 
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS) $(LIBS)

$(BIKE): $(COMMON_SRCS) $(BIKE_SRCS)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS) $(LIBS)

$(TRACK): $(COMMON_SRCS) $(TRACK_SRCS)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS) $(LIBS)

clean:
	$(RM) -rf *~ $(RIDER)
	$(RM) -rf *~ $(BIKE)
	$(RM) -rf *~ $(TRACK)
