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


#include <mypngimage.h>
#include <CellCV_spe.h>

inline void imageSetPixel(pngimage_t * p, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	png_byte * row = p->row_pointers[y];
	png_byte * ptr = &(row[x*3]);
	ptr[0] = r;
	ptr[1] = g;
	ptr[2] = b;

}

inline void imageGetPixel(pngimage_t * p, int x, int y, unsigned char * r, unsigned char * g, unsigned char * b)
{
	png_byte * row = p->row_pointers[y];
	png_byte * ptr = &(row[x*3]);
	*r = ptr[0];
	*g = ptr[1];
	*b = ptr[2];

}



void destroy_pngimage(pngimage_t ** ppimage)
{
   if((!ppimage)||(!(*ppimage))) return;
   for ((*ppimage)->y=0; (*ppimage)->y < (*ppimage)->info_ptr->height; (*ppimage)->y++)
     free((*ppimage)->row_pointers[(*ppimage)->y]);
   free((*ppimage)->row_pointers);

   // png_destroy_read_struct((*ppimage)->png_ptr, (*ppimage)->info_ptr, NULL); //TODO: cleanup
   png_destroy_info_struct((*ppimage)->png_ptr, &(*ppimage)->info_ptr);
   

   *ppimage = 0; 
}

void read_png_file(pngimage_t * p, char* file_name)
{
	int width, height;
	char header[8];	// 8 is the maximum size that can be checked

	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");

	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_bytep)header, 0, 8))
		 exit(-1); //not recognized as a PNG file

	p->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	p->info_ptr = png_create_info_struct(p->png_ptr);
	
	if (setjmp(png_jmpbuf(p->png_ptr)))
		exit(-1); // Error during init_io

	png_init_io(p->png_ptr, fp);
	png_set_sig_bytes(p->png_ptr, 8);

	png_read_info(p->png_ptr, p->info_ptr);

//printf("%s %dx%d\n", file_name, p->info_ptr->width, p->info_ptr->height);

	width = p->info_ptr->width;
	height = p->info_ptr->height;
	p->color_type = p->info_ptr->color_type;
	p->bit_depth = p->info_ptr->bit_depth;

	p->number_of_passes = png_set_interlace_handling(p->png_ptr);
	png_read_update_info(p->png_ptr, p->info_ptr);


	/* read file */
	if (setjmp(png_jmpbuf(p->png_ptr)))
		exit(-1); //  Error during read_image

	// TODO: allocations are done in the constructor too!!!
	p->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (p->y=0; p->y<height; p->y++)
		p->row_pointers[p->y] = (png_byte*) malloc(p->info_ptr->rowbytes);

	png_read_image(p->png_ptr, p->row_pointers);

/*	for (y=0; y<height; y++)
		free(row_pointers[y]);
	free(row_pointers);
*/

///AAA
     p->imgWidth = p->info_ptr->width;
    p->imgHeight = p->info_ptr->height;

    fclose(fp);
}

void write_png_file(pngimage_t * p, char* file_name)
{
	FILE *fp = fopen(file_name, "wb");
	
	p->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	p->info_ptr = png_create_info_struct(p->png_ptr);
	
	if (setjmp(png_jmpbuf(p->png_ptr)))
		exit(-1); // Error during init_io

	png_init_io(p->png_ptr, fp);


	if (setjmp(png_jmpbuf(p->png_ptr)))
		exit(-1); // Error during writing header

	png_set_IHDR(p->png_ptr, p->info_ptr, p->imgWidth, p->imgHeight,
		     8 /* bit_depth */, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(p->png_ptr, p->info_ptr);
 

	/* write bytes */
	if (setjmp(png_jmpbuf(p->png_ptr)))
		exit(-1); // Error during writing bytes

	png_write_image(p->png_ptr, p->row_pointers);


	/* end write */
	if (setjmp(png_jmpbuf(p->png_ptr)))
		exit(-1); // Error during end of write

	png_write_end(p->png_ptr, NULL);

	png_destroy_write_struct(&p->png_ptr, &p->info_ptr);

    fclose(fp);
}

