#!/usr/bin/env python

import numpy as np
import math
import cv2 as cv
from cv2 import aruco
import sys
import time
import glob
import threading
from PIL import Image
import os
import rospy
from geometry_msgs.msg import Twist


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

# Checks if a matrix is a valid rotation matrix.
def isRotationMatrix(R) :
    Rt = np.transpose(R)
    shouldBeIdentity = np.dot(Rt, R)
    I = np.identity(3, dtype = R.dtype)
    n = np.linalg.norm(I - shouldBeIdentity)
    return n < 1e-6


# Calculates rotation matrix to euler angles
# The result is the same as MATLAB except the order
# of the euler angles ( x and z are swapped ).
def rotationMatrixToEulerAngles(R) :

    assert(isRotationMatrix(R))

    sy = math.sqrt(R[0,0] * R[0,0] +  R[1,0] * R[1,0])

    singular = sy < 1e-6

    if  not singular :
        x = math.atan2(R[2,1] , R[2,2])
        y = math.atan2(-R[2,0], sy)
        z = math.atan2(R[1,0], R[0,0])
    else :
        x = math.atan2(-R[1,2], R[1,1])
        y = math.atan2(-R[2,0], sy)
        z = 0

    return np.array([x, y, z])

rospy.init_node('TrackingNode', anonymous=False)
vel_pub = rospy.Publisher("mobile_base/cmd_vel", Twist, queue_size=1) #Adjust topic name accordingly. Default is mobile_base/cmd_vel.

msg = Twist()

ARUCO_PARAMETERS = aruco.DetectorParameters_create()
ARUCO_DICT = aruco.Dictionary_get(aruco.DICT_6X6_250)


# Create vectors we'll be using for rotations and translations for postures
rvecs, tvecs = None, None

cam = cv.VideoCapture(2, cv.CAP_V4L)
time.sleep(3)
# time.sleep(2)
cam.set(3, 640) #Width
cam.set(4, 480) #Height
cam.set(cv.CAP_PROP_AUTOFOCUS, 0)
# exposureVal = 105
# saturationVal = 0
# brightnessVal = 150
# contrastVal = 300
# gainVal = 300
#
# cam.set(cv.CAP_PROP_AUTO_EXPOSURE,1)# TEMP:
# cam.set(15,exposureVal)
# cam.set(12,saturationVal)
# cam.set(10,brightnessVal)
# cam.set(11,contrastVal)
# cam.set(14,gainVal)

ShowImages = False
if ShowImages == True:
    cv.namedWindow( "outputWindow", cv.WINDOW_AUTOSIZE );
    def exposureCallback(val):
        exposureVal = cv.getTrackbarPos("exposureTrackbar", "outputWindow")
        cam.set(15,exposureVal)

    def saturationCallback(val):
        saturationVal = cv.getTrackbarPos("saturationTrackbar", "outputWindow")
        cam.set(12,saturationVal)

    def brightnessCallback(val):
        brightnessVal = cv.getTrackbarPos("brightnessTrackbar", "outputWindow")
        cam.set(10,brightnessVal)

    def contrastCallback(val):
        contrastVal = cv.getTrackbarPos("contrastTrackbar", "outputWindow")
        cam.set(11,contrastVal)

    def gainCallback(val):
        gainVal = cv.getTrackbarPos("gainTrackbar", "outputWindow")
        cam.set(14,gainVal)

    cv.createTrackbar("exposureTrackbar",   "outputWindow", exposureVal, 300, exposureCallback)
    cv.createTrackbar("saturationTrackbar", "outputWindow", saturationVal, 300, saturationCallback)
    cv.createTrackbar("brightnessTrackbar", "outputWindow", brightnessVal, 300, brightnessCallback)
    cv.createTrackbar("contrastTrackbar",   "outputWindow", contrastVal, 500, contrastCallback)
    cv.createTrackbar("gainTrackbar",       "outputWindow", gainVal, 300, gainCallback)

# else:
#     cam.set(15,exposureVal)
#     cam.set(12,saturationVal)
#     cam.set(10,brightnessVal)
#     cam.set(11,contrastVal)
#     cam.set(14,gainVal)

