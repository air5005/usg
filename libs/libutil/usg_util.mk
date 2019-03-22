####################################################################
# FILE NAME: usg_util.mk                                           #
####################################################################
PRIVATE_INC_FILE   = \
		     makefile\
		     $(LIBUTIL_DIR)/usg_util.h \
         $(LIBUTIL_DIR)/usg_util.mk

T_LIBUTIL = \
	  $(OBJ_DIR)/usg_util.o 

####################################################################
#                    COMPILE RULES                                 #
####################################################################

$(OBJ_DIR)/usg_util.o: \
    $(LIBUTIL_DIR)/usg_util.c \
	$(PRIVATE_INC_FILE)
	$(call cmd, cc_o_c)
 
