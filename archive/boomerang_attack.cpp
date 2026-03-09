#include "chacha.hpp"
#define ptSize 1<<20
#define countRounds 8

enum IDOD{
    idWordIndex=12,
    idBitIndex=0,
    odWordIndex=15,
    odBitIndex=16
};
block plainText[ptSize];
std::vector<double> boomerangAttack(bool isNumberOfRoundsOdd){
    for(int i=0;i<ptSize;i++){
        initialize(&plainText[i]);
    }
    int cntrForp1=0,cntrForp2=0,cntrForq1=0,cntrForq2=0,cntrForp2q2=0;
    for(int i=0;i<ptSize;i++){
        block p1=plainText[i],p2=plainText[i];
        p2.w[idWordIndex].bits^=(1<<idBitIndex);
        block x1=p1,x2=p2;
        for(int i=1;i<=countRounds/2;i++){
            if(i%2){
                oddRounds(&x1);
                oddRounds(&x2);
            }
            else{
                evenRounds(&x1);
                evenRounds(&x2);
            }
        }
        if(isNumberOfRoundsOdd){
            if((countRounds/2)%2){
                halfEvenRounds(&x1);
                halfEvenRounds(&x2);
            }
            else{
                halfOddRounds(&x1);
                halfOddRounds(&x2);
            }
        }
        block x1Xorx2=x1;
        xorBlock(&x1Xorx2,&x2);
        if(isNumberOfRoundsOdd){
            if((countRounds/2)%2){
                inverseHalfEvenRounds(&x1);
                inverseHalfEvenRounds(&x2);
            }
            else{
                inverseHalfOddRounds(&x1);a
                inverseHalfOddRounds(&x2);
            }
        }
        // printBlock(&x1Xorx2);
        if(x1Xorx2.w[odWordIndex].bits&(1<<odBitIndex))cntrForp1++;
        block c1=x1,c2=x2;
        for(int i=(countRounds/2)+1;i<=countRounds;i++){
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
        c3.w[idWordIndex].bits^=1<<idBitIndex;
        c4.w[idWordIndex].bits^=1<<idBitIndex;
        block x3=c3,x4=c4;
        for(int i=countRounds;i>=(countRounds/2)+1;i--){
            if(i%2){
                inverseOddRounds(&x3);
                inverseOddRounds(&x4);
            }
            else{
                inverseEvenRounds(&x3);
                inverseEvenRounds(&x4);
            }
        }
        if(isNumberOfRoundsOdd){
            if((countRounds/2)%2){
                inverseHalfEvenRounds(&x3);
                inverseHalfEvenRounds(&x4);
            }
            else{
                inverseHalfOddRounds(&x3);
                inverseHalfOddRounds(&x4);
            }
        }
        block x2Xorx4=x2,x1Xorx3=x1,x3Xorx4=x3;
        xorBlock(&x2Xorx4,&x4);
        xorBlock(&x1Xorx3,&x3);
        xorBlock(&x3Xorx4,&x4);
        if(isNumberOfRoundsOdd){
            if((countRounds/2)%2){
                halfEvenRounds(&x3);
                halfEvenRounds(&x4);
            }
            else{
                halfOddRounds(&x3);
                halfOddRounds(&x4);
            }
        }

        if((x3Xorx4.w[odWordIndex].bits)&(1<<odBitIndex))cntrForp2++;
        // printBlock(&x2Xorx4);
        // printBlock(&x1Xorx3);
        if((x2Xorx4.w[odWordIndex].bits&(1<<odBitIndex))){
            cntrForq1++;
        }
        if(((x1Xorx3.w[odWordIndex].bits)&(1<<odBitIndex))){
            cntrForq2++;
        }
        block p3=x3,p4=x4;
        for(int i=countRounds/2;i>=1;i--){
            if(i%2){
                inverseOddRounds(&p3);
                inverseOddRounds(&p4);
            }
            else{
                inverseEvenRounds(&p3);
                inverseEvenRounds(&p4);
            }
        }
        block p3Xorp4=p3;
        xorBlock(&p3Xorp4,&p4);
        if(p3Xorp4.w[idWordIndex].bits&(1<<idBitIndex))cntrForp2q2++;
    }
    double p1=cntrForp1,p2=cntrForp2,q1=cntrForq1,q2=cntrForq2,p2q2=cntrForp2q2;
    p1/=ptSize,p2/=ptSize,q1/=ptSize,q2/=ptSize;
    return {p1,p2,q1,q2,p2q2};
    
}
int main(){
    clock_t startTime=clock();
    std::vector<double> probs(5);
    std::vector<double> curProbs=boomerangAttack(countRounds%2);
    for(int i=0;i<5;i++){
        probs[i]+=curProbs[i];
    }
    std::cout<<"p1:"<<probs[0]<<" p2:"<<probs[1]<<" q1:"<<probs[2]<<" q2:"<<probs[3]<<" p^2 * q^2:"<<(probs[0]*probs[1]*probs[2]*probs[3])<<" Bias:"<<(2*(probs[0]*probs[1]*probs[2]*probs[3]))-1<<" 2^:"<<log2(4/(probs[0]*probs[1]*probs[2]*probs[3]))<<std::endl;
    int timeTaken=(clock() - startTime)/CLOCKS_PER_SEC;
    std::cout<<"Time Taken:"<<timeTaken<<" Secs \n";
    return 0;
}