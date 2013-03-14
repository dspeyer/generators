#include<stdio.h>
#include<string>
#include"generators.h"

using std::string;

struct BlockReader : GeneratorHeart<string> {
  FILE* file;
  string block; // Destructor *will* be called
  void run(const char* filename) {
    file = fopen(filename, "r");
    if (!file) return;
    block = "";
    int nesting = 0;
    while(!feof(file)) {
      char c = fgetc(file);
      if (c=='\n' && block.back()=='\n') {
	yield(block);
	block = "";
	continue;
      }
      block += c;
    }
    yield(block);
  }
  ~BlockReader() {
    if (file) {
      fclose(file);
    }
  }
};

main() {
  for (string b : Gen<BlockReader>("file_example.cc")) {
    printf("%s\n---\n", b.substr(0,30).c_str());
  }
}
