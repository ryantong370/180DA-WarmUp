import numpy as np
import cv2

cap = cv2.VideoCapture(0)

def doNothing(x):
    pass

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
    #print(frame.shape)
    # (480, 640, 3)
    # height, width, channels (3 bc rgb)
    
    # Used
    # https://www.geeksforgeeks.org/filter-color-with-opencv/
    # https://answers.opencv.org/question/200861/drawing-a-rectangle-around-a-color-as-shown/
    
    # Crop image to 256x256
    crop_size = 256
    lower_height = int((frame.shape[0] - crop_size)/2)
    upper_height = int((frame.shape[0] + crop_size)/2)
    lower_width = int((frame.shape[1] - crop_size)/2)
    upper_width = int((frame.shape[1] + crop_size)/2)

    cropped_image = frame[lower_height:upper_height, lower_width:upper_width]

    frame = cv2.rectangle(frame, (lower_width, lower_height), (upper_width, upper_height), (255, 0, 0), 2)

    cv2.imshow('cropped', cropped_image)
    cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()