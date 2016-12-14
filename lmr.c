#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
#include <stdbool.h>


#define NUM_LMR 60000
#define MIN_DIFF 2
#define MIN_HEIGHT 3
#define MAX_GAP 3
#define QUEUESIZE 400
#define VISITED 0

VXparam_t par[] =       
{ //prefix, value,  description                           
{    "if=",    0,   " input file"},
{    "of=",    0,   " output file"},
{     0,       0,   0}  
};

#define  IVAL   par[0].val
#define  OVAL   par[1].val


typedef struct{
    long data[QUEUESIZE];
    int front,rear;        
}SeqQueue;

void InitQueue(SeqQueue *SQ){
      SQ->front =0;
      SQ->rear = 0;
}


int QueueEmpty(SeqQueue SQ){
    if(SQ.front == SQ.rear){
         return 1;
    }else{
        return 0;      
    }
}

int QueueLength(SeqQueue Q)
{
	return  (Q.rear-Q.front+QUEUESIZE)%QUEUESIZE;
}

int EnQueue(SeqQueue *Q,long e)
{
	if ((Q->rear+1)%QUEUESIZE== Q->front)	
		return 0;
	Q->data[Q->rear]=e;			
	Q->rear=(Q->rear+1)%QUEUESIZE;
	  return 1;
}

DeQueue(SeqQueue *Q){
  long e;
	if (Q->front == Q->rear)		
		return 0;
	e=Q->data[Q->front];			
	Q->front=(Q->front+1)%QUEUESIZE;
								
	return  e;
}

