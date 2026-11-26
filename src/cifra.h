#ifndef CIFRA_H
#define CIFRA_H

#include <random>
#include <string>

const int TAMANHO_ALFABETO = 26;

std::string normalizarParaCifra(const std::string& texto);

bool chaveValida(const std::string& chave);

std::string gerarChave(std::mt19937& gerador);

std::string inverterChave(const std::string& chave);

std::string cifrar(const std::string& texto, const std::string& chave);

std::string decifrar(const std::string& texto, const std::string& chave);

#endif
