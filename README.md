# MAdetection
This project is to automatically detect microaneurysms in fundus images, and is built in [VisionX](http://www.via.cornell.edu/visionx/vxmore.html) system  

It mainly implements algorithm described in the paper: Lazar, Istvan, and Andras Hajdu. "Retinal microaneurysm detection through local rotating cross-section profile analysis." IEEE Trans. Med. Imag., vol. 32, no. 2, pp. 400-407, Feb. 2013

Implemented stages:      
1. Image Preprocessing---single.sh                                      
  (inverted green channel, gaussian filtering), this part utilizes VisionX image processing command            
2. Local Maximum Region Extraction---lmr.c                                
3. Cross-Sectional Scanning Peak Detection and Property Measurement on the Cross-Section Profile, and Feature set building---peak.c           (cross-section scanning is implemented by Ling Zeng)                           
4. Clasisfication---afterpeak.sh                     
  (uses VisionX command vrclasstt in afterpeak)                                                                                                                                    
5. Nonmaximum Suppression and Output---afterclass.sh, output.c                          

With one input fundus image, it can output detected MA coordinates in a .csv file (one column y, one column x) and an image annotated with circles.


 
