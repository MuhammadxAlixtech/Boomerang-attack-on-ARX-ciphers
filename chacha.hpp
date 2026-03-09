#include "arx_utils_efficient.hpp"
#define rounds 20
/* Constants for the shift operations */
enum ShiftConstants{
  r1=16,
  r2=12,
  r3=8,
  r4=7
};
/* 4x4 Block Initialization Constants for ChaCha Cipher */
enum InitailBlockConstants{
  c0=0x61707865,
  c1=0x3320646e,
  c2=0x79622d32,
  c3=0x6b206574
};
/* 
Quarter-Round Implementation for 8-bit Chacha Cipher
*/
void quarterRound(word* a,word* b,word* c,word* d){
  //Statement 1 of Quarter Round
  addWords(a,b);
  xorWords(d,a);
  leftShift(d,r1);
  //Statement 2 of Quarter Round  
  addWords(c,d);
  xorWords(b,c);
  leftShift(b,r2);
  //Statement 3 of Quarter Round
  addWords(a,b);
  xorWords(d,a);
  leftShift(d,r3);
  //Statement 4 of Quarter Round
  addWords(c,d);
  xorWords(b,c);
  leftShift(b,r4);
}
void halfQuarterRound(word* a,word* b,word* c,word* d){
  //Statement 1 of Quarter Round
  addWords(a,b);
  xorWords(d,a);
  leftShift(d,r1);
  //Statement 2 of Quarter Round  
  addWords(c,d);
  xorWords(b,c);
  leftShift(b,r2);
}
void inverseQuarterRound(word *a,word *b,word *c,word *d){
  //Statement 1 of Inverse-Quarter Round  
  rightShift(b,r4);
  xorWords(b,c);
  subtractWords(c,d);
  //Statement 2 of Inverse-Quarter Round
  rightShift(d,r3);
  xorWords(d,a);
  subtractWords(a,b);
  //Statement 3 of Inverse-Quarter Round
  rightShift(b,r2);
  xorWords(b,c);
  subtractWords(c,d);
  //Statement 4 of Inverse-Quarter Round
  rightShift(d,r1);
  xorWords(d,a);
  subtractWords(a,b); 
}
void inverseHalfQuarterRound(word* a,word* b,word* c,word* d){
  //Statement 3 of Inverse-Quarter Round
  rightShift(b,r2);
  xorWords(b,c);
  subtractWords(c,d);
  //Statement 4 of Inverse-Quarter Round
  rightShift(d,r1);
  xorWords(d,a);
  subtractWords(a,b);
}
void initialize(block * x){
  /* Intializes the block with the X_0 values of the cipher */
  assign(&x->w[0],c0),assign(&x->w[1],c1),assign(&x->w[2],c2),assign(&x->w[3],c3);
  for(int i=4;i<16;i++){
    int random=getRandom();
    assign(&x->w[i],random); 
  }
}
void oddRounds(block * x){
  /* Performs the odd rounds of the ChaCha cipher */
  quarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12]);  
  quarterRound(&x->w[1],&x->w[5],&x->w[9],&x->w[13]);  
  quarterRound(&x->w[2],&x->w[6],&x->w[10],&x->w[14]);  
  quarterRound(&x->w[3],&x->w[7],&x->w[11],&x->w[15]);  
}
void halfOddRounds(block * x){
  /* Performs half of the odd rounds of the ChaCha cipher */
  halfQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12]);  
  halfQuarterRound(&x->w[1],&x->w[5],&x->w[9],&x->w[13]);  
  halfQuarterRound(&x->w[2],&x->w[6],&x->w[10],&x->w[14]);  
  halfQuarterRound(&x->w[3],&x->w[7],&x->w[11],&x->w[15]);  
}
void inverseOddRounds(block * x){
  /* Performs the inverse of odd rounds of the ChaCha cipher */
  inverseQuarterRound(&x->w[3],&x->w[7],&x->w[11],&x->w[15]);  
  inverseQuarterRound(&x->w[2],&x->w[6],&x->w[10],&x->w[14]);  
  inverseQuarterRound(&x->w[1],&x->w[5],&x->w[9],&x->w[13]);  
  inverseQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12]);  

}
void inverseHalfOddRounds(block * x){
  /* Performs the inverse of half odd rounds of the ChaCha cipher */
  inverseHalfQuarterRound(&x->w[3],&x->w[7],&x->w[11],&x->w[15]);  
  inverseHalfQuarterRound(&x->w[2],&x->w[6],&x->w[10],&x->w[14]);  
  inverseHalfQuarterRound(&x->w[1],&x->w[5],&x->w[9],&x->w[13]);  
  inverseHalfQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12]);  

}
void evenRounds(block * x){
  /* Performs the even rounds of the ChaCha cipher */
  quarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15]);  
  quarterRound(&x->w[1],&x->w[6],&x->w[11],&x->w[12]);  
  quarterRound(&x->w[2],&x->w[7],&x->w[8],&x->w[13]);  
  quarterRound(&x->w[3],&x->w[4],&x->w[9],&x->w[14]);  
}
void halfEvenRounds(block * x){
  /* Performs half of the even rounds of the ChaCha cipher */
  halfQuarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15]);  
  halfQuarterRound(&x->w[1],&x->w[6],&x->w[11],&x->w[12]);  
  halfQuarterRound(&x->w[2],&x->w[7],&x->w[8],&x->w[13]);  
  halfQuarterRound(&x->w[3],&x->w[4],&x->w[9],&x->w[14]);  
}
void inverseEvenRounds(block * x){
  /* Performs the inverse of even rounds of the ChaCha cipher */
  inverseQuarterRound(&x->w[3],&x->w[4],&x->w[9],&x->w[14]);  
  inverseQuarterRound(&x->w[2],&x->w[7],&x->w[8],&x->w[13]); 
  inverseQuarterRound(&x->w[1],&x->w[6],&x->w[11],&x->w[12]);    
  inverseQuarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15]);    
}
void inverseHalfEvenRounds(block * x){
  /* Performs the inverse of half even rounds of the ChaCha cipher */
  inverseHalfQuarterRound(&x->w[3],&x->w[4],&x->w[9],&x->w[14]);  
  inverseHalfQuarterRound(&x->w[2],&x->w[7],&x->w[8],&x->w[13]); 
  inverseHalfQuarterRound(&x->w[1],&x->w[6],&x->w[11],&x->w[12]);    
  inverseHalfQuarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15]);    

}
void chachaBlockEncryption(block * x){
  /* 20 Round Chacha Block encryption*/
  for(int roundCounter=1;roundCounter<=rounds;roundCounter++){
    if(roundCounter%2==0){
      evenRounds(x);
    }
    else{
      oddRounds(x);
    }
  }
}
void chachaBlockDecryption(block * x){
  /* 20 Round Chacha Block decryption*/
  for(int roundCounter=rounds;roundCounter>0;roundCounter--){
    if(roundCounter%2==0){
      inverseEvenRounds(x);
    }
    else{
      inverseOddRounds(x);
    }
  }
}