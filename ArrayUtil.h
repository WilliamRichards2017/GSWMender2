#ifndef __ARRAYUTIL_H__
#define __ARRAYUTIL_H__

#include <vector>
#include "Class-Node.h"

using namespace std;

class ArrayUtil{
 public:
  static vector<vector<int> > buildArray2D(unsigned, unsigned);
  static void printNode(Node *, vector<vector<int> >, string);
  static void printArray2D(vector<vector<int> >);
};

#endif
