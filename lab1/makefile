# set this variable to the director in which you saved the common files
commondir = ../common/

all : lab1-1.out lab1-2.out lab1-3.out lab1-4.out lab1-5.out lab1-6.out

lab1-1.out : lab1-1.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab1-1.out -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab1-1.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm

lab1-2.out : lab1-2.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab1-2.out -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab1-2.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm

lab1-3.out : lab1-3.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab1-3.out -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab1-3.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm

lab1-4.out : lab1-4.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab1-4.out -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab1-4.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm

lab1-5.out : lab1-5.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab1-5.out -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab1-5.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm

lab1-6.out : lab1-6.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab1-6.out -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab1-6.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm

clean :
	rm lab1-1.out
	rm lab1-2.out
	rm lab1-3.out
	rm lab1-4.out
	rm lab1-5.out
	rm lab1-6.out
