#include <vector>
#include <iostream>
#include <GL/glut.h>
#include <time.h> //to seed rand
bool DEBUG = false;
class Maze{
private:
	int _x;
	int _y;
	std::vector<std::vector<bool> > walls;

	bool setup_walls(){
		walls =  std::vector<std::vector<bool> >(_y,std::vector<bool>(_x,false));
		srand (time(NULL));
		for(int y=0;y<_y;++y){
			for(int x=0;x<_x;++x){
				if((x==0||y==0||x==_x-1||y==_y-1||rand()%100<30))
					if(!(x==_x-5&&y==_y-1)&&!(x==1&&y==1))//Add an exit
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
public:
	Maze(int x, int y): walls(y,std::vector<bool>(x,false)),_x(x),_y(y) {}
	void add_wall(int x,int y){
		walls[y][x]=true;
	}
	void generate(){
		//loop on setup_walls until it says we have a valid maze
		while(!setup_walls()){}
	}
	void draw_walls(){
		glMatrixMode(GL_MODELVIEW); // return to modelview mode
		for(int y=0;y<_y;++y){
			for(int x=0;x<_x;++x){
				glPushMatrix();
					glTranslatef(x*3+1.5, y*3+1.5, 1); // position wall
					if(walls[y][x])
						glutSolidCube(3); // wall

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