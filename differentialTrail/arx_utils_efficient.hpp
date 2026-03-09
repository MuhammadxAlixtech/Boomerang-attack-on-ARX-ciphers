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
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<wordType> distrib(0, maxNumber);
  return distrib(gen);
}
bool wordCompare(word * a,word * b){
  if(a->bits < b->bits)return false;
  return true;
}
void assign(word * a,wordType number){
  a->bits=number;
}
void addWords(word * a, word * b){
  a->bits+=b->bits;

}
void subtractWords(word * a,word * b){
  a->bits-=b->bits;
}
void leftShift(word * a,int shiftCnt){
  a->bits=((a->bits)<<shiftCnt)|((a->bits)>>(wordSize-shiftCnt));
}
void rightShift(word * a,int shiftCnt){
  a->bits=((a->bits)>>shiftCnt)|((a->bits)<<(wordSize-shiftCnt));
}
void xorWords(word * a, word *b){
  a->bits=a->bits ^ b->bits;
}
void addBlock(block * x,block * y){
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    addWords(&x->w[wordIndex],&y->w[wordIndex]);
  }
}
void subtractBlock(block * x,block * y){
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    subtractWords(&x->w[wordIndex],&y->w[wordIndex]);
  }
}
void xorBlock(block * x,block * y){
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    xorWords(&x->w[wordIndex],&y->w[wordIndex]);
  }
}
void printWord(word * a){
  for(int i=0;i<wordSize;i++){
    // Uncomment below lines to see the binary representation of the word
    // if(a->bits & (1<<i))cout<<1;
    // else cout<<0;
  }
  std::cout<<"0x"<<std::hex<<static_cast<int>(a->bits);
}
void printBlock(block * x){
  for(int i=0;i<16;i++){
    printWord(&x->w[i]);
    std::cout<<" ";
    if(i%4==3)std::cout<<"\n";
  }
  std::cout<<"\n";
}
/*
Find the number of similar bits in 2 different blocks
*/
uint32_t countSimilarBits(block * x0, block * x1){
  uint32_t count=0;
  for(int wordIndex=0;wordIndex<16;wordIndex++){
    count+=wordSize- __builtin_popcount(x0->w[wordIndex].bits ^ x1->w[wordIndex].bits);
  }
  return count;
}
/*
Find Data and Time Complexity
*/
float calculateN(float alpha,float epsilonA,float epsilonD){
  float n=sqrt((sqrt(alpha*(log(4))) + 3*sqrt(1-(epsilonA*epsilonD*epsilonA*epsilonD)))/(epsilonA*epsilonD));
  return n;
}