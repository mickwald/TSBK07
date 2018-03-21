# set this variable to the director in which you saved the common files
commondir = ../common/

all : Project-Main.out


Project-Main.out : Project-Main.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o Project-Main.out -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES Project-Main.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm


clean :
	rm Project-Main.out
