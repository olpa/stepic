#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

class Solution {
  public:
    int search(vector<int>& nums, int target) {
      int pos = 0;
      for (auto v: nums) {
        if (target == v) {
          break;
        }
        pos++;
      }
      return (pos < nums.size()) ? pos : -1;
    }
};

int main() {
  std::vector<int> v{4,5,6,7,0,1,2};
  int val;
  while (1) {
    std::cout << "Value: ";
    std::cin >> val;
    std::cout << " Found at " << Solution().search(v, val) << std::endl;
  }
}
