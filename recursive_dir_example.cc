#include <sys/types.h>
#include <dirent.h>
#include<string>
#include<iostream>
#include"generators.h"

using namespace std;

#define MAX_PATH 4096

struct Ls : GeneratorHeart<dirent*> {
  DIR* dir;
  string dirname; // Just so that we can report it in the debugging info
  void run(string _dirname) {
    dirname = _dirname;
    dir = opendir(dirname.c_str());
    while(dirent* file = readdir(dir)) {
      yield(file);
    }
  }
  // We need a desctructor here because DIRs are a c-style concept
  ~Ls(){
    closedir(dir);
    cout << "closing " << dirname << endl;  // So you can see it works
  }
};

struct LsDashR : GeneratorHeart<string> {
  void run (string dirname) {
    for (dirent* file : Gen<Ls>(dirname)) {
      if (file->d_name[0] == '.') {
	continue;
      }
      string fullpath = dirname + "/" + file->d_name;
      if (file->d_type == DT_DIR) {
	run(fullpath);
      } else {
	yield(fullpath);
      }
    }
  }
  // We don't need a destructor because everything is RAII
};

main() {
  for (string fn : Gen<LsDashR>("/etc/X11")) {
    cout << fn << endl;
    if (fn == "/etc/X11/xinit/xinput.d/all_ALL") {
      break;
    }
  }
}
