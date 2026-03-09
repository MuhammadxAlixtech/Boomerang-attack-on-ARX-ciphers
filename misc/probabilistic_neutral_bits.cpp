#include "chacha.hpp"
#define loop (1<<16)
#define forwardRounds 3
#define backwardRounds 4
#define halfForwardRound 1
#define halfBackwardRound 1
enum differnetialAnalysis{
  idWordIndex=13,
  idBitIndex=6,
  odWordIndex=2,
  odBitIndex=0
};
block plainTexts[loop];
int main(){
	clock_t startTime=clock();
	for(int loopCnt=0;loopCnt<loop;loopCnt++){
		initialize(&plainTexts[loopCnt]);
	}
	uint32_t totalPNBCount=0;
	for(int wordIndex=4;wordIndex<12;wordIndex++){
		for(int bitIndex=0;bitIndex<wordSize;bitIndex++){
			uint32_t counter=0;
			for(int loopCnt=0;loopCnt<loop;loopCnt++){
				block x0,x1;
				x0=plainTexts[loopCnt];
				x1=plainTexts[loopCnt];
				x1.w[idWordIndex].bits^=1<<idBitIndex;
				block x0Init=x0,x1Init=x1;
				for(int rCnt=1;rCnt<=forwardRounds;rCnt++){
					if(rCnt%2==0){
						evenRounds(&x0);
						evenRounds(&x1);
					}
					else{
						oddRounds(&x0);
						oddRounds(&x1);
					}
				}
				if(halfForwardRound){
					if(forwardRounds%2){
						halfEvenRounds(&x0);
						halfEvenRounds(&x1);
					}
					else{
						halfOddRounds(&x0);
						halfOddRounds(&x1);
					}
				}
				block x0After3R=x0,x1After3R=x1;
				if(halfForwardRound){
					if(forwardRounds%2){
						inverseHalfEvenRounds(&x0);
						inverseHalfEvenRounds(&x1);
					}
					else{
						inverseHalfOddRounds(&x0);
						inverseHalfOddRounds(&x1);
					}
				}
				for(int rCnt=forwardRounds+1;rCnt<=forwardRounds+backwardRounds;rCnt++){
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
				x0Init.w[wordIndex].bits^=1<<bitIndex;
				x1Init.w[wordIndex].bits^=1<<bitIndex;
				block x0Telda=z0,x1Telda=z1;
				subtractBlock(&x0Telda,&x0Init);
				subtractBlock(&x1Telda,&x1Init);
				for(int rCnt=forwardRounds+backwardRounds;rCnt>=forwardRounds+1;rCnt--){
					if(rCnt%2==0){
						inverseEvenRounds(&x0Telda);
						inverseEvenRounds(&x1Telda);
					}
					else{
						inverseOddRounds(&x0Telda);
						inverseOddRounds(&x1Telda);
					}
				}
				if(halfForwardRound){
					if(forwardRounds%2){
						halfEvenRounds(&x0Telda);
						halfEvenRounds(&x1Telda);
					}
					else{
						halfOddRounds(&x0Telda);
						halfOddRounds(&x1Telda);
					}
				}
				block x0Teldaxorx1Telda=x0Telda;
				xorBlock(&x0Teldaxorx1Telda,&x1Telda);
				if((x0After3Rxorx1After3R.w[odWordIndex].bits&(1<<odBitIndex))==(x0Teldaxorx1Telda.w[odWordIndex].bits&(1<<odBitIndex))){
					counter++;
				}
			}
			double probability=counter;
			probability/=loop;
			double bias=(2*probability) - 1.0000000;
			if(bias>= 0.2000001){
				std::cout<<32*(wordIndex-4) + bitIndex<<":"<<bias<<" ";
				totalPNBCount++;				
			}
		}
	}
	std::cout<<"PNB Count "<<totalPNBCount<<" \n";
	clock_t endTime=clock();
	double timeTaken=double(endTime-startTime)/CLOCKS_PER_SEC;
	std::cout<<"Time Taken "<<timeTaken<<" Seconds\n";
}