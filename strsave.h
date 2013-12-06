#include <string.h>
#include <stdlib.h>


class strsave {
  char *ptr;
public:
  strsave(char*);
  ~strsave();
  void dump();
  char *get();
  bool equals(char *);
};

