#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_Q 3000
#define NUM_LMR 30000
#define NUM_V 30000

VXparam_t par[] =             /* command line structure            */
{ /* prefix, value,   description                         */   
{    "if=",    0,   " input file"},
{    "of=",    0,   " output file "},
{     0,       0,   0}  /* list termination */
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val


main(argc, argv)
int argc;
char *argv[];
{
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfread(&im,"image10_pre.vx");
Vfembed(&tm,&im,1,1,1,1);
//Vfnewim(&tm,im.type,im.bbx,im.chan);
long countLMR=0;
int queueX[MAX_Q],queueY[MAX_Q];
long front=0,rear=-1,itemCount=0;
int lmrX[NUM_LMR],lmrY[NUM_LMR];

int visited[NUM_V];

int y=0,x=0;//for image index
int m=0,n=0;//count local maximum,m for y axis, n for x axis

int peekX() {
   return queueX[front];
}

int peekY() {
   return queueY[front];
}

bool isEmptyX() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == MAX_Q;
}

int sizeX() {
   return itemCount;
}  

void insertX(int data) {

   if(!isFull()) {
	
      if(rear == MAX_Q-1) {
         rear = -1;            
      }       

     queueX[++rear] = data;
      itemCount++;
   }
}

int removeDataX() {
   int data = queueX[front++];
	
   if(front == MAX_Q) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}

void insertY(int data) {

   if(!isFull()) {
	
      if(rear == MAX_Q-1) {
         rear = -1;            
      }       

      queueY[++rear] = data;
      itemCount++;
   }
}

int removeDataY() {
   int data = queueY[front++];
	
   if(front == MAX_Q) {
      front = 0;
   }
	
   itemCount--;
   return data;  
 }

int y_axis,x_axis;
int countQ=0;//index of element in queue

for(y=tm.ylo+10;y<=tm.yhi-10;y++) //traverse all the pixels in the image
for(x=tm.xlo+10;x<=tm.xhi-10;x++){
//for(y=im.ylo;y<im.ylo+20;y++)
//for(x=im.xlo;x<im.xlo+20;x++){
	 if(tm.u[y][x]>=tm.u[y-1][x-1] && tm.u[y][x]>=tm.u[y-1][x] && tm.u[y][x]>=tm.u[y-1][x+1] //compare the pixel with its 8-neighbours
		&& tm.u[y][x]>=tm.u[y][x-1] && tm.u[y][x]>=tm.u[y][x+1] 
		&& tm.u[y][x]>=tm.u[y+1][x-1] && tm.u[y][x]>=tm.u[y+1][x] && tm.u[y][x]>=tm.u[y+1][x+1]){
			  if(tm.u[y][x]==tm.u[y-1][x-1] || tm.u[y][x]==tm.u[y-1][x] || tm.u[y][x]==tm.u[y-1][x+1] || tm.u[y][x]==tm.u[y][x-1] || tm.u[y][x]==tm.u[y][x+1] 
				  || tm.u[y][x]==tm.u[y+1][x-1] || tm.u[y][x]==tm.u[y+1][x] || tm.u[y][x]==tm.u[y+1][x+1]){//if there is same-intensity neighbor
            if(isEmptyX()){ //queue is empty (traversing the first pixel that has same-intensity neighbor)
                  insertY(y); //add the pixel's y axis and x axis to two queues separately
                  insertX(x);
            }else{ //if the queue is not empty
					    y_axis=peekY(); //return the pixel value's [y,x] axises currently stored in the queue 
              x_axis=peekX();
              if(tm.u[y][x]>tm.u[y_axis][x_axis]){ //if the traversed pixel's intensity is greater than the pixel's in the queue
                while(!isEmptyX()){//empty the queue
                //for(countQ=front;countQ<=rear;countQ++){
                  lmrY[m]=removeDataY();//add those connnexted lmr's axises to 2 arrays
                  lmrX[m]=removeDataX();
                  m++;
                   countLMR++; //increase number of local maxima
                  // fprintf(stderr,"local maxima axis [x,y]=is:%d,%d  intensity:%d\n",x,y,tm.u[y][x]);

					}
           insertY(y);
           insertX(x);
		  }
    }
  }else{//if the pixel's intensity is greater than its 8 neighbors
			lmrY[m]=y; //add the axises to the 2 arrays
			lmrX[m]=x;
      m++;
      countLMR++;
      fprintf(stderr,"no equal, only max!\n");
     // fprintf(stderr,"local maxima axis [x,y]=is:%d,%d\n",x,y);
		}
   }
}

int countM=0;

for(countM=0;countM<=m;countM++){//print out the LMR's location and intensity
  int lmr_intX=lmrX[countM];
  int lmr_intY=lmrY[countM];
  fprintf(stderr,"local maxima [x,y]:%d %d intensity:%d\n",lmrX[countM],lmrY[countM],tm.u[lmr_intY][lmr_intX]);
}
  fprintf(stderr,"number of local maxima is:%l\n",countLMR);
   fprintf(stderr,"yhi=%d,xhi=%d\n",im.yhi,im.xhi);
}
