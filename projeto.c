#include <stdio.h>
#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h" //arquivo de definição necessário função SHA256
#include <string.h>
#include "mtwister.c"
#include <assert.h>

struct BlocoNaoMinerado
{
  unsigned int numero;
  unsigned int nonce;
  unsigned char data[184];
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
};
typedef struct BlocoNaoMinerado BlocoNaoMinerado;

struct BlocoMinerado
{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  struct BlocoMinerado *prox;
};

typedef struct BlocoMinerado BlocoMinerado;

BlocoNaoMinerado inicializaBloco(int i, unsigned char hash[SHA256_DIGEST_LENGTH]);
BlocoNaoMinerado gerarTransacoes(BlocoNaoMinerado bloco);
//unsigned char *MinerarBloco(BlocoNaoMinerado bloco);
void printHash(unsigned char hash[], int length);
BlocoMinerado *aloca(BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[]);
BlocoMinerado *insereInicio(BlocoMinerado **prim, BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[]);


int main() {
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
  for(int j = 0; j < SHA256_DIGEST_LENGTH; ++j){
    hashAnterior[j] = 0;
  } 
  BlocoMinerado *prim;
  prim = NULL;
  for( int i = 1; i <= 5; i++){// quant blocos
    BlocoNaoMinerado bloco = inicializaBloco(i, hashAnterior);
    BlocoNaoMinerado blocoTran = gerarTransacoes(bloco);
    
    unsigned char hashMine[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)&blocoTran, sizeof(blocoTran), hashMine);
    while(hashMine[0]!=0 && hashMine[1]!=0)
    {
      printf("Minerando... \n");
      blocoTran.nonce = blocoTran.nonce + 1;
      SHA256((unsigned char *)&blocoTran, sizeof(blocoTran), hashMine);
    }
      printHash(hashMine, SHA256_DIGEST_LENGTH);
      prim = insereInicio(&prim, blocoTran, hashMine);
    }
  return 0;
}

BlocoNaoMinerado inicializaBloco(int i, unsigned char hash[SHA256_DIGEST_LENGTH]){
  BlocoNaoMinerado bloco;
  bloco.numero = i;
  bloco.nonce = 0;
  for(int j = 0; j < SHA256_DIGEST_LENGTH; ++j){
    bloco.hashAnterior[j] = hash[j];
  } 
  for(int j = 0; j < 184; ++j){
    bloco.data[j] = 0;
  } 
  
  return bloco;
}

BlocoNaoMinerado gerarTransacoes(BlocoNaoMinerado bloco){
  MTRand randNumber = seedRand(1234567);
  unsigned char qtdTransacoes = (unsigned char) (1 + (genRandLong(&randNumber) % 61));
  for (int i = 0; i < qtdTransacoes; i++){
    bloco.data[(i*3)] = (unsigned char) genRandLong(&randNumber) % 256;
    bloco.data[((i*3)+1)] = (unsigned char) genRandLong(&randNumber)% 256;
    bloco.data[((i*3)+2)] = (unsigned char) (1 + (genRandLong(&randNumber) % 50));
  }
  return bloco;
}

/*
unsigned char *MinerarBloco(BlocoNaoMinerado bloco){
  
  unsigned char hashMine[SHA256_DIGEST_LENGTH];
  SHA256((unsigned char *)&bloco, sizeof(bloco), hashMine);
  while(hashMine[0]!=0 && hashMine[1]!=0)
  {
    bloco.nonce = bloco.nonce + 1;
    SHA256((unsigned char *)&bloco, sizeof(bloco), hashMine);
  }
  unsigned char *hashEnd = malloc(sizeof(unsigned char)*SHA256_DIGEST_LENGTH);
  hashEnd = &hashMine;
  return hashEnd;
}
*/

void printHash(unsigned char hash[] , int length)
{
  int i;
  for(i=0;i<length;++i)
    printf("%02x", hash[i]);
  printf("\n");
}


BlocoMinerado *aloca(BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[]){
  BlocoMinerado *novoNo = (BlocoMinerado *)malloc(sizeof(BlocoMinerado));
  if(novoNo == NULL){
    return 0;
  }
  novoNo->bloco = blocoAminerar;
  for(int j = 0; j < SHA256_DIGEST_LENGTH; ++j){
    novoNo->hash[j] = hashBloco[j];
  }  
  novoNo->prox = NULL;
  return novoNo;
}

BlocoMinerado *insereInicio(BlocoMinerado **prim, BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[]){
  assert(prim);
  BlocoMinerado *novoNo = aloca(blocoAminerar, hashBloco);
  if (novoNo == NULL){
    return 0;
  }
  novoNo->prox = *prim;
  *prim = novoNo;
  return novoNo;
}
