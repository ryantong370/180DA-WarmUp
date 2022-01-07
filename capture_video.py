import numpy as np
import cv2

cap = cv2.VideoCapture(0)

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # Convert image to gray and blur it
    src_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    src_gray = cv2.blur(src_gray, (3,3))

    # Display the resulting frame
    cv2.imshow('frame',src_gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()