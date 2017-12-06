#include "ArrayUtil.h"
#include <iostream>
#include <vector>
#include "termcolor.hpp"

using namespace std;

vector<vector<int> > ArrayUtil::buildArray2D(unsigned height, unsigned width){
  vector<vector<int> > array(height, vector<int>(width, 0));
  return array;
}

void ArrayUtil::printArray2D(vector<vector<int> > vec){
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

void ArrayUtil::printNode(Node * node, vector<vector<int> > vec, vector<string> queries){

  std::string subect = node->getSequence();
  cout << termcolor::blue << termcolor::bold << " - " << termcolor::reset << "  ";
  for(char& c : subect) {
    cout << termcolor::blue << termcolor::bold << c << termcolor::reset << "  ";
  }
  cout << std::endl;

  for (int i = 0; i < vec.size(); i++){
    cout  << termcolor::blue << termcolor::bold << queries[0][i]  << " " << queries[1][i] << " |"  << termcolor::reset;
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



