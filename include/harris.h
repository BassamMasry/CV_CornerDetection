#ifndef HARRIS_H
#define HARRIS_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

struct HarrisPoint2D{
    int x;
    int y;
    double r;
};

struct Harris{
    cv::Mat R;
    std::vector<HarrisPoint2D> points;
};

cv::Mat addPadding(cv::Mat img, int padding);
cv::Mat applySobelX(cv::Mat src);
cv::Mat applySobelY(cv::Mat src);
cv::Mat multiplyMat(cv::Mat src1, cv::Mat src2);
Harris applyHarris(cv::Mat image, int threshold, int padding);
void drawCorners(cv::Mat &img, std::vector<HarrisPoint2D> points, int padding);
cv::Mat harrisCornerDetection(cv::Mat src, int padding, long long threshold, Harris &harris);

#endif //HARRIS_H
