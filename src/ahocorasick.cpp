#include "ahocorasick.h"

const int SIMBOLOS = 256;

AhoCorasick::AhoCorasick() {
    criarEstado();
}

int AhoCorasick::criarEstado() {
    for (int simbolo = 0; simbolo < SIMBOLOS; simbolo++) {
        proximo.push_back(-1);
    }
    falha.push_back(0);
    saidas.push_back(std::vector<int>());
    return static_cast<int>(falha.size()) - 1;
}

void AhoCorasick::adicionar(const std::string& padrao) {
    int estado = 0;
    for (char caractere : padrao) {
        int simbolo = static_cast<unsigned char>(caractere);
        if (proximo[estado * SIMBOLOS + simbolo] == -1) {
            int novo = criarEstado();
            proximo[estado * SIMBOLOS + simbolo] = novo;
        }
        estado = proximo[estado * SIMBOLOS + simbolo];
    }
    saidas[estado].push_back(static_cast<int>(tamanhos.size()));
    tamanhos.push_back(static_cast<int>(padrao.size()));
}

void AhoCorasick::montar() {
    std::vector<int> fila;
    for (int simbolo = 0; simbolo < SIMBOLOS; simbolo++) {
        int filho = proximo[simbolo];
        if (filho == -1) {
            proximo[simbolo] = 0;
        } else {
            falha[filho] = 0;
            fila.push_back(filho);
        }
    }

    size_t frente = 0;
    while (frente < fila.size()) {
        int estado = fila[frente];
        frente++;
        for (int simbolo = 0; simbolo < SIMBOLOS; simbolo++) {
            int filho = proximo[estado * SIMBOLOS + simbolo];
            int destinoDaFalha = proximo[falha[estado] * SIMBOLOS + simbolo];
            if (filho == -1) {
                proximo[estado * SIMBOLOS + simbolo] = destinoDaFalha;
            } else {
                falha[filho] = destinoDaFalha;
                for (int padrao : saidas[destinoDaFalha]) {
                    saidas[filho].push_back(padrao);
                }
                fila.push_back(filho);
            }
        }
    }
}

std::vector<OcorrenciaAho> AhoCorasick::buscar(const std::string& texto) const {
    std::vector<OcorrenciaAho> ocorrencias;
    int estado = 0;
    for (size_t i = 0; i < texto.size(); i++) {
        estado = proximo[estado * SIMBOLOS + static_cast<unsigned char>(texto[i])];
        for (int padrao : saidas[estado]) {
            OcorrenciaAho ocorrencia;
            ocorrencia.posicao = static_cast<int>(i) - tamanhos[padrao] + 1;
            ocorrencia.padrao = padrao;
            ocorrencias.push_back(ocorrencia);
        }
    }
    return ocorrencias;
}

long long AhoCorasick::contar(const std::string& texto) const {
    long long total = 0;
    int estado = 0;
    for (size_t i = 0; i < texto.size(); i++) {
        estado = proximo[estado * SIMBOLOS + static_cast<unsigned char>(texto[i])];
        total += static_cast<long long>(saidas[estado].size());
    }
    return total;
}

int AhoCorasick::quantidadeEstados() const {
    return static_cast<int>(falha.size());
}

int AhoCorasick::tamanhoDoPadrao(int padrao) const {
    return tamanhos[padrao];
}
