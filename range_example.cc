#include<iostream>
#include"generators.h"

struct Range : GeneratorHeart<int> {
  void run(int start, int end, int interval=1) {
    for (int i=start; i<end; i+=interval) {
      yield(i);
    }
  }
};

main() {
  auto iter = Gen<Range>(1,10);
  do {
    std::cout << *iter << std::endl;
  }while(++iter);
  for (int i : Gen<Range>(42,53,2)) {
    std::cout << i << std::endl;
  }
  for (int i : Gen<Range>(-5,0)) {
    std::cout << i << std::endl;
    if (i == -2) break;
  }
}
