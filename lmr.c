#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
#include <stdbool.h>

#define MAX_Q 100
#define NUM_LMR 60000
#define MIN_DIFF 2
#define MIN_HEIGHT 3
#define MAX_GAP 3


VXparam_t par[] =       
{ //prefix, value,  description                           
{    "if=",    0,   " input file"},
{     0,       0,   0}  
};

#define  IVAL   par[0].val


main(argc, argv)
int argc;
char *argv[];
{
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfread(&im,"image10_pre.vx");
//Vfread(&im,"small.vx");//for test
Vfembed(&tm,&im,0,0,0,0);
//Vfnewim(&tm,im.type,im.bbx,im.chan);
long countLMR=0;
int queue[MAX_Q];
long front=0,rear=-1,itemCount=0;
long label[im.yhi][im.xhi];
int y_label=0,x_label=0;

int y=0,x=0;//for image index
int m=0,n=0;//count local maximum,m for y axis, n for x axis

struct Outdata{
    int x;
    int y;
    long label;

};
struct Outdata Out[NUM_LMR];


for(y_label=im.ylo;y_label<=im.yhi;y_label++)
  for(x_label=im.xlo;x_label<=im.xhi;x_label++){
      label[y_label][x_label]=0;
  }

//define basic operations of queue
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
long code=0;

int offset=im.xhi-im.xlo+1;
int countQ=0;//index of element in queue
long label_value=1;

fprintf(stderr,"offset=%d\n",offset);
for(y=row_start;y<=row_end;y++) //traverse all the pixels in the image

  for(x=col_start;x<=col_end;x++){
  
   if(tm.u[y][x]>=240)  {continue;} //do not consider those margin areas
   else if(label[y][x]!=0){continue;}
    
  else if(tm.u[y][x]>=tm.u[y-1][x-1] && tm.u[y][x]>=tm.u[y-1][x] && tm.u[y][x]>=tm.u[y-1][x+1] //compare the pixel with its 8-neighbours

  && tm.u[y][x]>=tm.u[y][x-1] && tm.u[y][x]>=tm.u[y][x+1] 

  && tm.u[y][x]>=tm.u[y+1][x-1] && tm.u[y][x]>=tm.u[y+1][x] && tm.u[y][x]>=tm.u[y+1][x+1]){
     
   
     code=y*offset+x;
    
     insert(code);
     
     while(!isEmpty()){
       code=removeData();
       i=code/offset;//row
       j=code%offset;//column
       if(!label[i][j]){
            label[i][j]=label_value;
            Out[countLMR].x=j;
            Out[countLMR].y=i;
            Out[countLMR].label=label_value;
            countLMR++;
         fprintf(stderr,"i=%d j=%d label_value=%d pixel_value=%d\n\n",i,j,label_value,tm.u[i][j]);
    	   if(j>col_start+1 && i>row_start+1 && tm.u[i][j]==tm.u[i-1][j-1] && !label[i-1][j-1] && 
         tm.u[i-1][j-1]>=tm.u[i-2][j-2] && tm.u[i-1][j-1]>=tm.u[i-2][j-1] && tm.u[i-1][j-1]>=tm.u[i-2][j] && tm.u[i-1][j-1]>=tm.u[i-1][j]
         && tm.u[i-1][j-1]>=tm.u[i][j] && tm.u[i-1][j-1]>=tm.u[i][j-1] &&tm.u[i-1][j-1]>=tm.u[i][j-2]&&tm.u[i-1][j-1]>=tm.u[i-1][j-2]){ //left-down
           
              insert((i-1)*offset+j-1);
           
        }
        if(i>row_start && tm.u[i][j]==tm.u[i-1][j] && !label[i-1][j]&&
        tm.u[i-1][j]>=tm.u[i-2][j-1] && tm.u[i-1][j]>=tm.u[i-2][j] && tm.u[i-1][j]>=tm.u[i-2][j+1] && tm.u[i-1][j]>=tm.u[i-1][j+1]
        && tm.u[i-1][j]>=tm.u[i][j+1] && tm.u[i-1][j]>=tm.u[i][j] &&tm.u[i-1][j]>=tm.u[i][j-1]&&tm.u[i-1][j]>=tm.u[i-1][j-1]){ //down
         
           insert((i-1)*offset+j);
           
        }
        
        if(i>row_start+1 && j<col_end-1 && tm.u[i][j]==tm.u[i-1][j+1]&& !label[i-1][j+1] &&
        tm.u[i-1][j+1]>=tm.u[i-2][j] && tm.u[i-1][j+1]>=tm.u[i-2][j+1] && tm.u[i-1][j+1]>=tm.u[i-2][j+2] && tm.u[i-1][j+1]>=tm.u[i-1][j+2]
        && tm.u[i-1][j+1]>=tm.u[i][j+2] && tm.u[i-1][j+1]>=tm.u[i][j+1] &&tm.u[i-1][j+1]>=tm.u[i-1][j]){//right-down
           insert((i-1)*offset+(j+1));

        }
        if(j<col_end-1 && tm.u[i][j]==tm.u[i][j+1]&& !label[i][j+1] &&
        tm.u[i][j+1]>=tm.u[i-1][j] && tm.u[i][j+1]>=tm.u[i-1][j+1] && tm.u[i][j+1]>=tm.u[i-1][j+2] && tm.u[i][j+1]>=tm.u[i][j+2]
        && tm.u[i][j+1]>=tm.u[i+1][j+2] && tm.u[i][j+1]>=tm.u[i+1][j+1] &&tm.u[i][j+1]>=tm.u[i+1][j]){//right
           insert((i)*offset+(j+1));
        }
        if(i<row_end-1 && j<col_end-1 && tm.u[i][j]==tm.u[i+1][j+1]&& !label[i+1][j+1] &&
        tm.u[i+1][j+1]>=tm.u[i][j+1]  && tm.u[i+1][j+1]>=tm.u[i][j+2] && tm.u[i+1][j+1]>=tm.u[i+1][j+2]
        && tm.u[i+1][j+1]>=tm.u[i+2][j+2] && tm.u[i+1][j+1]>=tm.u[i+2][j+1] &&tm.u[i+1][j+1]>=tm.u[i+2][j]&& tm.u[i+1][j+1]>=tm.u[i+1][j]){//up-right
            insert((i+1)*offset+(j+1));
         }
        if(i<row_end-1 && tm.u[i][j]==tm.u[i+1][j]&& !label[i+1][j] &&
        tm.u[i+1][j]>=tm.u[i][j+1] && tm.u[i+1][j]>=tm.u[i+1][j+1] && tm.u[i+1][j]>=tm.u[i+2][j+1]
        && tm.u[i+1][j]>=tm.u[i+2][j] && tm.u[i+1][j]>=tm.u[i+2][j-1] &&tm.u[i+1][j]>=tm.u[i+1][j-1]&& tm.u[i+1][j]>=tm.u[i][j-1]){//up
            insert((i+1)*offset+j);
         }
        if(i<row_end-1 && j>col_start+1 && tm.u[i][j]==tm.u[i+1][j-1] && !label[i+1][j-1] &&
        tm.u[i+1][j-1]>=tm.u[i][j-2] && tm.u[i+1][j-1]>=tm.u[i][j-1] && tm.u[i+1][j-1]>=tm.u[i+1][j]
        && tm.u[i+1][j-1]>=tm.u[i+2][j] && tm.u[i+2][j-1]>=tm.u[i+2][j-1] &&tm.u[i+1][j-1]>=tm.u[i+2][j-2]&& tm.u[i+1][j-1]>=tm.u[i+1][j-2]){//up-left
            insert((i+1)*offset+(j-1));
         }
        if(j>col_start+1 && tm.u[i][j]==tm.u[i][j-1]&& !label[i][j-1] && 
        tm.u[i][j-1]>=tm.u[i-1][j-2] && tm.u[i][j-1]>=tm.u[i-1][j-1] && tm.u[i][j-1]>=tm.u[i-1][j]  
        && tm.u[i][j-1]>=tm.u[i+1][j] && tm.u[i][j-1]>=tm.u[i+1][j-1] &&tm.u[i][j-1]>=tm.u[i+1][j-2]&&tm.u[i][j-1]>=tm.u[i][j-2]){//left
           insert((i)*offset+(j-1));
        }
      
         fprintf(stderr,"elements in queue: ");
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
  

FILE *f=fopen("lmrout.csv","w"); 
if(f==NULL) fprintf(stderr,"can't open such file\n");

fprintf(stderr,"output is:\n");
for(i=0;i<countLMR;i++){
  fprintf(stderr,"%d,%d,%ld\n",Out[i].y,Out[i].x,Out[i].label);
  fprintf(f,"%d,%d,%ld\n",Out[i].y,Out[i].x,Out[i].label);
}

 fclose(f); 
 fprintf(stderr,"total lmr pixels:%ld\n",countLMR);
 exit(0);
}


