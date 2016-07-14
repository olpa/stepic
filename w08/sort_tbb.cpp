#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <cassert>
#include <algorithm>

int main(int argc, char **argv) {
  assert (argc >= 2);
  std::vector<int> v;
  {
    std::ifstream f(argv[1]);
    assert(f);
    std::copy(std::istream_iterator<int>(f), std::istream_iterator<int>(), std::back_inserter(v));
  }
  std::cout << "N = " << v.size() << std::endl;
}
