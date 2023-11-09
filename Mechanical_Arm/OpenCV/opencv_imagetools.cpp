#include "opencv_imagetools.h"
#include <QDebug>

QImage cvMat2Qimage(const cv::Mat &input)
{
    // 判断输入的 Mat 是否为空
    if (input.empty()) {
        return QImage();
    }

    cv::Mat rgbMat;
    // 如果输入图像是单通道灰度图像，将其转换为三通道 RGB 图像
    if (input.channels() == 1) {
        cv::cvtColor(input, rgbMat, cv::COLOR_GRAY2RGB);
    } else {
        cv::cvtColor(input, rgbMat, cv::COLOR_BGR2RGB);
    }
    QImage image(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
    return image.copy();
}

cv::Mat Qimage2cvMat(QImage &input)
{
    if (input.isNull())
    {
        return cv::Mat();
    }

    cv::Mat mat;

    switch (input.format())
    {
        case QImage::Format_RGB888:
            mat = cv::Mat(input.height(), input.width(), CV_8UC3, const_cast<uchar*>(input.constBits()), input.bytesPerLine());
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            break;
        case QImage::Format_Grayscale8:
            mat = cv::Mat(input.height(), input.width(), CV_8UC1, const_cast<uchar*>(input.constBits()), input.bytesPerLine());
            break;
        case QImage::Format_RGB32:
            mat = cv::Mat(input.height(), input.width(), CV_8UC4, const_cast<uchar*>(input.constBits()), input.bytesPerLine());
            break;
        default:
            mat = cv::Mat();
            break;
    }
    return mat;
}




cv::Mat stackImages(const cv::Mat& img, const QList<int>& Arry)
{
    // HSV
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar colorLow(Arry[0], Arry[1], Arry[2]);
    cv::Scalar colorHigh(Arry[3], Arry[4], Arry[5]);

    cv::Mat mask;
    cv::inRange(hsv, colorLow, colorHigh, mask);

    cv::Mat kernal = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernal);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernal);

    return mask;
}

// 返回最大轮廓
std::vector<cv::Point> findLargestContour(const std::vector<std::vector<cv::Point>>& contours)
{
    std::vector<cv::Point> largestContour;
    double largestArea = 0;

    for (const auto& contour : contours)
    {
        double area = cv::contourArea(contour); // 计算轮廓的面积
        if (area > largestArea)
        {
            largestArea = area;
            largestContour = contour;
        }
    }
    return largestContour;
}

// 裁剪轮廓
cv::Mat cropContour(const cv::Mat& frame, const std::vector<cv::Point>& contour)
{
    cv::Rect boundingRect = cv::boundingRect(contour); // 获取轮廓的包围矩形

    // 裁剪图像
    cv::Mat croppedImage = frame(boundingRect);

    return croppedImage;
}
