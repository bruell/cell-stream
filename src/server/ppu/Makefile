# --------------------------------------------------------------  
# (C)Copyright 2001,2007,                                         
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

PROGRAM_ppu	= ../../../bin/csserver


########################################################################
#			Local Defines
########################################################################

IMPORTS   = -lspe2 -lc -lpthread -limage -lsync
IMPORTS  += -lboost_system-gcc41-mt -lboost_program_options-gcc41-mt -lboost_thread-gcc41-mt
INCLUDE  += -I/usr/local/ppu-libs/include/boost-1_35
# CXXFLAGS += --param large-function-growth=800000 --param max-inline-insns-single=80000 --param inline-unit-growth=80000

########################################################################
#			make.footer
########################################################################

include /opt/cell/sdk/buildutils/make.footer

#ifdef CELL_TOP
#	include $(CELL_TOP)/make.footer
#else
#	include /opt/ibm/cell-sdk/prototype/make.footer
#endif
