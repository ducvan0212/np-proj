include ../Make.defines

cli:	cli.o request.o
		${CC} ${CFLAGS} -o $@ cli.o request.o ${LIBS}

mcli:	mcli.o
		${CC} ${CFLAGS} -o $@ mcli.o request.o ${LIBS}

serv:	serv.o sigchldwaitpid.o request.o
		${CC} ${CFLAGS} -o $@ serv.o sigchldwaitpid.o request.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}