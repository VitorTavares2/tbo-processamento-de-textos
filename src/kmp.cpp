#include "kmp.h"

std::vector<int> calcularFalhas(const std::string& padrao) {
    std::vector<int> falha(padrao.size(), 0);
    int tamanho = 0;
    for (size_t i = 1; i < padrao.size(); i++) {
        while (tamanho > 0 && padrao[i] != padrao[tamanho]) {
            tamanho = falha[tamanho - 1];
        }
        if (padrao[i] == padrao[tamanho]) {
            tamanho++;
        }
        falha[i] = tamanho;
    }
    return falha;
}

BuscaKmp iniciarBuscaKmp(const std::string& padrao) {
    BuscaKmp busca;
    busca.padrao = padrao;
    busca.falha = calcularFalhas(padrao);
    busca.posicao = 0;
    busca.casados = 0;
    return busca;
}

int proximaOcorrencia(BuscaKmp& busca, const std::string& texto) {
    int tamanhoPadrao = static_cast<int>(busca.padrao.size());
    while (busca.posicao < texto.size()) {
        char letra = texto[busca.posicao];
        while (busca.casados > 0 && letra != busca.padrao[busca.casados]) {
            busca.casados = busca.falha[busca.casados - 1];
        }
        if (letra == busca.padrao[busca.casados]) {
            busca.casados++;
        }
        busca.posicao++;
        if (busca.casados == tamanhoPadrao) {
            busca.casados = busca.falha[tamanhoPadrao - 1];
            return static_cast<int>(busca.posicao) - tamanhoPadrao;
        }
    }
    return -1;
}

std::vector<int> buscarTodasKmp(const std::string& texto, const std::string& padrao) {
    std::vector<int> ocorrencias;
    if (padrao.empty()) {
        return ocorrencias;
    }
    BuscaKmp busca = iniciarBuscaKmp(padrao);
    int posicao = proximaOcorrencia(busca, texto);
    while (posicao != -1) {
        ocorrencias.push_back(posicao);
        posicao = proximaOcorrencia(busca, texto);
    }
    return ocorrencias;
}
