#
# Makefile for the WXCS_DB project.
#
# Author: Ship PengXu
#

#
# Global variables
#
CC             = g++
BUILD_DIR      = ./build/
LIB_DIR        = ./lib/
SRC_DIR        = ./src/
AR             = ar -rc
RANLIB         = ranlib

# -g - Debugging information
# -O1 - Basic optimization
# -Wall - All warnings
CFLAGS         = -g -O1 -Wall

#
# Please modify SOURCES variables as needed when changing the source code
#
PF_SOURCES     = $(addprefix $(SRC_DIR)/pf/, pf_buffermgr.cpp pf_filehandle.cpp \
                 pf_pagehandle.cpp pf_hashtable.cpp pf_manager.cpp \
                 pf_statistics.cpp statistics.cpp)
RM_SOURCES     = $(addprefix $(SRC_DIR)/rm/, rm_manager.cpp rm_filehandle.cpp rm_record.cpp \
                 rm_filescan.cpp rm_error.cpp rm_rid.cpp)
TESTER_SOURCES = $(addprefix $(SRC_DIR)/test/, rm_testkpg.cpp)
UTILS_SOURCES  = $(addprefix $(SRC_DIR)/, base.cpp)

PF_OBJECTS     = $(addprefix $(BUILD_DIR), $(PF_SOURCES:.cpp=.o))
TESTER_OBJECTS = $(addprefix $(BUILD_DIR), $(TESTER_SOURCES:.cpp=.o))

#
# declare previously for building target
#
LIBRARY_PF     = $(LIB_DIR)libpf.a
LIBRARY_RM     = $(LIB_DIR)librm.a
LIBRARIES      = $(LIBRARY_PF)


UTILS          = $(UTILS_SOURCES:.cpp=)
TESTS          = $(TESTER_SOURCES:.cpp=)
EXECUTABLES    = $(UTILS) $(TESTS)

LIBS           = -lparser -lpf

#
# Build targets
#
all: $(LIBRARIES) $(UTILS)

clean:
	rm -f $(BUILD_DIR)*.o $(BUILD_DIR)*.d y.output y.tab.h parse.c $(LIBRARIES) $(EXECUTABLES)

testers: all $(TESTS)

#
# Libraries
#
$(LIBRARY_PF): $(PF_OBJECTS)
    $(AR) $(LIBRARY_PF) $(PF_OBJECTS)
    $(RANLIB) $(LIBRARY_PF)

$(LIBRARY_RM): $(RM_OBJECTS)
	$(AR) $(LIBRARY_RM) $(RM_OBJECTS)
	$(RANLIB) $(LIBRARY_RM)

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