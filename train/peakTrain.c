#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
#include <string.h>
#include <math.h> 
#include <stdbool.h>
#define ARRAY_WIDTH 3
#define MAX_LMR 300 //in this file, this stands for number of training data
#define ARRAYSIZE(x)  (sizeof(x)/sizeof(*(x)))

#define MIN_DIFF 2
#define MIN_HEIGHT 3
#define MAX_GAP 3
#define THRESH_TOP 1//the intensity range of the top
#define SLOPE_MAX 500
#define ELEMENT 31//number of elements in array p[]
#define NUM_PRO 30//corresponds to 6 degree cross profile
#define I 11//for test


int main(int argc, char** argv){

int p[NUM_PRO][ELEMENT];

int count_pro=0,count_ele=0;

int max[NUM_PRO],max_d[NUM_PRO];
int inc_s[NUM_PRO],inc_e[NUM_PRO],dec_s[NUM_PRO],dec_e[NUM_PRO],center[NUM_PRO];//pixel indexes 
int gap[NUM_PRO][ELEMENT],inc[NUM_PRO][ELEMENT],count_gap[NUM_PRO],count_inc[NUM_PRO],stop[NUM_PRO];
int gap_dec[NUM_PRO][ELEMENT],dec[NUM_PRO][ELEMENT],count_gapd[NUM_PRO],count_dec[NUM_PRO];

int xoffset[NUM_PRO][ELEMENT];
int yoffset[NUM_PRO][ELEMENT];

int array_in=0;
int y_in=0,x_in=0;

int xx=0,yy=0;//count element
double theta=0;
long k=0,i=0,j=0;
FILE *fp_in1;
FILE *fp_out;

//VXparse(&argc,&argv,par);

int array[MAX_LMR][ARRAY_WIDTH];
long idx = 0;
long time=0;

struct Filedata{
    char name[40];
    char x[20];
    char y[20];
    int read_x;
    int read_y;

};
struct Filedata Data[MAX_LMR];

int count_out=0;
struct Outdata{
    char name[40];
    int x_out;
    int y_out;
    float mpwidths;
    float sdpwidths;
    float mtwidths;
    float sdtwidths;
    float sdrslopes;
    float cvrheights;
    float cvpheights;
};
struct Outdata Out[MAX_LMR];

//read data from csv
fp_in1=fopen("annotation_ma.csv","r");
char buffer[MAX_LMR],*ptr;
size_t read_i=0,read_j=0,read_k=0;


if(!fp_in1){
  fprintf(stderr,"failed to open training name data!\n");
}else{
    char line[40];
    char *pt;
    char *read,*record;
    while (fgets(line, 60, fp_in1)){
      
           strcpy(Data[read_i].name,strtok(line,","));        
           strcpy(Data[read_i].x,strtok(NULL,","));
           strcpy(Data[read_i].y,strtok(NULL,","));
           
           Data[read_i].read_x=atoi(Data[read_i].x);
           Data[read_i].read_y=atoi(Data[read_i].y);
      
          
           read_i++;
    }
  }

fclose(fp_in1);




//start read input csv file
//input y,x axises, and corresponding label value
for(array_in=0;array_in<read_i;array_in++){

  Vfstruct(im);
  

   char input2[3]=".vx";
   strcat(Data[array_in].name,input2);
   
   Vfread(&im,Data[array_in].name);

   x_in=Data[array_in].read_x;
   y_in=im.yhi-Data[array_in].read_y; //invert y-axis here, due to original annotations 
   
    fprintf(stderr,"\nfile=%s\n",Data[array_in].name);
    fprintf(stderr,"y=%d\n",x_in);
    fprintf(stderr,"x=%d\n",y_in);

  

//profile scanning
for(k = 0; k<NUM_PRO; k++){
   theta = 6 * k;  //angle
   if(theta<45 || theta >135){
    i = 0;
    for (xx = -15; xx <= 15; xx++){				
          yy = round(xx*tan(theta / 180.0*3.1415926));
  	       xoffset[k][i] = xx;
	         yoffset[k][i] = yy;
				   i++;
       }
	}else{
      i = 0;
      for (yy = -15; yy <= 15; yy++){
         xx = round(yy / tan(theta / 180.0*3.1415926));
		     xoffset[k][i] = xx;
         yoffset[k][i] = yy;		
         i++;
       }
		}
}
	
//Get their pixel values
for (k = 0; k < NUM_PRO; k++){
    int xx,yy;
    for(i=0;i<ELEMENT;i++){ 
       xx=xoffset[k][i];
       yy=yoffset[k][i];
       p[k][i]=im.u[y_in+yy][x_in+xx];
       }
}



for(i=0;i<NUM_PRO;i++)
  for(j=0;j<ELEMENT;j++){
    gap[i][j]=0;
    inc[i][j]=0;
    gap_dec[i][j]=0;
    dec[i][j]=0;
}

for(i=0;i<NUM_PRO;i++){
  max[i]=0;
  max_d[i]=0;
  inc_s[i]=0;
  inc_e[i]=ELEMENT/2;
  dec_s[i]=ELEMENT/2+1;
  dec_e[i]=ELEMENT-1;
  center[i]=ELEMENT/2;
  stop[i]=0;
  count_gap[i]=0;
  count_inc[i]=0;
  count_gapd[i]=0;
  count_dec[i]=0;
}
  

for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  for(count_ele=0;count_ele<ELEMENT;count_ele++){ //peak detection
    if(p[count_pro][count_ele]>max[count_pro]){
      max[count_pro]=p[count_pro][count_ele];//record peak value
      inc_e[count_pro]=count_ele;//record peak index, also the end index of rising ramp
    }
  }
}


//find starting point of decreasing ramp
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  for(count_ele=inc_e[count_pro];count_ele<ELEMENT;count_ele++){
    if(max[count_pro]-p[count_pro][count_ele]>THRESH_TOP){
      max_d[count_pro]=p[count_pro][count_ele-1];
      dec_s[count_pro]=count_ele-1;
      break;
    }
  }
}



