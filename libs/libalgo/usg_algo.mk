####################################################################
# FILE NAME: usg_algo.mk                                           #
####################################################################
PRIVATE_INC_FILE   = \
		     makefile\
		     $(LIBALGO_DIR)/bubble_sort.h \
         $(LIBALGO_DIR)/usg_algo.mk

T_LIBALGO = \
	  $(OBJ_DIR)/bubble_sort.o 

####################################################################
#                    COMPILE RULES                                 #
####################################################################

$(OBJ_DIR)/bubble_sort.o: \
    $(LIBALGO_DIR)/bubble_sort.c \
	$(PRIVATE_INC_FILE)
	$(call cmd, cc_o_c)
 
