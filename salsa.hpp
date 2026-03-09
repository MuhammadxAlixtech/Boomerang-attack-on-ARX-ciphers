#include "arx_utils_efficient.hpp"
#define rounds 20 /* Number of rounds in the Salsa cipher */
/* Constants for the shift operations */
enum ShiftConstants{
  r1=7,
  r2=9,
  r3=13,
  r4=18
};
/* 4x4 Block Initialization Constants for Salsa Cipher */
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
  word a_=*a;
  addWords(&a_,d);
  leftShift(&a_,r1);
  xorWords(b,&a_);
  //Statement 2 of Quarter Round  
  word b_=*b;
  addWords(&b_,a);
  leftShift(&b_,r2);
  xorWords(c,&b_);
  //Statement 3 of Quarter Round
  word c_=*c;
  addWords(&c_,b);
  leftShift(&c_,r3);
  xorWords(d,&c_);
  //Statement 4 of Quarter Round
  word d_=*d;
  addWords(&d_,c);
  leftShift(&d_,r4);
  xorWords(a,&d_);
} 
void halfQuarterRound(word*a,word*b,word*c,word*d){
  //Statement 1 of Quarter Round
  word a_=*a;
  addWords(&a_,d);
  leftShift(&a_,r1);
  xorWords(b,&a_);
  //Statement 2 of Quarter Round  
  word b_=*b;
  addWords(&b_,a);
  leftShift(&b_,r2);
  xorWords(c,&b_);
}
void inverseQuarterRound(word *a,word *b,word *c,word *d){
  //Statement 1 of Inverse-Quarter Round  
  word d_=*d;
  addWords(&d_,c);  
  leftShift(&d_,r4);
  xorWords(a,&d_);
  //Statement 2 of Inverse-Quarter Round
  word c_=*c;
  addWords(&c_,b);
  leftShift(&c_,r3);
  xorWords(d,&c_);
  //Statement 3 of Inverse-Quarter Round
  word b_=*b;
  addWords(&b_,a);
  leftShift(&b_,r2);
  xorWords(c,&b_);
  //Statement 4 of Inverse-Quarter Round
  word a_=*a;
  addWords(&a_,d);
  leftShift(&a_,r1);  
  xorWords(b,&a_);
  
}
void inverseHalfQuarterRound(word*a,word*b,word*c,word*d){
  //Statement 3 of Inverse-Quarter Round
  word b_=*b;
  addWords(&b_,a);
  leftShift(&b_,r2);
  xorWords(c,&b_);
  //Statement 4 of Inverse-Quarter Round
  word a_=*a;
  addWords(&a_,d);
  leftShift(&a_,r1);  
  xorWords(b,&a_);
}
void initialize(block * x){
  /* Intializes the block with the X_0 values of the cipher*/
  assign(&x->w[0],c0),assign(&x->w[5],c1),assign(&x->w[10],c2),assign(&x->w[15],c3);
  for(int i=0;i<16;i++){
    if(i%5==0)continue;
    uint32_t random=getRandom();
    assign(&x->w[i],random); 
  }
}
void oddRounds(block * x){
  /* Performs the odd rounds of the Salsa cipher */
  quarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12]);  
  quarterRound(&x->w[5],&x->w[9],&x->w[13],&x->w[1]);  
  quarterRound(&x->w[10],&x->w[14],&x->w[2],&x->w[6]);  
  quarterRound(&x->w[15],&x->w[3],&x->w[7],&x->w[11]);  
}
void halfOddRounds(block * x){
  /* Performs half of the odd rounds of the Salsa cipher */
  halfQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12]);  
  halfQuarterRound(&x->w[5],&x->w[9],&x->w[13],&x->w[1]);  
  halfQuarterRound(&x->w[10],&x->w[14],&x->w[2],&x->w[6]);  
  halfQuarterRound(&x->w[15],&x->w[3],&x->w[7],&x->w[11]);  

}
void inverseOddRounds(block * x){
  /* Performs the inverse of odd rounds of the Salsa cipher */
  inverseQuarterRound(&x->w[15],&x->w[3],&x->w[7],&x->w[11]);  
  inverseQuarterRound(&x->w[10],&x->w[14],&x->w[2],&x->w[6]);  
  inverseQuarterRound(&x->w[5],&x->w[9],&x->w[13],&x->w[1]);    
  inverseQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12]);  

}
void inverseHalfOddRounds(block * x){
  /* Performs the inverse of half odd rounds of the Salsa cipher */
  inverseHalfQuarterRound(&x->w[15],&x->w[3],&x->w[7],&x->w[11]);  
  inverseHalfQuarterRound(&x->w[10],&x->w[14],&x->w[2],&x->w[6]);  
  inverseHalfQuarterRound(&x->w[5],&x->w[9],&x->w[13],&x->w[1]);    
  inverseHalfQuarterRound(&x->w[0],&x->w[4],&x->w[8],&x->w[12]);  

}
void evenRounds(block * x){
  /* Performs the even rounds of the Salsa cipher */
  quarterRound(&x->w[0],&x->w[1],&x->w[2],&x->w[3]);  
  quarterRound(&x->w[5],&x->w[6],&x->w[7],&x->w[4]);  
  quarterRound(&x->w[10],&x->w[11],&x->w[8],&x->w[9]);  
  quarterRound(&x->w[15],&x->w[12],&x->w[13],&x->w[14]);  
}
void halfEvenRounds(block * x){
  /* Performs half of the even rounds of the Salsa cipher */
  halfQuarterRound(&x->w[0],&x->w[1],&x->w[2],&x->w[3]);  
  halfQuarterRound(&x->w[5],&x->w[6],&x->w[7],&x->w[4]);  
  halfQuarterRound(&x->w[10],&x->w[11],&x->w[8],&x->w[9]);  
  halfQuarterRound(&x->w[15],&x->w[12],&x->w[13],&x->w[14]);  
}
void inverseEvenRounds(block * x){
  /* Performs the inverse of even rounds of the Salsa cipher */
  inverseQuarterRound(&x->w[15],&x->w[12],&x->w[13],&x->w[14]);  
  inverseQuarterRound(&x->w[10],&x->w[11],&x->w[8],&x->w[9]);
  inverseQuarterRound(&x->w[5],&x->w[6],&x->w[7],&x->w[4]);      
  inverseQuarterRound(&x->w[0],&x->w[1],&x->w[2],&x->w[3]);      

}
void inverseHalfEvenRounds(block * x){
  /* Performs the inverse of half even rounds of the Salsa cipher */
  inverseHalfQuarterRound(&x->w[15],&x->w[12],&x->w[13],&x->w[14]);  
  inverseHalfQuarterRound(&x->w[10],&x->w[11],&x->w[8],&x->w[9]);
  inverseHalfQuarterRound(&x->w[5],&x->w[6],&x->w[7],&x->w[4]);      
  inverseHalfQuarterRound(&x->w[0],&x->w[1],&x->w[2],&x->w[3]);      

}
void salsaBlockEncryption(block * x){
  /* 20 Round Salsa Block encryption */
  for(int roundCounter=1;roundCounter<=rounds;roundCounter++){
    if(roundCounter%2==0){
      evenRounds(x);
    }
    else{
      oddRounds(x);
    }
  }
}
void salsaBlockDecryption(block * x){
  /* 20 Round Salsa Block decryption */
  for(int roundCounter=rounds;roundCounter>0;roundCounter--){
    if(roundCounter%2==0){
      inverseEvenRounds(x);
    }
    else{
      inverseOddRounds(x);
    }
  }
}