# video recorder
fourcc = cv.VideoWriter_fourcc(*'XVID')
video_writer = cv.VideoWriter("output.avi", fourcc, 15, (640,480))


cameraMatrix=cv.UMat(np.array([[ 609.710537, 0.000000, 303.200522 ],
               [0.000000, 606.011374, 258.905227],
               [0.000000, 0.000000, 1.000000]],dtype=np.float32))

distCoeffs = cv.UMat(np.array([0.084930, -0.153198, 0.011283, -0.000882, 0.000000],dtype=np.float32))

sheep = Point(0,0)
shepherd = Point(0,0)

sheep.angle = 0

Robot_Force_x = 0
Robot_Force_y = 0
while(True):
    corners = []
    # Capturing each frame of our video stream
    ret,QueryImg = cam.read()
    QueryImg = cv.resize(QueryImg,(640, 480), interpolation = cv.INTER_CUBIC)

    # QueryImg = QueryImg[20::, ::]

    # grayscale image
    gray = cv.cvtColor(QueryImg, cv.COLOR_BGR2GRAY)

    # Detect Aruco markers
    corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, ARUCO_DICT, parameters=ARUCO_PARAMETERS)


    if corners != []: #And push button is pressed

        # c = corners[0][0]

        # centre = (c[:,0].mean(),c[:,1].mean())

        # Outline all of the markers detected in our image
        QueryImg = aruco.drawDetectedMarkers(QueryImg, corners,ids, borderColor=(255, 0, 255))

        for i in range(len(corners)):
            rvec, tvec, markerPoints = aruco.estimatePoseSingleMarkers(corners[i], 0.02, cameraMatrix,distCoeffs)
            QueryImg = aruco.drawAxis(	QueryImg, cameraMatrix, distCoeffs, rvec, tvec, 0.01)
            centre = (corners[i][0][:,0].mean(),corners[i][0][:,1].mean())
            if ids[i][0] == 12:
                sheep.x = centre[0]
                sheep.y = centre[1]
                QueryImg = cv.circle(QueryImg, (sheep.x,sheep.y), 150, (0,0,255), 1)
                rotM = np.zeros(shape=(3,3))
                rotM,_ = cv.Rodrigues(rvec, rotM, jacobian = 0)
                ypr = cv.RQDecomp3x3(rotM)
                ypr = ypr[0]
                sheep.angle = math.radians(ypr[2])
            elif ids[i][0] == 1:
                shepherd.x = centre[0]
                shepherd.y = centre[1]
            # print(ids[i][0],centre)

        # cv.circle(QueryImg, (int(centre[0]),int(centre[1])), 5, ( 255, 0, 0 ), -1, 8 )
        distance_x = sheep.x - shepherd.x
        distance_y = sheep.y - shepherd.y
        distance_sq = distance_x**2 + distance_y**2
        distance = distance_sq**0.5

        if(distance < 150 and distance > 0):
            Force_x = (500/distance_sq)*(distance_x/distance)
            Force_y = (500/distance_sq)*(distance_y/distance)

            cs = math.cos(sheep.angle)
            sn = math.sin(sheep.angle)
            Robot_Force_x = cs*Force_x - sn*Force_y;
            Robot_Force_y = sn*Force_x + cs*Force_y;
            # print(Robot_Force_x,Robot_Force_y)
        else:
            Robot_Force_x = 0
            Robot_Force_y = 0
    else:
        Robot_Force_x = 0
        Robot_Force_y = 0


    cv.imshow('QueryImg', QueryImg)
    # video_writer.write(QueryImg)
    K1 = 100
    K2 = 15
    msg.linear.x = K1*Robot_Force_y #Linear vel in cm/second
    msg.angular.z = K2*Robot_Force_x #Angular vel in radians/second
    vel_pub.publish(msg)
    print("Linear Velocity:{:0.2f}, Angular Velocity:{:0.2f}".format(msg.linear.x,msg.angular.z))
    # print(msg)

    # Exit at the end of the video on the 'q' keypress
    if cv.waitKey(1) & 0xFF == ord('q'):
        cv.destroyAllWindows()
        # video_writer.release()
        msg.linear.x = 0
        msg.angular.z = 0

        vel_pub.publish(msg)
        cam.release()
        sys.exit()