//calculate elements in gap[]   (increasing)
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  for(j=inc_e[count_pro];j>0;j--){
    if(p[count_pro][j]-p[count_pro][j-1]>=MIN_DIFF){
    gap[count_pro][count_gap[count_pro]]=j;
    count_gap[count_pro]++;
    }else{
      if(p[count_pro][j+1]-p[count_pro][j]>=MIN_DIFF){
        gap[count_pro][count_gap[count_pro]]=j;
        count_gap[count_pro]++;
        }
    }
  }
}

if(p[count_pro][1]-p[count_pro][0]>=MIN_DIFF){
    gap[count_pro][count_gap[count_pro]++]=0;
}


//calculate elements in gap_dec[]  (decreasing)
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  for(j=dec_s[count_pro];j<ELEMENT-1;j++){
    if(p[count_pro][j]-p[count_pro][j+1]>=MIN_DIFF){
      gap_dec[count_pro][count_gapd[count_pro]]=j;
      count_gapd[count_pro]++;
      }else{
          if(p[count_pro][j-1]-p[count_pro][j]>=MIN_DIFF){
          gap_dec[count_pro][count_gapd[count_pro]]=j;
          count_gapd[count_pro]++;
          }
      }
  }
}

if(p[count_pro][ELEMENT-2]-p[count_pro][ELEMENT-1]>=MIN_DIFF){
    gap_dec[count_pro][count_gapd[count_pro]++]=ELEMENT-1;
}






//store increasing ramp's indexes in inc[]
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  for(j=0;j<count_gap[count_pro];j++){
    if(gap[count_pro][j]-gap[count_pro][j+1]>MAX_GAP){
      if(gap[count_pro][j-1]-gap[count_pro][j]<=MAX_GAP){
        inc[count_pro][j]=gap[count_pro][j];
        count_inc[count_pro]++;
      }
      break;
    }else{
      inc[count_pro][j]=gap[count_pro][j];
      count_inc[count_pro]++;
    }
  }
}

for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  inc_s[count_pro]=inc[count_pro][count_inc[count_pro]-1];
}

//store decreasing ramp's indexes in dec[]
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  for(j=0;j<count_gapd[count_pro];j++){
    if(abs(gap_dec[count_pro][j]-gap_dec[count_pro][j+1])>MAX_GAP){
      if(abs(gap_dec[count_pro][j-1]-gap_dec[count_pro][j])<=MAX_GAP){
        dec[count_pro][j]=gap_dec[count_pro][j];
        count_dec[count_pro]++;
      }
      break;
    }else{
      dec[count_pro][j]=gap_dec[count_pro][j];
      count_dec[count_pro]++;
    }
  }
}


