#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "library.h"

// Função principal com menu interativo
int main(void) {
    Livro acervo[TAMANHO_ACERVO];
    int opcao;
    int codigoBusca;

    inicializarAcervo(acervo, TAMANHO_ACERVO);
    carregarAcervoCSV(acervo, TAMANHO_ACERVO, ARQUIVO_CSV); // Carrega ao iniciar

    do {
        printf("==================================================================");
        printf("\n            📚 Sistema Simplificado de Livros (CSV) 📚\n\n");
        printf("1️⃣  - Cadastrar livros\n");
        printf("2️⃣  - Imprimir todos os livros\n");
        printf("3️⃣  - Pesquisar livro por código\n");
        printf("4️⃣  - Ordenar livros por ano\n");
        printf("5️⃣  - Salvar e Sair\n");
        printf("6️⃣  - Sair sem salvar\n\n");
        printf("Escolha uma opção: ➡️   ");
        if (scanf("%d", &opcao) != 1) {
            printf("⚠️ Entrada inválida.\n");
            consumirRestoLinhas();
            opcao = 0;
            continue;
        }
        consumirRestoLinhas();

        system("clear || cls"); // Limpa a tela (Linux/Windows)
        
        switch (opcao) {
            case 1:
                cadastrarLivros(acervo, TAMANHO_ACERVO);
                break;
            case 2:
                imprimirLivros(acervo, TAMANHO_ACERVO);
                break;
            case 3:
                printf("Digite o código para pesquisa: ");
                if (scanf("%d", &codigoBusca) != 1) {
                    printf("⚠️ Entrada inválida.\n");
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
                salvarAcervoCSV(acervo, TAMANHO_ACERVO, ARQUIVO_CSV);
                printf("✅ Acervo salvo em %s. Encerrando programa.\n", ARQUIVO_CSV);
                return 0;
            case 6:
                printf("Encerrando sem salvar.\n");
                return 0;
            default:
                printf("⚠️ Opção inválida. Tente novamente.\n");
        }

    } while (1);

    return 0;
}