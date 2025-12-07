#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAMANHO_ACERVO 20
#define ARQUIVO_BIN "acervo.dat"
#define ARQUIVO_CSV "acervo.csv"

typedef struct {
    int codigo;
    char titulo[50];
    char autor[30];
    char area[30];
    int ano;
    char editora[30];
} Livro;

/* Protótipos */
void inicializarAcervo(Livro acervo[], int tamanho);
void carregarAcervo(Livro acervo[], int tamanho);
void carregarAcervoBinario(Livro acervo[], int tamanho, const char *nomeArquivo);
void carregarAcervoCSV(Livro acervo[], int tamanho, const char *nomeArquivo);

void salvarAcervo(Livro acervo[], int tamanho);
int salvarAcervoBinario(Livro acervo[], int tamanho, const char *nomeArquivo, int confirmarSobrescrever);
int salvarAcervoCSV(Livro acervo[], int tamanho, const char *nomeArquivo, int confirmarSobrescrever);

void cadastrarLivros(Livro acervo[], int tamanho);
void imprimirLivros(Livro acervo[], int tamanho);
void pesquisarLivro(Livro acervo[], int tamanho, int codigoBusca);
void ordenarLivros(Livro acervo[], int tamanho);
void consumirRestoLinhas(void);
int lerInteiroComValidacao(const char *prompt);
int confirma(const char *prompt);
int arquivoExiste(const char *nomeArquivo);

int main(void) {
    Livro acervo[TAMANHO_ACERVO];
    int opcao;
    int codigoBusca;

    inicializarAcervo(acervo, TAMANHO_ACERVO);
    carregarAcervo(acervo, TAMANHO_ACERVO);

    do {
        printf("\n=== Sistema de Gerenciamento de Livros (com arquivo & opcoes) ===\n");
        printf("1 - Cadastrar livros\n");
        printf("2 - Imprimir todos os livros\n");
        printf("3 - Pesquisar livro por codigo\n");
        printf("4 - Ordenar livros por ano\n");
        printf("5 - Salvar (sem sair)\n");
        printf("6 - Salvar e Sair\n");
        printf("7 - Sair sem salvar\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida.\n");
            consumirRestoLinhas();
            opcao = 0;
            continue;
        }
        consumirRestoLinhas();

        switch (opcao) {
            case 1:
                cadastrarLivros(acervo, TAMANHO_ACERVO);
                break;
            case 2:
                imprimirLivros(acervo, TAMANHO_ACERVO);
                break;
            case 3:
                printf("Digite o codigo para pesquisa: ");
                if (scanf("%d", &codigoBusca) != 1) {
                    printf("Entrada invalida.\n");
                    consumirRestoLinhas();
                } else {
                    consumirRestoLinhas();
                    pesquisarLivro(acervo, TAMANHO_ACERVO, codigoBusca);
                }
                break;
            case 4:
                ordenarLivros(acervo, TAMANHO_ACERVO);
                printf("Livros ordenados por ano.\n");
                break;
            case 5:
                salvarAcervo(acervo, TAMANHO_ACERVO);
                break;
            case 6:
                if (salvarAcervoBinario(acervo, TAMANHO_ACERVO, ARQUIVO_BIN, 1) ||
                    salvarAcervoCSV(acervo, TAMANHO_ACERVO, ARQUIVO_CSV, 1)) {
                    /* salvo ao menos um (a implementação pede confirmacao por arquivo) */
                }
                printf("Acervo salvo. Encerrando programa.\n");
                return 0;
            case 7:
                printf("Encerrando sem salvar.\n");
                return 0;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }

    } while (1);

    return 0;
}

/* Inicializa o vetor marcando posicoes vazias com codigo = -1 */
void inicializarAcervo(Livro acervo[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        acervo[i].codigo = -1;
        acervo[i].titulo[0] = '\0';
        acervo[i].autor[0] = '\0';
        acervo[i].area[0] = '\0';
        acervo[i].ano = 0;
        acervo[i].editora[0] = '\0';
    }
}

/* Carrega acervo: primeiro tenta binario, se nao existir tenta CSV */
void carregarAcervo(Livro acervo[], int tamanho) {
    if (arquivoExiste(ARQUIVO_BIN)) {
        carregarAcervoBinario(acervo, tamanho, ARQUIVO_BIN);
    } else if (arquivoExiste(ARQUIVO_CSV)) {
        carregarAcervoCSV(acervo, tamanho, ARQUIVO_CSV);
    } else {
        printf("Nenhum arquivo de dados encontrado. Iniciando acervo vazio.\n");
    }
}