if(gap_dec[count_pro][count_gapd[count_pro]-1]-gap_dec[count_pro][count_gapd[count_pro]-2]>MAX_GAP){
}

for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(dec[count_pro][count_dec[count_pro]-1]!=0){
  dec_e[count_pro]=dec[count_pro][count_dec[count_pro]-1];
  }
}
    


//calculate profile property
int w_peak[NUM_PRO],w_top[NUM_PRO],h_inc[NUM_PRO],h_dec[NUM_PRO],h_peak[NUM_PRO];//profile properties
float s_inc[NUM_PRO],s_dec[NUM_PRO]; //increasing and decreasing slope

for(i=0;i<NUM_PRO;i++){
  w_peak[i]=0;
  w_top[i]=1;
  h_inc[i]=0;
  h_dec[i]=0;
  h_peak[i]=0;
  s_inc[i]=0;
  s_dec[i]=0;  
}

//if only exists rising ramp, deceasing index starts and ends at last index 
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(inc_e[count_pro]==ELEMENT-1){
    dec_s[count_pro]=ELEMENT-1;
    dec_e[count_pro]=ELEMENT-1;
  }
}
 //if only exists descending ramp, increasing index starts and ends at first index
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(dec_s[count_pro]==0){
    inc_s[count_pro]=0;
    inc_e[count_pro]=0;
  }
}

for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(dec_s[count_pro]!=inc_e[count_pro]){
    w_top[count_pro]=dec_s[count_pro]-inc_e[count_pro];
  }else{
    w_top[count_pro]=1;
  }
  w_peak[count_pro]=dec_e[count_pro]-inc_s[count_pro];
  h_inc[count_pro]=p[count_pro][inc_e[count_pro]]-p[count_pro][inc_s[count_pro]];
  h_dec[count_pro]=p[count_pro][dec_s[count_pro]]-p[count_pro][dec_e[count_pro]];
  center[count_pro]=(inc_e[count_pro]+dec_s[count_pro])/2;
}

//calculate increasing slope (rise&descend / only descending ramp)
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(inc_s[count_pro]!=inc_e[count_pro]){
    s_inc[count_pro]=(float)(h_inc[count_pro]/(inc_e[count_pro]-inc_s[count_pro]));
  }else{
    s_inc[count_pro]=SLOPE_MAX;
  }
}

//calculate decreasing slope (rise&descend / only rising ramp)
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(dec_s[count_pro]!=dec_e[count_pro]){
    s_dec[count_pro]=(float)(h_dec[count_pro]/(dec_e[count_pro]-dec_s[count_pro]));
  }else{
    s_dec[count_pro]=SLOPE_MAX;//decreasing slope is also considered as positive
  }
}

//calculate peak height
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(p[count_pro][inc_s[count_pro]]>p[count_pro][dec_e[count_pro]]){
    h_peak[count_pro]=p[count_pro][center[count_pro]]-p[count_pro][inc_s[count_pro]]+((p[count_pro][inc_s[count_pro]]-p[count_pro][dec_e[count_pro]])*(center[count_pro]-inc_s[count_pro])/w_peak[count_pro]);
  }else if(p[inc_s[count_pro]]<p[dec_e[count_pro]]){
    h_peak[count_pro]=p[count_pro][center[count_pro]]-p[count_pro][dec_e[count_pro]]+((p[count_pro][dec_e[count_pro]]-p[count_pro][inc_s[count_pro]])*(dec_e[count_pro]-center[count_pro])/w_peak[count_pro]);
  }else{  
    h_peak[count_pro]=p[count_pro][center[count_pro]]-p[count_pro][inc_s[count_pro]];
  }
}

for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(!w_top[count_pro] && !w_peak[count_pro] && !h_inc[count_pro] && !h_dec[count_pro]){
    s_inc[count_pro]=0;
    s_dec[count_pro]=ELEMENT-1;
    h_peak[count_pro]=0;
    center[count_pro]=ELEMENT/2;
  }
}


fprintf(stderr,"start calculating properties...\n");

