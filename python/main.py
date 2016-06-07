# -*- coding: utf-8 -*-

import numpy
import cv2
import os

def getImageFromData():
    dataDir = os.path.join(os.path.dirname(__file__), '..', 'data')
    files = os.listdir(dataDir)
    for f in files:
        if f.endswith('.jpg') or f.endswith('.png'):
            return os.path.join(dataDir, f)

    return None

def showImage(image, wait=0):
    cv2.imshow('EbookChecker', image)
    cv2.waitKey(wait)

if __name__ == '__main__':

    fname = getImageFromData()

    # 画像の読み込み
    image = cv2.imread(fname, cv2.IMREAD_GRAYSCALE)
    assert image.dtype == numpy.uint8 and len(image.shape) == 2
    showImage(image)

    # 二値化
    binaryMax = 1
    binaryThreshold = 128
    (ret, binary) = cv2.threshold(image, binaryThreshold, binaryMax, cv2.THRESH_BINARY_INV)
    assert len(binary.shape) == 2 and binary.dtype == numpy.uint8
    showImage(binary)
    cv2.imwrite('binary.jpg', binary)

    # 積分画像の生成
    integral = cv2.integral(binary)
    assert len(integral.shape) == 2 and integral.dtype == numpy.int32
    showImage(integral)
    cv2.imwrite('integral.jpg', integral)

    # 積分画像を見やすくする処理
    (min, max, minLoc, maxLoc) = cv2.minMaxLoc(integral)
    assert min == 0, 'min must be 0'

    integralVisible = (integral * 255 / max).astype(numpy.uint8)
    showImage(integralVisible)
    cv2.imwrite('integralVisible.jpg', integralVisible)

