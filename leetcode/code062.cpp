#include <iostream>
#include <vector>

class Solution {
public:
    int uniquePaths(int m, int n) {
      using V = std::vector<int>;
      unsigned int cmn = 1;
      V prev(m, 1);
      for (int i=1; i < n; i++) {
        V cur(m, 0);
        cmn = 0;
        auto iprev = prev.begin();
        auto icur  = cur.begin();
        for (int j=0; j < m; j++) {
          cmn += *iprev++;
          //std::cout << "cmn = " << cmn << std::endl;
          *icur++ = cmn;
        }
        prev = cur;
      }
      return cmn;
    }
};

int main() {
  Solution s;
  int m, n;
  while (1) {
    std::cout << "C^m_n, what are m and n?" << std::endl;
    std::cin >> m >> n;
    std::cout << "C_" << m << "_" << n << " = " << s.uniquePaths(m, n) << std::endl;
  }
}
