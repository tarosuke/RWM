all: rwm

COPTS += -IX11 -Ilibovr_nsb -Igl-matrix -Iroom -Irift

include make.in


libs= libovr_nsb/libovr_nsb.a gl-matrix/libgl-matrix.a room/room.a -lGL -lGLU -lm -lX11 -lXmu -lXi -lXext -lXcomposite -lstdc++

test: rwm.test
	./rwm.test

run: rwm
	./rwm

rwm: makefile $(objs) extralibs
	gcc -o $@ $(objs) $(libs)

rwm.test: COPTS+=-DTEST
rwm.test: makefile $(objs) extralibs
	gcc -o $@ $(objs) $(libs)


extralibs:
	make -C gl-matrix
	make -C libovr_nsb
	make -C room
