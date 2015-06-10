examples: libs example1 example2

example1:
	mpicc -o exercise1/heat exercise1/example/program.c\
				exercise1/example/problem.c\
				exercise1/example/jacobi.c\
				libs/bitmap.o\
				libs/visualize.o\
				libs/gettime.o

example2:
	mpicc -o exercise2/heat exercise2/example/program.c\
				libs/bitmap.o\
				libs/visualize.o\
				libs/gettime.o

libs: libs/bitmap.o libs/visualize.o libs/gettime.o

libs/bitmap.o: utilities/bitmap.c
	mpicc -c -o libs/bitmap.o utilities/bitmap.c

libs/visualize.o: utilities/visualize.c
	mpicc -c -o libs/visualize.o utilities/visualize.c

libs/gettime.o: utilities/gettime.c
	mpicc -c -o libs/gettime.o utilities/gettime.c

install_ffmpeg: utilities/ffmpeg
	cp -v utilities/ffmpeg ~/bin/ffmpeg
