PROG=	ls
OBJS=	ls.o print.o sort.o util.o main.o preprocess.o
CFLAGS=	-Wall -Werror -Wextra -g

all: ${PROG}

depend:
	mkdep -- ${CFLAGS} *.c

${PROG}: ${OBJS}
	@echo $@ depends on $?
	${CC} -lm ${OBJS} -o ${PROG} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJS}
