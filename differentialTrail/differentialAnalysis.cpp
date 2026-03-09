#include "chacha.hpp"
#define loop 1<<20
#define roundCnt 4
enum differnetialAnalysis{
  idWordIndex=12,
  idBitIndex=0,
  odWordIndex=1,
  odBitIndex=6
};
bool differnetialAnalysis(uint32_t differedWordIndex,uint32_t differedBitIndex,uint32_t compareWordIndex,uint32_t compareBitIndex){
  block x0,x1;
  initialize(&x0);
  x1=x0;
  x1.w[differedWordIndex].bits^=(1<<differedBitIndex);
  // x1.w[differedWordIndex+4].bits^=(1<<differedBitIndex);
  block x1Xorx0;
  x1Xorx0=x1;
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    xorWords(&x1Xorx0.w[wordIndex],&x0.w[wordIndex]);
  }
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
  // halfEvenRounds(&x0);
  // halfEvenRounds(&x1);
  x1Xorx0=x1;
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    xorWords(&x1Xorx0.w[wordIndex],&x0.w[wordIndex]);
  }
  if(x1Xorx0.w[compareWordIndex].bits&(1<<compareBitIndex)){
    return false;
  }
  return true;
}
int main(){
  clock_t startTime=clock();
  for(int odw=0;odw<16;odw++){
    for(int odb=0;odb<32;odb++){
      int count=0;
      for(int iter=0;iter<loop;iter++){
        if(differnetialAnalysis(idWordIndex,idBitIndex,odw,odb))count++;    
      }
      float probab=count;
      probab/=loop;
      double bias=(2*probab)-1;
      std::cout<<idWordIndex<<" "<<idBitIndex<<" "<<odw<<" "<<odb<<std::endl;
      std::cout<<"Probability:"<<probab<<" Bias:"<<bias<<std::endl;
    }
  }
  // int count=0;
  // for(int iter=0;iter<loop;iter++){
  //   if(differnetialAnalysis(idWordIndex,idBitIndex,odWordIndex,odBitIndex))count++;    
  // }
  // float probab=count;
  // probab/=loop;
  // double bias=(2*probab)-1;
  // std::cout<<idWordIndex<<" "<<idBitIndex<<" "<<odWordIndex<<" "<<odBitIndex<<std::endl;
  // std::cout<<"Probability:"<<probab<<" Bias:"<<bias<<std::endl;
  clock_t endTime=clock();
  double timeTaken=double(endTime-startTime)/CLOCKS_PER_SEC;
  std::cout<<"Time Taken "<<timeTaken<<" sec\n";
  return 0;
}