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


#include <ImageAcquisition.h>

static int theCounter = 0;

static pngimage_t * getTheImage(void)
{
  char mystring[100];
  pngimage_t * theImage = (pngimage_t *) malloc(sizeof(pngimage_t));
  sprintf(mystring, "./img%03d.png", theCounter%3);
  read_png_file(theImage, mystring);
  theCounter ++;
  return theImage;
}

void * ImageAcquisitionMainLoop(void * arg)
{
  ImageAcquisition_t * pia = (ImageAcquisition_t *) arg;
  pngimage_t * theImage = 0;
  for (; 1; ) {
    theImage = getTheImage();
    pthread_mutex_lock (&pia->m);
    while (pia->num == BUF_SIZE)
    {
      pthread_cond_wait (&pia->c_prod, &pia->m);
    }
    pia->buffer[pia->add] = theImage;
    pia->add = (pia->add+1) % BUF_SIZE;
    pia->num++;
    pthread_mutex_unlock (&pia->m);
    pthread_cond_signal (&pia->c_cons);
    
  }
  return 0;
}

pngimage_t * ImageAcquisition_GetImage(ImageAcquisition_t * pia)
{
  pngimage_t * toReturn = 0;
  pthread_mutex_lock (&pia->m);
  while (pia->num == 0)
  {
    pthread_cond_wait (&pia->c_cons, &pia->m);
  }
  toReturn = pia->buffer[pia->rem];
  pia->rem = (pia->rem+1) % BUF_SIZE;
  pia->num--;
  pthread_mutex_unlock (&pia->m);
  pthread_cond_signal (&pia->c_prod);

  return toReturn;
}

ImageAcquisition_t * create_ImageAcquisition(void)
{
   ImageAcquisition_t * pia;
   pia = (ImageAcquisition_t *) malloc(sizeof(ImageAcquisition_t));
   pia->add = pia->rem = pia->num = 0;
   pthread_mutex_init(&pia->m, NULL);
   pthread_cond_init(&pia->c_cons, NULL);
   pthread_cond_init(&pia->c_prod, NULL);
   pthread_create(&pia->thread, NULL, ImageAcquisitionMainLoop, (void *) pia);
   return pia;
}

void destroy_ImageAcquisition(ImageAcquisition_t * pia)
{
  pthread_join(pia->thread, NULL);
  pthread_mutex_destroy(&pia->m);
  pthread_cond_destroy(&pia->c_cons);
  pthread_cond_destroy(&pia->c_prod);


}


