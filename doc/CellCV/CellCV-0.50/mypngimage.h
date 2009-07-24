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


#ifndef MYPNGIMAGE_H
#define MYPNGIMAGE_H

#include <png.h>

typedef struct {
	int x, y;
	png_byte color_type;
	png_byte bit_depth;
	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	int imgWidth, imgHeight;
        unsigned char * blockbuffer;
} pngimage_t;

#ifdef __cplusplus
extern "C" {
#endif



void destroy_pngimage(pngimage_t ** ppimage);
void read_png_file(pngimage_t * p, char* file_name);
void write_png_file(pngimage_t * p, char* file_name);
void imageGetPixel(pngimage_t * p, int x, int y, unsigned char * r, unsigned char * g, unsigned char * b);
void imageSetPixel(pngimage_t * p, int x, int y, unsigned char r, unsigned char g, unsigned char b);

#ifdef __cplusplus
}
#endif

#endif
