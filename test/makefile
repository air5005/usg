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
export USG_TOP_DIR         = $(PWD)/../
export USG_TARGET_DIR      = $(USG_TOP_DIR)/.output
export USG_LIBS_DIR        = $(USG_TOP_DIR)/libs
export USG_TEST_DIR        = $(USG_TOP_DIR)/test

export QUIET=@

ADDED_CFLAGS  += -g
ADDED_CFLAGS  += -I$(USG_TOP_DIR)
ADDED_CFLAGS  += -I$(USG_LIBS_DIR)
ADDED_LDFLAGS += -lpthread -lstdc++

COPTS   =  $(ADDED_CFLAGS)
COPTS  += -I$(USG_TARGET_DIR)/include/
LDFLAGS +=  $(ADDED_LDFLAGS)

include $(USG_TOP_DIR)/rules

USG_LIB = $(USG_TARGET_DIR)/lib/libusg.a

COVERAGE=$(COPTS) $(LDFLAGS) -o $@ -Wl,--whole-archive $(USG_LIB) -Wl,--no-whole-archive

BINS = \
	$(USG_TEST_DIR)/bin_test_atomic \
	$(USG_TEST_DIR)/bin_test_spinlock \
	$(USG_TEST_DIR)/bin_test_rwlock \
	$(USG_TEST_DIR)/bin_test_ring \
	$(USG_TEST_DIR)/bin_test_count

all: $(BINS)

$(USG_TEST_DIR)/bin_test_atomic: $(UPU_LIB)
				$(QUIET)$(CC) $(USG_TEST_DIR)/test_atomic.c $(COVERAGE)
$(USG_TEST_DIR)/bin_test_spinlock: $(UPU_LIB)
				$(QUIET)$(CC) $(USG_TEST_DIR)/test_spinlock.c $(COVERAGE)
$(USG_TEST_DIR)/bin_test_rwlock: $(UPU_LIB)
				$(QUIET)$(CC) $(USG_TEST_DIR)/test_rwlock.c $(COVERAGE)
$(USG_TEST_DIR)/bin_test_ring: $(UPU_LIB)
				$(QUIET)$(CC) $(USG_TEST_DIR)/test_ring.c $(COVERAGE)
$(USG_TEST_DIR)/bin_test_count: $(UPU_LIB)
				$(QUIET)$(CC) $(USG_TEST_DIR)/test_count.c $(COVERAGE)

clean-files	:=  $(USG_TEST_DIR)/*.obj $(BINS)

clean:
	@rm -f $(clean-files) >/dev/null
