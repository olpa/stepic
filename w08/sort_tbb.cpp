#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <cassert>
#include <algorithm>

// tmpbegin is of the same size
void merge_sort(auto in_begin, auto in_end, auto out_begin, auto tmp_begin) {
  auto n = std::distance(in_begin, in_end);
  if (1 == n) {
    *out_begin = *in_begin;
    return;
  }
  auto in_partb  = in_begin  + n/2;
  auto tmp_partb = tmp_begin + n/2;
  auto tmp_end   = tmp_begin + n;
  auto out_partb = out_begin + n/2;
  auto out_end   = out_begin + n;
  merge_sort(in_begin, in_partb, tmp_begin, out_begin);
  merge_sort(in_partb, in_end,   tmp_partb, out_partb);
  auto ptr_a = tmp_begin;
  auto ptr_b = tmp_partb;
  for (auto i = out_begin; i != out_end; ++i) {
    if ((ptr_a >= tmp_partb) || ((ptr_b < tmp_end) && (*ptr_a >= *ptr_b))) {
      *i = *ptr_b++;
    } else {
      *i = *ptr_a++;
    }
  }
}

int main(int argc, char **argv) {
  assert (argc >= 3);
  std::vector<int> v;
  {
    std::ifstream f(argv[1]);
    assert(f);
    std::copy(std::istream_iterator<int>(f), std::istream_iterator<int>(), std::back_inserter(v));
  }
  std::vector<int> tmp(v.size());
  merge_sort(v.begin(), v.end(), v.begin(), tmp.begin());
  //for (int i=0; i < 10; i++) {
  //  std::cout << "v[" << i << "] = " << v[i] << std::endl;
  //}
  {
    std::ofstream f(argv[2]);
    assert(f);
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(f, " "));
  }
}
