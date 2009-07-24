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



#include "GLUTWindow.h"
#include "mypngimage.h"

int GlutWindow::glutEnteredMainLoop = false;
int GlutWindow::initXPos = 100;
int GlutWindow::initYPos = 100;
vector<GlutWindow *> GlutWindow::pwindowv;
int * GlutWindow::gargc = NULL;
char ** GlutWindow::gargv = NULL;

void destroy_blobimage(blobimage_t ** ppimage)
{
   free((*ppimage)->blockbuffer);
   free((*ppimage));
   *ppimage = 0;
}

blobimage_t * copy_image_and_block_buffer(pngimage_t * src)
{
  unsigned char r, g, b, a;
  unsigned char * apointer;
  int i, j, nbytes, copied;
  blobimage_t * newimg = (blobimage_t*) malloc(sizeof(blobimage_t));
  newimg->imgWidth = src->imgWidth;
  newimg->imgHeight = src->imgHeight;
  newimg->blockbuffer = (unsigned char *) malloc(src->imgWidth * src->imgHeight * 3);
  nbytes = src->imgWidth * 3;
  copied = 0;
  for(i=0; i<src->imgHeight; i++)
  {
    apointer = & (newimg->blockbuffer[ copied ] );
    memcpy(apointer, src->row_pointers[i], nbytes);
    copied += nbytes;
/*
    for(j=0; j<src->imgWidth; j++)
    {
       imageGetPixel(src, j, i, &r, &g, &b);
       apointer = & (newimg->blockbuffer[ ( i * src->imgWidth + j ) * 3 ]);
       apointer[0] = r;
       apointer[1] = g;
       apointer[2] = b;
    }
*/
  }
  return newimg;
}

void GlutWindow::GL_Enter2DMode()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, wWidth, wHeight);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0, (GLdouble)wWidth, (GLdouble)wHeight, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

void GlutWindow::GL_Leave2DMode()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();
}



void GlutWindow::DrawTexture(void)
{

	blobimage_t *image;
	GLfloat texcoord[4];



	/* Load the image (could use SDL_image library here) */
	image = newImage; // pAcq->popImage(); // SDL_LoadBMP("c:\\temp\\a.bmp");
	newImage = NULL;
	if ( image == NULL ) {
		if(!global_texture) return;
	}else{
		if(global_texture){
			glDeleteTextures(1, &global_texture);
			global_texture = 0;
		}
	
		wBase = image->imgWidth; hBase = image->imgHeight;


		/* Convert the image into an OpenGL texture */
		global_texture = GL_LoadTexture(image, texcoord);

		/* Make texture coordinates easy to understand */
		texMinX = texcoord[0];
		texMinY = texcoord[1];
		texMaxX = texcoord[2];
		texMaxY = texcoord[3];

		/* We don't need the original image anymore */
		destroy_blobimage(&image);

	}

	w = wBase * scale;
	h = hBase * scale;
	/* Show the image on the screen */
	GL_Enter2DMode();
	glBindTexture(GL_TEXTURE_2D, global_texture);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(texMinX, texMinY); glVertex2i(x,   y  );
	glTexCoord2f(texMaxX, texMinY); glVertex2i(x+w, y  );
	glTexCoord2f(texMinX, texMaxY); glVertex2i(x,   y+h);
	glTexCoord2f(texMaxX, texMaxY); glVertex2i(x+w, y+h);
	glEnd();
	GL_Leave2DMode();
}




GlutWindow * getCurrentGlutWindowPointer()
{
	GlutWindow * p;
	int currWindowID = glutGetWindow();
	vector<GlutWindow*>::iterator i;
	for(i=GlutWindow::pwindowv.begin(); i!=GlutWindow::pwindowv.end(); i++){
		p = *i;
		if(p->windowID==currWindowID) return p;
	}
	return NULL;
}

