#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include "../include/greyscale.h"

using std::vector, cv::Vec3b;
using std::cout, std::endl;
using cv::Mat;

greyscale::greyscale(Mat image)
{
    img = image;
    grayscaled = Mat(toGrayScale()); //Grayscale the image
}

Mat greyscale::toGrayScale()
{
    grayscaled = Mat(img.rows, img.cols, CV_8UC1); //To one channel
    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
        {
            int b = img.at<Vec3b>(i, j)[0];
            int g = img.at<Vec3b>(i, j)[1];
            int r = img.at<Vec3b>(i, j)[2];

            double newValue = r * 0.2989 + g * 0.5870 + b * 0.1140;
            grayscaled.at<uchar>(i, j) = newValue;
        }
    return grayscaled;
}
