
#include "imageproc.h"

imageProcessor::imageProcessor(QObject *parent) : QObject{parent}
{
}

void imageProcessor::cvMatToQImage(const cv::Mat &image)
{
    QImage tempImage = cvMat2Qimage(image);
    emit sendQimage(tempImage);
}

#define ratioValue 10

void imageProcessor::getThingsPosition(QImage &image)
{
    QMap<QString, QList<int>> color;

    QList<int> red = {149, 172, 99, 180, 255, 255};
    QList<int> blue = {90, 147, 20, 108, 255, 92};
    QList<int> yellow = {0, 113, 148, 26, 239, 188};

    color.insert("red", red);
    color.insert("blue", blue);
    color.insert("yellow", yellow);

    cv::Mat cvImage = Qimage2cvMat(image);

    if (cvImage.empty())
    {
        qDebug() << "empty";
        return;
    }

    cv::Mat undistorted_image;
    // 进行畸变矫正
    imageCorrect(cvImage, undistorted_image);

    // 裁剪图像
    cv::Rect roi(130, 0, 410, 295);
    cv::Mat croppedImage = undistorted_image(roi);

    // 获取图像尺寸
    int imageWidth = croppedImage.cols;

    // 迭代遍历
    cv::Mat maskImage;
    std::vector<std::vector<cv::Point>> contours;

    QMultiMap<QString, QPointF> thingsPlace;

    QMap<QString, QList<int>>::iterator it;
    for (it = color.begin(); it != color.end(); ++it)
    {
        maskImage = stackImages(croppedImage, it.value());
        cv::findContours(maskImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        foreach (const auto &contour, contours)
        {
            cv::Moments moments = cv::moments(contour);
            int cx = static_cast<int>(moments.m10 / moments.m00);
            int cy = static_cast<int>(moments.m01 / moments.m00);

            // 在轮廓中心绘制十字线
            cv::line(croppedImage, cv::Point(cx - 10, cy), cv::Point(cx + 10, cy), cv::Scalar(0, 0, 255), 2);
            cv::line(croppedImage, cv::Point(cx, cy - 10), cv::Point(cx, cy + 10), cv::Scalar(0, 0, 255), 2);

            // 计算坐标并在图像上绘制文本
            qreal X = ((imageWidth / 2) - cx) / ratioValue;
            qreal Y = cy / ratioValue;
            QPointF value(X, Y);

            thingsPlace.insert(it.key(), value);

            std::stringstream ss;
            ss << "(" << std::fixed << std::setprecision(2) << X << ", " << Y << ")"
               << " " << it.key().toStdString();
            cv::putText(croppedImage, ss.str(), cv::Point(cx - 50, cy - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                        cv::Scalar(255, 0, 0), 1, cv::LINE_AA);
        }
    }
    cvMatToQImage(croppedImage);
    emit sendThingsPlace(thingsPlace);
}

void imageProcessor::imageCorrect(cv::Mat &inputImage, cv::Mat &outputImage)
{
    // 定义相机内部参数和畸变系数
    double fx = 2.24824966e+03;
    double cx = 3.40545445e+02;
    double fy = 2.25573014e+03;
    double cy = 1.67740060e+02;

    double k1 = -8.61088965e+00;
    double k2 = 1.42240820e+02;
    double p1 = -2.66223417e-03;
    double p2 = -3.17775226e-02;
    double k3 = -6.32746977e+03;

    // 定义相机内部参数和畸变系数
    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);

    cv::Mat distortion_coefficients = (cv::Mat_<double>(5, 1) << k1, k2, p1, p2, k3);

    // 进行畸变矫正
    cv::undistort(inputImage, outputImage, camera_matrix, distortion_coefficients);
}
