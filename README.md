# MAdetection
This project is to automatically detect Microaneurysms in Fundus Images.    
It is built in VisionX system http://www.via.cornell.edu/visionx/vxmore.html    
It mainly implements algorithm described in Lazar, Istvan, and Andras Hajdu. "Retinal microaneurysm detection through local rotating cross-section profile analysis." IEEE Trans. Med. Imag., vol. 32, no. 2, pp. 400-407, Feb. 2013

Implemented stages:      
1. Image Preprocessing (inversed green channel image, gaussian filtering), this part utilizes VisionX image processing command            
2. Local Maximum Region Extraction        
3. Cross-Sectional Scanning (implemented by Ling Zeng)     
4. Peak Detection and Property Measurement on the Cross-Section Profile     
5. Feature set building           
6. Clasisfication uses VisionX command                    
7. Nonmaximum Suppression                    

With an input fundus image, it can output detected MA coordinates in a .csv file (one column y, one column x) and an image annotated with circles.
 