main(argc, argv)
int argc;
char *argv[];
{
VXparse(&argc,&argv,par);
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfstruct (om);
Vfread(&im,IVAL);
Vfembed(&tm,&im,0,0,0,0);
Vfembed(&om,&im,0,0,0,0);
long countLMR=0;
long label[im.yhi][im.xhi];


int y=0,x=0;//for image index
int m=0,n=0;//count local maximum,m for y axis, n for x axis



struct Outdata{
    int x;
    int y;
    long label;

};
struct Outdata Out[NUM_LMR];

SeqQueue SQ;
InitQueue(&SQ);


for(y=0;y<=1500;y++)
  for(x=0;x<=1500;x++){
      label[y][x]=0;
  }

int i=0,j=0;
int row_start=im.ylo+1,row_end=im.yhi-1;
int col_start=im.xlo+1,col_end=im.xhi-1;

//find the rough region to search
for(i=0;i<im.xhi;i++){
  if(im.u[im.yhi/2][i]<240){
	col_start=i;
	break;
  }
}

for(i=im.xhi-1;i>0;i--){
   if(im.u[im.yhi/2][i]<240){
	col_end=i;
	break;
  }
}

//use breadth-first search to find local maximum regions
long code=0,element=0;
int countQ=0;
long offset=im.xhi-im.xlo+1;

long label_value=1;

fprintf(stderr,"offset=%d\n",offset);
for(y=row_start;y<=row_end;y++) //traverse all the pixels in the image
  for(x=col_start;x<=col_end;x++){
   if(tm.u[y][x]>=240 ||tm.u[y][x]<130)  
      {continue;} //do not consider those bright margin areas and set the low threshold for those can't be MA 
   else if(im.u[y][x]==VISITED)
      {continue;}
   else if(tm.u[y][x]>=tm.u[y-1][x-1] && tm.u[y][x]>=tm.u[y-1][x] && tm.u[y][x]>=tm.u[y-1][x+1] //compare the pixel with its 8-neighbours
   && tm.u[y][x]>=tm.u[y][x-1] && tm.u[y][x]>=tm.u[y][x+1] && tm.u[y][x]>=tm.u[y+1][x-1] && tm.u[y][x]>=tm.u[y+1][x] && tm.u[y][x]>=tm.u[y+1][x+1]){
   
       code=y*offset+x;
       fprintf(stderr,"enqueue: %ld %d %d\n",code,y,x);
       EnQueue(&SQ,code);
       while(!QueueEmpty(SQ)){
         code=DeQueue(&SQ);
         i=code/offset;//row
         j=code%offset;//column
         
         if(tm.u[i][j]>=tm.u[i-1][j-1] && tm.u[i][j]>=tm.u[i-1][j] && tm.u[i][j]>=tm.u[i-1][j+1] //compare the pixel with its 8-neighbours
         && tm.u[i][j]>=tm.u[i][j-1] && tm.u[i][j]>=tm.u[i][j+1] && tm.u[i][j]>=tm.u[i+1][j-1] && tm.u[i][j]>=tm.u[i+1][j] && tm.u[i][j]>=tm.u[i+1][j+1] &&im.u[i][j]){      
                 
            fprintf(stderr,"code=%ld i=%d j=%d im[i][j]=%d\n",code,i,j,im.u[i][j]);
            im.u[i][j]=VISITED;
            Out[countLMR].x=j;
            Out[countLMR].y=i;
            Out[countLMR].label=label_value;
            countLMR++;

    	if(j>=col_start+1 && i>=row_start+1 && tm.u[i][j]==tm.u[i-1][j-1] && im.u[i-1][j-1] ){ //left-down  
          element=(i-1)*offset+j-1;
          EnQueue(&SQ,element);
          //fprintf(stderr,"left-down enqueue: %ld %d %d\n",element,(i-1),j);
        }
        if(i>=row_start+1 && tm.u[i][j]==tm.u[i-1][j] && im.u[i-1][j]){ //down  
          element=(i-1)*offset+j;
          EnQueue(&SQ,element);
          //fprintf(stderr,"down enqueue: %ld %d %d\n",element,i-1,j);
        }
        
        if(i>=row_start+1 && j<=col_end-1 && tm.u[i][j]==tm.u[i-1][j+1]&& im.u[i-1][j+1] ){//right-down
          element=(i-1)*offset+(j+1);
          EnQueue(&SQ,element);
         //fprintf(stderr,"right-down enqueue: %ld %d %d\n",element,i-1,j+1);
        }
        if(j<=col_end-1 && tm.u[i][j]==tm.u[i][j+1]&& im.u[i][j+1] ){//right
           element=(i)*offset+(j+1);
           EnQueue(&SQ,element);
         //fprintf(stderr,"right enqueue: %ld %d %d\n",element,i,j+1);
        }
        //fprintf(stderr,"up-right label: %ld \n",label[i+1][j+1]);
        if(i<=row_end-1 && j<=col_end-1 && tm.u[i][j]==tm.u[i+1][j+1]&& im.u[i+1][j+1]){//up-right
           element=(i+1)*offset+(j+1);
           EnQueue(&SQ,element);
         //fprintf(stderr,"up-right enqueue: %ld %d %d\n",element,i+1,j+1);
        }
       // fprintf(stderr,"up label: %ld \n",label[i+1][j]);
        if(i<=row_end-1 && tm.u[i][j]==tm.u[i+1][j]&& im.u[i+1][j]){//up
           element=(i+1)*offset+j;
           EnQueue(&SQ,element);
         //fprintf(stderr,"up enqueue: %ld %d %d\n",element,i+1,j);
        }
        if(i<=row_end-1 && j>=col_start+1 && tm.u[i][j]==tm.u[i+1][j-1] && im.u[i+1][j-1] ){//up-left
           element=(i+1)*offset+(j-1);
           EnQueue(&SQ,element);
          //fprintf(stderr,"up-left enqueue: %ld %d %d\n",element,i+1,j-1);
        }
        if(j>=col_start+1 && tm.u[i][j]==tm.u[i][j-1]&& im.u[i][j-1] ){//left
           element=(i)*offset+(j-1);
           EnQueue(&SQ,element);
          //fprintf(stderr,"left enqueue: %ld %d %d\n",element,i,j-1);
        }
        
        fprintf(stderr,"%d elements in queue: ",QueueLength(SQ));
        for(countQ=SQ.front;countQ<=SQ.rear;countQ++){
           fprintf(stderr,"%d ",SQ.data[countQ]);
        }
        fprintf(stderr,"\n\n");
         
       } 
    }      
    label_value++;
    }
    
 }
fprintf(stderr,"row_end=%d col_end=%d\n",row_end,col_end);

 fprintf(stderr,"end bfs\n");
  
for(i=0;i<countLMR;i++){
  om.u[Out[i].y][Out[i].x]=0;
}
Vfwrite(&om, OVAL); 

FILE *f=fopen("lmrout.csv","w"); 
if(f==NULL) fprintf(stderr,"can't open such file\n");

fprintf(stderr,"output is:\n");
for(i=0;i<countLMR;i++){
  fprintf(stderr,"Y=%d,X=%d,pixel=%d,label=%ld\n",Out[i].y,Out[i].x,tm.u[Out[i].y][Out[i].x],Out[i].label);
  fprintf(f,"%d,%d,%ld\n",Out[i].y,Out[i].x,Out[i].label);
}

 fclose(f); 
 fprintf(stderr,"total lmr pixels:%ld\n",countLMR);
 exit(0);
 
}