/* Carrega arquivo binario (.dat) */
void carregarAcervoBinario(Livro acervo[], int tamanho, const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "rb");
    if (!f) {
        printf("Falha ao abrir %s para leitura.\n", nomeArquivo);
        return;
    }
    size_t lidos = fread(acervo, sizeof(Livro), (size_t)tamanho, f);
    if (lidos == 0) {
        printf("Arquivo binario vazio ou corrompido. Acervo vazio.\n");
        inicializarAcervo(acervo, tamanho);
    } else if (lidos < (size_t)tamanho) {
        for (size_t i = lidos; i < (size_t)tamanho; i++) {
            acervo[i].codigo = -1;
        }
    }
    printf("Carregados %zu registro(s) de %s.\n", lidos, nomeArquivo);
    fclose(f);
}

/* Carrega arquivo CSV (espera cabeçalho opcional; cada linha: codigo,titulo,autor,area,ano,editora) */
void carregarAcervoCSV(Livro acervo[], int tamanho, const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        printf("Falha ao abrir %s para leitura.\n", nomeArquivo);
        return;
    }
    char linha[512];
    int idx = 0;

    /* pula possivel cabecalho (verifica se primeira linha contem letras) */
    if (fgets(linha, sizeof(linha), f) != NULL) {
        int codigoTemp;
        if (sscanf(linha, "%d,", &codigoTemp) != 1) {
            /* parecia cabecalho; ignora e continua lendo linhas subsequentes */
        } else {
            /* primeira linha era um registro valido: processa-a */
            rewind(f);
        }
    } else {
        fclose(f);
        return;
    }

    while (idx < tamanho && fgets(linha, sizeof(linha), f) != NULL) {
        /* remove newline */
        linha[strcspn(linha, "\n")] = '\0';
        /* form: codigo,titulo,autor,area,ano,editora */
        Livro L;
        char *token;
        token = strtok(linha, ",");
        if (!token) continue;
        L.codigo = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(L.titulo, token, sizeof(L.titulo)); L.titulo[sizeof(L.titulo)-1]='\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(L.autor, token, sizeof(L.autor)); L.autor[sizeof(L.autor)-1]='\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(L.area, token, sizeof(L.area)); L.area[sizeof(L.area)-1]='\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        L.ano = atoi(token);

        token = strtok(NULL, ",");
        if (!token) token = "";
        strncpy(L.editora, token, sizeof(L.editora)); L.editora[sizeof(L.editora)-1]='\0';

        acervo[idx++] = L;
    }
    /* marca o restante como vazio */
    for (int i = idx; i < tamanho; i++) acervo[i].codigo = -1;
    printf("Carregados %d registro(s) de %s (CSV).\n", idx, nomeArquivo);
    fclose(f);
}

/* Função que oferece escolha de formato e salva o acervo (usada na opcao "Salvar sem sair") */
void salvarAcervo(Livro acervo[], int tamanho) {
    printf("Escolha o formato para salvar:\n");
    printf("1 - Binario (%s)\n", ARQUIVO_BIN);
    printf("2 - CSV (%s)\n", ARQUIVO_CSV);
    printf("3 - Ambos (binario + CSV)\n");
    printf("Escolha [1-3]: ");
    int escolha = 0;
    if (scanf("%d", &escolha) != 1) {
        printf("Entrada invalida. Abortando salvamento.\n");
        consumirRestoLinhas();
        return;
    }
    consumirRestoLinhas();

    if (escolha == 1) {
        if (salvarAcervoBinario(acervo, tamanho, ARQUIVO_BIN, 1)) {
            printf("Salvo em %s\n", ARQUIVO_BIN);
        } else {
            printf("Nao salvou em binario.\n");
        }
    } else if (escolha == 2) {
        if (salvarAcervoCSV(acervo, tamanho, ARQUIVO_CSV, 1)) {
            printf("Salvo em %s\n", ARQUIVO_CSV);
        } else {
            printf("Nao salvou em CSV.\n");
        }
    } else if (escolha == 3) {
        int ok1 = salvarAcervoBinario(acervo, tamanho, ARQUIVO_BIN, 1);
        int ok2 = salvarAcervoCSV(acervo, tamanho, ARQUIVO_CSV, 1);
        if (ok1) printf("Salvo em %s\n", ARQUIVO_BIN);
        if (ok2) printf("Salvo em %s\n", ARQUIVO_CSV);
        if (!ok1 && !ok2) printf("Nenhum arquivo salvo.\n");
    } else {
        printf("Opcao invalida. Abortando salvamento.\n");
    }
}

