all: rwm

COPTS += -IX11 -I.

include make.in


libs= window/window.a view/view.a headtracker/headtracker.a toolbox/glpose/glpose.a extra/extra.a settings/settings.a image/image.a toolbox/input/input.a toolbox/cyclic/cyclic.a toolbox/complex/complex.a -lGL -lGLU -lGLEW -lm -lX11 -lXmu -lXi -lXext -lXcomposite -lXdamage -lstdc++ -lgdbm -lpthread


install: rwm
	sudo cp rwm /usr/local/bin/

test: rwm.test
	./rwm.test

run: rwm
	./rwm

rwm: makefile $(objs) extralibs
	gcc -o $@ $(objs) $(libs)

rwm.test: COPTS+=-DTEST
rwm.test: makefile $(objs) extralibs
	gcc -Xlinker "-Map=rwm.map" -o $@ $(objs) $(libs)


extralibs:
	@for f in *; do if [ -f $$f/makefile ]; then make -j -C $$f; fi; done
	make -C toolbox/glpose
	make -C toolbox/input
	make -C toolbox/cyclic
	make -C toolbox/complex

