# config.mk

CC=gcc

PROG_NAME=software-renderer

INCLUDE_DIR=include

RES_DIR=resource

# SRC=src/*.c
SRC=src/main.c

BUILD_DIR=build

LIBS=-lpng -lm

FLAGS=${SRC} -I${INCLUDE_DIR} -o ${BUILD_DIR}/${PROG_NAME} -Wno-missing-braces -std=c99

O_RELEASE=-O2

O_DEBUG=-Og -g
