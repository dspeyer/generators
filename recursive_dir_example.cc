#include <sys/types.h>
#include <dirent.h>
#include<stdio.h>
#include<string.h>
#include<stack>
#include"generators.h"

using std::stack;

#define MAX_PATH 4096

struct LsDashR : GeneratorHeart<char*> {
  stack<DIR*> dirs; // This needs to live outside of run so we can close them all if needed
  void run(const char* dirname) {
    char fullpath[MAX_PATH]; // on the stack, so safe
    strcpy(fullpath, dirname);
    char* fullpathend = fullpath + strlen(fullpath);
    *fullpathend++ = '/';
    DIR* dir = opendir(dirname);
    if (!dir) return;
    dirs.push(dir);
    while(dirent* file = readdir(dir)) {
      if (file->d_name[0] == '.') {
	continue;
      }
      strcpy(fullpathend, file->d_name);
      if (file->d_type == DT_DIR) {
	run(fullpath);
      } else {
	yield(fullpath);
      }
    }
    closedir(dir);
    dirs.pop();
  }
  ~LsDashR() {
    while (!dirs.empty()) {
      closedir(dirs.top());
      dirs.pop();
    }
  }
};

main() {
  for (char* fn : Gen<LsDashR>("/etc/X11")) {
    printf("%s\n", fn);
  }
}
