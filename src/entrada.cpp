#include "entrada.h"
#include "texto.h"
#include <iostream>

bool lerLinha(const std::string& pergunta, std::string& texto) {
    std::cout << pergunta;
    std::string linha;
    if (!std::getline(std::cin, linha)) {
        std::cout << std::endl;
        return false;
    }
    texto = utf8ParaLatin1(aparar(linha));
    return true;
}

bool lerInteiro(const std::string& pergunta, int& valor, int minimo, int maximo) {
    while (true) {
        std::string texto;
        if (!lerLinha(pergunta, texto)) {
            return false;
        }
        int lido = 0;
        if (!converterInteiro(texto, lido)) {
            std::cout << "Erro: \"" << latin1ParaUtf8(texto) << "\" nao e um numero inteiro. Digite so numeros." << std::endl;
            continue;
        }
        if (lido < minimo || lido > maximo) {
            std::cout << "Erro: digite um numero entre " << minimo << " e " << maximo << "." << std::endl;
            continue;
        }
        valor = lido;
        return true;
    }
}

bool lerSimOuNao(const std::string& pergunta, bool& resposta) {
    while (true) {
        std::string texto;
        if (!lerLinha(pergunta, texto)) {
            return false;
        }
        texto = minusculas(texto);
        if (texto == "s" || texto == "n") {
            resposta = (texto == "s");
            return true;
        }
        std::cout << "Erro: digite S (sim) ou N (nao)." << std::endl;
    }
}
