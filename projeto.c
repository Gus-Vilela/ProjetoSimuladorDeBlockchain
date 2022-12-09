#include <stdio.h>
#include "openssl/crypto.h" 
#include "openssl/sha.h" 
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

struct BitContas
{
  int numero;
  int bitcoin;
}BContas[256];

BlocoNaoMinerado inicializaBloco(int i, unsigned char hash[SHA256_DIGEST_LENGTH]);
BlocoNaoMinerado gerarTransacoes(BlocoNaoMinerado bloco, struct BitContas *pont, MTRand *randNumber);
void printHash(unsigned char hash[], int length);
BlocoMinerado *insereFim(BlocoMinerado **ult, BlocoNaoMinerado blocoAminerar, unsigned char hashBloco[]);
BlocoMinerado * pesquisa(BlocoMinerado *ult, int numBloco, int qntBlocos);
void quickSort (struct BitContas v[], int p, int r, int flag);
static int separa (struct BitContas v[], int p, int r, int flag);

FILE *numbs;

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

  MTRand randNumber = seedRand(1234567);
  BlocoMinerado *ult = NULL;

  numbs = fopen("arquivo.txt", "w+");
  if (numbs == NULL)
    {
      printf("Problemas na CRIACAO do arquivo\n");
      system("pause");
      exit(1);
    }
  for(int i = 1; i <= qntBlocos; i++){
    BlocoNaoMinerado blocoTran = gerarTransacoes(inicializaBloco(i, hashAnterior), &BContas[0], &randNumber);
    
    unsigned char hashMine[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)&blocoTran, sizeof(blocoTran), hashMine);
    while((hashMine[0]!=0) || (hashMine[1]!=0))
    {
      blocoTran.nonce = blocoTran.nonce + 1;
      SHA256((unsigned char *)&blocoTran, sizeof(blocoTran), hashMine);
    }
    
    printf("Bloco %d :", blocoTran.numero);
    fprintf(numbs, "Bloco %d :", blocoTran.numero);
    printHash(hashMine, SHA256_DIGEST_LENGTH);
    
    insereFim(&ult, blocoTran, hashMine);
    for(int j = 0; j < SHA256_DIGEST_LENGTH; ++j){
      hashAnterior[j] = hashMine[j];
    } 
  }
  fclose(numbs);

  int escolha;
  do
  {
    printf("------------------MENU---------------------\n");
    printf("1. Pesquisar hash de um bloco. \n");
    printf("2. Listar enderecos com respectivas quantidades de bitcoins em ordem crescente.\n");
    printf("3. Listar enderecos com respectivas quantidades de bitcoins em ordem decrescente. \n");
    printf("4. Sair \n");
    scanf("%d", &escolha);

    switch (escolha)
    {
      case 1:
        int numBloco;
        printf("Digite o numero do bloco que deseja saber o hash: \n");
        scanf("%d", &numBloco);
        pesquisa(ult, numBloco, qntBlocos);
        break;
      case 2:
        quickSort(BContas, 0, 255, 1);
        for(int i = 0; i < 256; i++)
        {
          printf("Endereco %d = %d Bitcoin\n",BContas[i].numero ,BContas[i].bitcoin);
        } 
        break;
      case 3:
        quickSort(BContas, 0, 255, 2);
        for(int i = 0; i < 256; i++)
        {
          printf("Endereco %d = %d Bitcoin \n",BContas[i].numero ,BContas[i].bitcoin);
        } 
        break;
      default:
        break;
    }
  } while (escolha != 4);
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
    bloco.data[((i*3)+1)] = (unsigned char) genRandLong(randNumber)% 256;
    bloco.data[((i*3)+2)] = (unsigned char) (1 + (genRandLong(randNumber) % 50));

    pont[bloco.data[(i*3)]].bitcoin= pont[bloco.data[(i*3)]].bitcoin - bloco.data[((i*3)+2)];
    pont[bloco.data[((i*3)+1)]].bitcoin = pont[bloco.data[((i*3)+1)]].bitcoin + bloco.data[((i*3)+2)];
  }
  return bloco;
}

void printHash(unsigned char hash[] , int length)
{
  int i;
  for(i=0;i<length;++i){
    printf("%02x", hash[i]);
    fprintf(numbs, "%02x", hash[i]);
  }  
  printf("\n");
  fprintf(numbs, " \n");
}
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
  for(int i = 0; i < qntBlocos; i++){
    if(aux->bloco.numero == numBloco){
      printf("O hash do bloco %d é: ", numBloco);
      printHash(aux->hash, SHA256_DIGEST_LENGTH);
      return aux;
    }else
      aux = aux->prox;
  }
  printf("Bloco nao encontrado \n");
  return NULL;
}

void quickSort (struct BitContas v[], int p, int r, int flag)
{
  while (p < r) {      
      int j = separa (v, p, r, flag);    
      if (j - p < r - j) {     
        quickSort (v, p, j-1, flag);
        p = j + 1;            
      } else {                 
        quickSort (v, j+1, r, flag);
        r = j - 1;
      }
  }
}

static int separa (struct BitContas v[], int p, int r, int flag) 
{
  int c = v[r].bitcoin; 
  int  j = p;
  struct BitContas t;
  for (int k = p; k < r; ++k)
    if (flag == 1)
    {
      if (v[k].bitcoin <= c) {
      t = v[j], v[j] = v[k], v[k] = t;
      ++j; 
      } 
    }else if(flag == 2)
    {
      if (v[k].bitcoin >= c) {
      t = v[j], v[j] = v[k], v[k] = t;
      ++j; 
      } 
    }
  t = v[j], v[j] = v[r], v[r]= t;
  return j; 
}