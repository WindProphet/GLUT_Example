build:
	gcc -framework GLUT -framework OpenGL glut.c -o glut_example

buildpp:
	g++ -framework GLUT -framework OpenGL glut.c -o glut_example


clean:
	rm ./glut_example

all: build

run: build
	./glut_example
runpp: buildpp
	./glut_example
