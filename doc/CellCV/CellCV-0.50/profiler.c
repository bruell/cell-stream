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


#include <profiler.h>  
  
float my_timeval_subtract(struct timeval * x, struct timeval * y);  
  
  

profile_t * find_profile(profiler_t * pp, int id)
{
  int i;
  profile_t * theprofile;
  theprofile = 0;
  for(i=0; i<pp->Nprofiles; i++)
  {
    if(pp->profiles[i].id == id)
    {
      theprofile = &(pp->profiles[i]);
      break;	
    }
  }
  return theprofile;
}

  
void clean_profiles(profiler_t * pp)
{
  pp->Nprofiles = 0;	
}

void print_profiles(profiler_t * pp)
{
  int i;
  printf("Profili: ...\n");
  for(i=0; i<pp->Nprofiles; i++)
  {
    printf("  Intervallo %d) [%d]: %f secondi\n", i, pp->profiles[i].id, pp->profiles[i].seconds);	
  }
}

void print_profile(profiler_t * pp, int id)
{
  profile_t * theprofile;
  theprofile = find_profile(pp, id);
  if(theprofile==0) return;
  printf("::Intervallo [%d]: %f secondi\n", theprofile->id, theprofile->seconds);
}

void start_profile(profiler_t * pp, int id)
{
  if(pp->Nprofiles >= MAX_PROFILES) return;
  
  pp->profiles[pp->Nprofiles].seconds = 0.0f;
  pp->profiles[pp->Nprofiles].id = id;
  gettimeofday(&(pp->profiles[pp->Nprofiles].start), 0);
  pp->Nprofiles ++;	
}

void stop_profile(profiler_t * pp, int id)
{
  profile_t * theprofile;
  theprofile = find_profile(pp, id);
  if(theprofile==0) return;
  gettimeofday(&(theprofile->end), 0);
  theprofile->seconds = my_timeval_subtract(&(theprofile->end), &(theprofile->start));
}
 
   
float my_timeval_subtract(struct timeval * x, struct timeval * y)
{
  struct timeval result;
  float diffseconds;
  diffseconds = 0.0f;
  
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }
  
  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result.tv_sec = x->tv_sec - y->tv_sec;
  result.tv_usec = x->tv_usec - y->tv_usec;
  
  diffseconds = (float)result.tv_sec + (float)((float) result.tv_usec / 1000000.0f );
  return diffseconds;
}
