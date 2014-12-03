INC = -I../include
LDLIBS = -lglut -lGL -lGLU -lc -lm -lGLEW

.cpp:
	g++ ../Common/InitShader.cpp $@.cpp $(LDLIBS) $(INC) -o $@.out
