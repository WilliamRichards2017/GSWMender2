#ifndef __TRACEBACK_H_INCLUDED__
#define __TRACEBACK_H_INCLUDED__


#include "Class-GraphAlignment.h"
#include "Class-Node.h"
#include "Coords.h"


class Traceback {

 public:
  Traceback(GraphAlignment*, coords*);
  GraphAlignment *ga_;
  string getQuery();
  map<Node *, vector<vector<int> > > getTracebackMap();
  vector<pair<char, int> > parseCigar(string);

 private:
  coords *c_;
  vector<string> cigars_;
  string query_;
  int queryPos_;
  
  map<Node *, vector<vector<int> > > tracebacks_;
  void trimQuery();
  vector<vector<int> > buildTB(Node *);

};

#endif // __TRACEBACK_H_INCLUDED__
