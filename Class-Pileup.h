#ifndef __PILEUP_H_INCLUDED__
#define __PILEUP_H_INCLUDED__

#include "Class-Traceback.h"
#include "Class-Node.h"

class Pileup {
 public:
  Pileup(vector<Traceback>);
  map<Node *, vector<vector<int> > > getPileup();
  void printPileup();

 private:
  void sumTracebacks();
  map<Node *, vector<vector<int> > > pileup_;
  vector<Traceback> tbv_;

};

#endif // __PILEUP_H_INCLUDED__
