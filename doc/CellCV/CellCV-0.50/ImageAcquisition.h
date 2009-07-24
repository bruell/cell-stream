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


#ifndef IMAGE_ACQUISITION_H
#define IMAGE_ACQUISITION_H


#include<pthread.h>
#include <mypngimage.h>
#include <CellCV_spe.h>

#define BUF_SIZE 3

typedef struct {
  pngimage_t * buffer[BUF_SIZE];	/* shared buffer */
  int add;		/* place to add next element */
  int rem;		/* place to remove next element */
  int num;		/* number elements in buffer */
  pthread_mutex_t m;	/* mutex lock for buffer */
  pthread_cond_t c_cons; /* consumer waits on this cond var */
  pthread_cond_t c_prod; /* producer waits on this cond var */
  pthread_t thread;



} ImageAcquisition_t;



#ifdef __cplusplus
extern "C" {
#endif

ImageAcquisition_t * create_ImageAcquisition(void);
void destroy_ImageAcquisition(ImageAcquisition_t * pia);
pngimage_t * ImageAcquisition_GetImage(ImageAcquisition_t * pia);

#ifdef __cplusplus
}
#endif






#endif
