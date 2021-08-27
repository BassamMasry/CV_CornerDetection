#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include <chrono>
#include <queue>
#include "../../include/Tree.h"
#include "../../include/matching/SSD.h"

using namespace cv;
using namespace std;

SSD::SSD(Mat image1, Mat image2, vector<KeyPoint> keypoints1, vector<KeyPoint> keypoints2)
{
    img1 = image1;
    img2 = image2;
    kp1 = keypoints1;
    kp2 = keypoints2;

}

void SSD::testSSD()
{
 
  namedWindow("Matches", WINDOW_AUTOSIZE);


  vector<DMatch> matches = match(img1, kp1, img2, kp2);

  // choose 50 random matches if there are a lot of them
  random_shuffle(matches.begin(), matches.end());
  matches.resize(min((int)matches.size(), 50));
  
  drawMatches(img1, kp1, img2, kp2, matches, outputMatching, Scalar::all(-1), Scalar(0, 0, 255), vector<char>()); //, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
  imshow("Matches", outputMatching);
  waitKey(0);
}

vector<vector<float>> SSD::getDescriptors(Mat img, const vector<KeyPoint>& kp) {
  vector<vector<float>> ans(kp.size(), vector<float>(16));
  
  const int px[] = {-1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2};
  const int py[] = {-3, -3, -3, -2, -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2};
  for (int i = kp.size(); i--; ) {
    float x = kp[i].pt.x;
    float y = kp[i].pt.y;
    for (int f = 0; f < 16; ++f) {
      int u = img.at<unsigned char>(y + py[f], x + px[f]);
      ans[i][f] = u - 127;
    }
  }
  return ans;
}

vector<DMatch> SSD::match(
  Mat img1, const vector<KeyPoint>& kp1,
  Mat img2, const vector<KeyPoint>& kp2,
  float threshold /* 5000 */
) {
  vector<vector<float>> des0 = getDescriptors(img1, kp1);
  vector<vector<float>> des1 = getDescriptors(img2, kp2);
  Tree stree(des0);
  vector<DMatch> ans;

  for (int i = 0; i < des1.size(); ++i) {
    priority_queue<pair<float, int>> pq;
    stree.nearestNeighbor(des1[i], pq, 5);
    // try to choose closer point (on the image) in case
    // when the point with lowest SSD lays too far.
    int best = 0;
    float dis = 0;
    float eucldis2 = 1e9;
    while (pq.size()) {
      int f = pq.top().second;
      float d = pq.top().first;
      pq.pop();
      float dx = kp2[i].pt.x - kp1[f].pt.x;
      float dy = kp2[i].pt.y - kp1[f].pt.y;
      if (eucldis2 > dx * dx + dy * dy) {
        eucldis2 = dx * dx + dy * dy;
        dis = d * d;
        best = f;
      }
    }

    if (threshold != -1 && threshold > dis) {
      ans.push_back(DMatch(best, i, dis));
    }
  }
  return ans;
}
