include ../Make.defines

util:	
		${CC} ${CFLAGS} -c util.c  

cli:	cli.o request.o
		${CC} ${CFLAGS} -o $@ cli.o request.o ${LIBS}

mcli:	mcli.o util.o
		${CC} ${CFLAGS} -o $@ mcli.o request.o util.o ${LIBS}

serv:	serv.o sigchldwaitpid.o request.o
		${CC} ${CFLAGS} -o $@ serv.o sigchldwaitpid.o request.o util.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
