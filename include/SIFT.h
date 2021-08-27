#ifndef SIFT_HPP
#define SIFT_HPP

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
using cv::Mat;


class sift {
  private:
    Mat img;        //Original Image
    void applySIFT(cv::Mat img);
    float matmul(const float h[3],const float H[3][3]);

  public:
    sift(cv::Mat); //Constructor
    Mat sift_Image;       
    std::vector<cv::KeyPoint> keypoints;

};
          
struct xyoctscale
{
  uint x = 0;
  uint y = 0;
  uint o = 0;
  uint s = 0;
};

struct xyoctscaleangle
{
    uint x = 0;
    uint y = 0;
    uint o = 0;
    uint s = 0;
    float angle = 0;
};

struct xyscaleangledescriptor
{
    uint x = 0;
    uint y = 0;
    uint s = 0;
    float angle = 0;
    std::vector<float> feature_desc;
};

#endif //SIFT_HPP
