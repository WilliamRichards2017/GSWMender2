#include "Class-Pileup.h"
#include "Class-Traceback.h"
#include "ArrayUtil.h"

Pileup::Pileup(vector<Traceback> tbv) : tbv_(tbv){
  cout << "Inside pileup constuctor\n";
  sumTracebacks();
}

void Pileup::printPileup(){
  int c = 1;
  for(auto it = std::begin(pileup_); it != std::end(pileup_); ++ it){
    cout << "Printing out Node: " << c << std::endl;
    ArrayUtil::printArray2D(*it);
    c++;
  }
}


void Pileup::sumTracebacks(){
  pileup_ = tbv_.back().getTracebackVector();
  tbv_.pop_back();
  while(tbv_.size() > 0){
    vector<vector<vector<int> > > tb = tbv_.back().getTracebackVector();
    tbv_.pop_back();
    unsigned c = 0;
    for(auto it = std::begin(tb); it != std::end(tb); ++it){
      vector<vector<int> > m = *it;
      for (unsigned i = 0; i < m.size(); i++){
	for(unsigned j = 0; j < m[0].size(); j++){
	  pileup_[c][i][j] += m[i][j];
	}
      }
      c++;
    }
  }
}

vector<vector<vector<int> > > Pileup::getPileup(){
  return pileup_;
}

