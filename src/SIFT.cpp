#include "../include/SIFT.h"
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
// #include <math.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include "../include/greyscale.h"

using std::list, std::vector;
using std::cout, std::endl;

sift::sift(Mat image)
{
    img = image;
    greyscale gray(img);
    applySIFT(gray.grayscaled);
    cv::drawKeypoints(gray.grayscaled, keypoints, sift_Image);
}

float sift::matmul(const float h[3],const float H[3][3])
{
    float a[3];
    a[0] = h[0] * H[0][0] + h[1] * H[1][0] + h[2] * H[2][0];
    a[1] = h[0] * H[0][1] + h[1] * H[1][1] + h[2] * H[2][1];
    a[2] = h[0] * H[0][2] + h[1] * H[1][2] + h[2] * H[2][2];
    float val = a[0] * h[0] + a[1] * h[1] + a[2] * h[2];
    return val;
}


void sift::applySIFT(Mat img)
{
  uint num_octaves = 4;
  uint num_scales = 5;
  uint edgeThreshold = 10;
  float _contrastThreshold = 0.03F;
  float sigma = 1.6f;
  float k = 1.414f;
  uint contrastThreshold = 255 * _contrastThreshold + 1;
  // make a 2D array of scales
  Mat scales[num_octaves][num_scales];
  // make Difference of gaussian matrix
  Mat DoG[num_octaves][num_scales - 1];
  uint index1 = 0;
  uint index2 = 0;
  uint index3 = 0;
  // Accept only char type matrix
  CV_Assert(img.depth() == CV_8U);
  // convert image to grayscale
  Mat gray;
  if (img.channels() > 1)
  { cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY); }
  // else clone original image to color_photo
  else {gray = img.clone();}

  scales[0][0] = gray;

  // Get octaves
  for (index1 = 1; index1 < num_octaves; ++index1)
  {
    // get new width and height
    int width = scales[index1 - 1][0].cols / 2;
    int height = scales[index1 - 1][0].rows / 2;

    cv::Size newSize(width, height);

    // resize image
    Mat resized;
    cv::resize(scales[index1 - 1][0], resized, newSize);
    scales[index1][0] = resized.clone();
  }

  // _____________________________________________

  // Get scales
  for (index1 = 0; index1 < num_octaves; ++index1)
  {
    Mat blurred;
    for (index2 = 1; index2 < num_scales; ++index2)
    {
      cv::Size ksize(0,0);
      cv::GaussianBlur(scales[index1][index2 - 1], blurred, ksize, pow(k, index2) * sigma);
      scales[index1][index2] = blurred.clone();
    }
  }

  // _____________________________________________

  // Get difference of gaussian
  for (index1 = 0; index1 < num_octaves; ++index1)
  {
    for (index2 = 0; index2 < num_scales - 1; ++index2)
    {
      uint x = 0;
      uint y = 0;
      uchar * p_temp;
      uchar * d_temp;

      Mat temp = scales[index1][index2].clone();

      // get the difference betweeen current and next scale index
      for(y = 0; y < temp.rows; ++y)
      {
        p_temp = temp.ptr <uchar> (y);
        d_temp = scales[index1][index2 + 1].ptr <uchar> (y);
        for(x = 0; x < temp.cols; ++x)
        {
          p_temp[x] = p_temp[x] - d_temp[x];
        }
      }

      DoG[index1][index2] = temp.clone();
    }
  }

  // _____________________________________________

  // Get maximas and minimas
  // list < xyoctscale > maximas;
  // list < xyoctscale > minimas;
  list < xyoctscale > extremas;

  for (index1 = 0; index1 < num_octaves; ++index1)
  {
    for (index2 = 0; index2 < num_scales - 3; ++index2)
    {
      uint y = 0;
      uint x = 0;
      char maxima = 1;
      char minima = 1;
      uint scale_in = 0;
      uint y_in = 0;
      uint x_in = 0;

      int calc = 0;

      for(y = 0; y < DoG[index1][index2].rows - 2; ++y)
      {
        for(x = 0; x < DoG[index1][index2].cols - 2; ++x)
        {
          maxima = 1;
          minima = 1;
          for(scale_in = 0; scale_in < 3; ++scale_in)
          {
            for(y_in = 0; y_in < 3; ++y_in)
            {
              for(x_in = 0; x_in < 3; ++x_in)
              {
                if ((!maxima) && (!minima)) { break; }
                calc = (int) DoG[index1][index2].at <uchar> (y,x) -
                 (int) DoG[index1][index2 + scale_in].at <uchar> (y + y_in, x + x_in);


                if (calc > 0) { minima = 0; }
                if (calc < 0) { maxima = 0; }
              }
            }
          }
          if (maxima && minima) { continue; }
          if (maxima)
          {
            xyoctscale tempxy = {x, y, index1, index2};
            extremas.push_back(tempxy);
          }
          if (minima)
          {
            xyoctscale tempxy = {x, y, index1, index2};
            extremas.push_back(tempxy);
          }
        }
      }
    }
  }

  cout << "Extrmas size: " << extremas.size() << endl;

  // _____________________________________________

  /*
  uint counter = 0;
  for (std::list<xyoctscale>::iterator it=maximas.begin(); it != maximas.end(); ++it)
  {
    if (counter > 200) { break; }
    printf("Max; x: %d, y: %d, o: %d, s: %d\n", it -> x, it -> y, it -> o, it -> s);
    counter += 1;
  }
  printf("\n\n");
  counter = 0;
  for (std::list<xyoctscale>::iterator it=minimas.begin(); it != minimas.end(); ++it)
  {
    if (counter > 100) { break; }
    printf("Max; x: %d, y: %d, o: %d, s: %d\n", it -> x, it -> y, it -> o, it -> s);
    counter += 1;
  }
  */

  // _____________________________________________

  // Apply edge and contrast filters
  list < xyoctscale > contrast_extremas;

  for (std::list<xyoctscale>::iterator it=extremas.begin(); it != extremas.end(); ++it)
  {
    float dx;
    float dy;
    float ds;
    int x;
    int y;
    uint o;
    int s;
    // Dummy variables for calculations
    float calc1;
    float calc2;
    float calc3;

    x = it -> x;
    y = it -> y;
    o = it -> o;
    s = it -> s;

    char boolean = (x + 1 >= scales[o][s].cols);
    boolean = boolean || (y + 1 >= scales[o][s].rows);
    boolean = boolean || (s + 1 >= num_scales);
    if (boolean) { continue; }

    if ((x - 1 < 0) || (y - 1 < 0) || (s - 1 < 0)) { continue; }

    dx = ((float) scales[o][s].at <uchar> (y, x + 1) - (float) scales[o][s].at <uchar> (y, x - 1)) / 2;
    dy = ((float) scales[o][s].at <uchar> (y + 1, x) - (float) scales[o][s].at <uchar> (y - 1, x)) / 2;
    ds = ((float) scales[o][s + 1].at <uchar> (y, x) - (float) scales[o][s - 1].at <uchar> (y, x)) / 2;

    float D[3] = {dx,dy,ds};
    float h[3] = {(float) x, (float) y, (float) s};

    calc1 = scales[o][s].at<uchar>(y, x+1);
    calc2 = scales[o][s].at<uchar>(y, x);
    calc3 = scales[o][s].at<uchar>(y, x - 1);
    float dxx = ( calc1 - 2 * calc2 + calc3 ) / 3;

    calc1 = scales[o][s].at<uchar>(y+1, x);
    // calc2 = scales[o][s].at<uchar>(y, x);
    calc3 = scales[o][s].at<uchar>(y - 1, x);
    float dyy = ( calc1 - 2 * calc2 + calc3 ) / 3;

    calc1 = scales[o][s+1].at<uchar>(y, x);
    // calc2 = scales[o][s].at<uchar>(y, x);
    calc3 = scales[o][s-1].at<uchar>(y, x);
    float dss = ( calc1 - 2 * calc2 + calc3 ) / 3;

    calc1 = (float) (scales[o][s].at<uchar>(y+1, x+1)) + (float) (scales[o][s].at<uchar>(y-1, x-1));
    calc2 = float(scales[o][s].at<uchar>(y-1, x+1) + scales[o][s].at<uchar>(y+1, x-1));
    float dxy = (calc1 - calc2) / 4;

    calc1 = (float) (scales[o][s+1].at<uchar>(y, x+1)) + (float) (scales[o][s-1].at<uchar>(y, x-1));
    calc2 = (float) (scales[o][s-1].at<uchar>(y, x+1)) + (float) (scales[o][s+1].at<uchar>(y, x-1));
    float dxs = (calc1 - calc2) / 4;

    calc1 = (float) (scales[o][s+1].at<uchar>(y+1, x)) + (float) (scales[o][s-1].at<uchar>(y-1, x));
    calc2 = (float) (scales[o][s-1].at<uchar>(y+1, x)) + (float) (scales[o][s+1].at<uchar>(y-1, x));
    float dys = (calc1 - calc2) / 4;

    float H[3][3] = {
      {dxx, dxy, dxs},
      {dxy, dyy, dys},
      {dxs, dys, dss}
    };

    float trace_squared = (dxx + dyy) * (dxx + dyy);
    float det = dxx * dyy - dxy * dxy;
    if (det == 0) {continue;}

    float Threshval = (float) (scales[o][s].at<uchar>(y, x)) + (dx * x + dy * y + ds * s) + (0.5 * matmul(h, H));

    char edgeBool = (trace_squared / det) > ( (edgeThreshold + 1) * (edgeThreshold + 1) / edgeThreshold);
    char contrastBool = (std::abs(Threshval)) > contrastThreshold;

    if(edgeBool && contrastBool){
      xyoctscale temp = {(uint) x, (uint) y, o, (uint) s};
      contrast_extremas.push_back(temp);
    }
  }

  cout << "Contrast size: " << contrast_extremas.size() << endl;

  // _____________________________________________

  list < xyoctscaleangle > features;
  vector<float> hist(36, 0.0);
  for (std::list<xyoctscale>::iterator it=contrast_extremas.begin(); it != contrast_extremas.end(); ++it){
      int x;
      int y;
      uint o;
      int s;
      float theta;

      x = it -> x;
      y = it -> y;
      o = it -> o;
      s = it -> s;

      std::fill(hist.begin(), hist.end(), 0.0);

      float sig = std::pow(k, s) * sigma;

      int x_counter = -1;
      int y_counter = -1;
      int k_size = int(((sig - 0.8) / 0.3 + 1) * 2 + 1);
      if(k_size % 2 == 0)
      {
          k_size += 1;
      }
      int offset = k_size / 2;

      Mat gaussian_window = Mat(k_size, k_size, CV_32FC1, 0.0F);

      gaussian_window.at<float>(offset, offset) = 1.0F;

      cv::GaussianBlur(gaussian_window, gaussian_window, cv::Size(k_size, k_size), -1);

      // cout << "Gauss kernel: " << format(gaussian_window, cv::Formatter::FMT_PYTHON) << endl;

      for(int x_sub = x - offset; x_sub <= x + offset; ++x_sub){
          y_counter = -1;
          x_counter += 1;
          if(x_sub - 1 < 0){ continue; }
          if(x_sub + 1 >= scales[o][s].cols){ continue; }

          for(int y_sub = y - offset; y_sub <= y + offset; ++y_sub){
              y_counter += 1;
              if(y_sub - 1 < 0){ continue; }
              if(y_sub + 1 >= scales[o][s].rows){ continue; }

              float term1 = (float) (scales[o][s].at<uchar>(y_sub, x_sub + 1)) - (float) (scales[o][s].at<uchar>(y_sub, x_sub - 1));
              float term2 = (float) (scales[o][s].at<uchar>(y_sub + 1, x_sub)) - (float) (scales[o][s].at<uchar>(y_sub - 1, x_sub));
              float mag = std::sqrt(term1 * term1 + term2 * term2);

              if(term1 == 0) { theta = 1.5708; }
              else{
                  theta = atan2(term2, term1);
              }
              theta = theta * 180 / 3.14159;
              if(theta < 0){
                  theta += 360;
              }
              theta = int(theta/10);
              // cout << theta << endl;
              hist[theta] += mag * gaussian_window.at<float>(y_counter, x_counter);
          }
      }

      float max_hist = *std::max_element(hist.begin(), hist.end());

      for(uint hist_index = 0; hist_index < 36; ++hist_index)
      {
        if(hist[hist_index] > 0.8 * max_hist){
          xyoctscaleangle temp = {(uint) x, (uint) y, (uint) o,(uint) s, (float) hist_index * 10.0F};

          features.push_back(temp);
        }
      }
  }

  cout << "Features size: " << features.size() << endl;

  // _____________________________________________

  /*
  uint counter = 0;
  for (std::list<xyoctscaleangle>::iterator it=features.begin(); it != features.end(); ++it)
  {
    if (counter > 200) { break; }
    cout << "Max; x: " << it -> x << " y: " << it->y << " o: " << it -> o << " s " << it->s << " theta: " << it-> angle << endl;
    counter += 1;
  }
  */

  // _____________________________________________

  // Get fingerprint

  vector<xyscaleangledescriptor> full_features;
  vector<float> feature_desc (128, 0.0);

  for (std::list<xyoctscaleangle>::iterator it=features.begin(); it != features.end(); ++it)
  {
    int x;
    int y;
    int o;
    int s;
    float angle;
    float term1;
    float term2;
    float mag;
    float theta;

    x = it -> x;
    y = it -> y;
    o = it -> o;
    s = it -> s;
    angle = it -> angle;

    std::fill(hist.begin(), hist.end(), 0.0);
    std::fill(feature_desc.begin(), feature_desc.end(), 0.0);

    int x_counter = -1;
    int offset = 8;
    float sig = offset;

    if(x - offset < 0 || y - offset < 0){ continue; }
    if(x + offset >= scales[o][s].cols || y + offset >= scales[o][s].rows){ continue; }

    Mat sub_sample = Mat(scales[o][s], cv::Range(y - offset , y + offset), cv::Range(x - offset, x + offset)).clone();
    // cout << "Sub mat: " << format(sub_sample, cv::Formatter::FMT_PYTHON) << endl;

    cv::Mat sample_gauss;
    cv::GaussianBlur(sub_sample, sample_gauss, cv::Size(0, 0), sig);
    int counter = 0;

    for(index2 = 0; index2 < 2 * offset; index2 += 4)
    {
      for(int y_in = 0; y_in < 4; ++y_in)
      {
        for(int x_in = 0; x_in < 4; ++x_in)
        {
          counter += 1;
          term1 = (float) (scales[o][s].at<uchar>(y_in, x_in + 1)) - (float) (scales[o][s].at<uchar>(y_in, x_in - 1));
          term2 = (float) (scales[o][s].at<uchar>(y_in + 1, x_in)) - (float) (scales[o][s].at<uchar>(y_in - 1, x_in));
          mag = std::sqrt(term1 * term1 + term2 * term2);

          if(term1 == 0) { theta = 1.5708; }
          else{ theta = atan2(term2, term1); }

          theta = theta * 180 / 3.14159;

          theta -= angle;

          int t_index = theta;

          while (t_index < 0) { t_index += 360; }


          t_index = t_index / 45;


          feature_desc[index2 * t_index + t_index] += mag * sample_gauss.at<uchar>(index2 + y_in, index2 + x_in);

        }
      }
    }

    double summer = 0.0;
    uint featDescSize = feature_desc.size();

    for(int i = 0; i < featDescSize; ++i)
    {
      summer += (feature_desc[i] * feature_desc[i]);
    }
    double root_summer = std::sqrt(summer);
    for(int i = 0; i < featDescSize; ++i)
    {
      feature_desc[i] = (feature_desc[i] / root_summer);
    }

    // achieve illumination invariance

    for (int i = 0; i < featDescSize; ++i)
    {
      if (feature_desc[i] > 0.2) { feature_desc[i] = 0.2; }
    }

    summer = 0.0;
    featDescSize = feature_desc.size();

    for(int i = 0; i < featDescSize; ++i)
    {
      summer += (feature_desc[i] * feature_desc[i]);
    }
    root_summer = std::sqrt(summer);
    for(int i = 0; i < featDescSize; ++i)
    {
      feature_desc[i] = (feature_desc[i] / root_summer);
    }

    xyscaleangledescriptor temp_descriptor;
    temp_descriptor.x = x * (1 << o);
    temp_descriptor.y = y * (1 << o);
    temp_descriptor.s = s;
    temp_descriptor.angle = angle;
    temp_descriptor.feature_desc = feature_desc;

    full_features.push_back(temp_descriptor);

  }

  cout << "Full features size: " << full_features.size() << endl;

  // _____________________________________________

  // Make the actual keypoints

  uint fullsize = full_features.size();
  float x_p = 0.0F;
  float y_p = 0.0F;

  for (uint i = 0; i < fullsize; ++i)
  {
    x_p = full_features[i].x;
    y_p = full_features[i].y;
    cv::KeyPoint kpoint (x_p, y_p, 1, full_features[i].angle);
    keypoints.push_back(kpoint);
  }

}
