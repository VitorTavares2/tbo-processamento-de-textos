#include <chrono>
#include <iostream>
#include <stdexcept>
#include "entrada.h"
#include "opcoes.h"
#include "texto.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    auto inicio = std::chrono::steady_clock::now();

    Base base;
    std::string conteudoDicionario;
    try {
        base.texto = utf8ParaLatin1(lerArquivo("dados/dom_casmurro.txt"));
        base.outrosLivros = utf8ParaLatin1(lerArquivo("dados/outros_livros.txt"));
        conteudoDicionario = utf8ParaLatin1(lerArquivo("dados/dicionario.txt"));
    } catch (const std::runtime_error& problema) {
        std::cout << "Erro: " << problema.what() << std::endl;
        std::cout << "Dica: execute o programa de dentro da pasta do projeto, onde existe a pasta dados." << std::endl;
        return 1;
    }
    base.textoMinusculo = minusculas(base.texto);
    base.dicionario.carregar(conteudoDicionario);
    base.dicionario.contarFrequencias(base.texto + "\n" + base.outrosLivros);
    base.modelo.treinar(base.outrosLivros);
    std::random_device semente;
    base.gerador.seed(semente());

    double segundos = std::chrono::duration<double>(std::chrono::steady_clock::now() - inicio).count();

    std::cout << "Texto principal (Dom Casmurro): " << formatarNumero(static_cast<long long>(base.texto.size())) << " caracteres" << std::endl;
    std::cout << "Outros livros: " << formatarNumero(static_cast<long long>(base.outrosLivros.size())) << " caracteres" << std::endl;
    std::cout << "Dicionario: " << formatarNumero(static_cast<long long>(base.dicionario.tamanho())) << " palavras, ordenadas" << std::endl;
    std::cout << "Modelo de quadrigramas treinado com " << formatarNumero(base.modelo.letrasDeTreino()) << " letras" << std::endl;
    std::cout << "Tempo de carregamento: " << segundos << " s" << std::endl;

    int opcao = -1;
    while (opcao != 0) {
        std::cout << std::endl;
        std::cout << "1 - Buscar uma palavra, uma ocorrencia por vez (KMP, aceita coringa *)" << std::endl;
        std::cout << "2 - Buscar varias palavras ao mesmo tempo (Aho-Corasick)" << std::endl;
        std::cout << "3 - Comparar desempenho: ingenua x KMP x Aho-Corasick" << std::endl;
        std::cout << "4 - Cifrar ou decifrar uma mensagem com chave" << std::endl;
        std::cout << "5 - Cifrar um arquivo" << std::endl;
        std::cout << "6 - Quebrar a cifra de um arquivo, sem a chave" << std::endl;
        std::cout << "7 - Marcar erros ortograficos e sugerir correcoes" << std::endl;
        std::cout << "8 - Comprimir e descomprimir um arquivo (Huffman)" << std::endl;
        std::cout << "0 - Sair" << std::endl;
        if (!lerInteiro("Escolha uma opcao: ", opcao, 0, 8)) {
            std::cout << "Entrada encerrada. Ate logo." << std::endl;
            break;
        }

        try {
            if (opcao == 1) {
                buscarUmaPalavra(base);
            } else if (opcao == 2) {
                buscarVariasPalavras(base);
            } else if (opcao == 3) {
                compararDesempenho(base);
            } else if (opcao == 4) {
                cifrarOuDecifrarMensagem(base);
            } else if (opcao == 5) {
                cifrarArquivo(base);
            } else if (opcao == 6) {
                quebrarCifraDeArquivo(base);
            } else if (opcao == 7) {
                marcarErrosOrtograficos(base);
            } else if (opcao == 8) {
                comprimirArquivo();
            }
        } catch (const std::runtime_error& problema) {
            std::cout << "Erro: " << problema.what() << std::endl;
        }
    }

    return 0;
}
