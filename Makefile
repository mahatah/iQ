# This is the makefile for iQ. Files needed are MM1.c, MM1_build.c, MM1_header.h, mm1system.h
CC=cc

all: iQ

iQ: MM1.c
        cc -o iQ -pthread MM1.c -lm
