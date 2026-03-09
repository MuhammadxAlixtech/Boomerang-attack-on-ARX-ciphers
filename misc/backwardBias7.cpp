#include "chacha.hpp"
#define loop (1<<25)
enum differnetialAnalysis{
  idWordIndex=13,
  idBitIndex=13,
  odWordIndex=11,
  odBitIndex=0
};
block plainTexts[loop];
uint32_t pnbs[]={3,6,15,16,31,35,67,68,71,91,92,93,94,95,96,97,98,99,100,103,104,127,136,191,223,224,225,248,249,250,251,252,253,254,255};
double calculateEplisonA(){
    uint32_t counter=0;
    for(int loopCnt=0;loopCnt<loop;loopCnt++){
        block x0,x1;
        x0=plainTexts[loopCnt];
        x1=plainTexts[loopCnt];
        x1.w[idWordIndex].bits^=1<<idBitIndex;
        block x0Init=x0,x1Init=x1;
        for(int rCnt=1;rCnt<=3;rCnt++){
            if(rCnt%2==0){
                evenRounds(&x0);
                evenRounds(&x1);
            }
            else{
                oddRounds(&x0);
                oddRounds(&x1);
            }
        }
        block x0After3R=x0,x1After3R=x1;
        for(int rCnt=4;rCnt<=7;rCnt++){
            if(rCnt%2==0){
                evenRounds(&x0);
                evenRounds(&x1);
            }
            else{
                oddRounds(&x0);
                oddRounds(&x1);
            }
        }
        block z0=x0,z1=x1;
        addBlock(&z0,&x0Init);
        addBlock(&z1,&x1Init);
        block x0After3Rxorx1After3R=x0After3R;
        xorBlock(&x0After3Rxorx1After3R,&x1After3R);
        block x0Telda=x0Init,x1Telda=x1Init;
        for(int curw=4;curw<12;curw++){
            for(int curb=0;curb<32;curb++){
                int curbitnum=((curw-4)*32)+curb;
                bool ispnb=false;
                for(auto &pnb:pnbs){
                    if(pnb==curbitnum)ispnb=true;
                }
                if(ispnb){
                    if(x0Telda.w[curw].bits&(1<<curb)){
                        x0Telda.w[curw].bits^=1<<curb;                        
                    }
                    if(x1Telda.w[curw].bits&(1<<curb)){
                        x1Telda.w[curw].bits^=1<<curb;
                    }
                }
            }
        }
        block m0=z0,m1=z1;
        subtractBlock(&m0,&x0Telda);
        subtractBlock(&m1,&x1Telda);
        for(int rCnt=7;rCnt>=4;rCnt--){
            if(rCnt%2==0){
                inverseEvenRounds(&m0);
                inverseEvenRounds(&m1);
            }
            else{
                inverseOddRounds(&m0);
                inverseOddRounds(&m1);
            }
        }
        block m0Xorm1=m0;
        xorBlock(&m0Xorm1,&m1);
        if((x0After3Rxorx1After3R.w[odWordIndex].bits&(1<<odBitIndex))==(m0Xorm1.w[odWordIndex].bits&(1<<odBitIndex))){
            counter++;
        }
        if(loopCnt>>10 && ((loopCnt-1)&(loopCnt))==0){
            double probability=counter;
            probability/=loop;
            double epsilonA=(2*probability) - 1.0000000;
            std::cout<<loopCnt<<":"<<epsilonA<<" ";
        }

    }
    double probability=counter;
    probability/=loop;
    double epsilonA=(2*probability) - 1.0000000;
    return epsilonA;
}
int main(){
	clock_t startTime=clock();
	for(int loopCnt=0;loopCnt<loop;loopCnt++){
		initialize(&plainTexts[loopCnt]);
	}
    double epsilonA=calculateEplisonA();
    float epsilonD=0.026;
    std::cout<<"Epsilon A: "<<epsilonA<<std::endl;
    std::cout<<"N:"<<calculateN(123,epsilonA,epsilonD)<<std::endl;
	clock_t endTime=clock();
	double timeTaken=double(endTime-startTime)/CLOCKS_PER_SEC;
	std::cout<<"Time Taken "<<timeTaken<<" Seconds\n";
}