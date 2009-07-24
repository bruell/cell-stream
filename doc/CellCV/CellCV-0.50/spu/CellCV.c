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


#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include <CellCV_spe.h>
#include <subimage.h>

#define BIGBUFFER_SIZE 240000

volatile thecontext ctx;
volatile subimage_t mysubimage;
volatile unsigned char bigbuffer[BIGBUFFER_SIZE];

#define MAX_TRANSFER_SIZE 16384

void LSsubimage_setpixel(subimage_t * destsi, int x, int y, int r, int g, int b)
{
   unsigned char * basep = & ( destsi->dataLS[ y * destsi->w * destsi->Bpp + destsi->Bpp * x] );
   basep[0] = r;
   basep[1] = g;
   basep[2] = b;
}

void LSsubimage_getpixel(subimage_t * destsi, int x, int y, int * r, int * g, int * b)
{
   unsigned char * basep = & ( destsi->dataLS[ y * destsi->w * destsi->Bpp + destsi->Bpp * x] );
   *r = basep[0];
   *g = basep[1];
   *b = basep[2];

}

void elaborate_LSimage(void)
{
   int i, j;
   for(i=0; i<mysubimage.h; i++)
   {
     for(j=0; j<mysubimage.w-1; j++)
     {
        int r1, g1, b1, r2, g2, b2, r3, g3, b3;
        LSsubimage_getpixel(&mysubimage, j, i, &r1, &g1, &b1);
        LSsubimage_getpixel(&mysubimage, j+1, i, &r2, &g2, &b2);
        r3 = abs(r2-r1);
        g3 = abs(g2-g1);
        b3 = abs(b2-b1);
        LSsubimage_setpixel(&mysubimage, j, i, r3, g3, b3);
     }
   }
   // communicate a single bbox
   mysubimage.Nbbox = 1;
   mysubimage.bboxData[0] = 30; //xupleft
   mysubimage.bboxData[1] = 30; //yupleft
   mysubimage.bboxData[2] = 8;  //width
   mysubimage.bboxData[3] = 20; //height
}

void sidata_toMM(subimage_t * psi)
{
  int tocopy, copied, chunk;
  unsigned char * start, * end;
  if(!psi) return;
  psi->dataLS = bigbuffer; 
  tocopy = psi->bytesAllocated;
  if(tocopy>BIGBUFFER_SIZE)
  {
     printf("tocopy too much\n");
     exit(4);
  }
  copied = 0;
  while(tocopy>0)
  {
    chunk = 0;
    if(tocopy>MAX_TRANSFER_SIZE)
    {
      chunk = MAX_TRANSFER_SIZE;
      tocopy -= MAX_TRANSFER_SIZE;
    }
    else
    {
      chunk = tocopy;
      tocopy = 0;
    }
    start = (unsigned char *) (psi->dataLS + copied);
    end = (unsigned char *) (psi->parameter_data + copied);
    spu_mfcdma32((void *)(start), (unsigned int) (end), chunk, 0, MFC_PUT_CMD);
    copied += chunk;
  }
  (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);
}

void sidata_toLS(subimage_t * psi)
{
  int tocopy, copied, chunk;
  unsigned char * start, * end;
  if(!psi) return;
  psi->dataLS = bigbuffer; 
  tocopy = psi->bytesAllocated;
  if(tocopy>BIGBUFFER_SIZE)
  {
     printf("tocopy too much\n");
     exit(4);
  }
  copied = 0;
  while(tocopy>0)
  {
    chunk = 0;
    if(tocopy>MAX_TRANSFER_SIZE)
    {
      chunk = MAX_TRANSFER_SIZE;
      tocopy -= MAX_TRANSFER_SIZE;
    }
    else
    {
      chunk = tocopy;
      tocopy = 0;
    }
    start = (unsigned char *) (psi->dataLS + copied);
    end = (unsigned char *) (psi->parameter_data + copied);
    spu_mfcdma32((void *)(start), (unsigned int) (end), chunk, 0, MFC_GET_CMD);
    copied += chunk;
  }
  (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);
}

int main(unsigned long long spu_id, unsigned long long parm)
{
  int i, j;
  unsigned int d;
  unsigned int opcode;
  spu_writech(MFC_WrTagMask, -1);

  spu_mfcdma32((void *)(&ctx), (unsigned int)parm, sizeof(thecontext), 0, MFC_GET_CMD);
  printf("ciao spu_id=%llx parm=%u size=%d tag_id=%u\n", spu_id, parm, sizeof(thecontext), ctx.tag_id);

  d = spu_mfcstat(MFC_TAG_UPDATE_ALL);

  while (1)
    {
      opcode = (unsigned int) spu_read_in_mbox ();
      switch (opcode)
	{
	case EXIT_SPE_OP:
	  goto FINE;
	  break;
        case ELAB_IMAGE:

          //copy the input image to the local store buffer
          spu_mfcdma32((void *)(&mysubimage), (unsigned int) (ctx.thesubimage), sizeof(subimage_t), 0, MFC_GET_CMD);
          (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);

          sidata_toLS(&mysubimage);

          elaborate_LSimage();

          // put the output image back to main memory
          spu_mfcdma32((void *)(&mysubimage), (unsigned int) (ctx.theoutsubimage), sizeof(subimage_t), 0, MFC_PUT_CMD);
          (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);

          sidata_toMM(&mysubimage);

          spu_write_out_mbox(1);

          break;
       }
    }

FINE:
   spu_mfcdma32((void *)(&ctx), (unsigned int)parm, sizeof(thecontext), 0 /*ctx.tag_id*/, MFC_PUT_CMD);
   //spu_writech(MFC_WrTagMask, ctx.tag_id);
   (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);
   
  return (0);
}
