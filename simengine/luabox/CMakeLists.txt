#CORE_O=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o \
#	lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o \
#	ltm.o lundump.o lvm.o lzio.o
#LIB_O=	lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o \
#	lmathlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o loadlib.o linit.o

add_library(lua 
	lapi.c
	lauxlib.c
	lbaselib.c
	lbitlib.c
	lcode.c
	lcorolib.c
	lctype.c
	ldblib.c
	ldebug.c
	ldo.c
	ldump.c
	lfunc.c
	lgc.c
	linit.c
	liolib.c
	llex.c
	lmathlib.c
	lmem.c
	loadlib.c
	lobject.c
	lopcodes.c
	loslib.c
	lparser.c
	lstate.c
	lstring.c
	lstrlib.c
	ltable.c
	ltablib.c
	ltm.c
	lua.c
	luac.c
	lundump.c
	lutf8lib.c
	lvm.c
	lzio.c
# Custom stuff
	scripter.cpp
)