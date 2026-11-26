#ifndef KMP_H
#define KMP_H

#include <string>
#include <vector>

struct BuscaKmp {
    std::string padrao;
    std::vector<int> falha;
    size_t posicao;
    int casados;
};

std::vector<int> calcularFalhas(const std::string& padrao);

BuscaKmp iniciarBuscaKmp(const std::string& padrao);

int proximaOcorrencia(BuscaKmp& busca, const std::string& texto);

std::vector<int> buscarTodasKmp(const std::string& texto, const std::string& padrao);

#endif
