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
TRACK_SRCS = ./src/track_gl_framework.cpp ./src/track.cpp

.PHONY: clean all

all: $(TRACK)

$(TRACK): $(COMMON_SRCS) $(TRACK_SRCS)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS) $(LIBS)

clean:
	$(RM) -rf *~ $(TRACK)
