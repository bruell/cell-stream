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
#include <libspe.h>
#include <CellCV_spe.h>
#include <subimage.h>
#include <cppwrapper.h>
#include <stdlib.h>
#include <cbe_mfc.h>
#include <profiler.h>
#include <ImageAcquisition.h>
#include <mypngimage.h>

typedef struct
{
  speid_t id;
} spe_params_t;

extern spe_program_handle_t CellCV;

#define SPE_THREADS 6



  thecontext ctxs[SPE_THREADS] __attribute__ ((aligned (16)));
  subimage_t siarray[SPE_THREADS] __attribute__ ((aligned (16)));
  subimage_t sioutarray[SPE_THREADS] __attribute__ ((aligned (16)));
  spe_params_t spe_params[SPE_THREADS];



void sync_spe (void)
{
  unsigned int spei;

  for (spei=0; spei<SPE_THREADS; spei++) { 
     unsigned int foo;       
     while (spe_stat_out_mbox(spe_params[spei].id) <= 0) ; 
     foo = spe_read_out_mbox(spe_params[spei].id);   
  }

}

void trigger_spe (void)
{
  int spei;
  for (spei = 0; spei < SPE_THREADS; spei++)
    {
      while (spe_stat_in_mbox(spe_params[spei].id)<1);
      spe_write_in_mbox(spe_params[spei].id, ELAB_IMAGE);
    }
}

void exit_spe (void)
{
  int spei;
  for (spei = 0; spei < SPE_THREADS; spei++)
    {
      while (spe_stat_in_mbox(spe_params[spei].id)<1);
      spe_write_in_mbox(spe_params[spei].id, EXIT_SPE_OP);
    }
}

int main(int argc, char ** argv)
{
  int status, i;
  char astring[100];
  profiler_t theprofiler;
  ImageAcquisition_t * myIA;
  pngimage_t * pInputImg;
  pngimage_t * pOutputImg;

  launch(&argc, argv); 

  myIA = create_ImageAcquisition();

  pOutputImg = (pngimage_t *) malloc(sizeof(pngimage_t));

  read_png_file(pOutputImg, "./out.png");

  for(i=0; i<SPE_THREADS; i++)
  {
    create_subimage(&sioutarray[i], pOutputImg, SPE_THREADS, i, 5);
  }

  for (i=0; i<SPE_THREADS; i++) {
    ctxs[i].thesubimage = & siarray[i];
    ctxs[i].theoutsubimage = & sioutarray[i];
    ctxs[i].tag_id = 1 << i;

    // Create a SPE thread of execution passing the context as a parameter.
    spe_params[i].id = spe_create_thread(SPE_DEF_GRP, &CellCV, &ctxs[i], NULL, -1, SPE_MAP_PS);
    if (spe_params[i].id == (void *) -1) {
      perror("Unable to create SPE thread");
      return (1);
    }
  }

  // main cycle
  int cycles = 0;
  while(cycles<9)
  {
     printf("START %d\n", cycles);

     clean_profiles(&theprofiler);

     start_profile(&theprofiler, 6);

     start_profile(&theprofiler, 1);
     pInputImg = ImageAcquisition_GetImage(myIA);
     stop_profile(&theprofiler, 1);

     start_profile(&theprofiler, 2); // split immagine

     for(i=0; i<SPE_THREADS; i++)
     {
       create_subimage(&siarray[i], pInputImg, SPE_THREADS, i, 5);
     }
     destroy_pngimage(&pInputImg);

     stop_profile(&theprofiler, 2);

     start_profile(&theprofiler, 3); // attività spe

     trigger_spe();

     sync_spe();

     stop_profile(&theprofiler, 3);

     start_profile(&theprofiler, 4); // join immagine
     join_subimages(sioutarray, pOutputImg, SPE_THREADS, 5);
     for(i=0; i<SPE_THREADS; i++)
     {
       destroy_subimage(&siarray[i]);
     }
     stop_profile(&theprofiler, 4);

     //write_png_file(&outputImg, "./out2.png");
     start_profile(&theprofiler, 5);
     showanimage(pOutputImg);
     stop_profile(&theprofiler, 5);

     stop_profile(&theprofiler, 6);

     print_profiles(&theprofiler);

     cycles++;
  }

  exit_spe();

  // Wait for all the SPE threads to complete.
  for (i=0; i<SPE_THREADS; i++) {
    (void)spe_wait(spe_params[i].id, &status, 0);
  }

  destroy_ImageAcquisition(myIA);

  for(i=0; i<SPE_THREADS; i++)
  {
    destroy_subimage(&sioutarray[i]);
  }

  destroy_pngimage(&pOutputImg);

  for(;1;);
  
  return (0);
}
