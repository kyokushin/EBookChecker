# -*- coding: utf-8 -*-

import os

import cv2
import numpy


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


class Range:
    def __init__(self, start=-1, end=-1):
        self.start = start
        self.end = end


def findSameValueHorizontal(src):
    assert (src.dtype == numpy.int32)

    ranges = []

    (rows, cols) = src.shape
    srcLine = src[rows - 1]

    r = Range()
    for i in range(1, cols):

        sameValue = srcLine[i] == srcLine[i - 1]
        if sameValue and r.start < 0:
            r.start = i - 1
        elif not sameValue and r.start >= 0:

            r.end = i - 1
            ranges.append(r)

            r = Range()

    if r.start >= 0 and r.end < 0:
        r.end = cols - 1
        ranges.append(r)

    return ranges


def findSameValueVertical(src):
    assert (src.dtype == numpy.int32)

    ranges = []

    r = Range()

    (rows, cols) = src.shape
    endPos = cols - 1
    src0 = src[0, endPos]

    for i in range(1, rows):
        src1 = src[i, endPos]

        sameValue = src0 == src1
        if sameValue and r.start < 0:
            r.start = i - 1

        elif not sameValue and r.start >= 0:

            r.end = i - 1
            ranges.append(r)
            r = Range()

        src0 = src1

    if r.start >= 0 and r.end < 0:
        r.end = rows - 1

    return ranges


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

    # 横方向
    horizontalRanges = findSameValueHorizontal(integral)
    horizontalRangeDst = cv2.merge([image, image, image])

    for r in horizontalRanges:
        horizontalRangeDst[:, r.start:r.end, :] = (240, 176, 0)

    showImage(horizontalRangeDst)
    cv2.imwrite('horizontalDst.jpg', horizontalRangeDst)

    # 縦方向
    verticalRanges = findSameValueVertical(integral)
    verticalRangeDst = cv2.merge([image, image, image])

    for r in verticalRanges:
        verticalRangeDst[r.start:r.end, :, :] = (0, 0, 255)

    showImage(verticalRangeDst)
    cv2.imwrite('verticalDst.jpg', verticalRangeDst)

    # 横方向の結果と縦方向の結果を合わせる
    for r in verticalRanges:
        horizontalRangeDst[r.start:r.end, :, :] = (0, 0, 255)

    showImage(horizontalRangeDst)
    cv2.imwrite('horizontalVerticalDst.jpg', horizontalRangeDst)
