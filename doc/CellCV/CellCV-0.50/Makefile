##-----------------------------------------------------------------------
##
## This file is part of the CellCV Project
## (The Cell Processor Computer Vision Project)
##
##
##  by Amos Tibaldi - tibaldi at users.sourceforge.net
##
## http://sourceforge.net/projects/cellcv/
##
## http://cellcv.sourceforge.net/
##
##
## COPYRIGHT: http://www.gnu.org/licenses/gpl.html
##            COPYRIGHT-gpl-3.0.txt
##
##     The CellCV Project - for parallel elaboration of image sequences
##           using the STI Cell Processor in order to achieve a speedup
##     Copyright (C) 2007 Amos Tibaldi
##
##     This program is free software: you can redistribute it and/or modify
##     it under the terms of the GNU General Public License as published by
##     the Free Software Foundation, either version 3 of the License, or
##     (at your option) any later version.
##
##     This program is distributed in the hope that it will be useful,
##     but WITHOUT ANY WARRANTY; without even the implied warranty of
##     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##     GNU General Public License for more details.
##
##     You should have received a copy of the GNU General Public License
##     along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
##-----------------------------------------------------------------------

########################################################################
#                       Subdirectories
########################################################################

DIRS		:= spu


########################################################################
#                       Target
########################################################################

PROGRAM_ppu	:= CellCV_spe


########################################################################
#                       Local Defines
########################################################################

IMPORTS         := spu/lib_CellCV_spu.a -lspe

INSTALL_DIR		= $(SDKBIN)/samples
INSTALL_FILES		= $(PROGRAM_ppu)

########################################################################
#                       make.footer
########################################################################

ifdef CELL_TOP
	include $(CELL_TOP)/make.footer
else
	include ../../../../../make.footer
endif


CPPFLAGS += -I /usr/include -g
LDFLAGS += -lpng -lGL -lGLU -lglut 