void GlutWindow::adjustWindowDimension()
{
	if((!hBase)||(!wBase)) return;
	if((wHeight!=hBase*scale)||
		(wWidth!=wBase*scale) ){
		wHeight = hBase*scale;
		wWidth = wBase*scale;
		glutSetWindow(windowID);
		glutReshapeWindow(wWidth, wHeight);
		glutPostRedisplay();	
	}
}


void main_reshape(int width,  int height) 
{
	GlutWindow * p;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	p = getCurrentGlutWindowPointer();
	if(p){
		p->wWidth = width;
		p->wHeight = height;
		p->adjustWindowDimension();
	}
    
}

void main_display(void)
{
	GlutWindow * p;
    glClearColor(0.8, 0.8, 0.8, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3ub(0, 0, 0);
	
	p = getCurrentGlutWindowPointer();
	if(p){
		p->DrawTexture();
	}

    glutSwapBuffers();
}

void main_keyboard(unsigned char key, int x, int y)
{
	GlutWindow * p;
	p = getCurrentGlutWindowPointer();
	if(p){
		switch(key){
			case '1':
				p->scale = 1;	
				p->adjustWindowDimension();
				break;
			case '2':
				p->scale = 2;
				p->adjustWindowDimension();
				break;
			case 'q':
			case 'Q':
				exit(0);
				break;
		}
	}
}

void main_idle(void)
{
	GlutWindow * p;
	vector<GlutWindow*>::iterator i;
	for(i=GlutWindow::pwindowv.begin(); i!=GlutWindow::pwindowv.end(); i++){
		p = *i;
		list<GlutCommand *>::iterator gci;
		pthread_mutex_lock(&p->cmdMutex);
		for(gci = p->cmdList.begin(); gci != p->cmdList.end(); gci++){
			switch((*gci)->theCommand){
				case GlutCommand::GlutCmdCreateWindow:
					glutInitWindowSize(p->wWidth, p->wHeight);	
					glutInitWindowPosition(p->initXPos, p->initYPos);
					p->updateInitPos();
					p->windowID = glutCreateWindow(p->windowName);
					glutReshapeFunc(main_reshape);
					glutDisplayFunc(main_display);
					glutKeyboardFunc(main_keyboard);
					glutPostRedisplay();
					break;
				case GlutCommand::GlutCmdDisplayImage:
					if(p->newImage!=NULL){
						destroy_blobimage(&p->newImage);
						p->newImage = NULL;
					}
					p->newImage = (*gci)->theImage;
					(*gci)->theImage = NULL;
					glutSetWindow(p->windowID);
					if((p->wHeight!=p->newImage->imgHeight*p->scale)||
						(p->wWidth!=p->newImage->imgWidth*p->scale) ){
							p->wHeight = p->newImage->imgHeight*p->scale;
							p->wWidth = p->newImage->imgWidth*p->scale;
							glutReshapeWindow(p->wWidth, p->wHeight);
					}
					glutPostRedisplay();
					break;
			} // end switchc gc.theCommand
			GlutCommand * pgc = *gci;
			delete pgc;
		} // end foreach command
		p->cmdList.clear();
		pthread_mutex_unlock(&p->cmdMutex);
		pthread_cond_signal(&p->cmdListEmpty);
	}

}

void GlutWindow::updateInitPos()
{
	this->initXPos += this->wWidth + 10;
	if(this->initXPos>=800) this->initYPos += 100;
	this->initXPos %= 800;
	
}

void * TheGlutMainLoopThread(void * theparam)
{
	GlutWindow * p = (GlutWindow *) theparam;

	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(p->wWidth, p->wHeight);
	glutInitWindowPosition(p->initXPos, p->initYPos);
	p->updateInitPos();
	glutInit(GlutWindow::gargc, GlutWindow::gargv);
	p->windowID = glutCreateWindow(p->windowName);
	glutReshapeFunc(main_reshape);
	glutDisplayFunc(main_display);
	glutKeyboardFunc(main_keyboard);
	glutPostRedisplay();
	glutIdleFunc(main_idle);
    
		

	glutMainLoop();
	return NULL;
}

void GlutWindow::showImageCopy(pngimage_t * theSurface)
{
	GlutCommand * pgc = new GlutCommand();
	blobimage_t * theCopy = copy_image_and_block_buffer(theSurface);
	pthread_mutex_lock(&cmdMutex);
	if(!cmdList.empty())
		pthread_cond_wait(&cmdListEmpty, &cmdMutex);
	pgc->theCommand = pgc->GlutCmdDisplayImage;
	pgc->theImage = theCopy;
	cmdList.insert(cmdList.end(), pgc);
	pthread_mutex_unlock(&cmdMutex);
}


void GlutWindow::showImage(blobimage_t * theSurface)
{
	GlutCommand * pgc = new GlutCommand();
	pthread_mutex_lock(&cmdMutex);
	if(!cmdList.empty())
		pthread_cond_wait(&cmdListEmpty, &cmdMutex);
	pgc->theCommand = pgc->GlutCmdDisplayImage;
	pgc->theImage = theSurface;
	cmdList.insert(cmdList.end(), pgc);
	pthread_mutex_unlock(&cmdMutex);
}

void GlutWindow::InitArgs(int * argc, char ** argv)
{
	gargc = argc;
	gargv = argv;
}

GlutWindow::GlutWindow(char * name)
{
	pwindowv.push_back(this);
	newImage = NULL;
	strcpy(windowName, name);
	scale = 1;
	hBase = wBase = 0;
	dumpNumber = 0;
	pthread_mutex_init(&cmdMutex, NULL);
	pthread_cond_init(&cmdListEmpty, NULL);
	wWidth = 400;
	wHeight = 300;
	global_texture = 0;
	x = y = 0;
	if(!glutEnteredMainLoop){
		glutEnteredMainLoop = true;
		pthread_create(&pMainLoopThread, NULL, TheGlutMainLoopThread, this);
	}else{
		GlutCommand * pgc = new GlutCommand();
		pgc->theCommand = pgc->GlutCmdCreateWindow;
		pthread_mutex_lock(&cmdMutex);
		cmdList.insert(cmdList.begin(), pgc); 
		pthread_mutex_unlock(&cmdMutex);
	}

}

GlutWindow::~GlutWindow()
{
	pthread_cond_destroy(&cmdListEmpty);
	pthread_mutex_destroy(&cmdMutex);
	
}


int GlutWindow::power_of_two(int input)
{
    int value = 1;

    while ( value < input ) {
        value <<= 1;
    }
    return value;
}

void GlutWindow::blit(unsigned char * dest, int dw, int dh, unsigned char * src, int sw, int sh)
{
   int i, j;
   unsigned char * psource;
   unsigned char * pdest;

   for(i=0; i<sh; i++)
   {
     for(j=0; j<sw; j++)
     {
        unsigned char r, g, b, a;
        psource = & (src[ ( i * sw + j ) * 3 ]);
        r = psource[0]; g = psource[1]; b = psource[2]; a = 255;
        pdest = & (dest[ (i*dw+j) * 4 ]);
        pdest[0] = r; 
        pdest[1] = g; 
        pdest[2] = b; 
        pdest[3] = a;
     }

   }

}

GLuint GlutWindow::GL_LoadTexture(blobimage_t *surface, GLfloat *texcoord)
{
    GLuint texture;
    int w, h;
    unsigned char * aux;

    w = power_of_two(surface->imgWidth);
    h = power_of_two(surface->imgHeight);
    texcoord[0] = 0.0f;         /* Min X */
    texcoord[1] = 0.0f;         /* Min Y */
    texcoord[2] = (GLfloat)(float)surface->imgWidth / (float)w;  /* Max X */
    texcoord[3] = (GLfloat)(float)surface->imgHeight / (float)h;  /* Max Y */

    aux = (unsigned char *) malloc(w*h*4);
  
    blit(aux, w, h, surface->blockbuffer, surface->imgWidth, surface->imgHeight);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA,
             w, h,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             aux
             );
    free(aux);

    return texture;
}

