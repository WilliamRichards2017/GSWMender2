#include "Class-Traceback.h"
#include "termcolor.hpp"
#include "Coords.h"

#include <string.h>
#include <assert.h>
#include "ArrayUtil.h"

#define toDigit(c) (c-'0')

bool isDigit( char c ) {
  return ( '0' <= c && c <= '9' );
}

bool isCigarChar( char c ){
  static const char alpha[] = "SMID";
  return ( std::strchr( alpha, c ) != NULL );
}


Traceback::Traceback(GraphAlignment* ga, coords* c): ga_(ga), c_(c), queryPos_(0), query_(ga->getQuerySequence()){
  formatedQuery_ = ga_->getQuerySequence();
  trimQuery();
  vector<Node*> matchedNodes = ga->getMatchedNodes();
  cout << "got " << matchedNodes.size() << " matched nodes\n";
  for(auto it = std::begin(matchedNodes); it != std::end(matchedNodes); ++it) {
    //tracebacks_.push_back(buildTB(* iter));
     Node * node = * it;
     //tracebacks_[(*it)->getId()] = buildTB(* it);
    tracebacks_[node] = buildTB(node);
  }
  cout << "leaving Traceback constructor\n";
}

string Traceback::getQuery(){
  cout << "Inside getQuery, query is  " << query_ << std::endl;
  return query_;
}

void Traceback::formatQuery(){
  return;
}

vector<pair<char, int> > Traceback::parseCigar(string cigar){
  //cout << "cigar is: " << cigar << endl;
  vector<pair<char, int> > parsedCigs;
  int pos = 0;
  while(pos < cigar.length()){
    pair<char, int> p;
    assert(isCigarChar(cigar[pos]));
    p.first = char(cigar[pos]);
    pos++;
    string number = "";
    while(isDigit(char(cigar[pos]))){
      number += char(cigar[pos]); 
      pos++;
    }


    char *c = &number[0u];

    p.second = atoi(c);
    parsedCigs.push_back(p); 
  }
  return parsedCigs;
}

map<Node *, vector<vector<int> > > Traceback::getTracebackMap(){
  cout << "inside Traceback::getTracebackMap()\n";
  return tracebacks_;
}

void Traceback::trimQuery(){
  assert(c_->SS >= c_->QS);
  assert(c_->QE >= c_->SE);
  cout << "Query is " << query_ << std::endl;
  query_ = query_.substr(c_->SS-c_->QS, c_->SE-c_->SS+1);
  cout << "Trimmed query is " << query_<< std::endl;
}



vector<vector<int> > Traceback::buildTB(Node * node){
  vector<vector<int > > tb  = ArrayUtil::buildArray2D(query_.length(), node->getSequence().length());
  vector<pair<char, int> > parsedCigar = parseCigar(ga_->getNodeCigar(node));
  int subjectPos = ga_->getNodeOffset(node);
  cout << "parsed cigar, got node offset, built empty array\n";
  cout << "node cigar is: " << ga_->getNodeCigar(node) << std::endl;

  /*if(node->getId()=="node3"){
    queryPos_ = queryPos_+30;
    }*/
  
  if( strcmp(node->getId().c_str(), "node1")==0){
    queryPos_=0;
  }
  else if( strcmp(node->getId().c_str(), "node2")==0){
    queryPos_ = node->getContributorNodes()[0]->getQueryEnd();
  }
  else if( strcmp(node->getId().c_str(), "node3")==0){
    queryPos_ = node->getContributorNodes()[0]->getQueryEnd();
  }
  else {
    assert(node->getContributorNodes().size() == 2);
    //queryPos_ = max(node->getContributorNodes()[0]->getQueryEnd(), node->getContributorNodes()[1]->getQueryEnd());
    queryPos_ = node->getContributorNodes()[0]->getQueryEnd();
  }

  cout << "setting query pos to be " << queryPos_ << std::endl;
  node->setQueryStart(queryPos_);


  for(auto it = std::begin(parsedCigar); it != std::end(parsedCigar); ++it){
    switch(it->first) 
      {
      case 'M':
	while(it->second > 0){
	  tb[queryPos_][subjectPos] = 1;
	  queryPos_++;
	  subjectPos++;
	  it->second--;
	}
      case 'I':
	while(it->second > 0){
	  tb[queryPos_][subjectPos] = 1;
	  queryPos_++;
	  it->second--;
	}
      case 'D':
	while(it->second > 0){
	  tb[queryPos_][subjectPos] = 1;
	  subjectPos++;
	  it->second--;
	}
      }
  }
  node->setQueryEnd(queryPos_);
  
  //cout << "printing out node " << node->getId() << "  offset is " << ga_->getNodeOffset(node) << endl;
  //ArrayUtil::printArray2D(tb); 
  return tb;
}
