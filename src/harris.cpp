#include "../include/harris.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>


cv::Mat addPadding(cv::Mat img, int padding)
{
    cv::Mat padded;
    padded.create(img.rows + 2*padding, img.cols + 2*padding, img.type());
    padded.setTo(cv::Scalar::all(0.0));
    img.copyTo(padded(cv::Rect(padding, padding, img.cols, img.rows)));
    return padded;
}

cv::Mat applySobelX(cv::Mat src)
{
    double sobelX[3][3] = {
            {-1.0, 0.0, 1.0},
            {-2.0, 0.0, 2.0},
            {-1.0, 0.0, 1.0}
    };



    int rows = src.rows;
    int cols = src.cols;
    cv::Mat dst = src.clone();
    double sum;
    for(int row = 1; row < rows - 1; row++)
    {
        for(int col = 1; col < cols - 1; col++)
        {
            sum = 0.0;
            for(int i = -1; i <= 1; i++)
            {
                for(int j = -1; j <= 1; j++)
                {
                    sum = sum + sobelX[i+1][j+1] * src.at<uchar>(row-i, col-j);
                }
            }
            dst.at<uchar>(row, col) = sum;
        }
    }
//    cv::imshow("SobelX", dst);
//    cv::waitKey(0);
    return dst;
}

cv::Mat applySobelY(cv::Mat src)
{
    double sobelY[3][3] = {
            {-1.0, -2.0, -1.0},
            {0.0, 0.0, 0.0},
            {1.0, 2.0, 1.0}
    };

    int rows = src.rows;
    int cols = src.cols;
    cv::Mat dst = src.clone();
    double sum;
    for(int row = 1; row < rows - 1; row++)
    {
        for(int col = 1; col < cols - 1; col++)
        {
            sum = 0.0;
            for(int i = -1; i <= 1; i++)
            {
                for(int j = -1; j <= 1; j++)
                {
                    sum = sum + sobelY[i+1][j+1] * static_cast<double>(src.at<uchar>(row + i, col + j));
                }
            }
            dst.at<uchar>(row, col) = sum;
        }
    }
//    cv::imshow("SobelY", dst);
//    cv::waitKey(0);
    return dst;
}

cv::Mat multiplyMat(cv::Mat src1, cv::Mat src2)
{
    if(src1.rows != src2.rows && src1.cols != src2.cols)
    {
        std::cout << "Matrices shapes.png don't match" << std::endl;
        return src1;
    }

    int rows = src1.rows;
    int cols = src1.cols;
    cv::Mat dst = cv::Mat(rows, cols, CV_64FC1);
    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            dst.at<double>(row, col) = static_cast<double>(src1.at<uchar>(row, col) * src2.at<uchar>(row, col));
        }
    }
    return dst;
}


Harris applyHarris(cv::Mat image, long long threshold, int padding)
{
    cv::Mat Ix = applySobelX(image);
    cv::Mat Iy = applySobelY(image);
    int rows = image.rows;
    int cols = image.cols;
    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            if(Ix.at<uchar>(row, col) < 0)
            {
                Ix.at<uchar>(row, col) *= -1;
            }
            if(Iy.at<uchar>(row, col) < 0)
            {
                Iy.at<uchar>(row, col) *= -1;
            }
        }
    }

    cv::Mat Ixx = multiplyMat(Ix, Ix);
    cv::Mat Iyy = multiplyMat(Iy, Iy);
    cv::Mat Ixy = multiplyMat(Ix, Iy);

    cv::Mat IxxGaussian = cv::Mat(rows, cols, CV_64FC1);
    cv::Mat IyyGaussian = cv::Mat(rows, cols, CV_64FC1);
    cv::Mat IxyGaussian = cv::Mat(rows, cols, CV_64FC1);

    cv::GaussianBlur(Ixx, IxxGaussian, cv::Size(9, 9), 0.5);
    cv::GaussianBlur(Iyy, IyyGaussian, cv::Size(9, 9), 0.5);
    cv::GaussianBlur(Ixy, IxyGaussian, cv::Size(9, 9), 0.5);

    cv::Mat R = cv::Mat(rows, cols, CV_64FC1);

    double detM, traceM;

    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            detM = IxxGaussian.at<double>(row, col) * IyyGaussian.at<double>(row, col) - IxyGaussian.at<double>(row, col) * IxyGaussian.at<double>(row, col);
            traceM = IxxGaussian.at<double>(row, col) + IyyGaussian.at<double>(row, col);
            R.at<double>(row, col) = detM - 0.1 * traceM * traceM;
        }
    }

    std::vector<HarrisPoint2D> points;


    double non_max;
    bool skip_point;
    for(int row = padding; row < rows - padding; row++)
    {
        for(int col = padding; col < cols - padding; col++)
        {
            if(R.at<double>(row, col) > static_cast<double>(threshold))
            {
                skip_point = false;
                non_max = R.at<double>(row, col);
                for(int i = -padding; i <= padding; i++)
                {
                    for(int j = -padding; j <= padding; j++)
                    {
                        if(R.at<double>(row + i, col + j) > non_max)
                        {
                            skip_point = true;
                            break;
                        }
                    }
                }
                if(!skip_point)
                {
                    HarrisPoint2D p2d{};
                    p2d.x = col;
                    p2d.y = row;
                    p2d.r = R.at<double>(row, col);
                    points.push_back(p2d);
                }
            }
        }
    }

    Harris harris;
    harris.R = R;
    harris.points = points;
    return harris;
}

void drawCorners(cv::Mat &img, std::vector<HarrisPoint2D> points, int padding)
{
    size_t size = points.size();
    for(int i = 0; i < size; i++)
    {
        cv::circle(img, cv::Point( points[i].x - padding, points[i].y - padding), 8, cv::Scalar( 0.0, 0.0, 255.0 ), 1, 7 );
    }
    (void)cv::imwrite("output.png", img);
}

cv::Mat harrisCornerDetection(cv::Mat src, int padding, long long threshold, Harris &harris)
{
    cv::Mat graySrc;
    if(src.channels() > 1)
    {
        cv::cvtColor(src, graySrc, cv::COLOR_BGR2GRAY);
    }
    else
    {
        graySrc = src;
    }
    cv::Mat imgBlurred;
    cv::GaussianBlur(graySrc, imgBlurred, cv::Size(9, 9), 0.5);
    cv::Mat padded = addPadding(imgBlurred, padding);
    harris = applyHarris(padded, threshold, padding);
    cv::Mat output = src.clone();
    drawCorners(output, harris.points, padding);
    return output;
}
