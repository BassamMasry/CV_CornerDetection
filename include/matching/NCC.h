#ifndef NCC_H
#define NCC_H

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class NCC
{
public:
    NCC(Mat, Mat, vector<KeyPoint>, vector<KeyPoint>);                                                                     /* Constructor */
    vector<vector<float>> NormalizedCrossCorrelation(Mat Image, vector<KeyPoint> &KeyPoint);                               /* Apply Normalized Cross Correlation and Get Descriptors Function */
    vector<float> DesMean(vector<vector<float>> &);                                                                        /* Get Descriptor Mean Function */
    vector<DMatch> NCCMatching(vector<float> &, vector<float> &, vector<vector<float>> &, vector<vector<float>> &, float); /* Result of NCC Matching Function */
    void testNCC();

private:
    Mat OriginalImg;                /* Orignal Image */
    Mat TemplateImg;                /* Template Image (Descriptor) */
    vector<KeyPoint> OriginalImgKP; /* Key Points of Original Image */
    vector<KeyPoint> TemplateImgKP; /* Key Points of Template Image */
    Mat ResultImg;                  /* Result Image (After NCC) */
};

#endif // NCC_H