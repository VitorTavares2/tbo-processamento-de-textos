#include "ingenua.h"

std::vector<int> buscarTodasIngenua(const std::string& texto, const std::string& padrao) {
    std::vector<int> ocorrencias;
    if (padrao.empty() || padrao.size() > texto.size()) {
        return ocorrencias;
    }
    for (size_t inicio = 0; inicio + padrao.size() <= texto.size(); inicio++) {
        size_t j = 0;
        while (j < padrao.size() && texto[inicio + j] == padrao[j]) {
            j++;
        }
        if (j == padrao.size()) {
            ocorrencias.push_back(static_cast<int>(inicio));
        }
    }
    return ocorrencias;
}
