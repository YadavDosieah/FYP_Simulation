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

for i in sys.argv[1:]:
    # print(i)
    num = int(i)

    ARUCO_PARAMETERS = aruco.DetectorParameters_create()
    ARUCO_DICT = aruco.Dictionary_get(aruco.DICT_6X6_250)

    # Constant parameters used in Aruco methods

    img = aruco.drawMarker(ARUCO_DICT,num, 700)
    # cv.imshow('img',img)
    cv.imwrite('Aruco%d.png' % num,img)
