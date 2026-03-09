#include "forro.hpp"
#define loop 1<<20
#define roundCnt 2
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
    return false;
  }
  return true;
}
int main(){
  clock_t startTime=clock();  
  std::vector<uint32_t> idWord={4,5,12,13};
  uint32_t odWordIndex=10,odBitIndex=0;
  for(uint32_t i=0;i<4;i++){
    for(uint32_t idBitIndex=0;idBitIndex<32;idBitIndex++){
        uint32_t idWordIndex=idWord[i];
        int count=0;
        for(int iter=0;iter<loop;iter++){
            if(differnetialAnalysis(idWordIndex,idBitIndex,odWordIndex,odBitIndex))count++;    
        }
        float probab=count;
        probab/=loop;
        double bias=(2*probab)-1;
        std::cout<<idWordIndex<<" "<<idBitIndex<<" "<<odWordIndex<<" "<<odBitIndex<<std::endl;
        std::cout<<"Probability:"<<probab<<" Bias:"<<bias<<std::endl;
    }

  }
  clock_t endTime=clock();
  double timeTaken=double(endTime-startTime)/CLOCKS_PER_SEC;
  std::cout<<"Time Taken "<<timeTaken<<" sec\n";
  return 0;
}