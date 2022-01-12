import numpy as np
import cv2
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans

def find_histogram(clt):
    """
    create a histogram with k clusters
    :param: clt
    :return:hist
    """
    numLabels = np.arange(0, len(np.unique(clt.labels_)) + 1)
    (hist, _) = np.histogram(clt.labels_, bins=numLabels)

    hist = hist.astype("float")
    hist /= hist.sum()

    return hist
def plot_colors2(hist, centroids):
    bar = np.zeros((50, 300, 3), dtype="uint8")
    startX = 0

    for (percent, color) in zip(hist, centroids):
        # plot the relative percentage of each cluster
        endX = startX + (percent * 300)
        cv2.rectangle(bar, (int(startX), 0), (int(endX), 50),
                      color.astype("uint8").tolist(), -1)
        startX = endX

    # return the bar chart
    return bar

cap = cv2.VideoCapture(0)

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
    #print(frame.shape)
    # (480, 640, 3)
    # height, width, channels (3 bc rgb)
    
    # Used
    # https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_gui/py_video_display/py_video_display.html    
    # https://code.likeagirl.io/finding-dominant-colour-on-an-image-b4e075f98097

    # Crop image to 256x256
    crop_size = 256
    lower_height = int((frame.shape[0] - crop_size)/2)
    upper_height = int((frame.shape[0] + crop_size)/2)
    lower_width = int((frame.shape[1] - crop_size)/2)
    upper_width = int((frame.shape[1] + crop_size)/2)

    cropped_img = frame[lower_height:upper_height, lower_width:upper_width]

    # Do Kmeans
    kmeans_img = cv2.cvtColor(cropped_img, cv2.COLOR_BGR2RGB)
    kmeans_img = kmeans_img.reshape((kmeans_img.shape[0] * kmeans_img.shape[1],3)) #represent as row*column,channel number
    clt = KMeans(n_clusters=3) #cluster number
    clt.fit(kmeans_img)

    hist = find_histogram(clt)
    bar = plot_colors2(hist, clt.cluster_centers_)
    bar = cv2.cvtColor(bar, cv2.COLOR_RGB2BGR)
    cv2.imshow('histogram', bar)

    # Show cropped image
    cv2.imshow('cropped', cropped_img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()