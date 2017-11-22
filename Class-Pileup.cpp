#include "Class-Pileup.h"
#include "ArrayUtil.h"

Pileup::Pileup(vector<Traceback> tbv) : tbv_(tbv){
  sumTracebacks();
}

void Pileup::printPileup(){
  for(auto it = std::begin(pileup_); it != std::end(pileup_); ++ it){
    ArrayUtil::printArray2D(*it);
  }
}


void Pileup::sumTracebacks(){
  pileup_ = tbv_.back().getTracebackVector();
  tbv_.pop_back();
  cout << "iterating though " << tbv_.size() << " tracebacks" << endl;
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

