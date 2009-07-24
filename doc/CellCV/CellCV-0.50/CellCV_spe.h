//-----------------------------------------------------------------------
//
// This file is part of the CellCV Project
// (The Cell Processor Computer Vision Project)
//
//
//  by Amos Tibaldi - tibaldi at users.sourceforge.net
//
// http://sourceforge.net/projects/cellcv/
//
// http://cellcv.sourceforge.net/
//
//
// COPYRIGHT: http://www.gnu.org/licenses/gpl.html
//            COPYRIGHT-gpl-3.0.txt
//
//     The CellCV Project - for parallel elaboration of image sequences
//           using the STI Cell Processor in order to achieve a speedup
//     Copyright (C) 2007 Amos Tibaldi
//
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------


#ifndef CELLCV_SPE_H
#define CELLCV_SPE_H


#include <subimage.h>
#include <mypngimage.h>

#define EXIT_SPE_OP             1
#define ELAB_IMAGE		2

typedef struct {

  subimage_t * thesubimage;
  subimage_t * theoutsubimage;

  unsigned int tag_id;

  unsigned char dummy[4]; 
} thecontext;



#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif
