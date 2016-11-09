/*peak detection in a profile*/
#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
//#include <string.h>
//#include <stdlib.h>
#include <stdbool.h>
//#include "angle.h"
#define MIN_DIFF 2
#define MIN_HEIGHT 3
#define MAX_GAP 3
#define THRESH 1
#define ELEMENT 12//number of elements in array p[]

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
//int p[ELEMENT]={1,3,4,5,12,14,10,8,6,3,2};
int p[ELEMENT]={2,3,12,13,14,15,16,18,15,14,10,4};
int i=0,j=0;//corresponding angel pointer:0 degree->0, 6 degree->1,etc.
int max=0,max_d=0,inc_s=0,inc_e=0,dec_s=0,dec_e=0;
int gap[ELEMENT]={0},inc[ELEMENT]={0},count_gap=0,count_inc=0,count_dec=0,stop=0;
int gap_dec[ELEMENT]={0},count_gapd=0;



fprintf(stderr,"there are %d elements in p[], the elements are:\n",ELEMENT);
for(i=0;i<ELEMENT;i++){
  fprintf(stderr,"%d ",p[i]);
}
fprintf(stderr,"\n");


for(i=0;i<ELEMENT;i++){ //peak detection
  if(p[i]>max){
    max=p[i];//record peak value
    inc_e=i;//record peak index
    }
}
fprintf(stderr,"max is %d ,inc_e index is %d \n",max,inc_e);


for(i=inc_e;i<ELEMENT;i++){
  if(max-p[i]>THRESH){
      max_d=p[i-1];
      dec_s=i;
      break;
  }
}
fprintf(stderr,"max_d is %d ,ramp decreses from index %d,that's %d \n",max_d,dec_s,p[dec_s]);

for(j=inc_e;j>0;j--){
  if(p[j]-p[j-1]>=MIN_DIFF){
    gap[count_gap]=j;
    count_gap++;
  }else{
      if(p[j+1]-p[j]>=MIN_DIFF){
        gap[count_gap]=j;
        count_gap++;
      }
    }
}

if(p[1]-p[0]>=MIN_DIFF){
  gap[count_gap++]=0;
}


for(j=dec_s;j<ELEMENT;j++){
  if(p[j]-p[j+1]>=MIN_DIFF){
    gap_dec[count_gapd]=j;
    count_gapd++;
    }else{
      
    }
}


if(p[ELEMENT-2]-p[ELEMENT-1]>=MIN_DIFF){
  gap_dec[count_gapd++]=ELEMENT-1;
}

fprintf(stderr,"number of elements in gap[] is:%d, the elements' value are:\n",count_gap);
for(j=0;j<count_gap;j++){
  fprintf(stderr,"%d ",p[gap[j]]);
}
fprintf(stderr,"\n");


fprintf(stderr,"number of elements in gap_dec[] is:%d, the elements' value are:\n",count_gapd);
for(j=0;j<count_gapd;j++){
  fprintf(stderr,"%d ",p[gap_dec[j]]);
}
fprintf(stderr,"\n");




for(j=0;j<count_gap;j++){
  if(gap[j]-gap[j+1]>MAX_GAP){
    if(gap[j-1]-gap[j]<=MAX_GAP){
      inc[j]=gap[j];
      count_inc++;
    }
    break;
  }else{
    inc[j]=gap[j];
    count_inc++;
  }
}

inc_s=inc[count_inc-1];

fprintf(stderr,"elements in inc[] are:\n");
for(j=0;j<count_inc;j++){
  fprintf(stderr,"%d ",inc[j]);
}
fprintf(stderr,"\n");
fprintf(stderr,"number of elements in inc[] is:%d \n",count_inc);
fprintf(stderr,"increasing ramp starts from: %d ends at:%d \n",inc_s,inc_e);


int w_peak=0,w_top=0,h_inc=0,h_dec=0,h_peak=0;//profile properties
w_top=dec_s-inc_e;
w_peak=dec_e-inc_s;
fprintf(stderr,"inc_s=%d inc_e=%d dec_s=%d dec_e=%d  \n",inc_s,inc_e,dec_s,dec_e);
fprintf(stderr,"w_top is:%d \n",w_top);
fprintf(stderr,"w_peak is:%d \n",w_peak);

}