/*

fprintf(stderr,"\n\ndec_s: \n");
for(i=0;i<NUM_PRO;i++){
  fprintf(stderr,"%d ",h_inc[i]);
}

fprintf(stderr,"\ninc_s: \n");
for(i=0;i<NUM_PRO;i++){
  fprintf(stderr,"%d ",inc_s[i]);
}

fprintf(stderr,"\nmax: \n");
for(i=0;i<NUM_PRO;i++){
  fprintf(stderr,"%d ",max[i]);
}

fprintf(stderr,"\ns_inc: \n");
for(i=0;i<NUM_PRO;i++){
  fprintf(stderr,"%f ",s_inc[i]);
}

fprintf(stderr,"\ns_dec: \n");
for(i=0;i<NUM_PRO;i++){
  fprintf(stderr,"%f ",s_dec[i]);
}

fprintf(stderr,"\nw_top: \n");
for(i=0;i<NUM_PRO;i++){
  fprintf(stderr,"%d ",w_top[i]);
}

fprintf(stderr,"\nw_peak: \n");
for(i=0;i<NUM_PRO;i++){
  fprintf(stderr,"%d ",w_peak[i]);
}

fprintf(stderr,"\nh_peak: \n");
for(i=0;i<NUM_PRO;i++){
  fprintf(stderr,"%d ",h_peak[i]);
}
fprintf(stderr,"\n");


//test for single profile
fprintf(stderr,"gap[] \n");
for(j=0;j<count_gap[I];j++){
  fprintf(stderr,"%d ",gap[I][j]);
}
fprintf(stderr,"\n");

fprintf(stderr,"inc[] \n");
for(j=0;j<count_inc[I];j++){
  fprintf(stderr,"%d ",inc[I][j]);
}
fprintf(stderr,"\n");

fprintf(stderr,"gap_d[] \n");
for(j=0;j<count_gapd[I];j++){
  fprintf(stderr,"%d ",gap_dec[I][j]);
}
fprintf(stderr,"\n");

fprintf(stderr,"dec[] \n");
for(j=0;j<count_dec[I];j++){
  fprintf(stderr,"%d ",dec[I][j]);
}
fprintf(stderr,"\n");
fprintf(stderr,"number in inc[] %d \n",count_inc[I]);

fprintf(stderr,"number in dec[] %d \n",count_dec[I]);
fprintf(stderr,"Inc_s=%d Inc_e=%d dec_s=%d dec_e=%d center=%d  \n",inc_s[I],inc_e[I],dec_s[I],dec_e[I],center[I]);
fprintf(stderr,"w_top Is:%d \n",w_top[I]);
fprintf(stderr,"w_peak Is:%d \n",w_peak[I]);
fprintf(stderr,"h_Inc Is:%d \n",h_inc[I]);
fprintf(stderr,"h_dec Is:%d \n",h_dec[I]);
fprintf(stderr,"s_Inc Is:%f \n",s_inc[I]);
fprintf(stderr,"s_dec Is:%f \n",s_dec[I]);
fprintf(stderr,"h_peak Is:%f \n",h_peak[I]);
*/

float twidths[NUM_PRO]; //top width
float pwidths[NUM_PRO];//peak width
float rheights[NUM_PRO*2];//ramp height
float pheights[NUM_PRO];//peak height
float rslopes[NUM_PRO*2];//ramp slope


int ct=0;
for(count_pro=0;count_pro<NUM_PRO;count_pro++){
  if(count_inc[count_pro] && count_dec[count_pro]){
    twidths[ct]=w_top[count_pro];
    pwidths[ct]=w_peak[count_pro];
    rheights[2*ct]=h_inc[count_pro];
    rheights[2*ct+1]=h_dec[count_pro];
    rslopes[2*ct]=s_inc[count_pro];
    rslopes[2*ct+1]=s_dec[count_pro];
    pheights[ct]=h_peak[count_pro];
    ct++;
    
  }
}

/*
fprintf(stderr,"\n%d elements in twidths[]: \n",ct);
for(i=0;i<ct;i++){
  fprintf(stderr,"%f ",twidths[i]);
}
fprintf(stderr,"\n");  

fprintf(stderr,"\n%d elements in pwidths[]: \n",ct);
for(i=0;i<ct;i++){
  fprintf(stderr,"%f ",pwidths[i]);
}
fprintf(stderr,"\n");

fprintf(stderr,"\n%d elements in rheights[]: \n",2*ct);
for(i=0;i<2*ct;i++){
  fprintf(stderr,"%f ",rheights[i]);
}
fprintf(stderr,"\n");

fprintf(stderr,"\n%d elements in rslopes[]: \n",2*ct);
for(i=0;i<2*ct;i++){
  fprintf(stderr,"%f ",rslopes[i]);
}
fprintf(stderr,"\n");

fprintf(stderr,"\n%d elements in pheights[]: \n",ct);
for(i=0;i<ct;i++){
  fprintf(stderr,"%f ",pheights[i]);
}
fprintf(stderr,"\n");
*/

