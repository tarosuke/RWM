target = settings.a
COPTS += -I ..
include ../make.in


db.o : gdbm.h

gdbm.h : gdbm.h.patch
	cp /usr/include/gdbm.h .
	patch --no-backup-if-mismatch gdbm.h gdbm.h.patch
