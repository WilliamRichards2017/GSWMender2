//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Class-GraphAlignment
// Class code for GraphAlignment object
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


#include "Class-GraphAlignment.h"

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
GraphAlignment::GraphAlignment(
			       vector<Node *> subjectNodes, // subject graph nodes
			       string query, // query sequence
			       int M, // match score
			       int X, // mismatch score
			       int GI, // gap initiation score
			       int GE, // gap extension score
			       bool debug // debug flag
			       ) {
  //----------------------------------------------------------------------------
  // derived quantities
  //----------------------------------------------------------------------------
  s2 = query;
  l2 = s2.length();

  //----------------------------------------------------------------------------
  // initialize constants
  //----------------------------------------------------------------------------
  D = 0;
  H = 1;
  V = 2;

  //----------------------------------------------------------------------------
  // initialize scores and related quantities
  //----------------------------------------------------------------------------
  I1M = 0; // running row index of traceback rouute 
  I2M = 0; // running column index of traceback route
  SM = 0; // optimal score

  //----------------------------------------------------------------------------
  // build SW matrices for each consecutive subject node
  //----------------------------------------------------------------------------
  for (vector<Node *>::const_iterator iter = subjectNodes.begin(); iter != subjectNodes.end(); iter++) {
    Node * node = * iter;

    //--------------------------------------------------------------------------
    // retreive necessary Node data
    //--------------------------------------------------------------------------
    string nodeId = node->getId();
    string s1 = node->getSequence();
    int l1 = s1.length();
    vector<Node *> contributorNodes = node->getContributorNodes();

    //--------------------------------------------------------------------------
    // define and initialize score matrix and traceback matrices for this node
    //--------------------------------------------------------------------------
    vector< vector< vector<int> > > S; // Score matrices for Graph nodes: inner two dimension are the row and column for each matrix; outer dimension is node serial
    vector< vector< vector< vector<bool> > > > T; // traceback matrices for Graph nodes; inner dimenstion is 3 possible states; middle two are rows and columns; outer is node serial
   
     // initialize score matrices with all 0s
    for (int i1=0; i1<=l1; i1++) {
      vector< vector<int> > zeroVectorVector;
      for (int i2=0; i2<=l2; i2++) {
	zeroVector.push_back(0);
      }
      zeroVectorVector.push_back(zeroVector);
    }
    
    // initialize gap score matrices with negative numbers
    for (int i1=0; i1<=l1; i1++) {
      // initialize top row with negative numbers that are guaranteed to be less then any observed penalty
      S[i1][0][H] = l1 * (X + GI + GE);
    }

    for (int i2=0; i2<=l2; i2++) {
      // initialize left column with negative numbers that are guaranteed to be less then any observed penalty                                                                                          
      S[0][i2][V] = l2 * (X + GI + GE);
    }
    
    // initialize affine gap penalty trace-back
    for (int i1=0; i1<=l1; i1++) {
      vector< vector< vector<bool> > > falseVectorVectorVector;
      for (int i2=0; i2<=l2; i2++) {
	vector< vector<bool> > falseVectorVector;
	for (int i3=0; i3<=2; i3++) {
	  vector<bool> falseVector;
	  for (int i4=0; i4<=2; i4++) {
	    falseVector.push_back(false);
	  }
	  falseVectorVector.push_back(falseVector);
	}
	falseVectorVectorVector.push_back(falseVectorVector);
      }
      T.push_back(falseVectorVectorVector);
    }

    //--------------------------------------------------------------------------
    // enforce continuity of scores from contributor nodes in utility column
    //--------------------------------------------------------------------------
    for (vector<Node *>::const_iterator iter = contributorNodes.begin(); iter != contributorNodes.end(); iter++) {
      Node * contributorNode = * iter;
      
      // retreive score matrix, sequence, and length for contributor node
      vector< vector< vector<int> > > SC = GS[contributorNode]; // Score matrices
      vector< vector< vector< vector<bool> > > > TC = GT[contributorNode]; // traceback matrices
      string s1 = contributorNode->getSequence();
      int l1 = s1.length();

      // update utility lane in current score matrix with higher values from this contributor
      for (int i2=0; i2<=l2; i2++) {
	if (SC[l1][i2][D] > S[0][i2][D]) {
	  S[0][i2][D] = SC[l1][i2][D]; 
	  T[0][i2][D][D] = TC[l1][i2][D][D]; 
	  T[0][i2][D][H] = TC[l1][i2][D][H]; 
	  T[0][i2][D][V] = TC[l1][i2][D][V]; 
	}
	if (SC[l1][i2][H] > S[0][i2][H]) {
	  S[0][i2][H] = SC[l1][i2][H];
	  T[0][i2][H][D] = TC[l1][i2][H][D];
	  T[0][i2][H][H] = TC[l1][i2][H][H];
	}
	if (SC[l1][i2][V] > S[0][i2][V]) {
	  S[0][i2][V] = SC[l1][i2][V];
	  T[0][i2][V][D] = TC[l1][i2][V][D];
	  T[0][i2][V][V] = TC[l1][i2][V][V];
	}
      }
    }
    
    //--------------------------------------------------------------------------
    // fill score matrices
    //--------------------------------------------------------------------------
    for (int i1=1; i1<=l1; i1++) {
      for (int i2=1; i2<=l2; i2++) {
	
	// calculate diagonal score (match or mismatch)
	string b1 = s1.substr(i1-1, 1); // subject base
	string b2 = s2.substr(i2-1, 1); // query base	
	int mx = 0;
	if (b1 == b2) {mx = M;} else {mx = X;} // update with match or mismatch score
	
	// calculate and register Match optimal score
	int mdd = S[i1-1][i2-1][D] + mx;
	int mdh = S[i1-1][i2-1][H] + mx;
	int mdv = S[i1-1][i2-1][V] + mx;
	int md = max(max(max(mdd, mdh), mdv), 0);
	S[i1][i2][D] = md;
	
	// determine and register Match tracebacks
	if (md > 0) {
	  if (mdd == md) {T[i1][i2][D][D] = true;}
	  if (mdh == md) {T[i1][i2][D][H] = true;}
	  if (mdv == md) {T[i1][i2][D][V] = true;}
	}
	
	// calculate and register horizontal gap optimal score
	int ihd = S[i1-1][i2][D] + GI;
	int ihh = S[i1-1][i2][H] + GE;
	int ih =  max(ihd, ihh);
	S[i1][i2][H] = ih;
	
	// determine and register Horizontal Gap tracebacks
	if (ih > 0) {
	  if (ihd == ih) {T[i1][i2][H][D] = true;}
	  if (ihh == ih) {T[i1][i2][H][H] = true;}
	}
	
	// calculate and register vertical gap optimal score
	int ivd = S[i1][i2-1][D] + GI;
	int ivv = S[i1][i2-1][V] + GE;
	int iv = max(ivd, ivv);
	S[i1][i2][V] = iv;
	
	// determine and register Horizontal Gap tracebacks
	if (iv > 0) {
	  if (ivd == iv) {T[i1][i2][V][D] = true;}
	  if (ivv == iv) {T[i1][i2][V][V] = true;}
	}
	
	// update largest affine score, and the indices or the corresponding cell
	if (md > SM) {
	  SM = md;
	  I1M = i1;
	  I2M = i2;
	  NodeM = node;
	  StateM = D;
	}
      } 
    }

    // register score and trace-back with the node
    GS[node] = S;
    GT[node] = T;
    if (debug) {
      printMatrix(node, cout);
    }
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // trace back: fill trace-back data structures
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // initialize starting point for traceback using values updated during the
  // score and traceback filling step
  //----------------------------------------------------------------------------
  int i1 = I1M;
  int i2 = I2M;
  int state = StateM;
  Node * node = NodeM;
  string s1 = node->getSequence();
  int l1 = s1.length();

  int s = GS[node][i1][i2][D];
  string a1 = "";
  string a2 = "";

  globalCigar = "";
  string nodeCigar = "";
  int nodeOffset = 0;
  string mt;
  int ml;
  int nml;

  // initialize by soft clipping unaligned region of query
  mt = "S";
  ml = l2 - i2;
  nml = l2 - i2;

  //----------------------------------------------------------------------------
  // construct alignment strings according to traceback
  //----------------------------------------------------------------------------
  while (s > 0) {

    //--------------------------------------------------------------------------
    // check for node boundary: figure out which contributor node to switch back to
    // in case the subject node is a deletion with no sequence, multiple consecutive node boundaries are possible
    //--------------------------------------------------------------------------
    while (i1 == 0) {

      // register last match element
      if (nml > 0) {
	nodeCigar = mt + to_string(nml) + nodeCigar;
      }
      
      // register matched node
      matchedNodes.push_back(node);

      // register corresponding CIGAR and offset
      nodeCigarMap[node] = nodeCigar;
      nodeOffsetMap[node] = i1;

      // add node name
      nodeCigar = "";
      
      // reset node match length
      nml = 0;

      vector<Node *> contributorNodes = node->getContributorNodes();
      Node * newNode;
      for (vector<Node *>::const_iterator iter = contributorNodes.begin(); iter != contributorNodes.end(); iter++) {
	Node * contributorNode = * iter;
      
	// retreive score matrix, sequence, and length for contributor node
	string cs1 = contributorNode->getSequence();
	int cl1 = cs1.length();

	// determine if the best score came from this contributor node
	if (GS[contributorNode][cl1][i2][state] == GS[node][0][i2][state]) {
	  newNode = contributorNode;
	  s1 = cs1;
	  i1 = cl1;
	}
      }

      // assign node to continue
      node = newNode;
      s1 = node->getSequence();
      i1 = s1.length();
    }
    
    //--------------------------------------------------------------------------
    // within-node trace-back
    //--------------------------------------------------------------------------

    // diagonal trace-back
    if (state == D) {
      string b1 = s1.substr(i1-1, 1); // subject base
      string b2 = s2.substr(i2-1, 1); // query base
      a1 = b1 + a1; // add subject base to alignment
      a2 = b2 + a2; // add quary base to alignment
      if (GT[node][i1][i2][D][D]) {state = D; s = GS[node][i1-1][i2-1][D];}
      else if (GT[node][i1][i2][D][H]) {state = H; s = GS[node][i1-1][i2-1][H];}
      else if (GT[node][i1][i2][D][V]) {state = V; s = GS[node][i1-1][i2-1][V];}
      i1--; // decrement subject index
      i2--; // decrement query index

      // manage cigars
      if (mt != "M") {
	if (ml > 0) {
	  globalCigar = mt + to_string(ml) + globalCigar;
	}
	if (nml > 0) {
	  nodeCigar = mt + to_string(nml) + nodeCigar;
	}
	mt = "M";
	ml = 1;
	nml = 1;
      }
      else {
	ml++;
	nml++;
      }
    }
    // horizontal trace-back
    else if (state == H) {
      string b1 = s1.substr(i1-1, 1); // subject base
      string b2 = "-"; // query base is a gap
      a1 = b1 + a1; // add subject base to alignment
      a2 = b2 + a2; // add quary base to alignment
      if (GT[node][i1][i2][H][D]) {state = D; s = GS[node][i1-1][i2][D];}
      else if (GT[node][i1][i2][H][H]) {state = H; s = GS[node][i1-1][i2][H];}
      i1--; // decrement subject index only

      // manage cigars
      if (mt != "D") {
	if (ml > 0) {
	  globalCigar = mt + to_string(ml) + globalCigar;
	}
	if (nml > 0) {
	  nodeCigar = mt + to_string(nml) + nodeCigar;
	}
	mt = "D";
	ml = 1;
	nml = 1;
      }
      else {
	ml++;
	nml++;
      }
    }
    // vertical trace-back
    else if (state == V) {
      string b1 = "-"; // subject base is a gap
      string b2 = s2.substr(i2-1, 1); // query base
      a1 = b1 + a1; // add subject base to alignment
      a2 = b2 + a2; // add quary base to alignment
      if (GT[node][i1][i2][V][D]) {state = D; s = GS[node][i1][i2-1][D];}
      else if (GT[node][i1][i2][V][V]) {state = V; s = GS[node][i1][i2-1][V];}
      i2--; // decrement query index only

      // manage cigars
      if (mt != "I") {
	if (ml > 0) {
	  globalCigar = mt + to_string(ml) + globalCigar;
	}
	if (nml > 0) {
	  nodeCigar = mt + to_string(nml) + nodeCigar;
	}
	mt = "I";
	ml = 1;
	nml = 1;
      }
      else {
	ml++;
	nml++;
      }
    }
  }
 
  //----------------------------------------------------------------------------
  // register last match element
  //----------------------------------------------------------------------------
  if (ml > 0) {
    globalCigar = mt + to_string(ml) + globalCigar;
  }
  if (nml > 0) {
    nodeCigar = mt + to_string(nml) + nodeCigar;
  }

  // register starting soft clip if necessary
  if (i2 > 0) {
    globalCigar = "S" + to_string(i2) + globalCigar;
    nodeCigar = "S" + to_string(i2) + nodeCigar;
  }

  //----------------------------------------------------------------------------
  // register trace-back for last node
  //----------------------------------------------------------------------------
  // register last matched node
  matchedNodes.push_back(node);

  // reverse matchedNodes to get correct order
  reverse(matchedNodes.begin(), matchedNodes.end());
  
  // register corresponding CIGAR
  nodeCigarMap[node] = nodeCigar;

  // register corresponding offset
  nodeOffsetMap[node] = i1;

  //----------------------------------------------------------------------------
  // register global trace-back quantities
  //----------------------------------------------------------------------------

  // compose globalAlignment
  globalAlignment = a1 + "\n" + a2;
}

//------------------------------------------------------------------------------
// getScore()
//------------------------------------------------------------------------------
int GraphAlignment::getScore() {
  return SM;
}

//------------------------------------------------------------------------------
// getQueryLength()
//------------------------------------------------------------------------------
int GraphAlignment::getQueryLength() {
  return l2;
}

//------------------------------------------------------------------------------
// getGlobalCigar()
//------------------------------------------------------------------------------
string GraphAlignment::getGlobalCigar() {
  return globalCigar;
}

//------------------------------------------------------------------------------
// getMatchedNodes()
//------------------------------------------------------------------------------
vector<Node *> GraphAlignment::getMatchedNodes() {
  return matchedNodes;
}
 

//------------------------------------------------------------------------------
// getNodeCigar()
//------------------------------------------------------------------------------
string GraphAlignment::getNodeCigar(Node * node) {
  return nodeCigarMap.at(node);
}

//------------------------------------------------------------------------------
// getNodeOffset()
//------------------------------------------------------------------------------
int GraphAlignment::getNodeOffset(Node * node) {
  return nodeOffsetMap.at(node);
}

//------------------------------------------------------------------------------
// getGlobalAlignment()
//------------------------------------------------------------------------------
string GraphAlignment::getGlobalAlignment() {
  
  return globalAlignment;
}

//------------------------------------------------------------------------------
// printMatrix()
//------------------------------------------------------------------------------
void GraphAlignment::printMatrix(Node * node, ostream &out) {

  out << "Score matrix for subject node:" << node->getId() << endl;

  // assign score and trace=back matrices, subject sequence and length
  vector< vector< vector<int> > > S = GS[node];
  vector< vector< vector< vector<bool> > > > T = GT[node];
  string s1 = node->getSequence();
  int l1 = s1.length();

  // first, print the table header
  out << boost::format("%1$2s | %2$27s | ") % "" % "-"; // print blank entry
  for (int i1=1; i1 <= l1; i1++) {
    out << boost::format("%1$27s | ") % s1.substr(i1-1, 1); // print subject base
  }
  out << endl; 
  out << boost::format("%1$s") % "----";
  for (int i1=0; i1 <= l1; i1++) {
    out << boost::format("%1$30s") % "------------------------------"; // print 
  }
  out << endl;
    
  // then print the table contents
  for (int i2=0; i2<=l2; i2++) {
    if (i2 == 0) {
      out << boost::format("%1$2s | ") % "-"; // print blank entry
    }
    else {
      out << boost::format("%1$2s | ") % s2.substr(i2-1, 1); // print query base
    }
    for (int i1=0; i1<=l1; i1++) {
      int sd = S.at(i1).at(i2).at(D); int tdd = T.at(i1).at(i2).at(D).at(D); int tdh = T.at(i1).at(i2).at(D).at(H); int tdv = T.at(i1).at(i2).at(D).at(V);
      int sh = S.at(i1).at(i2).at(H); int thd = T.at(i1).at(i2).at(H).at(D); int thh = T.at(i1).at(i2).at(H).at(H);
      int sv =  S.at(i1).at(i2).at(V); int tvd = T.at(i1).at(i2).at(V).at(D); int tvv = T.at(i1).at(i2).at(V).at(V);
      out << boost::format("%1$4d(%2$1d%3$1d%4$1d) %5$4d(%6$1d%7$1d) %8$4d(%9$1d%10$1d) | ") % sd % tdd % tdh % tdv % sh % thd % thh % sv % tvd % tvv;
    }
    out << endl;
    out << boost::format("%1$4s") % "----";
    for (int i1=0; i1 <= l1; i1++) {
      out << boost::format("%1$-30s") % "------------------------------"; // print 
    }
    out << endl;
  }
}


