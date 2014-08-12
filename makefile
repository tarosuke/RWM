all: rwm

.PHONY : clean test


####################################################################### VALUES

COPTS += -Wall -Werror -IX11


libs = GL GLU GLEW m X11 Xmu Xi Xext Xcomposite Xdamage stdc++ gdbm pthread
LIBOPTS = $(addprefix -l, $(libs))


ssfc = .c .cc .glsl
spth = */* */*/*
srcs = $(patsubst old/%,,$(foreach p, $(spth), $(foreach s, $(ssfc), $(wildcard $(p)$(s)))))

dirs = $(sort $(dir $(srcs)))
mods = $(basename $(notdir $(srcs)))
dmds= $(addprefix objs/, $(mods))
objs = $(addsuffix .o, $(dmds))
deps = $(addsuffix .d, $(dmds))

rwmObjs = objs/rift_dk1.o



######################################################################## RULES

install: rwm
	sudo cp rwm /usr/local/bin/

test: rwm.test rwmSoundd
	./rwm.test

run: rwm
	./rwm

rwm: makefile rwm.cc $(rwmObjs) userLib.a rwmSoundd
	gcc -Xlinker "-Map=rwm.map" -o $@ rwm.cc $(rwmObjs) userLib.a $(LIBOPTS)

rwm.test: COPTS+=-DTEST
rwm.test: makefile rwm.cc $(rwmObjs) userLib.a
	gcc -ggdb -Xlinker "-Map=rwm.map" -o $@ rwm.cc $(rwmObjs) userLib.a $(LIBOPTS)

rwmSoundd: makefile rwmSoundd.cc userLib.a
	gcc -Xlinker "-Map=rwmSoundd.map" -o $@ rwmSoundd.cc userLib.a $(LIBOPTS)

clean:
	rm -fr rwm rwm.test rwmSoundd *.map userLib.a objs/*


userLib.a: $(objs)
	ar -rc $@ $(objs)



################################################################# COMMON RULES


-include $(deps)

vpath %.o objs
vpath % $(dirs)


objs/%.o : %.cc makefile
	$(CC) $(COPTS) $(CCOPTS) -c -o $@ $<

objs/%.o : %.c makefile
	${CC} $(COPTS) -c -o $@ $<

objs/%.o : %.glsl makefile
	objcopy -I binary -O elf64-x86-64 -B i386 $< $@

objs/%.d : %.cc
	@echo $@
	@echo -n objs/ > $@
	@$(CPP) $(COPTS) $(CCOPTS) -MM $< >> $@

objs/%.d : %.c
	@echo $@
	@echo -n objs/ > $@
	@$(CPP) $(COPTS) -MM $< >> $@


