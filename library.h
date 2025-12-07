#define TAMANHO_ACERVO 20
#define ARQUIVO_CSV "acervo.csv"

// Estrutura para representar um livro
typedef struct {
    int codigo;
    char titulo[50];
    char autor[30];
    char area[30];
    int ano;
    char editora[30];
} Livro;

// Protótipos para as funções definidas em library.c
void inicializarAcervo(Livro acervo[], int tamanho);
void carregarAcervoCSV(Livro acervo[], int tamanho, const char *nomeArquivo);
void salvarAcervoCSV(Livro acervo[], int tamanho, const char *nomeArquivo);

void cadastrarLivros(Livro acervo[], int tamanho);
void imprimirLivros(Livro acervo[], int tamanho);
void pesquisarLivro(Livro acervo[], int tamanho, int codigoBusca);
void ordenarLivros(Livro acervo[], int tamanho);
void consumirRestoLinhas(void);
int confirma(const char *prompt);