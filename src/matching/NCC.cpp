#include "../../include/matching/NCC.h"
#include <math.h>

NCC::NCC(Mat original_image, Mat template_image, vector<KeyPoint> original_image_keypoints, vector<KeyPoint> template_image_keypoints)
{
    OriginalImg = original_image;
    TemplateImg = template_image;
    OriginalImgKP = original_image_keypoints;
    TemplateImgKP = template_image_keypoints;
}


void NCC::testNCC()
{
    vector<vector<float>> descriptor1 = NormalizedCrossCorrelation(OriginalImg, OriginalImgKP);
    vector<vector<float>> descriptor2 = NormalizedCrossCorrelation(TemplateImg, TemplateImgKP);

    vector<float> mean1 = DesMean(descriptor1);
    vector<float> mean2 = DesMean(descriptor2);

    vector<DMatch> matches = NCCMatching(mean1, mean2, descriptor1, descriptor2, 0.90);
    Mat testImg;

    namedWindow("Matches", WINDOW_AUTOSIZE);

    drawMatches(OriginalImg, OriginalImgKP, TemplateImg, TemplateImgKP, matches, testImg, Scalar::all(-1));

    imshow("Matches", testImg);
    waitKey(0);

}

vector<vector<float>> NCC::NormalizedCrossCorrelation(Mat Image, vector<KeyPoint> &KeyPoint)
{
    vector<vector<float>> descriptor(KeyPoint.size(), vector<float>(16));
    const int px[] = {-1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2};
    const int py[] = {-3, -3, -3, -2, -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2};

    for (int i = KeyPoint.size(); i--;)
    {
        float x = (int)KeyPoint[i].pt.x;
        float y = (int)KeyPoint[i].pt.y;
        for (int j = 0; j < 16; j++)
        {
            int des = Image.at<uchar>(y + py[j], x + px[j]);
            descriptor[i][j] = des - 127;
        }
    }

    return descriptor;
}

vector<float> NCC::DesMean(vector<vector<float>> &descriptor)
{
    vector<float> mean;
    for (int i = 0; i < descriptor.size(); i++)
    {
        float sum = 0.0f;
        for (int j = 0; j < 16; j++)
        {
            sum += (float)descriptor[i][j];
        }
        mean.push_back(sum / descriptor.size());
    }
    return mean;
}

vector<DMatch> NCC::NCCMatching(vector<float> &mean1, vector<float> &mean2, vector<vector<float>> &descriptor1, vector<vector<float>> &descriptor2, float threshold)
{
    int minimum = (descriptor1.size() > descriptor2.size()) ? descriptor2.size() : descriptor1.size();
    int maximum = (descriptor1.size() > descriptor2.size()) ? descriptor1.size() : descriptor2.size();
    vector<vector<float>> des1 = (descriptor1.size() > descriptor2.size()) ? descriptor2 : descriptor1;
    vector<vector<float>> des2 = (descriptor1.size() > descriptor2.size()) ? descriptor1 : descriptor2;
    vector<float> m1 = (mean1.size() > mean2.size()) ? mean2 : mean1;
    vector<float> m2 = (mean1.size() > mean2.size()) ? mean1 : mean2;
    vector<float> NCCresult;
    vector<DMatch> Matches;

    for (int i = 0; i < minimum; i++)
    {
        double maxNCC = 0.0f;
        int IndX = 0;
        for (int k = 0; k < maximum; k++)
        {
            double numenator = 0.0f;
            double dominator = 0.0f;
            double div = 0.0f;
            for (int j = 0; j < 16; j++)
            {
                numenator += ((double)des1[i][j] - m1[i]) * ((double)des2[k][j] - m2[k]);
                dominator += (double)sqrt(pow(((double)des1[i][j] - m1[i]), 2) * pow(((double)des2[k][j] - m2[k]), 2));
            }
            div = double((numenator / dominator));
            NCCresult.push_back(div);
            if (div > maxNCC)
            {
                maxNCC = div;
                IndX = k;
            }
        }
        if (maxNCC > threshold)
        {
            Matches.push_back(DMatch(IndX, i, maxNCC));
        }
    }

    return Matches;
}
