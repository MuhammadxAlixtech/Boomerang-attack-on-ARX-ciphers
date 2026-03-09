#include<iostream>
#include<algorithm>
#include<random>
#include<iomanip>
#include<climits>
#include<ctime>
#include<vector>
#include<cmath>
#define wordSize 32
#define wordType uint32_t
#define maxNumber UINT_MAX
/* 
Declaring structures of word and block, used to implement Chacha  
*/
struct word{
/*
Binary array to store each bit of the word
*/
  wordType bits;
};
struct block{
/*
An array of 16 words
*/
  word w[16];
};
/* 
Functions to implement the modular add, subtract, left-shift, right-shift, xor and assign operations
*/
wordType getRandom(){
  /*
    Generate a random 32-bit number
  */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<wordType> distrib(0, maxNumber);
  return distrib(gen);
}
void assign(word * a,wordType number){
  /*
    Assign a number to a word
  */
  a->bits=number;
}
void addWords(word * a, word * b){
  /*
    Modular Addition of two words
  */
  a->bits+=b->bits;

}
void subtractWords(word * a,word * b){
  /*
    Modular Subtraction of two words
  */
  a->bits-=b->bits;
}
void leftShift(word * a,int shiftCnt){
  /*
    Left Circular Shift of a word
  */
  a->bits=((a->bits)<<shiftCnt)|((a->bits)>>(wordSize-shiftCnt));
}
void rightShift(word * a,int shiftCnt){
  /*
    Right Circular Shift of a word
  */
  a->bits=((a->bits)>>shiftCnt)|((a->bits)<<(wordSize-shiftCnt));
}
void xorWords(word * a, word *b){
  /*
    XOR of two words
  */
  a->bits=a->bits ^ b->bits;
}
void addBlock(block * x,block * y){
  /*
    Modular Addition of two 4x4 word blocks
  */
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    addWords(&x->w[wordIndex],&y->w[wordIndex]);
  }
}
void subtractBlock(block * x,block * y){
  /*
    Modular Subtraction of two 4x4 word blocks
  */
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    subtractWords(&x->w[wordIndex],&y->w[wordIndex]);
  }
}
void xorBlock(block * x,block * y){
  /*
    XOR of two 4x4 blocks
  */
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    xorWords(&x->w[wordIndex],&y->w[wordIndex]);
  }
}
void printBinBlock(block * x){
  /* Print the binary representation of a 4x4 word block */
  for(int i=0;i<16;i++){
    for(int j=0;j<wordSize;j++){
      if(x->w[i].bits & (1<<j))std::cout<<1;
      else std::cout<<0;
    }
    std::cout<<" ";
    if(i%4==3)std::cout<<"\n";
  }
  std::cout<<"\n";
}
void printHexBlock(block * x){
  /* Print the hexadecimal representation of a 4x4 word block */
  for(int i=0;i<16;i++){
    std::cout<<"0x"<<std::hex<<static_cast<int>(x->w[i].bits);
    std::cout<<" ";
    if(i%4==3)std::cout<<"\n";
  }
  std::cout<<"\n";
}
/*
Find Data and Time Complexity
*/
float calculateN(float alpha,float epsilonA,float epsilonD){
  float n=sqrt((sqrt(alpha*(log(4))) + 3*sqrt(1-(epsilonA*epsilonD*epsilonA*epsilonD)))/(epsilonA*epsilonD));
  return n;
}