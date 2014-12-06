#include <vector>
#include <iostream>
#include <GL/glut.h>
#include <time.h> //to seed rand
//#include "LoadBMP.h"
#include <cassert>
#include <queue>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
//#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
bool DEBUG = false;





int mode=0;    //  Texture mode
int ntex=0;    //  Texture
int color=0;   //  Color
int axes=1;    //  Display axes
int th=0;      //  Azimuth of view angle
int ph=0;      //  Elevation of view angle
double rep=1;  //  Repetition
double asp=1;  //  Aspect ratio
double dim=2;  //  Size of world
double X=1;    //  Top Right X
double Y=1;    //  Top Right Y
unsigned int texture[2];  //  Texture names

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Print message to stderr and exit
 */
void fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}
unsigned int LoadTexBMP(char* file)
{
   unsigned int   texture;     // Texture name
   FILE*          f;           // File pointer
   char           magic[2];    // Image magic
   unsigned int   dx,dy,size;  // Image dimensions
   unsigned short nbp,bpp;     // Planes and bits per pixel
   unsigned char* image;       // Image data
   int            k;           // Counter

   //  Open file
   f = fopen(file,"rb");
   if (!f) fatal("Cannot open file %s\n",file);
   //  Check image magic
   if (fread(magic,2,1,f)!=1) fatal("Cannot read magic from %s\n",file);
   if (strncmp(magic,"BM",2)) fatal("Image magic not BMP in %s\n",file);
   //  Seek to and read header
   fseek(f,16,SEEK_CUR);
   if (fread(&dx ,4,1,f)!=1 || fread(&dy ,4,1,f)!=1 || fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1)
     fatal("Cannot read header from %s\n",file);
   //  Check image parameters
   if (nbp!=1) fatal("%s bit planes is not 1: %d\n",file,nbp);
   if (bpp!=24) fatal("%s bits per pixel is not 24: %d\n",file,bpp);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k++);
   if (k!=dx) fatal("%s image width not a power of two: %d\n",file,dx);
   for (k=1;k<dy;k++);
   if (k!=dy) fatal("%s image height not a power of two: %d\n",file,dy);
#endif

   //  Allocate image memory
   size = 3*dx*dy;
   image = (unsigned char*) malloc(size);
   if (!image) fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
   //  Seek to and read image
   fseek(f,24,SEEK_CUR);
   if (fread(image,size,1,f)!=1) fatal("Error reading data from image %s\n",file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }

   //  Generate 2D texture
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,3,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}



















































struct cell{
	bool a;
	int x;
	int y;
};



class Smart_Maze{
private:
	int _x;
	int _y;
	std::vector<std::vector<bool> > walls;
	unsigned int texture[2];  //  Texture names

	//Initially, x=1,y=1,checked is filled w/ false's
	bool valid(int x,int y,std::vector<std::vector<int> >& checked){
		checked[y][x]=true;
		if(walls[y][x])	//If I'm inside a wall
			return false;	//Failure
		if(!walls[y][x]&&y==_y-1)//Break in the outer wall
			return true;
		bool east=checked[y][x+1];
		bool west=checked[y][x-1];
		bool north=checked[y+1][x];
		bool south=checked[y-1][x];
		bool ret=false;

		if(!north){
			ret = ret || valid(x,y+1,checked);
		}
		if(!south){
			ret = ret || valid(x,y-1,checked);
		}
		if(!east){
			ret = ret || valid(x+1,y,checked);
		}
		if(!west){
			ret = ret || valid(x-1,y,checked);
		}

		return ret;

	}


