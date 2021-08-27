#ifndef _GREYSCALE_
#define _GREYSCALE_
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

using std::vector;
using std::cout, std::endl;
using cv::Mat;

class greyscale
{
private:
    Mat img; //Original Image

public:
    greyscale(Mat);                                         //Constructor
    Mat toGrayScale();
    Mat grayscaled; // Grayscale

};

#endif