#ifndef AHOCORASICK_H
#define AHOCORASICK_H

#include <string>
#include <vector>

struct OcorrenciaAho {
    int posicao;
    int padrao;
};

class AhoCorasick {
public:
    AhoCorasick();
    void adicionar(const std::string& padrao);
    void montar();
    std::vector<OcorrenciaAho> buscar(const std::string& texto) const;
    long long contar(const std::string& texto) const;
    int quantidadeEstados() const;
    int tamanhoDoPadrao(int padrao) const;

private:
    std::vector<int> proximo;
    std::vector<int> falha;
    std::vector<std::vector<int>> saidas;
    std::vector<int> tamanhos;
    int criarEstado();
};

#endif
