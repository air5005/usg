####################################################################
# FILE NAME: usg_lock.mk                                           #
####################################################################
PRIVATE_INC_FILE   = \
		     makefile\
		     $(LIBLOCK_DIR)/usg_lock.h \
		     $(LIBLOCK_DIR)/usg_atomic.h \
		     $(LIBLOCK_DIR)/usg_rwlock.h \
		     $(LIBLOCK_DIR)/usg_spinlock.h \
         $(LIBLOCK_DIR)/usg_lock.mk

T_LIBLOCK = \
	  $(OBJ_DIR)/usg_atomic.o \
	  $(OBJ_DIR)/usg_rwlock.o \
	  $(OBJ_DIR)/usg_spinlock.o \
	  $(OBJ_DIR)/usg_lock.o

####################################################################
#                    COMPILE RULES                                 #
####################################################################

$(OBJ_DIR)/usg_lock.o: \
    $(LIBLOCK_DIR)/usg_lock.c \
	$(PRIVATE_INC_FILE)
	$(call cmd, cc_o_c)

$(OBJ_DIR)/usg_atomic.o: \
    $(LIBLOCK_DIR)/usg_atomic.c \
	$(PRIVATE_INC_FILE)
	$(call cmd, cc_o_c)

$(OBJ_DIR)/usg_rwlock.o: \
    $(LIBLOCK_DIR)/usg_rwlock.c \
	$(PRIVATE_INC_FILE)
	$(call cmd, cc_o_c)

$(OBJ_DIR)/usg_spinlock.o: \
    $(LIBLOCK_DIR)/usg_spinlock.c \
	$(PRIVATE_INC_FILE)
	$(call cmd, cc_o_c)

