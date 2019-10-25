CCLANG=gcc
INFILE=maysh.c
FLAGS=g
OUTFILE=maysh

all:
	${CCLANG} ${INFILE} -${FLAGS} -o ${OUTFILE}