  	// Has undefined behavior on non-adjacent walls
	void remove_wall(int c1, int c2){
//		std::cout<<"Asked to remove wall btwn"<<c1<<","<<c2<<std::endl;

		int x=_x/2;
		int y= _y/2;
		//convert to grid
		int c1x = c1% x;
		int c1y = c1/ x;
		int c2x = c2% x;
		int c2y = c2/ x;
		if(!((c1x-c2x>-2&&c1x-c2x<2)&&(c1y-c2y>-2&&c1y-c2y<2))){
			std::cout<< "Invald cells provided:"<<c1x<<","<<c1y<<
				" and "<<c2x<<","<<c2y<<std::endl;
			assert(false);
		}
		//map to wall grid
		c1x =c1x*2;
		c1y =c1y*2;
		c2x =c2x*2;
		c2y =c2y*2;
		int r_x = (c1x+c2x)/2;
		int r_y = (c1y+c2y)/2;
//		std::cout<<"Trying to remove wall "<<r_x<<","<<r_y<<std::endl;
		walls[r_y+1][r_x+1]=0;
//		std::cout<<"Removed wall"<<std::endl;
	}

	bool setup_walls(){
		std::queue<int> Cells; 
		int x=_x/2;
		int y= _y/2;
		int TotalCells = x*y;
		std::vector<int> accessable = std::vector<int>(TotalCells,0);
		int VisitedCells = 1;
		int CurrentCell = 0;
		while(VisitedCells< TotalCells){
//		std::cout<<"Loop"<<std::endl;
			std::vector<int> free_neighbors;

			if((CurrentCell%x)+1<x && !accessable[CurrentCell+1]){
				free_neighbors.push_back(CurrentCell+1);
			}
			if((CurrentCell%x)-1>=0 && !accessable[CurrentCell-1]){
				free_neighbors.push_back(CurrentCell-1);
			}

			if(CurrentCell+x<TotalCells && !accessable[CurrentCell+x]){
				free_neighbors.push_back(CurrentCell+x);
			}

			if(CurrentCell-x>=0 && !accessable[CurrentCell-x]){
				free_neighbors.push_back(CurrentCell-x);
			}

			if(free_neighbors.size()>0){
				int chosen = free_neighbors[rand()%free_neighbors.size()];
				remove_wall(CurrentCell,chosen);
				accessable[CurrentCell]=1;
				Cells.push(CurrentCell);
				CurrentCell = chosen;
				++VisitedCells;
			}
			else{
				CurrentCell=Cells.front();
				Cells.pop();
			}
//			std::cout<<"Visited "<<VisitedCells<<" of "<<TotalCells<<std::endl;
		}
//		std::cout<<"Done Setting Up Maze"<<std::endl;
		walls[_y-1][3]=0; //Break through the wall!
//		print_maze();		
		std::vector<std::vector<int> > checked= std::vector<std::vector<int> > (_y,std::vector<int>(_x,false));
		if(valid(1,1,checked))
			return 1;
		else
			return 0;
	}
public:
	void print_maze(){
		for (int j=_y-1;j>=0;--j){
			for(int k=0; k<_x;++k){
				if(walls[j][k])
					std::cout<<'0';
				else
					std::cout<<'.';
			}
			std::cout<<std::endl;
		}
			std::cout<<std::endl;
	}

	Smart_Maze(int x, int y):
	 _x((x%2)? x+2: x+3),
	 _y((y%2)? y+2: y+3),
	 walls(_y,std::vector<bool>(_x,true)) {

  		texture[0] = LoadTexBMP("crate.bmp");
   		glBindTexture(GL_TEXTURE_2D,texture[0]);
		glEnable(GL_TEXTURE_2D);
   		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//			std::cout<<_x<<" , "<<_y<<" Walls: "<<walls.size()<<std::endl;
   		//clear cells
   		for(int ix=1;ix<_x-1;ix+=2){
   			for(int iy=1;iy<_y-1;iy+=2){
   				walls[iy][ix]=0;
   			}
   		}
//			std::cout<<"Things happened"<<std::endl;
  	}

