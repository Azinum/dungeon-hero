# config.mk

CC=gcc

PROG_NAME=software-renderer

INCLUDE_DIR=include

RES_DIR=resource

PROF_DIR=profile

SCREENSHOT_DIR=screenshot

X11_LIB=/usr/X11R6/lib

X11_INC=/usr/include/X11

SRC=src/main.c

BUILD_DIR=build

LIBS=-lpng -lm -I${X11_INC} -L${X11_LIB} -lX11 -lGL -lGLU -lGLEW

FLAGS=${SRC} -o ${BUILD_DIR}/${PROG_NAME} -I${INCLUDE_DIR} -Wall -W -Wno-missing-braces -std=c99

O_RELEASE=-O2

O_DEBUG_PROFILE=-p -Og -W

O_DEBUG=-Og -g

RENDERER=-D RENDERER_OPENGL