/* Salva binario; se confirmarSobrescrever==1 e arquivo existe, pergunta ao usuario */
int salvarAcervoBinario(Livro acervo[], int tamanho, const char *nomeArquivo, int confirmarSobrescrever) {
    if (arquivoExiste(nomeArquivo) && confirmarSobrescrever) {
        if (!confirma("Arquivo binario ja existe. Deseja sobrescrever? (s/n): ")) {
            printf("Operacao de salvar binario abortada.\n");
            return 0;
        }
    }

    FILE *f = fopen(nomeArquivo, "wb");
    if (!f) {
        perror("Erro ao abrir arquivo binario para escrita");
        return 0;
    }
    size_t escritos = fwrite(acervo, sizeof(Livro), (size_t)tamanho, f);
    if (escritos != (size_t)tamanho) {
        printf("Aviso: nem todos os registros foram gravados corretamente (escritos=%zu).\n", escritos);
    }
    fclose(f);
    return 1;
}

/* Salva CSV; formato: codigo,titulo,autor,area,ano,editora. Ignora posicoes vazias. */
int salvarAcervoCSV(Livro acervo[], int tamanho, const char *nomeArquivo, int confirmarSobrescrever) {
    if (arquivoExiste(nomeArquivo) && confirmarSobrescrever) {
        if (!confirma("Arquivo CSV ja existe. Deseja sobrescrever? (s/n): ")) {
            printf("Operacao de salvar CSV abortada.\n");
            return 0;
        }
    }

    FILE *f = fopen(nomeArquivo, "w");
    if (!f) {
        perror("Erro ao abrir arquivo CSV para escrita");
        return 0;
    }

    /* opcional: escreve cabecalho */
    fprintf(f, "codigo,titulo,autor,area,ano,editora\n");
    for (int i = 0; i < tamanho; i++) {
        if (acervo[i].codigo != -1) {
            /* substituir virgulas internas? aqui simplificamos e assumimos que campos nao contem virgulas */
            fprintf(f, "%d,%s,%s,%s,%d,%s\n",
                    acervo[i].codigo,
                    acervo[i].titulo,
                    acervo[i].autor,
                    acervo[i].area,
                    acervo[i].ano,
                    acervo[i].editora);
        }
    }
    fclose(f);
    return 1;
}

/* Cadastro de livros: preenche posicoes vazias; evita codigo duplicado; valida ano entre 1500 e 2100 */
void cadastrarLivros(Livro acervo[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (acervo[i].codigo != -1) continue; // pula ocupados

        printf("\n--- Cadastro (posicao %d de %d) ---\n", i + 1, tamanho);

        int codigo;
        printf("Codigo (numero): ");
        if (scanf("%d", &codigo) != 1) {
            printf("Entrada invalida. Abortando cadastro.\n");
            consumirRestoLinhas();
            break;
        }
        consumirRestoLinhas();

        /* verifica duplicado */
        int existe = 0;
        for (int k = 0; k < tamanho; k++) {
            if (acervo[k].codigo == codigo) { existe = 1; break; }
        }
        if (existe) {
            printf("Ja existe um livro com esse codigo. Deseja tentar outro? (s/n): ");
            if (!confirma("")) break;
            else { i--; continue; } /* repetir mesma posicao */
        }

        acervo[i].codigo = codigo;

        printf("Titulo (ate 49 chars): ");
        if (!fgets(acervo[i].titulo, sizeof(acervo[i].titulo), stdin)) acervo[i].titulo[0] = '\0';
        acervo[i].titulo[strcspn(acervo[i].titulo, "\n")] = '\0';

        printf("Autor (ate 29 chars): ");
        if (!fgets(acervo[i].autor, sizeof(acervo[i].autor), stdin)) acervo[i].autor[0] = '\0';
        acervo[i].autor[strcspn(acervo[i].autor, "\n")] = '\0';

        printf("Area (ate 29 chars): ");
        if (!fgets(acervo[i].area, sizeof(acervo[i].area), stdin)) acervo[i].area[0] = '\0';
        acervo[i].area[strcspn(acervo[i].area, "\n")] = '\0';

        /* Validacao do ano: entre 1500 e 2100 */
        int ano = lerInteiroComValidacao("Ano de publicacao (1500-2100): ");
        if (ano == -1) {
            printf("Ano invalido. Ajustando para 0.\n");
            acervo[i].ano = 0;
        } else {
            acervo[i].ano = ano;
        }

        printf("Editora (ate 29 chars): ");
        if (!fgets(acervo[i].editora, sizeof(acervo[i].editora), stdin)) acervo[i].editora[0] = '\0';
        acervo[i].editora[strcspn(acervo[i].editora, "\n")] = '\0';

        printf("Livro cadastrado com sucesso.\n");

        printf("Cadastrar outro livro? (s/n): ");
        if (!confirma("")) break;
    }

    int cont = 0;
    for (int i = 0; i < tamanho; i++) if (acervo[i].codigo != -1) cont++;
    printf("Total de livros atualmente no acervo: %d/%d\n", cont, tamanho);
}

