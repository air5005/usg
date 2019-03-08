#####################################################################
# FILE NAME: makefile                                               #
#   PURPOSE: makefile                                               #
#####################################################################

########################################################
# 1. Set envirnment micro                              #
########################################################
ADDED_CFLAGS  += -D_GNU_SOURCE
ADDED_CFLAGS  += -Waddress   
ADDED_CFLAGS  += -Wchar-subscripts  
ADDED_CFLAGS  += -Wenum-compare
ADDED_CFLAGS  += -Wimplicit-int
ADDED_CFLAGS  += -Wimplicit-function-declaration
ADDED_CFLAGS  += -Wcomment  
ADDED_CFLAGS  += -Wformat   
ADDED_CFLAGS  += -Wmain
ADDED_CFLAGS  += -Wmaybe-uninitialized 
ADDED_CFLAGS  += -Wmissing-braces
ADDED_CFLAGS  += -Wnonnull  
ADDED_CFLAGS  += -Wparentheses  
ADDED_CFLAGS  += -Wpointer-sign  
ADDED_CFLAGS  += -Wreturn-type  
ADDED_CFLAGS  += -Wsequence-point  
ADDED_CFLAGS  += -Wstrict-aliasing  
ADDED_CFLAGS  += -Wstrict-overflow=1  
ADDED_CFLAGS  += -Wswitch  
ADDED_CFLAGS  += -Wtrigraphs  
ADDED_CFLAGS  += -Wuninitialized  
ADDED_CFLAGS  += -Wunknown-pragmas  
ADDED_CFLAGS  += -Wunused-function  
ADDED_CFLAGS  += -Wunused-label     
ADDED_CFLAGS  += -Wunused-value     
ADDED_CFLAGS  += -Wunused-variable  
ADDED_CFLAGS  += -Wvolatile-register-var 

########################################################
# 2. Set compiling path                                #
########################################################
export USG_TOP_DIR         = $(PWD)
export USG_TARGET_DIR      = $(USG_TOP_DIR)/.output
export USG_LIBS_DIR        = $(USG_TOP_DIR)/libs

BIN_DIR     = $(USG_TARGET_DIR)/bin
LIB_DIR     = $(USG_TARGET_DIR)/lib
INCLUDE_DIR = $(USG_TARGET_DIR)/include
OBJ_DIR     = $(USG_TARGET_DIR)/obj
OBJ_SUFFIX  = o

ADDED_CFLAGS  += -g
ADDED_CFLAGS  += -I$(USG_TOP_DIR)
ADDED_CFLAGS  += -I$(USG_LIBS_DIR)
ADDED_LDFLAGS += -lpthread -lstdc++

COPTS   =  $(ADDED_CFLAGS) $(LIBS_COPTS)
LDFLAGS +=  $(ADDED_LDFLAGS)

include $(USG_TOP_DIR)/rules

TARGET = $(BIN_DIR)/usg
USG_LIB = $(LIB_DIR)/libusg.a

all: check_output_dir LIB BIN copy_head_file

LIB: $(USG_LIB)

BIN: $(TARGET)

$(USG_LIB): TARGET_LIBS
			$(call cmd_ar_target)

$(TARGET): $(OBJ_DIR)/main.obj $(USG_LIB)
				@echo " LD   $@"
				$(CC) $(OBJ_DIR)/main.obj $(COPTS) $(LDFLAGS) -o $@ -Wl,--whole-archive $(USG_LIB) -Wl,--no-whole-archive
				echo "FINISHED CREATING $(TARGET)"
				@echo "-----------------------------------------------------"

$(OBJ_DIR)/main.obj: $(USG_TOP_DIR)/main.c
	$(call cmd, cc_o_c)

include $(USG_LIBS_DIR)/libs.mk

clean-files	:=  $(OBJ_DIR)/*.* \
                $(LIB_DIR)/*.*  \
                $(INCLUDE_DIR)/*.h  \
                $(BIN_DIR)/*  

clean:
	rm -f $(clean-files) > /dev/null

copy_head_file:
	@find $(USG_LIBS_DIR) -name "*.h" |xargs -i cp {} $(INCLUDE_DIR) > /dev/null

check_output_dir:
	mkdir -p $(BIN_DIR) $(LIB_DIR) $(INCLUDE_DIR) $(OBJ_DIR) > /dev/null 
	
.PHONY: all clean copy_head_file
