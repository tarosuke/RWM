all: rwm

COPTS += -IX11 -I.

include make.in


libs= avatar/avatar.a view/view.a ghost/ghost.a rift/rift.a world/world.a image/image.a toolbox/qon/qon.a toolbox/input/input.a toolbox/cyclic/cyclic.a -lGL -lGLU -lm -lX11 -lXmu -lXi -lXext -lXcomposite -lstdc++


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
	@for f in *; do if [ -f $$f/makefile ]; then make -j -C $$f; fi; done
	make -C toolbox/qon
	make -C toolbox/input
	make -C toolbox/cyclic

