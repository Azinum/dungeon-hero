# Makefile

include config.mk

all: prepare compile run

mac: prepare compile_mac run

clean:
	rm -dr ${BUILD_DIR}/*
	rm -dr ${PROF_DIR}/*

prepare:
	mkdir -p ${BUILD_DIR}
	mkdir -p ${PROF_DIR}
	cp -rp ${RES_DIR} ${BUILD_DIR}

compile:
	${CC} ${FLAGS} ${LIBS} ${O_RELEASE} ${RENDERER}

debug:
	${CC} ${FLAGS} ${LIBS} ${O_DEBUG} ${RENDERER}
	gdb ./${BUILD_DIR}/${PROG_NAME}

debug_profile:
	${CC} ${FLAGS} ${LIBS} ${O_DEBUG_PROFILE}
	./${BUILD_DIR}/${PROG_NAME}
	gprof -b ./${BUILD_DIR}/${PROG_NAME} gmon.out > ./${PROF_DIR}/profile.out
	mv gmon.out ./${PROF_DIR}

run:
	./${BUILD_DIR}/${PROG_NAME}
