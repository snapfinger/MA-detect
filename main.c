//date: 11/05/2016
//task accomplished: detect local maxima
#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */

VXparam_t par[] =             /* command line structure            */
{ /* prefix, value,   description                         */   
{    "if=",    0,   " input file  vtemp: local max filter "},
{    "of=",    0,   " output file "},
{     0,       0,   0}  /* list termination */
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define X_END 400000
#define Y_END 400000

main(argc, argv)
int argc;
char *argv[];
{
Vfstruct (im);                      /* i/o image structure          */
Vfstruct (tm);                      /* temp image structure         */
Vfread(&im,"image10_pre.vx");
Vfnewim(&tm,im.type,im.bbx,im.chan);
int x,y;
//x=im.xhi,y=im.yhi;
//x=259,y=18;
//fprintf(stderr,"x=%d\n",x);
//fprintf(stderr,"y=%d\n",y);
//fprintf(stderr,"inverted y=%d\n",im.yhi-y);

int m1[X_END];
int n1[Y_END];

int count=0;//count the point whose pixel value greater than zero
int sum=0;   //count the sum of these non-zero points' pixels
int average =0;

for(y=im.ylo;y<im.yhi;y++){
			for(x=im.xlo;x<im.xhi;x++){
				if(im.u[y][x]>10){
					sum+=im.u[y][x];
					count++;       
				}
			}
		}
		
average=sum/count+10;
fprintf(stderr,"total number of pixels (value>10):%d\n",count);
fprintf(stderr,"average pixel value is:%d\n",average); 

int countpoint=0;//count the number of local maximum£»
int max;
int m,n;

for(y=im.ylo+18;y<im.yhi-18;y++){
			for(x=im.xlo+18;x<im.xhi-18;x++){
				if(im.u[y][x]<average) {continue;}
				
        else{
				  if(im.u[y-15][x-15]<10||im.u[y+15][x+15]<10||im.u[y-15][x+15]<10||im.u[y+15][x-15]<10){continue;} 
					else{
					    max=0;
							for(m=y-8;m<=y+8;m++)
							{
								for(n=x-8;n<=x+8;n++)
								{
										if(im.u[m][n]>max)
										{
											max=im.u[m][n];
										}
                }
							}
       
							if(max==im.u[y][x]){
							//	m1[countpoint]=y;
							//	n1[countpoint]=x;
								countpoint++;
							 }
						}
            
   }
  }
}

fprintf(stderr,"number of local maxima:%d\n",countpoint);

/*		
		int  x1[countpoint];
		int  y1[countpoint];//x1,y1 for storing local maximum coordinates
    int i;
		for(i=0;i<countpoint;i++){
			x1[i]=0;
			y1[i]=0;
		} 
	
		for(i=0;i<countpoint;i++)
		{
			x1[i]=m1[i];
			y1[i]=n1[i];
		}


    fprintf(stderr,"number of preliminary candidates is:%d\n",countpoint);
*/
}