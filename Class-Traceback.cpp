#include "Class-Traceback.h"
#include "termcolor.hpp"
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

Traceback::Traceback(GraphAlignment* ga){
  ga_ = ga;
  query_ = ga->getQuerySequence();
  queryPos_ = 0;
  vector<Node *> matchedNodes = ga->getMatchedNodes();
  for (vector<Node *>::const_iterator iter = matchedNodes.begin(); iter != matchedNodes.end(); iter++) {
    tracebacks_.push_back(buildTB(* iter));
  }
}

vector<pair<char, int> > Traceback::parseCigar(string cigar){
  cout << "cigar is: " << cigar << endl;
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

vector<vector<vector<int> > > Traceback::getTracebackVector(){
  return tracebacks_;
}

vector<vector<int> > Traceback::buildTB(Node * node){
  vector<vector<int > > tb;
  //mstring cigar = ga_->getNodeCigar(node);
  vector<pair<char, int> > parsedCigar = parseCigar(ga_->getNodeCigar(node));
  int subjectPos = ga_->getNodeOffset(node);
  tb = buildArray2D(ga_->getQuerySequence().length(), node->getSequence().length());
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
  
  cout << "printing out node " << node->getId() << "  offset is " << ga_->getNodeOffset(node) << endl;
  printArray2D(tb); 
  return tb;
}
