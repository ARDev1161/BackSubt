import cv2
import numpy as np

def contours_by_mat(src_bank):
    contours = cv2.Canny(src_bank,100,200)
    return contours;
 
def tresh_contours_by_length(contours,min_length,max_length):
	threshed = []
	for i in contours:
		if min_length <= len(contours[i]) <= max_length:
			threshed.append(contours[i])
	return threshed
	
def rects_by_contours(contours):
    rects=[]
    for i in len(contours):
        rects.append(contours[i])
    return rects
    
def draw_rects(src_image,rects[]):
    rected=[]
	rng=RNG(12345)
	for i in range(0,rects.size()):		
		color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255))
        rectangle(rected, rects[i].tl(), rects[i].br(), color, 2, 8, 0)
	return rected
    
def submatrixes_by_rects(src_bank,rects):
    subm=[]
    #дописать
    return subm
    
def detect_submatrixes():
    submatr[0]
    
    return submatr
    
capture = cv2.VideoCapture(0)

while(True):
    src = capture.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    detect_submatrixes(
        submatrixes_by_rects(
            src,rects_by_contours(
                contours_by_mat()
            )
        )
    )
    
    cv2.imshow('frame',gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
        
cap.release()
cv2.destroyAllWindows()
