CC=gcc
LD=ld
CFLAG=-D_KERNEL -m64 -mcmodel=kernel -mno-red-zone -ffreestanding -nodefaultlibs
LDFLAG=-r

revid: code.o
	${LD} ${LDFLAG} code.o -o revid 

code.o: code.c 
	${CC} ${CFLAG} -c code.c

