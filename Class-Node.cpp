//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Class-Node
// Class code for Varioant Graph Node object
// Copyright 2015 Gabor T. Marth, University of Utah
// All rights reserved
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <iterator>
#include <algorithm>
#include <cmath>

// "boost" regular expression library
#include <boost/regex.hpp>
#include <boost/format.hpp>


#include "Class-Node.h"

using std::ios;
using std::ifstream;
using std::ofstream;
using std::ostream;
using std::istream;
using std::fstream;
using std::cin;
using std::cout;
using std::clog;
using std::endl;
using std::string;
using std::vector;
using std::deque;
using std::map;
using std::min;
using std::max;

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// utility routines
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// static class-wide variable initializations
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// class methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
Node::Node(
	   string idIn, // ID
	   string sequenceIn, // node sequence
	   vector<Node *> contributorsIn // contributor nodes
	   ) {
  //----------------------------------------------------------------------------
  // derived quantities
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // put input into object fields
  //----------------------------------------------------------------------------
  id = idIn;
  sequence = sequenceIn;
  contributors = contributorsIn;
}

//------------------------------------------------------------------------------
// getId()
//------------------------------------------------------------------------------
string Node::getId() {
  return id;
}


//------------------------------------------------------------------------------
// getSubjectLength()
//------------------------------------------------------------------------------
string Node::getSequence() {
  return sequence;
}

//------------------------------------------------------------------------------
// getContributors()
//------------------------------------------------------------------------------
vector<Node *> Node::getContributorNodes() {
  return contributors;
}

int Node::getQueryStart(){
  return queryStart_;
}

int Node::getQueryEnd(){
  return queryEnd_;
}

void Node::setQueryStart(int q){
  queryStart_ = q;
}

void Node::setQueryEnd(int q){
  queryEnd_ = q;
}


//------------------------------------------------------------------------------
// printNode()
//------------------------------------------------------------------------------
void Node::printNode(ostream &out) {
  out << "Node ID=" << id << endl;
  out << "Node sequence=" << sequence << endl;
  out << "Contributor nodes=(";
  for (vector< Node *>::const_iterator iter = contributors.begin(); iter != contributors.end(); iter++) {
    Node * node = * iter;
    out << node->getId() << ",";
  }
  out << ")" << endl;
}


