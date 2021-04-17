all: libsbmemlib.a  app create_memory_sb destroy_memory_sb

libsbmemlib.a:  ../untitled/sbmemlib.c
	gcc -Wall -c sbmemlib.c
	ar -cvq libsbmemlib.a sbmemlib.o
	ranlib libsbmemlib.a

app: ../untitled/app.c
	gcc -Wall -o app app.c -L. -lsbmemlib

create_memory_sb: ../untitled/create_memory_sb.c
	gcc -Wall -o create_memory_sb create_memory_sb.c -L. -lsbmemlib

destroy_memory_sb: ../untitled/destroy_memory_sb.c
	gcc -Wall -o destroy_memory_sb destroy_memory_sb.c -L. -lsbmemlib

clean: 
	rm -fr *.o *.a *~ a.out  app sbmemlib.o sbmemlib.a libsbmemlib.a  create_memory_sb destroy_memory_sb
