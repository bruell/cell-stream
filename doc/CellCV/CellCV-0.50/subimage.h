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


#ifndef SUBIMAGE_H
#define SUBIMAGE_H


#include <mypngimage.h>

#define MAXBBOXPERSUBIMAGE 16
typedef struct {
  unsigned char * data __attribute__((aligned(16)));
  unsigned char * dataLS __attribute__((aligned(16)));
  int w __attribute__((aligned(16)));
  int h __attribute__((aligned(16)));
  int Bpp __attribute__((aligned(16)));
  int bytesAllocated __attribute__((aligned(16)));
  int Nbbox __attribute__((aligned(16)));
  int bboxData[MAXBBOXPERSUBIMAGE*4] __attribute__((aligned(16))); // xupleft, yupleft, width, height

  unsigned char dummy[20] __attribute__((aligned(16)));
} subimage_t;

#ifdef __cplusplus
extern "C" {
#endif

void create_subimage(subimage_t * destsi, pngimage_t * origimg, int slices, int theslicenumber, int pixelborder);
void destroy_subimage(subimage_t * destsi);
void join_subimages(subimage_t * srcsiarray, pngimage_t * destimg, int slices, int pixelborder);
void subimage_setpixel(subimage_t * destsi, int x, int y, int r, int g, int b);
void subimage_getpixel(subimage_t * destsi, int x, int y, int * r, int * g, int * b);

#ifdef __cplusplus
}
#endif

#endif
