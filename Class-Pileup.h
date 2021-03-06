#ifndef __PILEUP_H_INCLUDED__
#define __PILEUP_H_INCLUDED__

#include "Class-Traceback.h"
#include "Class-Node.h"

class Pileup {
 public:
  Pileup(vector<Traceback>, string);
  map<Node *, vector<vector<int> > > getPileup();
  void printPileup();

 private:
  void sumTracebacks();
  string ref_;
  map<Node *, vector<vector<int> > > pileup_;
  vector<Traceback> tbv_;

};

#endif // __PILEUP_H_INCLUDED__
