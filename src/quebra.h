#ifndef QUEBRA_H
#define QUEBRA_H

#include <random>
#include <string>
#include <vector>

std::vector<int> extrairLetras(const std::string& textoNormalizado, size_t limite);

std::string ordenarPorFrequencia(const std::vector<long long>& contagem);

class ModeloQuadrigramas {
public:
    void treinar(const std::string& texto);
    double pontuar(const std::vector<int>& cifradas, const int* mapa) const;
    std::string letrasPorFrequencia() const;
    long long letrasDeTreino() const;

private:
    std::vector<double> logProbabilidades;
    std::vector<long long> contagemLetras;
    long long totalLetras;
};

struct ResultadoQuebra {
    std::string chave;
    double pontuacao;
    int letrasUsadas;
    int trocasAceitas;
};

ResultadoQuebra quebrarCifra(const std::string& cifrado, const ModeloQuadrigramas& modelo, std::mt19937& gerador);

#endif
