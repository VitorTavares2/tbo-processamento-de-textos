#ifndef DICIONARIO_H
#define DICIONARIO_H

#include <string>
#include <vector>

struct PalavraNoTexto {
    std::string palavra;
    int posicao;
};

struct Sugestao {
    std::string palavra;
    int distancia;
    int frequencia;
};

std::vector<PalavraNoTexto> separarPalavras(const std::string& texto);

void ordenarPalavras(std::vector<std::string>& palavras);

int distanciaDeEdicao(const std::string& primeira, const std::string& segunda, int limite);

class Dicionario {
public:
    void carregar(const std::string& conteudo);
    bool contem(const std::string& palavra) const;
    void contarFrequencias(const std::string& texto);
    std::vector<Sugestao> sugerir(const std::string& palavra) const;
    size_t tamanho() const;
    const std::vector<std::string>& palavras() const;

private:
    std::vector<std::string> lista;
    std::vector<int> frequencias;
    int posicao(const std::string& palavra) const;
};

#endif
