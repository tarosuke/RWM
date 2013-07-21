all: rwm

COPTS += -IX11 -I.

include make.in


libs= avatar/avatar.a room/room.a rift/rift.a image/image.a ghost/ghost.a toolbox/qon/qon.a toolbox/input/input.a toolbox/cyclic/cyclic.a -lGL -lGLU -lm -lX11 -lXmu -lXi -lXext -lXcomposite -lstdc++

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
	make -C room
	make -C rift
	make -C image
	make -C avatar
	make -C ghost
	make -C toolbox/qon
	make -C toolbox/input
	make -C toolbox/cyclic

