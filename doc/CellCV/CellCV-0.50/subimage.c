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


#include "subimage.h"
#include <CellCV_spe.h>

inline void subimage_setpixel(subimage_t * destsi, int x, int y, int r, int g, int b)
{
   unsigned char * basep = & ( destsi->data[ y * destsi->w * destsi->Bpp + destsi->Bpp * x] );
   basep[0] = r;
   basep[1] = g;
   basep[2] = b;
}

inline void subimage_getpixel(subimage_t * destsi, int x, int y, int * r, int * g, int * b)
{
   unsigned char * basep = & ( destsi->data[ y * destsi->w * destsi->Bpp + destsi->Bpp * x] );
   *r = basep[0];
   *g = basep[1];
   *b = basep[2];

}

void draw_HVline(pngimage_t * dstsi, int x1, int y1, int x2, int y2)
{ // x1<x2 e y1<y2
   int i;


   if(y1==y2)
   { // horizontal line
      for(i=x1; i<=x2; i++)
      {
         imageSetPixel(dstsi, i, y1, 255, 0, 0);
      }
   }
   else
   { //vertical line
      for(i=y1; i<=y2; i++)
      {
         imageSetPixel(dstsi, x1, i, 255, 0, 0);
      }
      
   }
}

void draw_bbox(pngimage_t * dstsi, int xupleft, int yupleft, int width, int height)
{
   draw_HVline(dstsi, xupleft, yupleft, xupleft+width, yupleft);
   draw_HVline(dstsi, xupleft+width, yupleft, xupleft+width, yupleft+height);
   draw_HVline(dstsi, xupleft, yupleft, xupleft, yupleft+height);
   draw_HVline(dstsi, xupleft, yupleft+height, xupleft+width, yupleft+height);
}

void join_subimages(subimage_t * srcsiarray, pngimage_t * destimg, int slices, int pixelborder)
{
  int xstart, i, j, imgno, wreal, bbnum;
  xstart = 0;

  for(imgno=0; imgno<slices; imgno++)
  {
    wreal = srcsiarray[imgno].w - ( ((imgno==0)||(imgno==slices-1)) ? pixelborder : 2*pixelborder );
//printf("imgno=%d di %d - w=%d h=%d\n", imgno, slices, srcsiarray[imgno].w, srcsiarray[imgno].h);
    for(i=0; i<srcsiarray[imgno].h; i++)
    {
      for(j=0; j<wreal; j++)
      {
         int r, g, b;
         subimage_getpixel(&srcsiarray[imgno], j + ( imgno!=0 ? pixelborder : 0 ), i, &r, &g, &b);
         imageSetPixel(destimg, xstart+j, i, r, g, b);
      }
    }
    //draw bboxes
    for(bbnum=0; bbnum<srcsiarray[imgno].Nbbox; bbnum++)
    {
       int realxupleft, realyupleft, width, height;
       width = srcsiarray[imgno].bboxData[bbnum*4+2];
       height = srcsiarray[imgno].bboxData[bbnum*4+3];
       realxupleft = srcsiarray[imgno].bboxData[bbnum*4+0] - (imgno!=0 ? pixelborder : 0) + xstart;
       realyupleft = srcsiarray[imgno].bboxData[bbnum*4+1];
       draw_bbox(destimg, realxupleft, realyupleft, width, height);
    }

    xstart += wreal;
  }

}

int next16multiple(int number)
{
   for(int i=0; i<16; i++)
   {
      if((number+i)%16==0)
      {

         return number+i;
      }
   }
   exit(3);
   return -1;
}

void create_subimage(subimage_t * destsi, pngimage_t * origimg, int slices, int theslicenumber, int pixelborder)
{
   int xstart, i, j, wth, w, n, wth2;

   wth = origimg->imgWidth / slices;
   if((wth * slices < origimg->imgWidth) && (origimg->imgWidth % slices > theslicenumber))
   {
      wth ++;
   }
   w = wth + ( ((theslicenumber==0)||(theslicenumber==slices-1)) ? pixelborder : 2*pixelborder );
   destsi->w = w;
   destsi->h = origimg->imgHeight;
   destsi->Bpp = 3;
   destsi->bytesAllocated = next16multiple(destsi->w * destsi->h * destsi->Bpp);
   destsi->data = (unsigned char *) memalign(16, destsi->bytesAllocated);
   destsi->dataLS = 0;

   xstart = 0;
   for(n=0; n<theslicenumber; n++)
   {
     wth2 = origimg->imgWidth / slices;
     if((wth2 * slices < origimg->imgWidth) && (origimg->imgWidth % slices > n))
     {
        wth2 ++;
     }

     xstart += wth2;
   }
   // printf("AAA slices=%d tsn=%d wth=%d w=%d xstart=%d\n", slices, theslicenumber, wth, w, xstart);
   for(i=0; i<destsi->h; i++)
   {
     for(j=0; j<w; j++)
     {
        unsigned char r, g, b;
        imageGetPixel(origimg, xstart + j - ( theslicenumber==0 ? 0 : pixelborder ), i, &r, &g, &b);
        subimage_setpixel(destsi, j, i, r, g, b);
     }

   }

}

void destroy_subimage(subimage_t * destsi)
{
   free(destsi->data);
   destsi->data = 0;
}
