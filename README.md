# Microaneurysm Detection in Fundus Images
## Objective and Developing Tools
This project is to automatically detect microaneurysms in fundus images, and is built in [VisionX](http://www.via.cornell.edu/visionx/vxmore.html) system  



## Implemented stages
The sequence of the following code and scripts correspond to the experiment stages in the reference paper.
- [prebatch.sh](https://github.com/snapfinger/MAdetection/blob/master/prebatch.sh)                                    
  - Image Preprocessing in batches including inverting green channel and gaussian filtering
  - This part utilizes VisionX image processing commands          
- [lmr.c](https://github.com/snapfinger/MAdetection/blob/master/lmr.c)                              
  - Local Maximum Region Extraction 
- [peak.c](https://github.com/snapfinger/MAdetection/blob/master/peak.c)  
  - Cross-Sectional Scanning 
  - Peak Detection 
  - Property Measurement on the Cross-Section Profile
  - Feature set building 
  - Cross-section scanning is implemented by Ling Zeng                         
- [afterpeak.sh](https://github.com/snapfinger/MAdetection/blob/master/afterpeak.sh)
  - Classification
  - Uses VisionX command ``` vrclasstt ``` here
- [afterclass.sh](https://github.com/snapfinger/MAdetection/blob/master/afterclass.sh)
  - Nonmaximum Suppression 
- [output.c](https://github.com/snapfinger/MAdetection/blob/master/output.c)
  - Final Output 
  - With one input fundus image, it can output detected MA coordinates in a .csv file (one column y, one column x) and an image annotated with circles.

## Reference
Lazar, I., & Hajdu, A. (2013). Retinal microaneurysm detection through local rotating cross-section profile analysis. IEEE transactions on medical imaging, 32(2), 400-407.
 
