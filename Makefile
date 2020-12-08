CC = g++
RM = rm
OPENGLLIB = -lGL
GLEWLIB = -lGLEW
GLFWLIB = -lglfw
LIBS = $(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS = -L /usr/local/lib 
CPPFLAGS := -I ./include

TRACK = ./fmx-animate 
COMMON_SRCS = ./src/shader_util.cpp ./src/hnode.cpp ./src/camera.cpp 
RIDER_SRCS = ./src/rider_gl_framework.cpp ./src/rider.cpp
BIKE_SRCS = ./src/bike_gl_framework.cpp ./src/bike.cpp
TRACK_SRCS = ./src/track_gl_framework.cpp ./src/track.cpp

OUT_DIR = bin

.PHONY: clean all

all: $(TRACK)

$(TRACK): $(COMMON_SRCS) $(TRACK_SRCS)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS) $(LIBS)

clean:
	$(RM) -rf *~ $(TRACK)
