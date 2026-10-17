#include "baton.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

// does fitting to grid, stacking, backtracking.
/* the small grid does a shortest path, but the robot is too small to benefit
from it directly, so instead it partitions the whole map *much resembling a
sudoku grid*
imagine coloring the robots path in the small squares, and then cutting with scissors out the big squares where backtracking was detected. the big squares are first popped from the stack, and then the small squares are compared to fit withing the saved big squares only.
   */

int resistpop;

// the robots center can never be directly on top of the left corner [0.0]
double fix = 45.0;
// occupy the whole cell
double fat_robot = 30.0;

// the mazes is < 800*800 in x*y
// Route 0 uses an 8x8 grid; route 1 uses a 256x256 grid.
int size[2] = {8, 256};
int depth[2];
int path_x[2][8192];
int path_y[2][8192];

// mutates x and y to be closer to the actual cells center.
void translate_right(double *x, double *y){
  double ang = baton.heading * M_PI * 2; 
  *x += cos(ang) * fat_robot;
  *y -= sin(ang) * fat_robot;
}

// applies off-center fix, can be reused to create a cloud of multiple copies.
// mutates in place
void fixcloud(int *x,int *y,double fix,int i){
  *x = (int)floor((*x+fix) * size[i] / (850.0));
  *y = (int)floor((*y+fix) * size[i] / (850.0));
}

// creates 3 mutable copies of the original x y
// applies fix and +margin -margin to the copies, and if all copies pass that
// means the coordinate is margin away from its neighbour.
// this helps when the grid doesnt sit perfectly on top of the maze walls.
bool pop(int m,int n,int x,int y){
  if (m >= 1 || n < 2) return false; // guard big grid or below zero
  int pcx = path_x[m][n-2]; // previous x
  int pcy = path_y[m][n-2]; // y

  int p_cx = x, m_cx = x; // plus_copy_x
  int p_cy = y, m_cy = y; // minus_copy_y
  fixcloud(&x,&y,fix,m); // apply normal fix to copy.

  if (n > 1 && x == pcx && y == pcy) {
    fixcloud(&p_cx,&p_cy,fix+8,m); // apply margin around the cells
    if (n > 1 && p_cx == pcx && p_cy == pcy){
    fixcloud(&m_cx,&m_cy,fix-8,m);
    if (n > 1 && m_cx == pcx && m_cy == pcy){ // accept if the pop clears margin.

      /* printf("pop  %.0f: %f %f %d\n", size[m], x, y, n); */
      if (++resistpop <= 20){ // tiny delay
      /* printf(" \n"); */
	return true;
      }
      printf("   \n");
      depth[m]--;
      resistpop=0;
      return true;
    }
    }
      printf("margin deny \n");
      resistpop=0;
      return true;
  }
  resistpop=0;
  return false;
}

void gridify(){
  for (int i = 0; i < 2; i++) {
    // do some copying before touching pointers.
    double fx = baton.X;
    double fy = baton.Y;
    translate_right(&fx, &fy); //right shift
    int x = fx;
    int y = fy;
    int n = depth[i];
    if (pop(i,n,x,y)) continue; // popped, time or margin rejected, skip pushing

    fixcloud(&x,&y,fix,i); // after popping isnt a concern anymore, apply fix to real x and y 


    if (n > 0 && x == path_x[i][n-1] && y == path_y[i][n-1])
      continue; // nothing happens, skip push

    if (n == 8192) continue;
    // push
    path_x[i][n] = x;
    path_y[i][n] = y;
    if (i < 1){
      printf(" %d %d \n", x, y);
    }
    depth[i]++;
  }
}

bool intersect(int i,int n){
  if (i == 0) return true;
  for (int j = 0; j < depth[0]; j++){
    if (path_x[i-1][j] == path_x[i][n]/32){
      if (path_y[i-1][j] == path_y[i][n]/32){
	return true;
      }
    }
  }
  return false;
}

void save_grid(void) {
  FILE *f = fopen("grid.txt", "w");
//_______________________
    fprintf(f, "# route unfiltered \n");
    for (int n = 0; n < depth[1]; n++)
      fprintf(f, "%d %d\n", path_x[1][n], path_y[1][n]);

//_______________________
  for (int i = 1; i >= 0; i--) { // start with bigger grid
    fprintf(f, "# route %d, size %d, depth %d\n", i, size[i], depth[i]);
    for (int n = 0; n < depth[i]; n++)
      if (intersect(i,n)) fprintf(f, "%d %d\n", path_x[i][n], path_y[i][n]);
  }
//_______________________
  fclose(f);
}
