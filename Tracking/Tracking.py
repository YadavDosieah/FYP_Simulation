#!/usr/bin/env python

import numpy as np
import cv2 as cv
from cv2 import aruco
import sys
import time
import glob
import threading
from PIL import Image
import os

ARUCO_PARAMETERS = aruco.DetectorParameters_create()
ARUCO_DICT = aruco.Dictionary_get(aruco.DICT_6X6_250)

# Constant parameters used in Aruco methods

# img = aruco.drawMarker(ARUCO_DICT,12, 700)
# cv.imshow('img',img)
# cv.imwrite('Aruco.png',img)
#
# while cv.waitKey(0) & 0xFF != ord('q'):
#     pass
# cv.destroyAllWindows()

# Create vectors we'll be using for rotations and translations for postures
rvecs, tvecs = None, None

cam = cv.VideoCapture(2)
# time.sleep(2)
cam.set(3, 640) #Width
cam.set(4, 480) #Height

cameraMatrix=cv.UMat(np.array([[ 609.710537, 0.000000, 303.200522 ],
               [0.000000, 606.011374, 258.905227],
               [0.000000, 0.000000, 1.000000]],dtype=np.float32))

distCoeffs = cv.UMat(np.array([0.084930, -0.153198, 0.011283, -0.000882, 0.000000],dtype=np.float32))

# Define the codec and create VideoWriter object
# fourcc = cv.VideoWriter_fourcc(*'XVID')
# out = cv.VideoWriter('Output.avi',fourcc, 20.0, (1280,480))
#
# fourcc = cv.VideoWriter_fourcc(*'XVID')
# out2 = cv.VideoWriter('RawVideo.avi',fourcc, 20.0, (640,480))

while(True):
    corners = []
    # Capturing each frame of our video stream
    ret,QueryImg = cam.read()

    QueryImg = QueryImg[20::, ::]

    # grayscale image
    gray = cv.cvtColor(QueryImg, cv.COLOR_BGR2GRAY)

    # Detect Aruco markers
    corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, ARUCO_DICT, parameters=ARUCO_PARAMETERS)


    if corners != []: #And push button is pressed
        rvec, tvec, markerPoints = aruco.estimatePoseSingleMarkers(corners[0], 0.02, cameraMatrix,distCoeffs)

        # print(corners)
        c = corners[0][0]
        # print(c)

        centre = (c[:,0].mean(),c[:,1].mean())

        # Outline all of the markers detected in our image
        QueryImg = aruco.drawDetectedMarkers(QueryImg, corners, borderColor=(0, 0, 255))

        QueryImg = aruco.drawAxis(	QueryImg, cameraMatrix, distCoeffs, rvec, tvec, 0.01)

        # cv.circle(QueryImg, (int(centre[0]),int(centre[1])), 5, ( 255, 0, 0 ), -1, 8 )

    # if push button 2 is pressed:
    # Drawing[:] = (0, 0, 0)
    # Do OCR

    # Display our image
    QueryImg = cv.flip( QueryImg, 1 )
    # Output_Image = QueryImg
    # cv.imshow('Drawing', Drawing_flipped)
    # Output_Image = np.concatenate((QueryImg, Drawing_flipped), axis=1)
    # out.write(Output_Image)
    cv.imshow('outputWindow', QueryImg)
    # out2.write(QueryImg)

    # Exit at the end of the video on the 'q' keypress
    if cv.waitKey(1) & 0xFF == ord('q'):
        cv.destroyAllWindows()
        cam.release()
        # out.release()
        sys.exit()