//calculate profile property statistics
float mpwidths,mtwidths,mrslopes,mrheights,mpheights;
float sdpwidths,sdtwidths,sdrslopes,sdrheights,sdpheights;
float cvrheights,cvpheights;

//twidths
float sum1=0,var1=0;//calculate mean twidths
for(i=0;i<ct;i++){
  sum1+=twidths[i];
}
mtwidths=sum1/(float)ct;

for(j=0;j<ct;j++){
  var1+=(twidths[j]-mtwidths)*(twidths[j]-mtwidths);
}
sdtwidths=sqrt(var1/(float)ct);

//pwidths
float sum2=0,var2=0;//calculate mean pwidths
for(i=0;i<ct;i++){
  sum2+=pwidths[i];
}
mpwidths=sum2/(float)ct;

for(j=0;j<ct;j++){
  var2+=(pwidths[j]-mpwidths)*(pwidths[j]-mpwidths);
}
sdpwidths=sqrt(var2/(float)ct);

//rslopes
float sum3=0,var3=0;//calculate mean rslopes
for(i=0;i<2*ct;i++){
  sum3+=rslopes[i];
}
mrslopes=sum3/(2*(float)ct);

for(j=0;j<2*ct;j++){
  var3+=(rslopes[j]-mrslopes)*(rslopes[j]-mrslopes);
}
sdrslopes=sqrt(var3/(2*(float)ct));

//rheights
float sum4=0,var4=0;//calculate mean mrheights
for(i=0;i<2*ct;i++){
  sum4+=rheights[i];
}
mrheights=sum4/(2*(float)ct);

for(j=0;j<2*ct;j++){
  var4+=(rheights[j]-mrheights)*(rheights[j]-mrheights);
}
sdrheights=sqrt(var4/(2*(float)ct));

cvrheights=sdrheights/mrheights;

//pheights
float sum5=0,var5=0;//calculate mean mpheights
for(i=0;i<ct;i++){
  sum5+=pheights[i];
}
mpheights=sum5/(float)ct;

for(j=0;j<ct;j++){
  var5+=(pheights[j]-mpheights)*(pheights[j]-mpheights);
}
sdpheights=sqrt(var5/(float)ct);

cvpheights=sdpheights/mpheights;


fprintf(stderr,"\nsum1=%f \n",sum1);
fprintf(stderr,"\nct=%d \n",ct);

fprintf(stderr,"mpwidths=%f \n",mpwidths);
fprintf(stderr,"sdpwidths=%f \n",sdpwidths);
fprintf(stderr,"sdtwidths=%f \n",sdtwidths);
fprintf(stderr,"\nmtwidths=%f \n",mtwidths);
fprintf(stderr,"sdpwidths=%f \n",sdrslopes);

fprintf(stderr,"sdrheights=%f \n",sdrheights);
fprintf(stderr,"sdpheights=%f \n",sdpheights);
fprintf(stderr,"cvrheights=%f \n",cvrheights);
fprintf(stderr,"cvpheights=%f \n",cvpheights);



strcpy(Out[array_in].name,Data[array_in].name);
Out[array_in].y_out=y_in;
Out[array_in].x_out=x_in;
Out[array_in].mpwidths=mpwidths;
Out[array_in].sdpwidths=sdpwidths;
Out[array_in].mtwidths=mtwidths;
Out[array_in].sdtwidths=sdtwidths;
Out[array_in].sdrslopes=sdrslopes;
Out[array_in].cvrheights=cvrheights;
Out[array_in].cvpheights=cvpheights;


}
fp_out=fopen("forTrain.csv","w");

if(!fp_out){
  fprintf(stderr,"failed to open peakoutput for writing!\n");
}else{
  for(i=0;i<read_i;i++){
    fprintf(fp_out,"%s,%d,%d,%f,%f,%f,%f,%f,%f,%f\n",Out[i].name,Out[i].y_out,Out[i].x_out,
    Out[i].mpwidths,Out[i].sdpwidths,Out[i].mtwidths,Out[i].sdtwidths,Out[i].sdrslopes,Out[i].cvrheights,Out[i].cvpheights);
  }
}

fclose(fp_out);
exit(0);

}





