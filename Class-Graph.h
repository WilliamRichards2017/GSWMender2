#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "Class-Node.h"
#include "Variant.h"

class Graph{
 public:
  Graph(Variant);
  vector<Node *> getSubjectNodes();
 private:
  Variant variant_;
  vector<string> subjectStrings_;
  vector<Node *> subjectNodes_;
  void buildNodeStrings();
  void buildDiamondGraph();

};

#endif
