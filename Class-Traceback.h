#ifndef __TRACEBACK_H_INCLUDED__
#define __TRACEBACK_H_INCLUDED__


#include "Class-GraphAlignment.h"

class Traceback {

 public:
  Traceback(GraphAlignment*);
  GraphAlignment *ga_;
  vector<vector<vector<int> > > getTracebackVector();
  vector<pair<char, int> > parseCigar(string);

 private:

  vector<string> cigars_;
  string query_;
  int queryPos_;
  vector<vector<vector<int> > > tracebacks_;
  vector<vector<int> > buildTB(Node *);


};

#endif // __TRACEBACK_H_INCLUDED__
