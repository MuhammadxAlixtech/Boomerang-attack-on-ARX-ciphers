#include "salsa.hpp"
#define loop 1<<20
#define roundCnt 4
enum differnetialAnalysis{
  idWordIndex=7,
  idBitIndex=31,
  odWordIndex=1,
  odBitIndex=14
};
bool differnetialAnalysis(uint32_t differedWordIndex,uint32_t differedBitIndex,uint32_t compareWordIndex,uint32_t compareBitIndex){
  block x0,x1;
  initialize(&x0);
  x1=x0;
  x1.w[differedWordIndex].bits^=(1<<differedBitIndex);
  // x1.w[differedWordIndex+4].bits^=(1<<differedBitIndex);
  for(int iter=1;iter<=roundCnt;iter++){  
    if(iter%2==0){
      evenRounds(&x0);
      evenRounds(&x1);
    }
    else{
      oddRounds(&x0);
      oddRounds(&x1);
    }
  }
  block x1Xorx0;
  x1Xorx0=x1;
  xorBlock(&x1Xorx0,&x0);
  if(x1Xorx0.w[compareWordIndex].bits&(1<<compareBitIndex)){
    return true;
  }
  return false;
}
int main(){
  clock_t startTime=clock();
  int count=0;
  for(int iter=0;iter<loop;iter++){
    if(differnetialAnalysis(idWordIndex,idBitIndex,odWordIndex,odBitIndex))count++;    
  }
  double probab=count;
  probab/=loop;
  double bias=(2*probab)-1;
  std::cout<<idWordIndex<<" "<<idBitIndex<<" "<<odWordIndex<<" "<<odBitIndex<<std::endl;
  std::cout<<"Probability:"<<probab<<" Bias:"<<bias<<std::endl;
  clock_t endTime=clock();
  double timeTaken=double(endTime-startTime)/CLOCKS_PER_SEC;
  std::cout<<"Time Taken "<<timeTaken<<" sec\n";
  return 0;
}