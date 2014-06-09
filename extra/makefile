# extra

.PHONY: clean all

subtargets = $(subst /makefile,,$(wildcard */makefile))

all :
	for p in $(subtargets); do make -j -C $$p || exit -1; done


clean:
	rm -f *.a
	for p in $(subtargets); do make -j -C $$p clean || exit -1; done

