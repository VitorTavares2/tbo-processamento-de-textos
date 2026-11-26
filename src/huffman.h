#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <vector>

const int TOTAL_SIMBOLOS = 256;

struct NoHuffman {
    long long peso;
    int simbolo;
    int esquerda;
    int direita;
};

struct ArvoreHuffman {
    std::vector<NoHuffman> nos;
    int raiz;
};

std::vector<long long> contarSimbolos(const std::string& dados);

ArvoreHuffman montarArvoreHuffman(const std::vector<long long>& frequencias);

std::vector<std::string> gerarCodigos(const ArvoreHuffman& arvore);

std::string comprimir(const std::string& dados);

std::string descomprimir(const std::string& comprimido);

#endif
