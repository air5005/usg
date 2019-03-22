####################################################################
# FILE NAME: libs.mk                                               #
####################################################################
export LIBCYCLES_DIR     = $(USG_LIBS_DIR)/libcycles
export LIBLOCK_DIR       = $(USG_LIBS_DIR)/liblock
export LIBRING_DIR       = $(USG_LIBS_DIR)/libring
export LIBS_INCLUDE_DIR  = $(USG_LIBS_DIR)/include
export LIBALGO_DIR       = $(USG_LIBS_DIR)/libalgo
export LIBUTIL_DIR       = $(USG_LIBS_DIR)/libutil

include $(LIBCYCLES_DIR)/usg_cycles.mk
include $(LIBLOCK_DIR)/usg_lock.mk
include $(LIBRING_DIR)/usg_ring.mk
include $(LIBALGO_DIR)/usg_algo.mk
include $(LIBUTIL_DIR)/usg_util.mk

LIBS_COPTS += -I$(LIBCYCLES_DIR) \
         -I$(LIBLOCK_DIR) \
         -I$(LIBRING_DIR) \
         -I$(LIBALGO_DIR) \
         -I$(LIBUTIL_DIR) \
         -I$(LIBS_INCLUDE_DIR) 

####################################################################
#                    DEPENDENCIES                                  #
####################################################################
TARGET_LIBS: \
        $(T_LIBCYCLES) \
		$(T_LIBLOCK) \
		$(T_LIBALGO) \
		$(T_LIBUTIL) \
        $(T_LIBRING)
