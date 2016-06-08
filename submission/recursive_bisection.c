#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define NUM_POINTS 524288
//#define NUM_POINTS 4

unsigned int X_axis[NUM_POINTS];
unsigned int Y_axis[NUM_POINTS];

typedef struct point point;

struct point {
  unsigned int x;
  unsigned int y;
};

struct point points[NUM_POINTS];

void printXAndY(){
  int lengthX = sizeof(X_axis)/sizeof(X_axis[0]);
  int i;

  for (i = 0; i < lengthX; i++){
    printf("point == (%d, %d)\n", X_axis[i], Y_axis[i]);
  }

}

static int data_cmp(const void *a, const void *b){
  const struct point *da = a, *db = b;

  return da->x < db->x ? -1 : da->x > db->x;
}
 
double euc_dist(unsigned x1, unsigned y1, unsigned x2, unsigned y2){

   double sq_x = pow((double)x1 - (double)x2, 2);
   double sq_y = pow((double)y1 - (double)y2, 2);
   
   double sum_sq = sq_x + sq_y;
   double dist = sqrt(sum_sq);
   return dist;
}

double compute_quadrant (int start, int end){
  int i,j;
  double cost = 0;
  for (i = start; i < end; i++){
    for (j = i+1; j < end; j++){
      //printf("X_axis[i],Y_axis[i] == (%d,%d); X_axis[j], Y_axis[j] == (%d,%d)", X_axis[i], Y_axis[i], X_axis[j], Y_axis[j]);
      cost += (euc_dist(X_axis[i], Y_axis[i], X_axis[j], Y_axis[j]));
      //printf("dist == %f\n", cost);
    }
  }
  //printf("dist == %f\n", cost);
  return cost;
}

void print_quad_location(start, end){
  printf("quadrant starts at (%d,%d) and includes all x up to (%d,%d)\n", X_axis[start],Y_axis[start],X_axis[end-1],Y_axis[end-1]);
}

void find_quadrants (num_quadrants)
     int num_quadrants;
{
  double startwtime = 0.0, endwtime;
  int myid, numprocs; 
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  double dist = 0.0;
  double total_dist = 0;
  int quadsPerProc = num_quadrants/numprocs;
  int i = 0;

  if (myid == 0){
    startwtime = MPI_Wtime();
    //printXAndY();
  }
  
  // equal or more quadrants than processors
  if (quadsPerProc >= 1){
    for (i = 0; i < quadsPerProc; i++){
      print_quad_location((myid*quadsPerProc+i)*NUM_POINTS/num_quadrants, (myid*quadsPerProc+i+1)*NUM_POINTS/num_quadrants);
      dist += compute_quadrant(((myid*quadsPerProc+i)*NUM_POINTS/num_quadrants),((myid*quadsPerProc+i+1)*NUM_POINTS/num_quadrants));
    }
  }else{ // less quadrants than processors
    // some processors will be idle
    if (myid < num_quadrants){
      print_quad_location((myid+i)*NUM_POINTS/num_quadrants, (myid+i+1)*NUM_POINTS/num_quadrants);
      dist += compute_quadrant(((myid+i)*NUM_POINTS/num_quadrants),((myid+i+1)*NUM_POINTS/num_quadrants));
    }
  }
 
  MPI_Reduce(&dist, &total_dist, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myid == 0){
    endwtime = MPI_Wtime();
    printf("wall clock time = %f\n", endwtime-startwtime);
    printf("total_dist == %f\n", total_dist); 
  }
}

int main(argc,argv)
  int argc;
 char *argv[];
{
  //printf("main()\n");
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
        X_axis[i] = (unsigned int)rand();

      for (i = 0; i < NUM_POINTS; i++)
        Y_axis[i] = (unsigned int)rand();
      
      for (i = 0; i < NUM_POINTS; i++) {
        point p = {X_axis[i], Y_axis[i]};
        points[i] = p;
      }   
      qsort(points, sizeof points / sizeof *points, sizeof *points, data_cmp);
      for (i = 0; i < NUM_POINTS; i++){
        X_axis[i] = points[i].x;
        Y_axis[i] = points[i].y; 
      } 
    }

  MPI_Bcast(&X_axis, NUM_POINTS, MPI_INT, 0, MPI_COMM_WORLD);  
  MPI_Bcast(&Y_axis, NUM_POINTS, MPI_INT, 0, MPI_COMM_WORLD);  

  find_quadrants (num_quadrants);
 
  MPI_Finalize();
  return 0;
}
  

