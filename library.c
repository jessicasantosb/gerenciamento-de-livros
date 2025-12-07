#include <stdio.h>
#include <string.h>
#include "library.h"

// Inicializa o acervo marcando todas as posições como vazias (codigo = -1)
void inicializarAcervo(Livro acervo[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        acervo[i].codigo = -1;
    }
}

// Carrega o acervo de um arquivo CSV; formato: codigo,titulo,autor,area,ano,editora. 
void carregarAcervoCSV(Livro acervo[], int tamanho, const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        printf("Nenhum arquivo %s encontrado. ⏳ Iniciando acervo vazio.\n", nomeArquivo);
        return;
    }

    char linha[256];
    int idx = 0;

    // Tenta ler e descartar a primeira linha (cabeçalho opcional)
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fclose(f);
        printf("⚠️ Arquivo CSV vazio.\n");
        return;
    }
    
    // Se a primeira linha não parecer um código, assume-se que é o cabeçalho
    if (strstr(linha, "codigo") == NULL && strstr(linha, "Codigo") == NULL) {
        // Se a primeira linha não for o cabeçalho, volta para o início do arquivo para lê-la como registro
        rewind(f);
    }

    while (idx < tamanho && fgets(linha, sizeof(linha), f) != NULL) {
        // Remove newline se presente
        linha[strcspn(linha, "\n")] = '\0';
        
        // Parse da linha CSV para o struct Livro
        Livro L;       
        int lidos = sscanf(
            linha, 
            "%d,%49[^,],%29[^,],%29[^,],%d,%29[^\n]",
            &L.codigo, L.titulo, L.autor, L.area, &L.ano, L.editora
        );

        // Verifica se leu todos os campos necessários
        if (lidos == 6) {
             acervo[idx++] = L;
        } else if (lidos == 5) {
             // Caso a editora esteja vazia (fim de linha após 'ano,')
             strcpy(L.editora, "");
             acervo[idx++] = L;
        }
    }
    
    // Marca o restante como vazio
    for (int i = idx; i < tamanho; i++) acervo[i].codigo = -1;
    printf("Carregados %d registro(s) de %s.\n", idx, nomeArquivo);
    fclose(f);
}

