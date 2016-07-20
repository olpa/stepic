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
          *icur++ = cmn;
        }
        prev = cur;
      }
      return cmn;
    }

    int altcalc(int a, int b) {
      unsigned long int row = a + b - 2;
      if (! row) {
        return 1;
      }
      unsigned long int nth = std::min(a, b) - 1;
      if (! nth) {
        return 1;
      }
      unsigned long int k = row - nth + 1;
      unsigned long long int cmn = k;
      unsigned long int c = 1;
      //std::cout << "row, nth, c, k = " << row << ", " << nth << ", " << c << ", " << k << std::endl;
      while (c < nth) {
        cmn = cmn * ++k / ++c;
        std::cout << "cmn, k, c: " << cmn << ", " << k << ", " << c << std::endl;
      }
      return cmn;
    }
};

int main() {
  Solution s;
  int m, n;
  while (1) {
    std::cout << "C^m_n, what are m and n?" << std::endl;
    n = 3;
    m = 2;
    std::cin >> m >> n;
    std::cout << "C_" << m << "_" << n << " = " << s.uniquePaths(m, n) << " // " << s.altcalc(m,n) << std::endl;
  }
}
