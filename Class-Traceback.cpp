#include "Class-Traceback.h"
#include "termcolor.hpp"
#include "Coords.h"

#include <string.h>
#include <assert.h>

#define toDigit(c) (c-'0')

bool isDigit( char c ) {
  return ( '0' <= c && c <= '9' );
}

bool isCigarChar( char c ){
  static const char alpha[] = "SMID";
  return ( std::strchr( alpha, c ) != NULL );
}


vector<vector<int> > buildArray2D(unsigned height, unsigned width){
  vector<vector<int> > array(height, vector<int>(width, 0));
  return array;
}

void printArray2D(vector<vector<int> > vec){
  for (int i = 0; i < vec.size(); i++){
    for (int j = 0; j < vec[i].size(); j++){
      if(vec[i][j] >=10 ){

	cout << termcolor::red << termcolor::bold << vec[i][j] << termcolor::reset << " ";
      }
      else if(vec[i][j] != 0){
	cout << termcolor::red << termcolor::bold << vec[i][j] << termcolor::reset << "  ";
      }
      else{
	cout << vec[i][j] << "  ";
      }
    }
    cout << endl;
  }
  cout << endl;
}

Traceback::Traceback(GraphAlignment* ga, coords* c): ga_(ga), c_(c), queryPos_(0){
  query_ = ga_->getQuerySequence();
  trimQuery();
  vector<Node*> matchedNodes = ga->getMatchedNodes();
  cout << "Got matched nodes before seg fault";
  for(auto it = std::begin(matchedNodes); it != std::end(matchedNodes); ++it) {
    //tracebacks_.push_back(buildTB(* iter));
    tracebacks_[(*it)->getId()] = buildTB(* it);
  }
}

string Traceback::getQuery(){
  return query_;
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

map<string, vector<vector<int> > > Traceback::getTracebackMap(){
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
  vector<vector<int > > tb;
  vector<pair<char, int> > parsedCigar = parseCigar(ga_->getNodeCigar(node));
  int subjectPos = ga_->getNodeOffset(node);
  tb = buildArray2D(query_.length(), node->getSequence().length());

  cout << "inside built TB\n";

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
    queryPos_ = ga_->getMatchedNodes()[1]->getQueryEnd();
  }

  cout << "reset query pos\n";
  node->setQueryStart(queryPos_);
  cout << "set Query start\n";


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
  cout << "made it past switch statements\n";
  node->setQueryEnd(queryPos_);
  
  //cout << "printing out node " << node->getId() << "  offset is " << ga_->getNodeOffset(node) << endl;
  //printArray2D(tb); 
  return tb;
}