// Salva o acervo em um arquivo CSV; formato: codigo,titulo,autor,area,ano,editora.
void salvarAcervoCSV(Livro acervo[], int tamanho, const char *nomeArquivo) {
    // Modo "w" sobrescreve o arquivo sem perguntar.
    FILE *f = fopen(nomeArquivo, "w");
    if (!f) {
        perror("⚠️ Erro ao abrir arquivo CSV para escrita");
        return;
    }

    // Escreve o cabeçalho
    fprintf(f, "codigo,titulo,autor,area,ano,editora\n");
    
    for (int i = 0; i < tamanho; i++) {
        if (acervo[i].codigo != -1) {
            // Imprime o registro no formato CSV
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
}

// Cadastra novos livros no acervo
void cadastrarLivros(Livro acervo[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (acervo[i].codigo != -1) continue; // Pula posições já ocupadas

        printf("\n--- Cadastro (posição %d de %d) ---\n", i + 1, tamanho);

        int codigo;
        // Laço simples para garantir entrada de um número (simplificado)
        printf("Código (número): ");
        if (scanf("%d", &codigo) != 1) {
            printf("⚠️ Entrada inválida. Abortando cadastro.\n");
            consumirRestoLinhas();
            break;
        }
        consumirRestoLinhas();

        // Verifica se o código já existe no acervo
        int existe = 0;
        for (int k = 0; k < tamanho; k++) {
            if (acervo[k].codigo == codigo) { existe = 1; break; }
        }
        if (existe) {
            printf("😟 Já existe um livro com esse código. Tente novamente.\n");
            i--; // Tenta a mesma posição novamente com um novo código
            continue;
        }

        // Preenche o restante dos campos do livro
        acervo[i].codigo = codigo;

        printf("Título (até 49 chars): ");
        // Obtém a linha inteira, incluindo espaços
        if (!fgets(acervo[i].titulo, sizeof(acervo[i].titulo), stdin)) acervo[i].titulo[0] = '\0';
        // Remove newline se presente
        acervo[i].titulo[strcspn(acervo[i].titulo, "\n")] = '\0';

        printf("Autor (até 29 chars): ");
        if (!fgets(acervo[i].autor, sizeof(acervo[i].autor), stdin)) acervo[i].autor[0] = '\0';
        acervo[i].autor[strcspn(acervo[i].autor, "\n")] = '\0';

        printf("Área (até 29 chars): ");
        if (!fgets(acervo[i].area, sizeof(acervo[i].area), stdin)) acervo[i].area[0] = '\0';
        acervo[i].area[strcspn(acervo[i].area, "\n")] = '\0';

        printf("Ano de publicaçãoo (número): ");
        if (scanf("%d", &acervo[i].ano) != 1) acervo[i].ano = 0;
        consumirRestoLinhas();

        printf("Editora (até 29 chars): ");
        if (!fgets(acervo[i].editora, sizeof(acervo[i].editora), stdin)) acervo[i].editora[0] = '\0';
        acervo[i].editora[strcspn(acervo[i].editora, "\n")] = '\0';

        printf("✅ Livro cadastrado com sucesso.\n");

        printf("😊 Cadastrar outro livro? (s/n): ");
        if (!confirma("")) break;
    }

    int cont = 0;
    // Conta quantos livros estão cadastrados
    for (int i = 0; i < tamanho; i++) if (acervo[i].codigo != -1) cont++;
    printf("📚 Total de livros atualmente no acervo: %d/%d\n", cont, tamanho);
}

// Imprime todos os livros cadastrados
void imprimirLivros(Livro acervo[], int tamanho) {
    int encontrou = 0;
    printf("==================================================================");
    printf("\n                      📚 Lista de livros 📚\n\n");
    for (int i = 0; i < tamanho; i++) {
        // Imprime apenas livros cadastrados
        if (acervo[i].codigo != -1) {
            encontrou = 1;
            printf("Codigo : %d\n", acervo[i].codigo);
            printf("Titulo : %s\n", acervo[i].titulo);
            printf("Autor  : %s\n", acervo[i].autor);
            printf("Area   : %s\n", acervo[i].area);
            printf("Ano    : %d\n", acervo[i].ano);
            printf("Editora: %s\n", acervo[i].editora);
        }
    }
    if (!encontrou) printf("⚠️ Nenhum livro cadastrado.\n");
}

// Pesquisa um livro pelo código, verifica apenas livros cadastrados
void pesquisarLivro(Livro acervo[], int tamanho, int codigoBusca) {
    for (int i = 0; i < tamanho; i++) {
        // Imprime os detalhes do livro encontrado
        if (acervo[i].codigo == codigoBusca) {
            printf("\nLivro encontrado:\n");
            printf("Código : %d\n", acervo[i].codigo);
            printf("Título : %s\n", acervo[i].titulo);
            printf("Autor  : %s\n", acervo[i].autor);
            printf("Área   : %s\n", acervo[i].area);
            printf("Ano    : %d\n", acervo[i].ano);
            printf("Editora: %s\n", acervo[i].editora);
            return;
        }
    }
    printf("😕 Livro com código %d não encontrado.\n", codigoBusca);
}

// Ordena os livros por ano de publicação (crescente) usando Bubble Sort
void ordenarLivros(Livro acervo[], int tamanho) {
    Livro tempLista[TAMANHO_ACERVO];
    int n = 0;
    // Copia apenas os livros cadastrados para uma lista temporária
    for (int i = 0; i < tamanho; i++) {
        if (acervo[i].codigo != -1) tempLista[n++] = acervo[i];
    }
    // Se houver 0 ou 1 livro, nada a ordenar
    if (n <= 1) return;
    
    // Bubble Sort simples na lista temporária
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            // Compara anos de publicação
            if (tempLista[j].ano > tempLista[j+1].ano) {
                // Realiza a troca, temporáriamente dopando o struct Livro               
                Livro tmp = tempLista[j];
                tempLista[j] = tempLista[j+1];
                tempLista[j+1] = tmp;
            }
        }
    }
    
    // Devolve para o acervo original e marca o restante como vazio
    int i;
    for (i = 0; i < n; i++) acervo[i] = tempLista[i];
    for (; i < tamanho; i++) acervo[i].codigo = -1;
    printf("😊 Acervo ordenado por ano de publicação.\n");
}

// Descarta o restante da linha no buffer de entrada
void consumirRestoLinhas(void) {
    int c;
    // Realiza a leitura até o fim da linha ou EOF e ignora os caracteres
    while ((c = getchar()) != '\n' && c != EOF);
}
      

// Função de confirmação simples (s/n)
int confirma(const char *prompt) {
    // Exibe o prompt se fornecido, e lê um caractere
    if (prompt && prompt[0] != '\0') printf("%s", prompt);
    int c = getchar();
    // Retorna 1 para 's' ou 'S', 0 caso contrário
    consumirRestoLinhas();
    return (c == 's' || c == 'S');
}