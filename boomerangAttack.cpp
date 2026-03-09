#include "forro.hpp" /*Implementation of the Cipher*/
#define ptSize 1<<20 /* Size of the plaintext array */
/* Decomposition specifications for Boomerang Attack */
enum boomerangParameters{
    e0Rounds=2,
    e0HalfRound=false,
    e0QuarterRound=false,
    e1Rounds=3,
    e1HalfRound=true,
    e1QuarterRound=false
};
/* Differential Indices for the Attack */
enum IDOD{
    idWordIndex=12,
    idBitIndex=0,
    odWordIndex=15,
    odBitIndex=16

};
block plainText[ptSize];
std::vector<double> boomerangAttack(uint32_t idWIndex,uint32_t idBIndex,uint32_t odWIndex,uint32_t odBIndex){
    /* Initialize the plaintext array */
    for(int i=0;i<ptSize;i++){
        initialize(&plainText[i]);
    }
    /* Counters for Probabilties */
    int cntrForp1=0,cntrForp2=0,cntrForq1=0,cntrForq2=0,cntrForp2q2=0;
    for(int i=0;i<ptSize;i++){
        block p1=plainText[i],p2=plainText[i];
        /* Introducing Initial Differential */
        p2.w[idWIndex].bits^=(1<<(idBIndex));
        /* Performing E0 Encryption */
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
        if(e0HalfRound){
            if((e0Rounds)%2){
                halfEvenRounds(&x1);
                halfEvenRounds(&x2);
            }
            else{
                halfOddRounds(&x1);
                halfOddRounds(&x2);
            }
        }
        if(e0HalfRound){
            if((e0Rounds)%2){
                halfEvenRounds(&x1);
                halfEvenRounds(&x2);
            }
            else{
                halfOddRounds(&x1);
                halfOddRounds(&x2);
            }
        }
        /* Storing the Intermediate CipherText */
        block x1Xorx2=x1;
        xorBlock(&x1Xorx2,&x2);
        if((x1Xorx2.w[odWIndex].bits&(1<<odBIndex)))cntrForp1++;
        block c1=x1,c2=x2;
        for(int i=e0Rounds+1;i<=e1Rounds+e0Rounds;i++){
            if(i%2){
                oddRounds(&c1);
                oddRounds(&c2);
            }
            else{
                evenRounds(&c1);
                evenRounds(&c2);
            }
        }
        if(e1HalfRound){
            if((e1Rounds+e0Rounds)%2){
                halfEvenRounds(&c1);
                halfEvenRounds(&c2);
            }
            else{
                halfOddRounds(&c1);
                halfOddRounds(&c2);
            }
        }
        if(e1QuarterRound){
            if((e1Rounds+e0Rounds)%2){
                quarterEvenRounds(&c1);
                quarterEvenRounds(&c2);
            }
            else{
                quarterOddRounds(&c1);
                quarterOddRounds(&c2);
            }
        }
        block c3=c1,c4=c2;
        c3.w[idWIndex].bits^=1<<idBIndex;
        c4.w[idWIndex].bits^=1<<idBIndex;
        block x3=c3,x4=c4;
        if(e1HalfRound){
            if((e1Rounds+e0Rounds)%2){  
                inverseHalfEvenRounds(&x1);
                inverseHalfEvenRounds(&x2);
            }
            else{
                inverseHalfOddRounds(&x1);
                inverseHalfOddRounds(&x2);
            }
        }
        if(e1QuarterRound){
            if((e1Rounds+e0Rounds)%2){
                inverseQuarterEvenRounds(&x1);
                inverseQuarterEvenRounds(&x2);
            }
            else{
                inverseQuarterOddRounds(&x1);
                inverseQuarterOddRounds(&x2);
            }
        }
        for(int i=e1Rounds+e0Rounds;i>=e0Rounds+1;i--){
            if(i%2){
                inverseOddRounds(&x3);
                inverseOddRounds(&x4);
            }
            else{
                inverseEvenRounds(&x3);
                inverseEvenRounds(&x4);
            }
        }
        block x2Xorx4=x2,x1Xorx3=x1,x3Xorx4=x3;
        xorBlock(&x2Xorx4,&x4);
        xorBlock(&x1Xorx3,&x3);
        xorBlock(&x3Xorx4,&x4);
        if(x3Xorx4.w[odWIndex].bits&(1<<odBIndex))cntrForp2++;
        if(x2Xorx4.w[odWIndex].bits&(1<<odBIndex))cntrForq1++;
        if(x1Xorx3.w[odWIndex].bits&(1<<odBIndex))cntrForq2++;
        block p3=x3,p4=x4;
        for(int i=e0Rounds;i>=1;i--){
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
        if(p3Xorp4.w[idWIndex].bits&(1<<idBIndex))cntrForp2q2++;
    }
    double p1=cntrForp1,p2=cntrForp2,q1=cntrForq1,q2=cntrForq2,p2q2=cntrForp2q2;
    p1/=ptSize,p2/=ptSize,q1/=ptSize,q2/=ptSize;
    return {p1,p2,q1,q2,p2q2};
    
}
void attackOnMultiID(std::vector<pair<uint32_t,uint32_t>> ids,uint32_t odWordIndex,uint32_t odBitIndex){
    for(int i=0;i<ids.size();i++){
        std::vector<double> probs=boomerangAttack(ids[i].first,ids[i].second,odWordIndex,odBitIndex);
        std::cout<<ids[i].first<<","<<ids[i].second<<" "<<odWordIndex<<","<<odBitIndex<<std::endl;
        std::cout<<"p1:"<<probs[0]<<" p2:"<<probs[1]<<" q1:"<<probs[2]<<" q2:"<<probs[3]<<" p^2 * q^2:"<<(probs[0]*probs[1]*probs[2]*probs[3])<<" Bias:"<<std::abs((2*(probs[0]*probs[1]*probs[2]*probs[3]))-1)<<" 2^:"<<log2(4/(probs[0]*probs[1]*probs[2]*probs[3]))<<std::endl;
    }
}
void findAttackOnGivenID(int idWIndex,int idBIndex){
    for(int wInd=0;wInd<16;wInd++){
        for(int bInd=0;bInd<32;bInd++){
            std::vector<double> probs=boomerangAttack(idWIndex,idBIndex,wInd,bInd);
            std::cout<<"E0:"<<e0Rounds<<(e0HalfRound?".5":".0")<<" E1:"<<e1Rounds<<(e1HalfRound?".5":".0")<<(e1QuarterRound?".25":".0")<<std::endl;
            std::cout<<idWIndex<<","<<idBIndex<<" "<<wInd<<","<<bInd<<std::endl;
            std::cout<<"p1:"<<probs[0]<<" p2:"<<probs[1]<<" q1:"<<probs[2]<<" q2:"<<probs[3]<<" p^2 * q^2:"<<(probs[0]*probs[1]*probs[2]*probs[3])<<" Bias:"<<std::abs((2*(probs[0]*probs[1]*probs[2]*probs[3]))-1)<<" 2^:"<<log2(4/(probs[0]*probs[1]*probs[2]*probs[3]))<<std::endl;
            
        }
    }
}
void attack(){
    std::vector<double> probs=boomerangAttack(idWordIndex,idBitIndex,odWordIndex,odBitIndex);
    std::cout<<"E0:"<<e0Rounds<<(e0HalfRound?".5":".0")<<" E1:"<<e1Rounds<<(e1HalfRound?".5":".0")<<(e1QuarterRound?".25":".0")<<std::endl;
    std::cout<<idWordIndex<<","<<idBitIndex<<" "<<odWordIndex<<","<<odBitIndex<<std::endl;
    std::cout<<"p1:"<<probs[0]<<" p2:"<<probs[1]<<" q1:"<<probs[2]<<" q2:"<<probs[3]<<" p^2 * q^2:"<<(probs[0]*probs[1]*probs[2]*probs[3])<<" Bias:"<<std::abs((2*(probs[0]*probs[1]*probs[2]*probs[3]))-1)<<" 2^:"<<log2(4/(probs[0]*probs[1]*probs[2]*probs[3]))<<std::endl;
}
int main(){
    clock_t startTime=clock();
    findAttack();
    int timeTaken=(clock() - startTime)/CLOCKS_PER_SEC;
    std::cout<<"Time Taken:"<<timeTaken<<" Secs \n";
    return 0;
}