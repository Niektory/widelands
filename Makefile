#####################################################################
#     W I D E L A N D S			M A K E F I L E                       #
#####################################################################

# Please edit everything in the general and in the OS specific sections
# after this, set the first variable to YES and start the build by 
# running GNU make

########################### GLOBAL SECTION ##########################
# NON CROSS COMPILE
# 
#  set this to YES if you're done here
IS_MAKEFILE_EDITED:=YES

# Is this a cross compile?
CROSS=NO

# on some systems (BSD) this is named sdl12-config or so
SDL_CONFIG:=sdl-config

ifeq ($(CROSS),NO)
# C compiler
CC=gcc

# c++ compiler
CXX=g++

# additional build flags. if you're not a developer, you don't want
# to change this
ADD_CFLAGS:=

# additional link flags. if you're not a developer, you don't want 
# to change this
ADD_LDFLAGS:=

# Different build-types:
#  debug    optimized, debugging symbols
#  release  optimized
#  profile  optimized, debugging symbols, profiling
#
ifndef BUILD
BUILD=debug
endif

endif

########################### LINUX SECTION ##########################

# nothing special about linux at the moment


####################################################################
#  NO USER CHANGES BELOW THIS POINT											 #
####################################################################

ifneq ($(IS_MAKEFILE_EDITED),YES)
$(error Please edit the Makefile and set the IS_MAKEFILE_EDITED variable to YES)
endif

##############################################################################
# Cross compiling options

ifneq ($(CROSS),NO) 
# CROSS COMPILE, for developer only
PREFIX:=/usr/local/cross-tools
TARGET:=i386-mingw32msvc
PATH:=$(PREFIX)/$(TARGET)/bin:$(PREFIX)/bin:$(PATH)

CC=$(TARGET)-gcc
CXX=$(TARGET)-g++

# manually overwrite
SDL_CONFIG=$(PREFIX)/$(TARGET)/bin/sdl-config

else
TARGET:=native
endif


##############################################################################
# Flags configuration
BUILD:=$(strip $(BUILD))

ifeq ($(BUILD),release)
OPTIMIZE:=yes
else
ifeq ($(BUILD),profile)
OPTIMIZE:=yes
DEBUG:=yes
PROFILE:=yes
else
BUILD:=debug
OPTIMIZE:=yes
DEBUG:=yes
endif
endif

ifdef OPTIMIZE
ADD_CFLAGS += -O3
endif

ifdef DEBUG
ADD_CFLAGS += -DDEBUG
else
ADD_CFLAGS += -DNDEBUG
endif

ifdef PROFILE
ADD_CFLAGS += -pg -fprofile-arcs
endif

##############################################################################
# Object files and directories, final compilation flags

OBJECT_DIR:=src/$(TARGET)-$(BUILD)

# note: all src/*.cc files are considered source code
WIDELANDS_SRC:=$(wildcard src/*.cc)
WIDELANDS_OBJ:=$(patsubst src/%.cc,$(OBJECT_DIR)/%.o,$(WIDELANDS_SRC))
WIDELANDS_DEP:=$(WIDELANDS_OBJ:.o=.d)

CFLAGS:=-Wall $(shell $(SDL_CONFIG) --cflags) $(ADD_CFLAGS)
CXXFLAGS:=$(CFLAGS)
LDFLAGS:=$(shell $(SDL_CONFIG) --libs) $(ADD_LDFLAGS)

##############################################################################
# Building
all: $(OBJECT_DIR) $(OBJECT_DIR)/widelands
	cp $(OBJECT_DIR)/widelands .
	@echo -ne "\nCongrats. Build seems to be complete. If there was no "
	@echo -ne "error (ignore file not found errors), you can run the game "
	@echo -ne "now. just type: './widelands' and enjoy!\n\n"
	@echo -e "\tTHE WIDELANDS DEVELOPMENT TEAM"

$(OBJECT_DIR):
	-mkdir -p $(OBJECT_DIR)

clean:
	@-rm -rf widelands
	@-rm -rf *.da src/*.da
	@-rm -rf src/*.o src/*/*.o
	@-rm -rf src/*.d src/*/*.d
	@-rm -rf *~ */*~ */*/*/*~

# WIDELANDS MAIN PROGRAM BUILD RULES

-include $(WIDELANDS_DEP)

%.h: 

DEP_SED:=sed -e 's@^\(.*\)\.o:@$(OBJECT_DIR)/\1.d $(OBJECT_DIR)/\1.o:@'

$(OBJECT_DIR)/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<
	$(DEP_SED) $*.d > $(OBJECT_DIR)/$*.d
	rm $*.d


$(OBJECT_DIR)/widelands: tags $(WIDELANDS_OBJ)
	$(CXX) $(WIDELANDS_OBJ) -o $@ $(LDFLAGS) $(CFLAGS)

tags: $(wildcard src/*.cc src/*.h)
	@ if [ -x /usr/bin/ctags ]; then ctags -R ; else true; fi
