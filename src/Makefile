#
# Makefile for the RedBase project.
#
# Author: Ship Xu
#


#
# Global variables
#
CC             = g++
BUILD_DIR      = ../build/
LIB_DIR        = ../lib/
INC_DIRS       = -I.
AR             = ar -rc
RANLIB         = ranlib
YACC           = bison -dy
LEX            = flex

# -m32 - Generates code that runs on any i386 system
#        sizeof(int) = sizeof(long) = sizeof(void *) = 4
# -g - Debugging information
# -O1 - Basic optimization
# -Wall - All warnings -m32 
# -DDEBUG_PF - This turns on the LOG file for lots of BufferMgr info
CFLAGS         = -g -O1 -Wall $(STATS_OPTION) $(INC_DIRS)

# The STATS_OPTION can be set to -DPF_STATS or to nothing to turn on and
# off buffer manager statistics.  The student should not modify this
# flag at all!
STATS_OPTION   = -DPF_STATS

#
# Students: Please modify SOURCES variables as needed.
#
IO_SOURCES     = db_io.cpp
PF_SOURCES     = pf_hashtable.cpp pf_buffermgr.cpp
TESTER_SOURCES = test_io.cpp test_hash_table.cpp test_pfbuffer.cpp

IO_OBJECTS     = $(addprefix $(BUILD_DIR), $(IO_SOURCES:.cpp=.o))
PF_OBJECTS     = $(addprefix $(BUILD_DIR), $(PF_SOURCES:.cpp=.o))
TESTER_OBJECTS = $(addprefix $(BUILD_DIR), $(TESTER_SOURCES:.cpp=.o))
OBJECTS        = $(IO_OBJECTS) $(PF_OBJECTS) $(TESTER_OBJECTS)

LIBRARY_IO     = $(LIB_DIR)libio.a
LIBRARY_PF     = $(LIB_DIR)libpf.a
LIBRARIES      = $(LIBRARY_IO) $(LIBRARY_PF)

TESTS          = $(TESTER_SOURCES:.cpp=)
EXECUTABLES    = $(TESTS)

LIBS           = -lpf #-lix -lrm  -lparser -lql -lsm 

#
# Build targets
#
all: $(LIBRARIES)

clean:
	rm -f $(BUILD_DIR)*.o $(BUILD_DIR)*.d y.output y.tab.h parse.c $(LIBRARIES) $(EXECUTABLES)

testers: all $(TESTS)

#
# Libraries
#
$(LIBRARY_IO): $(IO_OBJECTS)
	$(AR) $(LIBRARY_IO) $(IO_OBJECTS)
	$(RANLIB) $(LIBRARY_IO)

$(LIBRARY_PF): $(PF_OBJECTS)
	$(AR) $(LIBRARY_PF) $(PF_OBJECTS)
	$(RANLIB) $(LIBRARY_PF)

#
# Rules
#
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)%.d: %.cpp
	@set -e; \
	 rm -f $@; \
	 $(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' $@.$$$$ > $@; \
	 rm -f $@.$$$$

$(OBJECTS): %.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLES): %: $(BUILD_DIR)%.o $(LIBRARIES)
	$(CC) $(CFLAGS) $< -o $@ -L$(LIB_DIR) $(LIBS)