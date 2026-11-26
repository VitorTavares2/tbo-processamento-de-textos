#ifndef TEXTO_H
#define TEXTO_H

#include <string>
#include <vector>

std::string lerArquivo(const std::string& caminho);

void gravarArquivo(const std::string& caminho, const std::string& conteudo);

bool arquivoExiste(const std::string& caminho);

std::string utf8ParaLatin1(const std::string& texto);

std::string latin1ParaUtf8(const std::string& texto);

std::string minusculas(std::string texto);

std::string semAcentos(std::string texto);

bool ehLetra(char caractere);

std::vector<std::string> dividir(const std::string& texto, char delimitador);

std::string aparar(const std::string& texto);

bool converterInteiro(const std::string& texto, int& valor);

std::string formatarNumero(long long numero);

#endif
