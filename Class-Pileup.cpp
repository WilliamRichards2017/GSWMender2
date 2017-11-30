#include "Class-Pileup.h"
#include "Class-Traceback.h"
#include "ArrayUtil.h"

Pileup::Pileup(vector<Traceback> tbv) : tbv_(tbv){
  cout << "Inside pileup constuctor\n";
  sumTracebacks();
}

void Pileup::printPileup(){
  int c = 1;
  cout << "inside print pileup\n";
  for(auto it = std::begin(pileup_); it != std::end(pileup_); ++ it){
    cout << "Printing out Node: " << c << std::endl;
    ArrayUtil::printArray2D(it->second);
    c++;
  }
}


void Pileup::sumTracebacks(){
  cout << "inside sum tracebacks\n";
  pileup_ = tbv_.back().getTracebackMap();
  tbv_.pop_back();
  while(tbv_.size() > 0){
    map<string, vector<vector<int> > > tb = tbv_.back().getTracebackMap();
    tbv_.pop_back();
    unsigned c = 0;
    for(auto it = std::begin(tb); it != std::end(tb); ++it){
      string id = it->first;
      vector<vector<int> > matrix = it->second;
      
      if(pileup_.find(id) == pileup_.end()){
	pileup_[id] = matrix;
      }
      else{
	for(unsigned i = 0; i < pileup_.at(id).size(); ++i){
	  for(unsigned j = 0; j < pileup_.at(id)[0].size(); ++j){
	    pileup_.at(id)[i][j] += matrix[i][j];
	  }
	}
      }
    }
  }
}


map<string, vector<vector<int> > > Pileup::getPileup(){
  return pileup_;
}

