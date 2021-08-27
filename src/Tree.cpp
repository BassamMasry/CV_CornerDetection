#include <algorithm>
#include <cassert>
#include <iostream>
#include <cmath>
#include <utility>
#include "../include/Tree.h"

using namespace std;

float distance(const vector<float>& a, const vector<float>& b) {
  float ans = 0;
  for (int i = a.size(); i--; ) {
    float t = a[i] - b[i];
    ans += t * t;
  }
  return sqrt(ans);
}

struct TreeNode {
  int pivot;
  float radius;
  TreeNode *left, *right;
  vector<int> pointList;
  TreeNode(int id, float rad)
    : pivot(id)
    , radius(rad)
    , left(NULL)
    , right(NULL)
  {}
  ~TreeNode() {
    if (this->left) delete this->left;
    if (this->right) delete this->right;
  }
};

Tree::Tree(const vector<vector<float>>& _points, int leafCapacity)
  : points(_points)
  , leafCapacity(leafCapacity)
{
  int *idx = new int[_points.size()];
  for (int i = _points.size(); i--; ) {
    idx[i] = i;
  }
  this->root = this->build(idx, _points.size());
  delete[] idx;
}

TreeNode* Tree::build(int *idx, int n) {
  if (n <= 0) return NULL;
  int dimension = 0;
  float maxSpread = 0;
  float minDimensionVal;
  for (int f = this->points[0].size(); f--; ) {
    float minval = this->points[idx[0]][f];
    float maxval = minval;
    for (int i = n; i--; ) {
      int trueI = idx[i];
      minval = min(minval, this->points[trueI][f]);
      maxval = max(maxval, this->points[trueI][f]);
    }
    float spread = maxval - minval;
    if (spread > maxSpread) {
      maxSpread = spread;
      dimension = f;
      minDimensionVal = minval;
    }
  }

  int centerPoint = idx[0];
  float dis = abs(this->points[centerPoint][dimension] - minDimensionVal - maxSpread / 2);
  for (int i = 1; i < n; ++i) {
    int trueI = idx[i];
    float td = abs(this->points[trueI][dimension] - minDimensionVal - maxSpread / 2);
    if (td > dis) continue;
    centerPoint = trueI;
    dis = td;
  }

  float radius = 0;
  for (int i = n; i--; ) {
    int trueI = idx[i];
    radius = max(radius, distance(this->points[trueI], this->points[centerPoint]));
  }

  TreeNode* ans = new TreeNode(centerPoint, radius);
  if (n <= this->leafCapacity) {
    ans->pointList.insert(ans->pointList.end(), idx, idx + n);
    return ans;
  }

  auto comparator = [&](int u, int v) -> bool {
    return this->points[u][dimension] < this->points[v][dimension];
  };
  nth_element(idx, idx + n / 2, idx + n, comparator);
  ans->left = this->build(idx, n / 2);
  ans->right = this->build(idx + n / 2, n - n / 2);
  return ans;
}

Tree::~Tree() {
  delete this->root;
}

void Tree::nearestNeighbor(
  const vector<float>& point,
  priority_queue<pair<float, int>>& pq,
  int k /* 1 */
) {
  this->nearestNeighbor(this->root, point, pq, k);
}

void Tree::nearestNeighbor(
  TreeNode* node,
  const vector<float>& point,
  priority_queue<pair<float, int>>& pq,
  int k
) {
  if (!node) return ;

  vector<float>& pivot = this->points[node->pivot];
  float dis = distance(pivot, point);
  float threshold = pq.size() ? pq.top().first : dis;

  if (dis - node->radius > threshold and pq.size() == k) return;

  if (!node->left && !node->right) {  // is leaf
    for (auto i: node->pointList) {
      float d = distance(this->points[i], point);
      pq.push({d, i});
      if (pq.size() > k) pq.pop();
    }
    return ;
  }

  float dis1 = 0, dis2 = 0;
  dis1 = distance(point, this->points[node->left->pivot]);
  dis2 = distance(point, this->points[node->right->pivot]);
  if (dis1 > dis2) swap(node->left, node->right);
  this->nearestNeighbor(node->left, point, pq, k);
  this->nearestNeighbor(node->right, point, pq, k);
}
