#include "Class-Pileup.h"
#include "Class-Traceback.h"
#include "ArrayUtil.h"

Pileup::Pileup(vector<Traceback> tbv) : tbv_(tbv){
  cout << "inside pileup constructor\n";
  sumTracebacks();
}

void Pileup::printPileup(){
  cout << "query is " << query_ << std::endl;
  for(auto it = std::begin(pileup_); it != std::end(pileup_); ++ it){
    cout << "Printing out " << it->first->getId() << "  traceback dims: " << it->second.size() << ", " << it->second[0].size() << std::endl;
    //ArrayUtil::printArray2D(it->second);
    ArrayUtil::printNode(it->first, it->second, queries_);
  }
}


void Pileup::sumTracebacks(){
  queries_.push_back(tbv_.front().getQuery());
  queries_.push_back(tbv_.back().getQuery());


  
  cout << "inside sum tracebacks\n";
  pileup_ = tbv_.back().getTracebackMap();
  tbv_.pop_back();
  while(tbv_.size() > 0){
    map<Node *, vector<vector<int> > > tb = tbv_.back().getTracebackMap();
    tbv_.pop_back();
    unsigned c = 0;
    for(auto it = std::begin(tb); it != std::end(tb); ++it){
      Node * id = it->first;
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

map<Node * , vector<vector<int> > > Pileup::getPileup(){
  return pileup_;
}

