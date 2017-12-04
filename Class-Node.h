#ifndef __NODE_H__
#define __NODE_H__

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Class-Node
// Class definition for Variang Graph Node object
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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// utility routines
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// type definitions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// class definition
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


class Node {

public:

  //----------------------------------------------------------------------------
  // public functions
  //----------------------------------------------------------------------------

  // constructor
  Node(
       string, // id
       string, // sequence
       vector<Node *> // list of contributor nodes
       );
  
  // getId
  string getId();

  // getSequence
  string getSequence();

  // getContributorNodes
  vector<Node *> getContributorNodes();

  // printNode
  void printNode(ostream&);

  int getQueryStart();
  int getQueryEnd();			       
  void setQueryStart(int);
  void setQueryEnd(int);

  //----------------------------------------------------------------------------
  // public variables
  //----------------------------------------------------------------------------

private:

  //----------------------------------------------------------------------------
  // private variables
  //----------------------------------------------------------------------------
  string id;
  string sequence;
  vector<Node *> contributors;
  int queryStart_;
  int queryEnd_;
};

#endif
