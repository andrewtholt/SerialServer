#include <string.h>
#include <stdlib.h>


#ifdef __cplusplus
class strsave {
  char *ptr;
public:
  strsave(char*);
  ~strsave();
  void dump();
  char *get();
  bool equals(char *);
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
void *mkString(char *ptr);
void dumpString( void *foo);
void killString(void *foo);

#ifdef __cplusplus
}
#endif
