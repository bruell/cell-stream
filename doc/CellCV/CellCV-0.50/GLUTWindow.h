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


#ifndef GLUTWINDOW_H
#define GLUTWINDOW_H
#include <pthread.h>
#include <mypngimage.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <list>


using namespace std;

typedef struct {
   int imgWidth, imgHeight;
   unsigned char * blockbuffer;
} blobimage_t;

class GlutCommand {
public:
	typedef enum { GlutCmdCreateWindow = 0, GlutCmdDisplayImage } CommandType ;
	
	blobimage_t * theImage;
	CommandType theCommand;
};

class GlutWindow {
public:
	char windowName[100];
	static void InitArgs(int * argc, char ** argv);
	GlutWindow(char * name);
	~GlutWindow();
	void showImage(blobimage_t * theSurface);
	void showImageCopy(pngimage_t * theSurface);
	void DrawTexture();
	void adjustWindowDimension();
	list<GlutCommand *> cmdList;
	static vector<GlutWindow *> pwindowv;
	static int * gargc;
	static char ** gargv;
	int windowID;
	static int initXPos, initYPos;
	int wWidth, wHeight;
	blobimage_t * newImage;
	void updateInitPos();
	pthread_mutex_t cmdMutex;
	pthread_cond_t cmdListEmpty;
	int scale;
	int wBase, hBase;
	int dumpNumber;
	void dumpOneFile();
private:
	
	static int glutEnteredMainLoop;
	pthread_t pMainLoopThread;

	void blit(unsigned char * dest, int dw, int dh, unsigned char * src, int sw, int sh);
	void GL_Enter2DMode();
	void GL_Leave2DMode();
	GLuint global_texture;
	int power_of_two(int input);
	GLuint GL_LoadTexture(blobimage_t *surface, GLfloat *texcoord);
	GLfloat texMinX, texMinY;
	GLfloat texMaxX, texMaxY;
	int x;
	int y;
	int w, h; 
};

#endif
