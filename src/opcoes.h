#ifndef OPCOES_H
#define OPCOES_H

#include <random>
#include <string>
#include "dicionario.h"
#include "quebra.h"

struct Base {
    std::string texto;
    std::string textoMinusculo;
    std::string outrosLivros;
    Dicionario dicionario;
    ModeloQuadrigramas modelo;
    std::mt19937 gerador;
};

void buscarUmaPalavra(const Base& base);

void buscarVariasPalavras(const Base& base);

void compararDesempenho(const Base& base);

void cifrarOuDecifrarMensagem(Base& base);

void cifrarArquivo(Base& base);

void quebrarCifraDeArquivo(Base& base);

void marcarErrosOrtograficos(const Base& base);

void comprimirArquivo();

#endif
