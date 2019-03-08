####################################################################
# FILE NAME: usg_ring.mk                                         #
####################################################################
PRIVATE_INC_FILE   = \
		     makefile\
		     $(LIBRING_DIR)/usg_ring.h \
         $(LIBRING_DIR)/usg_ring.mk

T_LIBRING = \
	  $(OBJ_DIR)/usg_ring.o 

####################################################################
#                    COMPILE RULES                                 #
####################################################################

$(OBJ_DIR)/usg_ring.o: \
    $(LIBRING_DIR)/usg_ring.c \
	$(PRIVATE_INC_FILE)
	$(call cmd, cc_o_c)
 
