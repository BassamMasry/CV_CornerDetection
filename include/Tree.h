#pragma once

#include <vector>
#include <queue>
#include <utility>

struct TreeNode;

class Tree {
private:
  int leafCapacity;
  std::vector<std::vector<float>> points;
  TreeNode* root;

  TreeNode* build(int *idx, int n);
  void nearestNeighbor(TreeNode* node, const std::vector<float>& point, std::priority_queue<std::pair<float, int>>& pq, int k);

public:
  Tree(const std::vector<std::vector<float>>& _points, int leafCapacity = 16);
  ~Tree();
  void nearestNeighbor(const std::vector<float>& point, std::priority_queue<std::pair<float, int>>& pq, int k = 1);
};
