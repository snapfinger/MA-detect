#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_Q 30000
#define NUM_LMR 30000
#define NUM_V 30000
#define MIN_DIFF 2
#define MIN_HEIGHT 3
#define MAX_GAP 3


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
//Vfread(&im,"small.vx");
Vfembed(&tm,&im,0,0,0,0);
//Vfnewim(&tm,im.type,im.bbx,im.chan);
long countLMR=0;
int queue[MAX_Q];
long front=0,rear=-1,itemCount=0;
int lmrX[NUM_LMR],lmrY[NUM_LMR];
long label_value=0;
long label[im.yhi][im.xhi];
int y_label=0,x_label=0;

int y=0,x=0;//for image index
int m=0,n=0;//count local maximum,m for y axis, n for x axis

for(y_label=im.ylo;y_label<=im.yhi;y_label++)
  for(x_label=im.xlo;x_label<=im.xhi;x_label++){
      label[y_label][x_label]=0;
  }
 /*
 fprintf(stderr,"\ninitial label:\n");//show intialized labels
  for(y_label=im.ylo;y_label<=im.yhi;y_label++){
    for(x_label=im.xlo;x_label<=im.xhi;x_label++){
      fprintf(stderr,"%d ",im.u[y_label][x_label]);}
      fprintf(stderr,"\n");
    
  }
    fprintf(stderr,"\n");

fprintf(stderr,"\nimage is:\n");
  for(y_label=im.ylo;y_label<=im.yhi;y_label++){//show image pixels
    for(x_label=im.xlo;x_label<=im.xhi;x_label++){
      fprintf(stderr,"%d ",tm.u[y_label][x_label]);}
      fprintf(stderr,"\n");
    
  }
    fprintf(stderr,"\n");
*/
/*define basic operations of queue*/
bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == MAX_Q;
}

void insert(int data) {

   if(!isFull()) {
	
      if(rear == MAX_Q-1) {
         rear = -1;            
      }       

      queue[++rear] = data;
      itemCount++;
   }
}

int removeData() {
   int data = queue[front++];
	
   if(front == MAX_Q) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}


/*find the rough region to search*/
int row_start=im.ylo+1,row_end=im.yhi-1;
int col_start=im.xlo+1,col_end=im.xhi-1;

//use breadth-first search to find local maximum regions
long code=0;
int i=0,j=0;

int offset=im.xhi-im.xlo+1;

int countQ=0;//index of element in queue
label_value=1;

fprintf(stderr,"offset=%d\n",offset);
for(y=row_start;y<=row_end;y++) //traverse all the pixels in the image

  for(x=col_start;x<=col_end;x++){
  
   if(tm.u[y][x]>=240)  {continue;} //do not consider those margin areas
   else if(label[y][x]!=0){continue;}
    
  else if(tm.u[y][x]>=tm.u[y-1][x-1] && tm.u[y][x]>=tm.u[y-1][x] && tm.u[y][x]>=tm.u[y-1][x+1] //compare the pixel with its 8-neighbours

  && tm.u[y][x]>=tm.u[y][x-1] && tm.u[y][x]>=tm.u[y][x+1] 

  && tm.u[y][x]>=tm.u[y+1][x-1] && tm.u[y][x]>=tm.u[y+1][x] && tm.u[y][x]>=tm.u[y+1][x+1] && !label[y][x] ){
     
   
     code=y*offset+x;
    
     insert(code);
     
     while(!isEmpty()){
       code=removeData();
       i=code/offset;//row
       j=code%offset;//column
       if(!label[i][j]){
         label[i][j]=label_value;
         fprintf(stderr,"i=%d j=%d label_value=%d pixel_value=%d\n",i,j,label_value,tm.u[i][j]);
    	   if(j>col_start && i>row_start && tm.u[i][j]==tm.u[i-1][j-1] && !label[i-1][j-1]){ //left-down
          label[i-1][j-1]=label_value;
           insert((i-1)*offset+j-1);
        }
        if(i>row_start && tm.u[i][j]==tm.u[i-1][j] && !label[i-1][j] ){ //down
           im.u[i-1][j]=label_value;
           insert((i-1)*offset+j);
        }
        
        if(i>row_start && j<col_end && tm.u[i][j]==tm.u[i-1][j+1]&& !label[i-1][j+1]){//right-down
           label[i-1][j+1]=label_value;
           insert((i-1)*offset+(j+1));

        }
        if(j<col_end && tm.u[i][j]==tm.u[i][j+1]&& !label[i][j+1]){//right
           label[i][j+1]=label_value;
           insert((i)*offset+(j+1));
        }
        if(i<row_end && j<col_end && tm.u[i][j]==tm.u[i+1][j+1]&& !label[i+1][j+1]){//up-right
            label[i+1][j+1]=label_value;
            insert((i+1)*offset+(j+1));
         }
          if(i<row_end && tm.u[i][j]==tm.u[i+1][j]&& !label[i+1][j]){//up
            label[i+1][j]=label_value;
            insert((i+1)*offset+j);
         }
        if(i<row_end && j>col_start && tm.u[i][j]==tm.u[i+1][j-1] && !label[i+1][j-1]){//up-left
            label[i+1][j-1]=label_value;
            insert((i+1)*offset+(j-1));
         }
        if(j>col_start && tm.u[i][j]==tm.u[i][j-1]&& !label[i][j-1]){//left
           label[i][j-1]=label_value;
           insert((i)*offset+(j-1));
        }
      
         fprintf(stderr,"elements in queue\n");
         for(countQ=front;countQ<=rear;countQ++){
             fprintf(stderr,"%d ",queue[countQ]);
            
           }
         fprintf(stderr,"\n");
         fprintf(stderr,"number=%d \n",itemCount);
         
     } 
     }
      label_value++;
    
  }
   
}
 fprintf(stderr,"end bfs\n");

  
  fprintf(stderr,"\nprocessed label:\n");
  for(y_label=row_start;y_label<=row_end;y_label++){
    for(x_label=col_start;x_label<=col_end;x_label++){
      fprintf(stderr,"%d ",label[y_label][x_label]); 
      }
      fprintf(stderr,"\n");
   
  }
    fprintf(stderr,"\n");
 
    //Vfwrite(&im.OVAL);
    exit(0);
}  
