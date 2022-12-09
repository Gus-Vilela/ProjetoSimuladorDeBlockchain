#include <stdio.h>
#include "openssl/crypto.h" 
#include "openssl/sha.h" 
#include "mtwister.c"
#include <assert.h>
#include <stdlib.h>

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

struct BitContas
{
  int numero;
  int bitcoin;
}BContas[256];


BlocoNaoMinerado inicializaBloco(int i, unsigned char hash[SHA256_DIGEST_LENGTH]);
BlocoNaoMinerado gerarTransacoes(BlocoNaoMinerado bloco, struct BitContas *pont, MTRand *randNumber);
//unsigned char *MinerarBloco(BlocoNaoMinerado bloco);
void printHash(unsigned char hash[], int length);
//BlocoMinerado *aloca(BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[]);
//BlocoMinerado *insereInicio(BlocoMinerado **prim, BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[]);
BlocoMinerado *insereFim(BlocoMinerado **ult, BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[]);
BlocoMinerado * pesquisa(BlocoMinerado *ult, int numBloco, int qntBlocos);
void quickSort (struct BitContas v[], int p, int r);
static int separa (struct BitContas v[], int p, int r);


int main() {
  int qntBlocos = 10000;
  
  for(int i = 0; i < 256; i++){
    BContas[i].bitcoin = 0;
    BContas[i].numero = i;
  } 

  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
  for(int j = 0; j < SHA256_DIGEST_LENGTH; ++j){
    hashAnterior[j] = 0;
  } 

  //BlocoMinerado *prim;
  //prim = NULL;
  MTRand randNumber = seedRand(1234567);
  BlocoMinerado *ult = NULL;
  for(int i = 1; i <= qntBlocos; i++){
    BlocoNaoMinerado blocoZerado = inicializaBloco(i, hashAnterior);
    BlocoNaoMinerado blocoTran = gerarTransacoes(blocoZerado, &BContas[0], &randNumber);
    
    unsigned char hashMine[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)&blocoTran, sizeof(blocoTran), hashMine);
    while((hashMine[0]!=0) || (hashMine[1]!=0))
    {
      blocoTran.nonce = blocoTran.nonce + 1;
      SHA256((unsigned char *)&blocoTran, sizeof(blocoTran), hashMine);
    }
    printHash(hashMine, SHA256_DIGEST_LENGTH);
    insereFim(&ult, blocoTran, hashMine);
    for(int j = 0; j < SHA256_DIGEST_LENGTH; ++j){
      hashAnterior[j] = hashMine[j];
    } 
  }

/*
  printf("%d \n", prim->bloco.numero);
  printf("%d \n", prim->prox->bloco.numero);
  printf("%d \n", prim->prox->prox->bloco.numero);
  printf("%d \n", prim->prox->prox->prox->bloco.numero);
  printf("%d \n", prim->prox->prox->prox->prox->bloco.numero);  
*/   

  printf("Primeiro %d\n", ult->prox->bloco.numero);
  printHash(ult->prox->bloco.hashAnterior, SHA256_DIGEST_LENGTH);
  printf("Meio %d\n", ult->prox->prox->bloco.numero);
  printHash(ult->prox->prox->bloco.hashAnterior, SHA256_DIGEST_LENGTH);
  printf("Meio %d\n", ult->prox->prox->prox->bloco.numero);
  printHash(ult->prox->prox->prox->bloco.hashAnterior, SHA256_DIGEST_LENGTH);
  printf("Meio %d\n", ult->prox->prox->prox->prox->bloco.numero);
  printHash(ult->prox->prox->prox->prox->bloco.hashAnterior, SHA256_DIGEST_LENGTH);
  printf("Último %d\n", ult->bloco.numero);
  printHash(ult->bloco.hashAnterior, SHA256_DIGEST_LENGTH);

  int numBloco;
  printf("Digite o numero do bloco que deseja saber o hash: \n");
  scanf("%d", &numBloco);
  pesquisa(ult, numBloco, qntBlocos);

  for(int i = 0; i < 256; i++){
    printf("%d = %d \n",BContas[i].numero ,BContas[i].bitcoin);
  } 

  quickSort(BContas, 0, 255);

  for(int i = 0; i < 256; i++){
    printf("%d = %d \n",BContas[i].numero ,BContas[i].bitcoin);
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

BlocoNaoMinerado gerarTransacoes(BlocoNaoMinerado bloco, struct BitContas *pont, MTRand *randNumber){
  
  unsigned char qtdTransacoes = (unsigned char) (1 + (genRandLong(randNumber) % 61));
  for (int i = 0; i < qtdTransacoes; i++){
    bloco.data[(i*3)] = (unsigned char) genRandLong(randNumber) % 256;
    //printf("%d \n", bloco.data[(i*3)]);
    bloco.data[((i*3)+1)] = (unsigned char) genRandLong(randNumber)% 256;
    //printf("%d \n", bloco.data[((i*3)+1)]);
    bloco.data[((i*3)+2)] = (unsigned char) (1 + (genRandLong(randNumber) % 50));

    pont[bloco.data[(i*3)]].bitcoin= pont[bloco.data[(i*3)]].bitcoin - bloco.data[((i*3)+2)];
    pont[bloco.data[((i*3)+1)]].bitcoin = pont[bloco.data[((i*3)+1)]].bitcoin + bloco.data[((i*3)+2)];
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

/*
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
*/

BlocoMinerado *insereFim(BlocoMinerado **ult, BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[])
{
  BlocoMinerado *aux = malloc(sizeof(BlocoMinerado));
  if (aux == NULL)
    return NULL;
  aux->bloco = blocoAminerar;
  for(int j = 0; j < SHA256_DIGEST_LENGTH; ++j){
    aux->hash[j] = hashBloco[j];
  }  
  aux->prox = NULL;
  if (*ult == NULL)
  {
    *ult = aux; 
    aux->prox = aux; 
  }
  else
  { 
    aux->prox = (*ult)->prox;
    (*ult)->prox = aux;
    *ult = aux;
  }
  return aux;
}

BlocoMinerado * pesquisa(BlocoMinerado *ult, int numBloco, int qntBlocos){
  BlocoMinerado * aux;
  aux = ult;
  int flag = 0;
  for(int i = 0; i < qntBlocos; i++){
    if(aux->bloco.numero == numBloco){
      printf("O hash do bloco é: ");
      printHash(aux->hash, SHA256_DIGEST_LENGTH);
      flag = 1;
      return aux;
    }else
      aux = aux->prox;
  }
  if(flag == 0){
    printf("Bloco nao encontrado \n");
    return NULL;
  }
}

void quickSort (struct BitContas v[], int p, int r)
{
  while (p < r) {      
      int j = separa (v, p, r);    
      if (j - p < r - j) {     
        quickSort (v, p, j-1);
        p = j + 1;            
      } else {                 
        quickSort (v, j+1, r);
        r = j - 1;
      }
  }
}

static int separa (struct BitContas v[], int p, int r) 
{
  int c = v[r].bitcoin; // pivô
  int  j = p;
  struct BitContas t;
  for (int k = p; /*A*/ k < r; ++k)
    if (v[k].bitcoin <= c) {
      t = v[j], v[j] = v[k], v[k] = t;
      ++j; 
    } 
  t = v[j], v[j] = v[r], v[r]= t;
  return j; 
}