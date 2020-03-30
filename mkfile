</$objtype/mkfile

all:V:		$O.haltd $O.batteryd


$O.haltd:	haltd.$O
		$LD $LDFLAGS -o $O.haltd haltd.$O
haltd.$O:	haltd.c
		$CC $CFLAGS haltd.c

$O.batteryd:	batteryd.$O
		$LD $LDFLAGS -o $O.batteryd batteryd.$O
batteryd.$O:	batteryd.c
		$CC $CFLAGS batteryd.c


clean:
	rm -f [5678kq].haltd haltd.[5678kq] [5678kq].batteryd batteryd.[5678kq]
	
install:
	cp $O.haltd /$objtype/bin/haltd
	cp $O.batteryd /$objtype/bin/batteryd
	touch /sys/log/haltd

