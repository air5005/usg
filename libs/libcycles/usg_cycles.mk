####################################################################
# FILE NAME: usg_cycles.mk                                         #
####################################################################
PRIVATE_INC_FILE   = \
		     makefile\
		     $(LIBCYCLES_DIR)/usg_cycles.h \
         $(LIBCYCLES_DIR)/usg_cycles.mk

T_LIBCYCLES = \
	  $(OBJ_DIR)/usg_cycles.o 

####################################################################
#                    COMPILE RULES                                 #
####################################################################

$(OBJ_DIR)/usg_cycles.o: \
    $(LIBCYCLES_DIR)/usg_cycles.c \
	$(PRIVATE_INC_FILE)
	$(call cmd, cc_o_c)
 
