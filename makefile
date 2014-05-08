.PHONY: toolboxes
all: rwm

COPTS += -IX11 -I.

include make.in


libs= window/window.a view/view.a headtracker/headtracker.a util/util.a toolbox/glpose/glpose.a settings/settings.a image/image.a toolbox/input/input.a toolbox/cyclic/cyclic.a toolbox/complex/complex.a -lGL -lGLU -lGLEW -lm -lX11 -lXmu -lXi -lXext -lXcomposite -lXdamage -lstdc++ -lgdbm -lpthread


subtargetDir = $(subst /makefile,,$(wildcard */makefile toolbox/*/makefile))
subtargets = $(foreach p,$(subtargetDir),$(subst /toolbox/,/,$(p)/$(p).a))



install: rwm
	sudo cp rwm /usr/local/bin/

test: rwm.test
	./rwm.test

run: rwm
	./rwm

rwm: makefile $(objs) $(subtargets)
	gcc -o $@ $(objs) $(libs)

rwm.test: COPTS+=-DTEST
rwm.test: makefile $(objs) $(subtargets)
	gcc -ggdb -Xlinker "-Map=rwm.map" -o $@ $(objs) $(libs)

vpath

vpath %.a $(subtargets)

%.a :
	make -C $(dir $@)

toolboxes:
	make -C toolbox/glpose
	make -C toolbox/input
	make -C toolbox/cyclic
	make -C toolbox/complex

t:
	@echo $(subtargets)

clean:
	rm -f rwm rwm.test *.a *.map *.o *.d
	for p in $(subtargetDir); do make -C $$p clean; done
