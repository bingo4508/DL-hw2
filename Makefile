# Makefile for empty SVM-struct API, 03.10.06

#Call 'make' using the following line to make CYGWIN produce stand-alone Windows executables
#		make 'SFLAGS=-mno-cygwin'

#Use the following to compile under unix or cygwin
CC = g++
LD = g++

CFLAGS =   $(SFLAGS) -O3 -fomit-frame-pointer -ffast-math -Wall -std=c99 -fopenmp 
LDFLAGS =  $(SFLAGS) -O3 -lm -Wall -fopenmp -std=c99
#CFLAGS =  $(SFLAGS) -pg -Wall
#LDFLAGS = $(SFLAGS) -pg -lm -Wall 

all: svm_empty_learn svm_empty_classify

.PHONY: clean
clean: svm_light_clean svm_struct_clean
	rm -f *.o *.tcov *.d core gmon.out *.stackdump 

#-----------------------#
#----   SVM-light   ----#
#-----------------------#
svm_light_hideo_noexe: 
	cd svm_light; make svm_learn_hideo_noexe

svm_light_clean: 
	cd svm_light; make clean

#----------------------#
#----  STRUCT SVM  ----#
#----------------------#

svm_struct_noexe: 
	cd svm_struct; make svm_struct_noexe

svm_struct_clean: 
	cd svm_struct; make clean


#-------------------------#
#----  SVM empty API  ----#
#-------------------------#

svm_empty_classify: svm_light_hideo_noexe svm_struct_noexe svm_struct_api.o svm_struct/svm_struct_classify.o svm_struct/svm_struct_common.o svm_struct/svm_struct_main.o 
	$(LD) $(LDFLAGS) svm_struct_api.o svm_struct/svm_struct_classify.o svm_light/svm_common.o svm_struct/svm_struct_common.o -o svm_empty_classify $(LIBS)

svm_empty_learn: svm_light_hideo_noexe svm_struct_noexe svm_struct_api.o svm_struct_learn_custom.o svm_struct/svm_struct_learn.o svm_struct/svm_struct_common.o svm_struct/svm_struct_main.o
	$(LD) $(LDFLAGS) svm_struct/svm_struct_learn.o svm_struct_learn_custom.o svm_struct_api.o svm_light/svm_hideo.o svm_light/svm_learn.o svm_light/svm_common.o svm_struct/svm_struct_common.o svm_struct/svm_struct_main.o -o svm_empty_learn $(LIBS)


utility.o: utility.cpp utility.h
	$(CC) -c $(CFLAGS) utility.cpp -o utility.o

svm_struct_api.o: svm_struct_api.c svm_struct_api.h svm_struct_api_types.h svm_struct/svm_struct_common.h
	$(CC) -c $(CFLAGS) svm_struct_api.c -o svm_struct_api.o

svm_struct_learn_custom.o: svm_struct_learn_custom.c svm_struct_api.h svm_light/svm_common.h svm_struct_api_types.h svm_struct/svm_struct_common.h
	$(CC) -c $(CFLAGS) svm_struct_learn_custom.c -o svm_struct_learn_custom.o
run:
	mkdir data
	mkdir output
	wget https://googledrive.com/host/0B90HhQ5Tx3n8elRpVVRMY0UxRjQ
	mv 0B90HhQ5Tx3n8elRpVVRMY0UxRjQ data/train_normalized_hw1_merge.ark
	wget https://googledrive.com/host/0B90HhQ5Tx3n8TG9BUHRXSXlnS3c
	mv 0B90HhQ5Tx3n8TG9BUHRXSXlnS3c data/test_normalized_hw1.ark
	./svm_empty_learn -c 100 -e 0.05 -l 1 -o 2 data/train_normalized_hw1_merge.ark output/model_hw1_linear_C_100
	./svm_empty_classify data/test_normalized_hw1.ark output/model_hw1_linear_C_100 data/output.raw output/output.kaggle

