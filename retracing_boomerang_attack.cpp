#include "chacha.hpp"
#define ptSize 1<<20
enum retracingBoomerang{
    e0Rounds=5,
    e1Rounds=1,
};
enum differentialIndices{
    alphaWordIndex=13,
    alphaBitIndex=0,
    betaWordIndex=0,
    betaBitIndex=0,
    gammaWordIndex=0,
    gammaBitIndex=0,
    miuLWordIndex=0,
    miuLBitIndex=0,
    miuRWordIndex=15,
    miuRBitIndex=31,
    deltaLWordIndex=0,
    deltaLBitIndex=0,
    deltaRWordIndex=15,
    deltaRBitIndex=31,

};
block plainTexts[ptSize];
void retracingBoomerangAttack(){
    uint32_t cntForp=0,cntForq=0,cntForq2L=0,cntForq2R=0,cntTotal=ptSize;
    for(int i=0;i<ptSize;i++){
        initialize(&plainTexts[i]);
    }
    for(int i=0;i<ptSize;i++){
        block p1=plainTexts[i],p2=plainTexts[i];
        p2.w[alphaWordIndex].bits^=(1<<(alphaBitIndex));
        block x1=p1,x2=p2;
        for(int i=1;i<=e0Rounds;i++){
            if(i%2){
                oddRounds(&x1);
                oddRounds(&x2);
            }
            else{
                evenRounds(&x1);
                evenRounds(&x2);
            }
        }
        block c1=x1,c2=x2;
        for(int i=e0Rounds+1;i<=e0Rounds+e1Rounds;i++){
            if(i%2){
                oddRounds(&c1);
                oddRounds(&c2);
            }
            else{
                evenRounds(&c1);
                evenRounds(&c2);
            }
        }
        block c3=c1,c4=c2;
        c3.w[deltaLWordIndex].bits^=(1<<deltaLBitIndex);
        c3.w[deltaRWordIndex].bits^=(1<<deltaRBitIndex);
        c4.w[deltaLWordIndex].bits^=(1<<deltaLBitIndex);
        c4.w[deltaRWordIndex].bits^=(1<<deltaRBitIndex);
        block x3=c3,x4=c4;
        for(int i=e0Rounds+e1Rounds;i>=e0Rounds+1;i--){
            if(i%2){
                inverseOddRounds(&x3);
                inverseOddRounds(&x4);
            }
            else{
                inverseEvenRounds(&x3);
                inverseEvenRounds(&x4);
            }
        }
        if((x1.w[miuLWordIndex].bits^(x3.w[miuLWordIndex].bits))==(x2.w[miuLWordIndex].bits^(x4.w[miuLWordIndex].bits))){
            cntForq2L++;
        }
        if((x1.w[miuRWordIndex].bits^(x3.w[miuRWordIndex].bits))==(x2.w[miuRWordIndex].bits^(x4.w[miuRWordIndex].bits))){
            cntForq2R++;
        }
        if(x1.w[betaWordIndex].bits^(x2.w[betaWordIndex].bits)==x3.w[betaWordIndex].bits^(x4.w[betaWordIndex].bits)){
            cntForp++;
        }
        if(x1.w[gammaWordIndex].bits^(x3.w[gammaWordIndex].bits)==x2.w[gammaWordIndex].bits^(x4.w[gammaWordIndex].bits)){
            cntForq++;
        }   
    }
    double p=cntForp;
    double q1=cntForq;
    double q2L=cntForq2L;
    double q2R=cntForq2R;
    p/=cntTotal;
    q1/=cntTotal;
    q2L/=cntTotal;
    q2R/=cntTotal;
    double prob=(p*q1*q2R)*(p*q1*q2R)*q2L;
    std::cout<<cntForp<<" "<<cntForq<<" "<<cntForq2L<<" "<<cntForq2R<<"\n";
    std::cout<<p<<" "<<q1<<" "<<q2L<<" "<<q2R<<" "<<prob<<" "<<std::abs(2*prob -1)<<" "<<log2(4/prob)<<" \n";
}
int main(){
    retracingBoomerangAttack();
    return 0;
}