/* Imprime todos os livros cadastrados (pula posicoes vazias) */
void imprimirLivros(Livro acervo[], int tamanho) {
    int encontrou = 0;
    printf("\n=== Lista de livros ===\n");
    for (int i = 0; i < tamanho; i++) {
        if (acervo[i].codigo != -1) {
            encontrou = 1;
            printf("-------------------------------\n");
            printf("Codigo : %d\n", acervo[i].codigo);
            printf("Titulo : %s\n", acervo[i].titulo);
            printf("Autor  : %s\n", acervo[i].autor);
            printf("Area   : %s\n", acervo[i].area);
            printf("Ano    : %d\n", acervo[i].ano);
            printf("Editora: %s\n", acervo[i].editora);
        }
    }
    if (!encontrou) printf("Nenhum livro cadastrado.\n");
}

/* Pesquisa por codigo (retorna e imprime se encontrado) */
void pesquisarLivro(Livro acervo[], int tamanho, int codigoBusca) {
    for (int i = 0; i < tamanho; i++) {
        if (acervo[i].codigo == codigoBusca) {
            printf("\nLivro encontrado:\n");
            printf("Codigo : %d\n", acervo[i].codigo);
            printf("Titulo : %s\n", acervo[i].titulo);
            printf("Autor  : %s\n", acervo[i].autor);
            printf("Area   : %s\n", acervo[i].area);
            printf("Ano    : %d\n", acervo[i].ano);
            printf("Editora: %s\n", acervo[i].editora);
            return;
        }
    }
    printf("Livro com codigo %d nao encontrado.\n", codigoBusca);
}

/* Ordena livros por ano usando Bubble Sort (pula posicoes vazias) */
void ordenarLivros(Livro acervo[], int tamanho) {
    Livro tempLista[TAMANHO_ACERVO];
    int n = 0;
    for (int i = 0; i < tamanho; i++) {
        if (acervo[i].codigo != -1) tempLista[n++] = acervo[i];
    }
    if (n <= 1) return;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (tempLista[j].ano > tempLista[j+1].ano) {
                Livro tmp = tempLista[j];
                tempLista[j] = tempLista[j+1];
                tempLista[j+1] = tmp;
            }
        }
    }
    int i;
    for (i = 0; i < n; i++) acervo[i] = tempLista[i];
    for (; i < tamanho; i++) acervo[i].codigo = -1;
}

/* Utility: consome o resto da linha (limpa buffer de stdin) */
void consumirRestoLinhas(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
}

/* Lê um inteiro para o campo 'ano' com validação (1500..2100). Retorna -1 se falhar. */
int lerInteiroComValidacao(const char *prompt) {
    int valor;
    int tentativas = 0;
    while (tentativas < 3) {
        printf("%s", prompt);
        if (scanf("%d", &valor) != 1) {
            consumirRestoLinhas();
            printf("Entrada invalida. Digite um numero inteiro.\n");
            tentativas++;
            continue;
        }
        consumirRestoLinhas();
        if (valor >= 1500 && valor <= 2100) return valor;
        printf("Ano fora do intervalo permitido (1500-2100). Tente novamente.\n");
        tentativas++;
    }
    return -1;
}

/* Pergunta simples de confirmacao: devolve 1 para 's'/'S', 0 para qualquer outro */
int confirma(const char *prompt) {
    if (prompt && prompt[0] != '\0') printf("%s", prompt);
    int c = getchar();
    consumirRestoLinhas();
    return (c == 's' || c == 'S');
}

/* Verifica existencia de arquivo (retorna 1 se existe) */
int arquivoExiste(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}
