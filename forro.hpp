#include "arx_utils_efficient.hpp"
#define rounds 20 /* Number of rounds in the Forro cipher */
/* Constants for the shift operations */
enum ShiftConstants{
  r1=10,
  r2=27,
  r3=8,
};
/* 4x4 Block Initialization Constants for Forro Cipher */
enum InitailBlockConstants{
  c0=0x746c6f76,
  c1=0x61616461,
  c2=0x72626173,
  c3=0x61636e61
};
/* 
Quarter-Round Implementation for 8-bit Chacha Cipher
*/
void quarterRound(word* a,word* b,word* c,word* d,word *e){
  //Statement 1 of Quarter Round
  addWords(d,e);
  xorWords(c,d);
  addWords(b,c);
  leftShift(b,r1);
  //Statement 2 of Quarter Round  
  addWords(a,b);
  xorWords(e,a);
  addWords(d,e);
  leftShift(d,r2);
  //Statement 3 of Quarter Round
  addWords(c,d);
  xorWords(b,c);
  addWords(a,b);
  leftShift(a,r3);
}
void inverseQuarterRound(word *a,word *b,word *c,word *d, word *e){
  //Statement 1 of Inverse-Quarter Round  
  rightShift(a,r3);
  subtractWords(a,b);
  xorWords(b,c);
  subtractWords(c,d);
  //Statement 2 of Inverse-Quarter Round
  rightShift(d,r2);
  subtractWords(d,e);
  xorWords(e,a);
  subtractWords(a,b);
  //Statement 3 of Inverse-Quarter Round
  rightShift(b,r1);
  subtractWords(b,c);
  xorWords(c,d);
  subtractWords(d,e);
}
void initialize(block * x){
  /* Intializes the block with the X_0 values of the cipher */
  assign(&x->w[6],c0),assign(&x->w[7],c1),assign(&x->w[14],c2),assign(&x->w[15],c3);
  for(int i=0;i<16;i++){
    if(i==6 || i==7 || i==14 || i==15)continue;
    int random=getRandom();
    assign(&x->w[i],random); 
  }
}
void oddRounds(block * x){
  /* Performs the odd rounds of the Forro cipher */
  quarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12],&x->w[3]);  
  quarterRound(&x->w[1],&x->w[5],&x->w[9],&x->w[13],&x->w[0]);  
  quarterRound(&x->w[2],&x->w[6],&x->w[10],&x->w[14],&x->w[1]);  
  quarterRound(&x->w[3],&x->w[7],&x->w[11],&x->w[15],&x->w[2]);  
}
void inverseOddRounds(block * x){
  /* Performs the inverse of odd rounds of the Forro cipher */
  inverseQuarterRound(&x->w[3],&x->w[7],&x->w[11],&x->w[15],&x->w[2]);  
  inverseQuarterRound(&x->w[2],&x->w[6],&x->w[10],&x->w[14],&x->w[1]);  
  inverseQuarterRound(&x->w[1],&x->w[5],&x->w[9],&x->w[13],&x->w[0]);  
  inverseQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12],&x->w[3]);  
}
void halfOddRounds(block * x){
  /* Performs half of the odd rounds of the Forro cipher */
  quarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12],&x->w[3]);  
  quarterRound(&x->w[1],&x->w[5],&x->w[9],&x->w[13],&x->w[0]); 
}
void inverseHalfOddRounds(block * x){ 
  /* Performs the inverse of half odd rounds of the Forro cipher */
  inverseQuarterRound(&x->w[1],&x->w[5],&x->w[9],&x->w[13],&x->w[0]);  
  inverseQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12],&x->w[3]);  
}
void quarterOddRounds(block * x){
  /* Performs quarter of the odd rounds of the Forro cipher */
  quarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12],&x->w[3]);  
}
void inverseQuarterOddRounds(block * x){
  /* Performs the inverse of quarter odd rounds of the Forro cipher */
  inverseQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12],&x->w[3]);  
}
void evenRounds(block * x){
  /* Performs the even rounds of the Forro cipher */
  quarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15],&x->w[3]);  
  quarterRound(&x->w[1],&x->w[6],&x->w[11],&x->w[12],&x->w[0]);  
  quarterRound(&x->w[2],&x->w[7],&x->w[8],&x->w[13],&x->w[1]);  
  quarterRound(&x->w[3],&x->w[4],&x->w[9],&x->w[14],&x->w[2]);  
}
void inverseEvenRounds(block * x){
  /* Performs the inverse of even rounds of the Forro cipher */
  inverseQuarterRound(&x->w[3],&x->w[4],&x->w[9],&x->w[14],&x->w[2]);  
  inverseQuarterRound(&x->w[2],&x->w[7],&x->w[8],&x->w[13],&x->w[1]); 
  inverseQuarterRound(&x->w[1],&x->w[6],&x->w[11],&x->w[12],&x->w[0]);    
  inverseQuarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15],&x->w[3]);    

}
void halfEvenRounds(block * x){
  /* Performs half of the even rounds of the Forro cipher */
  quarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15],&x->w[3]);  
  quarterRound(&x->w[1],&x->w[6],&x->w[11],&x->w[12],&x->w[0]);  
 
}
void inverseHalfEvenRounds(block * x){
  /* Performs the inverse of half even rounds of the Forro cipher */
  inverseQuarterRound(&x->w[1],&x->w[6],&x->w[11],&x->w[12],&x->w[0]);    
  inverseQuarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15],&x->w[3]);    
 
}
void quarterEvenRounds(block * x){
  /* Performs quarter of the even rounds of the Forro cipher */
  quarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15],&x->w[3]);  
}
void inverseQuarterEvenRounds(block * x){ 
  /* Performs the inverse of quarter even rounds of the Forro cipher */  
  inverseQuarterRound(&x->w[0],&x->w[5],&x->w[10],&x->w[15],&x->w[3]);    
}
void forroBlockEncryption(block * x){
  /* 20 Round Forro Block encryption */
  for(int roundCounter=1;roundCounter<=rounds;roundCounter++){
    if(roundCounter%2==0){
      evenRounds(x);
    }
    else{
      oddRounds(x);
    }
  }
}
void forroBlockDecryption(block * x){
  /* 20 Round Forro Block decryption */
  for(int roundCounter=rounds;roundCounter>0;roundCounter--){
    if(roundCounter%2==0){
      inverseEvenRounds(x);
    }
    else{
      inverseOddRounds(x);
    }
  }
}