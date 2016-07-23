#include <iostream>
#include <iterator>
#include <vector>
#include "gtest/gtest.h"

//g++ -std=c++11 code033.cpp -lgtest

using namespace std;

class Solution {
  public:
    int search(vector<int>& nums, int target) {
      if (! nums.size()) {
        return -1;
      }
      vector<int>::const_iterator ileft = nums.begin(), iright = nums.end()-1, icur = nums.begin();
      int lval = *ileft, rval = *iright;
      while (ileft != iright) {
        icur = ileft + (iright - ileft) / 2;
        int cval = *icur;
        if (cval == target) {
          ileft = icur;
          break;
        }
        bool right_is_regular = cval <= rval;
        enum { left, right} go;
        if (right_is_regular) {
          if (target <= cval) {
            go = left;
          } else {
            if (target <= rval) {
              go = right;
            } else {
              go = left;
            }
          }
        } else { // left is regular
          if (target >= cval) {
            go = right;
          } else {
            if (target >= lval) {
              go = left;
            } else {
              go = right;
            }
          }
        }
        if (go == right) {
          ileft = icur + 1; // ileft can now =iright, but icur points elsewhere
          lval  = *ileft;
        } else {
          iright = icur; // No -1 for 2-element arrays
          rval   = cval;
        }
      }
      return (target == *ileft) ? (ileft - nums.begin()) : -1;
    }
};

TEST (EmptyVector, EmptyVector) {
  std::vector<int> v{};
  EXPECT_EQ (-1, Solution().search(v, 4));
}

TEST (OneElementVector, OneElementVector) {
  std::vector<int> v{5};
  EXPECT_EQ (-1, Solution().search(v, 4));
  EXPECT_EQ (0,  Solution().search(v, 5));
  EXPECT_EQ (-1, Solution().search(v, 6));
}

TEST (TwoElementVector, TwoElementVector) {
  std::vector<int> v{1, 3};
  EXPECT_EQ (-1, Solution().search(v, 0));
  EXPECT_EQ (0,  Solution().search(v, 1));
  EXPECT_EQ (-1, Solution().search(v, 2));
  EXPECT_EQ (1,  Solution().search(v, 3));
  EXPECT_EQ (-1, Solution().search(v, 4));
}

TEST (Regression, Regression) {
  std::vector<int> v1{4,5,6,7,8,1,2,3};
  EXPECT_EQ (4, Solution().search(v1, 8));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
  /*
  //std::vector<int> v{4,5,6,7,0,1,2};
  //std::vector<int> v{1,3};
  std::vector<int> v{4,5,6,7,8,1,2,3};
  int val;
  while (1) {
    std::cout << "Value: ";
    std::cin >> val;
    std::cout << " Found at " << Solution().search(v, val) << std::endl;
  }
*/
}
