MKDIR=mkdir
CP=cp
CC=gcc

BUILDDIR=build



# Object Files
OBJECTFILES= \
        ${BUILDDIR}/db.o \
        ${BUILDDIR}/ini.o \
        ${BUILDDIR}/logger.o \
        ${BUILDDIR}/main.o \
        ${BUILDDIR}/server.o


# C Compiler Flags
CFLAGS=


# Link Libraries and Options
LDLIBSOPTIONS=-lpq

# Build Targets

qotdserver: ${OBJECTFILES}
	${LINK.c} -o ${BUILDDIR}/qotdserver ${OBJECTFILES} ${LDLIBSOPTIONS}

${BUILDDIR}/db.o: db.c
	${MKDIR} -p ${BUILDDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -I/usr/include/postgresql/ -MMD -MP -MF "$@.d" -o ${BUILDDIR}/db.o db.c

${BUILDDIR}/ini.o: ini.c
	${MKDIR} -p ${BUILDDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${BUILDDIR}/ini.o ini.c

${BUILDDIR}/logger.o: logger.c
	${MKDIR} -p ${BUILDDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${BUILDDIR}/logger.o logger.c

${BUILDDIR}/main.o: main.c
	${MKDIR} -p ${BUILDDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${BUILDDIR}/main.o main.c

${BUILDDIR}/server.o: server.c
	${MKDIR} -p ${BUILDDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${BUILDDIR}/server.o server.c


clean: 
	${RM} -r ${BUILDDIR}
	

install:
	${CP} ${BUILDDIR}/qotdserver /usr/sbin/
	${CP} qotdserver.conf /etc/
