# --------------------------------------------------------------  
# (C)Copyright 2001,2006,                                         
# International Business Machines Corporation,                    
# Sony Computer Entertainment, Incorporated,                      
# Toshiba Corporation,                                            
#                                                                 
# All Rights Reserved.                                            
# --------------------------------------------------------------  
# PROLOG END TAG zYx                                              

########################################################################
#			Target
########################################################################

PROGRAM_spu	= conv9

########################################################################
#			Local Defines
########################################################################

# CFLAGS		+= --param max-inline-insns-single=0
IMPORTS 	= -lc -lsync -limage

# LDFLAGS_gcc		= -mstdmain

INSTALL_DIR		= ../../../bin/spu
INSTALL_FILES	= $(PROGRAM_spu)

########################################################################
#                       make.footer
########################################################################

include /opt/cell/sdk/buildutils/make.footer

#ifdef CELL_TOP
#	include $(CELL_TOP)/make.footer
#else
#	include /opt/ibm/cell-sdk/prototype/make.footer
#endif
