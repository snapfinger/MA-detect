/*peak detection in a profile*/
#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
//#include <string.h>
//#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
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
//int p[ELEMENT]={2,3,12,13,14,15,16,18,15,14,10,4};
int p[ELEMENT]={24,22,20,18,16,15,12,10,8,6,4,2};//test:only decreasing ramp
//int p[ELEMENT]={2,4,6,8,10,12,14,16,18,20,22,24};//test:only increasing ramp
int i=0,j=0;//corresponding angel pointer:0 degree->0, 6 degree->1,etc.
int max=0,max_d=0,inc_s=0,inc_e=0,dec_s=0,dec_e=0;
int gap[ELEMENT]={0},inc[ELEMENT]={0},count_gap=0,count_inc=0,stop=0;
int gap_dec[ELEMENT]={0},dec[ELEMENT]={0},count_gapd=0,count_dec=0;






for(i=0;i<ELEMENT;i++){ //peak detection
  if(p[i]>max){
    max=p[i];//record peak value
    inc_e=i;//record peak index
    }
}


/*find starting point of decreasing ramp*/
for(i=inc_e;i<ELEMENT;i++){
  if(max-p[i]>THRESH){
      max_d=p[i-1];
      dec_s=i-1;
      break;
  }
}

/*calculate elements in gap[]   (increasing)*/
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

/*calculate elements in gap_dec[]  (decreasing)*/
for(j=dec_s;j<ELEMENT-1;j++){
  if(p[j]-p[j+1]>=MIN_DIFF){
    gap_dec[count_gapd]=j;
    count_gapd++;
    }else{
        if(p[j-1]-p[j]>=MIN_DIFF){
        gap_dec[count_gapd]=j;
        count_gapd++;
        }
    }
}


if(p[ELEMENT-2]-p[ELEMENT-1]>=MIN_DIFF){
  gap_dec[count_gapd++]=ELEMENT-1;
}






/*store increasing ramp's indexes in inc[]*/
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

/*store decreasing ramp's indexes in dec[]*/
for(j=0;j<count_gapd;j++){
  if(abs(gap_dec[j]-gap_dec[j+1])>MAX_GAP){
    if(abs(gap_dec[j-1]-gap_dec[j])<=MAX_GAP){
      dec[j]=gap_dec[j];
      count_dec++;
    }
    break;
  }else{
    dec[j]=gap_dec[j];
    count_dec++;
  }
}

if(gap_dec[count_gapd-1]-gap_dec[count_gapd-2]>MAX_GAP){

}

dec_e=dec[count_dec-1];


/**************** print out code for testing stored data of ramp*******************************/
fprintf(stderr,"info about the ramp...\n");
fprintf(stderr,"there are %d elements in p[], the elements are:\n",ELEMENT);
for(i=0;i<ELEMENT;i++){
  fprintf(stderr,"%d ",p[i]);
}
fprintf(stderr,"\n\n");

fprintf(stderr,"ramp increases end at index %d,that's value %d \n",inc_e,p[inc_e]);
fprintf(stderr,"ramp decreses from index %d,that's value %d \n\n",dec_s,p[dec_s]);


fprintf(stderr,"number of elements in gap[] is:%d, the elements' value are:\n",count_gap);
for(j=0;j<count_gap;j++){
  fprintf(stderr,"%d ",p[gap[j]]);
}
fprintf(stderr,"\n");

fprintf(stderr,"corresponding indexes in gap[] are: \n");
for(j=0;j<count_gap;j++){
  fprintf(stderr,"%d ",gap[j]);
}
fprintf(stderr,"\n");

fprintf(stderr,"index of elements in inc ramp are:\n");
for(j=0;j<count_inc;j++){
  fprintf(stderr,"%d ",inc[j]);
}
fprintf(stderr,"\n");

fprintf(stderr,"corresponding values of elements in inc ramp are:\n");
for(j=0;j<count_inc;j++){
  fprintf(stderr,"%d ",p[inc[j]]);
}
fprintf(stderr,"\n");

fprintf(stderr,"number of elements in inc[] is:%d \n",count_inc);
fprintf(stderr,"increasing ramp starts from: %d ends at:%d \n\n",inc_s,inc_e);



fprintf(stderr,"number of elements in gap_dec[] is:%d, the elements' value are:\n",count_gapd);
for(j=0;j<count_gapd;j++){
  fprintf(stderr,"%d ",p[gap_dec[j]]);
}
fprintf(stderr,"\n");
fprintf(stderr,"corresponding indexes in gap_dec[] are: \n");
for(j=0;j<count_gapd;j++){
  fprintf(stderr,"%d ",gap_dec[j]);
}
fprintf(stderr,"\n");
fprintf(stderr,"index of elements in dec ramp are:\n");
for(j=0;j<count_dec;j++){
  fprintf(stderr,"%d ",dec[j]);
}

fprintf(stderr,"\n");
fprintf(stderr,"corresponding p values of elements in dec ramp are:\n");
for(j=0;j<count_dec;j++){
  fprintf(stderr,"%d ",p[dec[j]]);
}
fprintf(stderr,"\n");

fprintf(stderr,"number of elements in dec[] is:%d \n",count_dec);
fprintf(stderr,"decreasing ramp starts from: %d ends at:%d \n\n",dec_s,dec_e);

/*********************calculate profile property*************************************/
int w_peak=0,w_top=0,h_inc=0,h_dec=0,h_peak=0;//profile properties
float s_inc=0,s_dec=0; //increasing and decreasing slope

//if only exists rising ramp, deceasing index starts and ends at last index 
if(inc_e==ELEMENT-1){
  dec_s=ELEMENT-1;
  dec_e=ELEMENT-1;
}
 //if only exists descending ramp, increasing index starts and ends at first index
if(dec_s==0){
  inc_s=0;
  inc_e=0;
}

w_top=dec_s-inc_e;
w_peak=dec_e-inc_s;
h_inc=p[inc_e]-p[inc_s];
h_dec=p[dec_s]-p[dec_e];

//calculate increasing slope (rise&descend / only descending ramp)
if(inc_s!=inc_e){
  s_inc=(float)(h_inc/(inc_e-inc_s));
}else{
  s_inc=INT_MAX;
}
//calculate decreasing slope (rise&descend / only rising ramp)
if(dec_s!=dec_e){
  s_dec=(float)(h_dec/(dec_e-dec_s));
}else{
  s_dec=INT_MAX;//decreasing slope is also considered as positive
}


fprintf(stderr,"start calculate properties...\n");
fprintf(stderr,"inc_s=%d inc_e=%d dec_s=%d dec_e=%d  \n",inc_s,inc_e,dec_s,dec_e);
fprintf(stderr,"w_top is:%d \n",w_top);
fprintf(stderr,"w_peak is:%d \n",w_peak);
fprintf(stderr,"h_inc is:%d \n",h_inc);
fprintf(stderr,"h_dec is:%d \n",h_dec);
fprintf(stderr,"s_inc is:%f \n",s_inc);
fprintf(stderr,"s_dec is:%f \n",s_dec);
}