	void generate(){
		//loop on setup_walls until it says we have a valid maze
		int num_tries=0;
		while(!setup_walls()){
			num_tries++;
			if(!(num_tries%1000))
				std::cout<<"No path to exit. Generating maze "<<num_tries<<std::endl;
		}
	}
	void add_wall(int x,int y){
		walls[y+1][x+1]=true;
	}
	void draw_walls(){
	//	std::cout<<"Drawing Walls"<<std::endl;
		glMatrixMode(GL_MODELVIEW); // return to modelview mode
		for(int y=0;y<_y;++y){
			for(int x=0;x<_x;++x){
				glPushMatrix();
					glTranslatef(x*3+1.5, y*3+1.5, 1); // position wall
					if(walls[y][x]){
						//glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
					    //glEnable(GL_TEXTURE_GEN_T);
					    //glBindTexture(GL_TEXTURE_2D, texture[0]);
					    glutSolidCube(3);
					    //draw_wall();
					    //glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
					    //glDisable(GL_TEXTURE_GEN_T);
					}

				glPopMatrix();
			}
		}

//		std::cout<<"Done Drawing Walls"<<std::endl;
	}
	bool valid_move(double xd,double yd){
		// From 0.111 to 0.999 should count as 0
		int x=xd;
		int y=yd;
		//Add buffer around walls
		//if(xd-(double)x>0.99||xd-(double)x<0.01)
		//	x+=1;
		//if(yd-(double)y>0.99||yd-(double)y<0.01)
		//	y+=1;

		if(x>=_x || x<0 || y<0 || y>=_y)
			return 1;

		if(walls[y][x])
		if(DEBUG) std::cout<<"Hit a wall!"<<std::endl;
		return (!walls[y][x]);
	}

};












































class Maze{
private:
	int _x;
	int _y;
	std::vector<std::vector<bool> > walls;
	unsigned int texture[2];  //  Texture names

	bool setup_walls(){
		walls =  std::vector<std::vector<bool> >(_y,std::vector<bool>(_x,false));
		srand (time(NULL));
		for(int y=0;y<_y;++y){
			for(int x=0;x<_x;++x){
				if((x==0||y==0||x==_x-1||y==_y-1||rand()%100<35))
					if(!(x==_x-5&&y==_y-1)&&!(x==1&&y==1)
					   &&!(x==_x-5&&y==_y-2)&&!(x==1&&y==2))//Add an exit
						add_wall(x,y);
			}
		}
		std::vector<std::vector<int> > checked= std::vector<std::vector<int> > (_y,std::vector<int>(_x,false));
		//checked(_y,<std::vector<bool>(_x,false));
		if(valid(1,1,checked))
			return 1;
	}

