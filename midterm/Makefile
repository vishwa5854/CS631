PROG=	ls
OBJS=	ls.o print.o sort.o util.o
CFLAGS=	-Wall -Werror -Wextra -g

all: ${PROG}

depend:
	mkdep -- ${CFLAGS} -lbsd *.c

${PROG}: ${OBJS}
	@echo $@ depends on $?
	${CC} ${OBJS} -o ${PROG} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJS}