TARGET=tlss

OUTDIR:=bin/
OBJDIR:=obj/
SRCDIR:=src

CFLAGS:=-Wall -Wextra
LDFLAGS:=

CONFIG?=release
OUTDIR:=${OUTDIR}${CONFIG}/
OBJDIR:=${OBJDIR}${CONFIG}/

ifeq (${CONFIG}, release)
CFLAGS:=-O3
else ifeq (${CONFIG}, debug)
CFLAGS:=-g
else
$(error Invalid CONFIG variable ${CONFIG})
endif

CXXFLAGS:=${CFLAGS}

INCLUDE=-Iinclude/ -Isrc/include/

OBJS:=$(patsubst %.c,%.o,$(wildcard ${SRCDIR}/*.c))
OBJS:=$(subst ${SRCDIR},${OBJDIR},${OBJS})

TESTDIR=tests

TESTINCLUDE=${INCLUDE} -I${TESTDIR} -I${TESTDIR}/testsuite

TESTOBJS:=$(patsubst %.cpp,%.o,$(wildcard ${TESTDIR}/*.cpp))
TESTOBJS:=$(subst ${TESTDIR},${OBJDIR}/${TESTDIR},${TESTOBJS})

.phony: all clean dir ${TARGET}-dynamic ${TARGET}-static test

all: ${TARGET}-dynamic ${TARGET}-static test

${TARGET}-dynamic : ${OUTDIR}/lib${TARGET}.so
${TARGET}-static : ${OUTDIR}/lib${TARGET}.a
test: ${OUTDIR}tests
	@echo "Running Testsuite"
	@$< -n 1000

${OUTDIR}/lib${TARGET}.so : dir ${OBJS}
	@echo "Linking... lib${TARGET}.so"
	@${CC} ${LDFLAGS} -shared -o $@ ${OBJS}

${OUTDIR}/lib${TARGET}.a : dir ${OBJS}
	@echo "Linking... lib${TARGET}.a"
	@ar -crs $@ ${OBJS}

${OBJDIR}/%.o : ${SRCDIR}/%.c
	@echo "Compiling... $<"
	@${CC} ${CFLAGS} ${INCLUDE} -fpic $< -c -o $@ -MMD -MP -MF $@.d

${OUTDIR}tests : ${TARGET}-dynamic ${OBJDIR}/${TESTDIR}/testsuite.o ${TESTOBJS}
	@echo "Linking Testsuite"
	@${CXX} -o $@ ${LDFLAGS} ${TESTOBJS} ${OBJDIR}/${TESTDIR}/testsuite.o -L${OUTDIR} -l${TARGET}
	@patchelf --set-rpath ${OUTDIR} $@

${OBJDIR}/${TESTDIR}/testsuite.o :
	@echo "Compiling Testsuite"
	@${CXX} ${CXXFLAGS} ${TESTINCLUDE} ${TESTDIR}/testsuite/tests.cpp -c -o $@ -MMD -MP -MF $@.d

${OBJDIR}/${TESTDIR}/%.o : ${TESTDIR}/%.cpp
	@echo "Compiling Test group... $<"
	@${CXX} ${CXXFLAGS} ${TESTINCLUDE} $< -c -o $@ -MMD -MP -MF $@.d

dir:
	@mkdir -p ${OBJDIR}
	@mkdir -p ${OBJDIR}/${TESTDIR}
	@mkdir -p ${OUTDIR}

clean:
	@rm -rf ${OBJDIR}
	@rm -rf ${OUTDIR}

-include ${OBJDIR}/*.d
