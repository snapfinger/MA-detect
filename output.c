#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
#include <stdbool.h>
/*
VXparam_t par[] =       
{ //prefix, value,  description                           
{    "if=",    0,   " input file"},
{     0,       0,   0}  
};
*/
#define  IVAL   par[0].val
#define OVAL "image10_myout.vx"
#define  MAX_LMR 60000
#define MAX_BUFFER 200
#define MAX_MA 300




int main(int argc, char** argv){

Vfstruct (im);                      
Vfstruct (tm);                   
Vfread(&im,"image10_pre.vx");
Vfembed(&tm, &im, 0,0,0,0);

struct Filedata{
    int y;
    int x;
    long label;
    float score;
    int classf;
    
};
struct Filedata Data[MAX_LMR],input[MAX_LMR],output[MAX_MA];

long i;

FILE *fp_in;

fp_in=fopen("mocktest.csv","r");
char buffer[MAX_BUFFER];
size_t read_i;

if(!fp_in){
  fprintf(stderr,"failed to open local maximum record file!\n");
}else{
  for(read_i=0;fgets(buffer,sizeof buffer,fp_in);read_i++){
     fprintf(stderr,"buffer=%s\n",buffer);
      sscanf(buffer,"%d,%d,%ld,%f,%d\n",&Data[read_i].y,&Data[read_i].x,&Data[read_i].label,&Data[read_i].score,&Data[read_i].classf);
      fprintf(stderr,"y=%d x=%d,label=%ld,score=%f,class=%d\n",Data[read_i].y,Data[read_i].x,Data[read_i].label,Data[read_i].score,Data[read_i].classf);
    }
  }
fclose(fp_in);

fprintf(stderr,"read_i=%ld\n",read_i);

fprintf(stderr,"\n");

long count_in=0;
for(i=0;i<read_i;i++){
  if((int)Data[i].classf){
    input[count_in].y=Data[i].y;
    input[count_in].x=Data[i].x;
    input[count_in].label=Data[i].label;
    input[count_in].score=Data[i].score;
    input[count_in].classf=Data[i].classf;
    count_in++;
  }
}

for(i=0;i<count_in;i++){
  fprintf(stderr,"y=%d x=%d score=%f class=%d label=%ld\n",input[i].y,input[i].x,input[i].score,input[i].classf,input[i].label);
}
fprintf(stderr,"\ncount_in=%ld\n",count_in);
fprintf(stderr,"\nfor output\n");


long start=0,count_out=0;

while(start<count_in){
  long label_value=input[start].label; 
  float max_score=input[start].score;
  i=start+1;
   fprintf(stderr,"label=%ld score=%f \n",input[i].label,input[i].score); 
  if(input[i].label!=label_value){
    
      output[count_out].y=input[start].y;
      output[count_out].x=input[start].x;
      output[count_out].score=input[start].score;
      output[count_out].classf=input[start].classf;
      output[count_out].label=input[start].label;
      
      start=i;
      count_out++;

  }else{
    while(input[i].label==label_value){
      output[count_out].y=input[start].y;
      output[count_out].x=input[start].x;
      output[count_out].score=input[start].score;
      output[count_out].classf=input[start].classf;
      output[count_out].label=input[start].label;
        if(input[i].score>max_score){
          max_score=input[i].score;
          output[count_out].y=input[i].y;
          output[count_out].x=input[i].x;
          output[count_out].score=input[i].score;
          output[count_out].classf=input[i].classf;
          output[count_out].label=input[i].label;
      
      }
      i++;
    }
    start=i;
    count_out++;
  }
}


fprintf(stderr,"\ncount_out=%ld\n",count_out);

for(i=0;i<count_out;i++){
  fprintf(stderr,"y=%d x=%d score=%f class=%d label=%ld\n",output[i].y,output[i].x,output[i].score,output[i].classf,output[i].label);
}

FILE *fp_out;
fp_out=fopen("coout.csv","w");

if(!fp_out){
  fprintf(stderr,"failed to open cooridinate output for writing!\n");
}else{
  for(i=0;i<count_out;i++){
    fprintf(stderr,"write %d,%d\n",output[i].y,output[i].x);
    fprintf(fp_out,"%d,%d\n",output[i].y,output[i].x);
  }
}
fclose(fp_out);

//take from Yiting Xie's code, for making annotations in output images
for(i=0;i<count_out;i++){
  int val=5;//intensity of annotation
  float y=(float)output[i].y;
  float x=(float)output[i].x;
  double r=30;//radius
  double f, fi, fj;
  int d_i, d_j;

    for (f = 0; f < 2 * M_PI; f += 0.01) {
      fj = r * sin (f) + y;
      fi = r * cos (f) + x;

      if (ABS (fj - (int) fj) < 0.5)
        d_j = fj;
      else
        d_j = fj + 1;

      if (ABS (fi - (int) fi) < 0.5)
        d_i = fi;
      else
        d_i = fi + 1;

      if ((tm.bbx[0] <= d_i) && (d_i < tm.bbx[1]) &&
          (tm.bbx[2] <= d_j) && (d_j < tm.bbx[3])) {
             tm.u[d_j][d_i] = val;
      }
    }
  
 }


Vfwrite(&tm, OVAL);
exit(0);
}