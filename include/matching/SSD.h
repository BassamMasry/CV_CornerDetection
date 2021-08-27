#pragma once 
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

class SSD {
  private:
    Mat img1, img2, outputMatching;
    vector<KeyPoint> kp1, kp2;
    vector<vector<float>> getDescriptors(Mat image, const vector<KeyPoint>&);
    vector<DMatch> match(
      Mat, const vector<KeyPoint>&,
      Mat, const vector<KeyPoint>&,
      float threshold = 5000
  );
  public:
  SSD(Mat, Mat, vector<KeyPoint>, vector<KeyPoint>);
  void testSSD();
};
