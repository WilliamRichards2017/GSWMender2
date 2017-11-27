#include "Class-Graph.h"
#include "Variant.h"

Graph::Graph(Variant variant) : variant_(variant){
  buildNodeStrings();
  buildDiamondGraph();
}

vector<Node *> Graph::getSubjectNodes(){
  return subjectNodes_;
}


void Graph::buildNodeStrings(){
  vector<string> strings;
  strings.push_back(variant_.ref_.substr(0, variant_.pos_));
  strings.push_back(variant_.sv_.first);
  strings.push_back(variant_.sv_.second);
  strings.push_back(variant_.ref_.substr(variant_.pos_ + variant_.sv_.first.length(), variant_.ref_.length()));
  subjectStrings_ = strings;
}

void Graph::buildDiamondGraph(){
  vector<Node *> contributors1;
  Node * node1 = new Node("node1", subjectStrings_[0], contributors1);
  subjectNodes_.push_back(node1);
  vector<Node *> contributors2;
  contributors2.push_back(node1);

  Node * node2 = new Node("node2", subjectStrings_[1], contributors2);
  subjectNodes_.push_back(node2);
  vector<Node *> contributors3;
  contributors3.push_back(node1);

  Node * node3 = new Node("node3", subjectStrings_[2], contributors3);
  subjectNodes_.push_back(node3);
  vector<Node *> contributors4;
  contributors4.push_back(node2);
  contributors4.push_back(node3);

  Node * node4 = new Node("node4", subjectStrings_[3], contributors4);
  subjectNodes_.push_back(node4);
}
