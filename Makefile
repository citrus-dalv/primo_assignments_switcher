CXX=g++
FLAGS=-g -Wall -pedantic -O0 -std=c++14 -Iinclude -Iinclude/curl
CXX+FLAGS=${CXX} ${FLAGS}

SRCDIR=src
BUILDDIR=build
INCLUDEDIR=include
OBJDIR=obj

ifeq ($(OS), Windows_NT)
	PLATFORM=Windows
	LIBCURL=-L./lib/win -llibcurl
	TARGET=prog.exe
	RM=del /Q obj\*.o build\${TARGET} *~ *.ilk *.pdb
else 
	PLATFORM=Linux
	LIBCURL=-lcurl -lnghttp2 -lssl -lcrypto -lgssapi_krb5 -lidn2 -lldap -llber
	TARGET=prog
	RM=rm -f obj/*.o ${BUILDDIR}/${TARGET}
endif

LINK=${LIBCURL}

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
HEAD=$(wildcard $(INCLUDEDIR)/*.h)

${TARGET}: ${OBJ}
	${CXX+FLAGS} $^ -o $(BUILDDIR)/$@ ${LINK}

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDEDIR)/%.h
	${CXX+FLAGS} -c $< -o $@

.PHONY: directories clean rebuild run

directories:
	@mkdir -p ${BUILDDIR} ${OBJDIR}

run: ${TARGET}
	./${BUILDDIR}/${TARGET}

clean:
	${RM}

rebuild: clean directories ${TARGET}
