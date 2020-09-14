# config.mk

CC=gcc

PROG_NAME=software-renderer

INCLUDE_DIR=include

RES_DIR=resource

PROF_DIR=profile

X11_LIB=/usr/X11R6/lib

SRC=src/main.c

BUILD_DIR=build

LIBS=-lpng -lm -L${X11_LIB} -lX11

FLAGS=-I${INCLUDE_DIR} ${SRC} -o ${BUILD_DIR}/${PROG_NAME} -Wall -Wno-missing-braces -std=c99

O_RELEASE=-O2

O_DEBUG_PROFILE=-p -Og -W

O_DEBUG=-Og -g