	//Initially, x=1,y=1,checked is filled w/ false's
	bool valid(int x,int y,std::vector<std::vector<int> >& checked){
		checked[y][x]=true;
		if(walls[y][x])	//If I'm inside a wall
			return false;	//Failure
		if(!walls[y][x]&&y==_y-1)//Break in the outer wall
			return true;
		bool east=checked[y][x+1];
		bool west=checked[y][x-1];
		bool north=checked[y+1][x];
		bool south=checked[y-1][x];
		bool ret=false;

		if(!north){
			ret = ret || valid(x,y+1,checked);
		}
		if(!south){
			ret = ret || valid(x,y-1,checked);
		}
		if(!east){
			ret = ret || valid(x+1,y,checked);
		}
		if(!west){
			ret = ret || valid(x-1,y,checked);
		}

		return ret;

	}
	void drawPlane(void)
	{
   
	   //glShadeModel(GL_FLAT); // Flat shading to get the checkered pattern.
  		//texture[0] = LoadTexBMP("crate.bmp");

	   for(float z = 1.5; z >= -1.5; z -= 0.10)
	   {
		  glBegin(GL_TRIANGLE_STRIP);
	   	  int i = 0;

		  for(float x = -1.5; x <= 1.5; x += 0.10)
		  {
			 //if (i % 2) glColor3f(0.0, 0.5, 0.5);  
			 glColor3f(1.0, 1.0, 1.0); 
			 glNormal3f(0.0, 1.0, 0.0);
				if((i%3)==0)
					glTexCoord2f(0, 0);
				if((i%3)==1)
					glTexCoord2f(1, 0);
				if((i%3)==2)
					glTexCoord2f(1, 1);
				i++;
			 glVertex3f(x, 0.0, z - 0.1);


			 glNormal3f(0.0, 1.0, 0.0);
				if((i%3)==0)
					glTexCoord2f(0, 0);
				if((i%3)==1)
					glTexCoord2f(1, 0);
				if((i%3)==2)
					glTexCoord2f(1, 1);
			 glVertex3f(x, 0.0, z);
		  }
		  glEnd();
	   }
	   glShadeModel(GL_SMOOTH); // Restore smooth shading.
	}
	void draw_wall(){
		//	glEnable(GL_TEXTURE_2D);
		glPushMatrix();
			drawPlane();
		glPopMatrix();
		glPushMatrix();//North/south wall
			glTranslatef(0,1.5,0);
			glRotatef(90,0.0, 1.0, 0.0);
			drawPlane();
		glPopMatrix();
		glPushMatrix();//North/south wall
			glTranslatef(0,-1.5,0);
			glRotatef(-90,0.0, 1.0, 0.0);
			glRotatef(180,0.0, 0.0, 1.0);
			drawPlane();
		glPopMatrix();
		glPushMatrix();//East/West wall
			glTranslatef(-1.5,0,0);
			glRotatef(90,0.0, 0.0, 1.0);
			drawPlane();
		glPopMatrix();
		glPushMatrix();//East/West wall
			glTranslatef(1.5,0,0);
			glRotatef(-90,0.0, 0.0, 1.0);
			drawPlane();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,0,1.5);
			//glRotatef(180,0.0, 1.0, 0.0);
			glRotatef(90,1.0, 0.0, 0.0);
			//glTranslatef(0,0,1.5);
			drawPlane();
		glPopMatrix();
		//	glDisable(GL_TEXTURE_2D);
	}
public:
	Maze(int x, int y): walls(y,std::vector<bool>(x,false)),_x(x),_y(y) {
  		texture[0] = LoadTexBMP("crate.bmp");
   		glBindTexture(GL_TEXTURE_2D,texture[0]);
		glEnable(GL_TEXTURE_2D);
   		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	}
	void add_wall(int x,int y){
		walls[y][x]=true;
	}
	void generate(){
		//loop on setup_walls until it says we have a valid maze
		int num_tries=0;
		while(!setup_walls()){
			num_tries++;
			if(!(num_tries%1000))
				std::cout<<"No path to exit. Generating maze "<<num_tries<<std::endl;
		}
	}
	void draw_walls(){
		glMatrixMode(GL_MODELVIEW); // return to modelview mode
		for(int y=0;y<_y;++y){
			for(int x=0;x<_x;++x){
				glPushMatrix();
					glTranslatef(x*3+1.5, y*3+1.5, 1); // position wall
					if(walls[y][x]){
						//glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
					    //glEnable(GL_TEXTURE_GEN_T);
					    //glBindTexture(GL_TEXTURE_2D, texture[0]);
					    glutSolidCube(3);
					    //draw_wall();
					    //glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
					    //glDisable(GL_TEXTURE_GEN_T);
					}

				glPopMatrix();
			}
		}
	}
	bool valid_move(double xd,double yd){
		// From 0.111 to 0.999 should count as 0
		int x=xd;
		int y=yd;
		//Add buffer around walls
		//if(xd-(double)x>0.99||xd-(double)x<0.01)
		//	x+=1;
		//if(yd-(double)y>0.99||yd-(double)y<0.01)
		//	y+=1;

		if(x>=_x || x<0 || y<0 || y>=_y)
			return 1;

		if(walls[y][x])
		if(DEBUG) std::cout<<"Hit a wall!"<<std::endl;
		return (!walls[y][x]);
	}
};