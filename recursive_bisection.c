#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//#define NUM_POINTS 524288 

#define NUM_POINTS 4 

unsigned int X_axis[NUM_POINTS];
unsigned int Y_axis[NUM_POINTS];

typedef struct point point;

struct point {
  unsigned int x;
  unsigned int y;
};

struct point points[NUM_POINTS];

static int data_cmp(const void *a, const void *b){
  const struct point *da = a, *db = b;

  return da->x < db->x ? -1 : da->x > db->x;
}
 
double euc_dist(unsigned x1, unsigned y1, unsigned x2, unsigned y2){
   double sq_x = pow(x1 - x2, 2);
   double sq_y = pow(y1 - y2, 2);
   
   double sum_sq = sq_x + sq_y;
   double dist = sqrt(sum_sq);
   return dist;
}

void find_quadrants (num_quadrants)
     int num_quadrants;
{
  /* YOU NEED TO FILL IN HERE */

  printf("find_quadrants()\n");

  int i;
  qsort(points, sizeof points / sizeof *points, sizeof *points, data_cmp);
  for(i=0; i<NUM_POINTS; i++)
  { 
     unsigned int x_coord = X_axis[i];
     unsigned int y_coord = Y_axis[i];
     
     //printf("(%d,%d)\n", x_coord, y_coord);
     //double dist_to_origin = euc_dist(x_coord, y_coord, 0, 0);
     //printf("  dist to origin: %f\n", dist_to_origin);
     printf("(%d,%d)\n", points[i].x, points[i].y);
  }  

}

int main(argc,argv)
  int argc;
 char *argv[];
{
  printf("main()\n");
  int num_quadrants;
  int myid, numprocs;
  int  namelen;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
    
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  MPI_Get_processor_name(processor_name,&namelen);

  if (argc != 2)
    {
      fprintf (stderr, "Usage: recursive_bisection <#of quadrants>\n");
      MPI_Finalize();
      exit (0);
    }

  fprintf (stderr,"Process %d on %s\n", myid, processor_name);

  num_quadrants = atoi (argv[1]);

  if (myid == 0)
    fprintf (stdout, "Extracting %d quadrants with %d processors \n", num_quadrants, numprocs);

  if (myid == 0)
    {
      int i;

      srand (10000);
      
      for (i = 0; i < NUM_POINTS; i++)
	X_axis[i] = (unsigned int)rand()%10;

      for (i = 0; i < NUM_POINTS; i++)
	Y_axis[i] = (unsigned int)rand()%10;
      
      for (i = 0; i < NUM_POINTS; i++) {
        point p = {X_axis[i], Y_axis[i]};
	points[i] = p;
      }

    }

  MPI_Bcast(&X_axis, NUM_POINTS, MPI_INT, 0, MPI_COMM_WORLD);  
  MPI_Bcast(&Y_axis, NUM_POINTS, MPI_INT, 0, MPI_COMM_WORLD);  

  find_quadrants (num_quadrants);
 
  MPI_Finalize();
  return 0;
}
  

