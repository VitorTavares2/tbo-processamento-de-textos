#include "cifra.h"
#include "texto.h"

std::string normalizarParaCifra(const std::string& texto) {
    return semAcentos(minusculas(texto));
}

bool chaveValida(const std::string& chave) {
    if (chave.size() != TAMANHO_ALFABETO) {
        return false;
    }
    bool usada[TAMANHO_ALFABETO] = {false};
    for (char letra : chave) {
        if (letra < 'a' || letra > 'z' || usada[letra - 'a']) {
            return false;
        }
        usada[letra - 'a'] = true;
    }
    return true;
}

std::string gerarChave(std::mt19937& gerador) {
    std::string chave = "abcdefghijklmnopqrstuvwxyz";
    for (int i = TAMANHO_ALFABETO - 1; i > 0; i--) {
        std::uniform_int_distribution<int> sorteio(0, i);
        int j = sorteio(gerador);
        char guardada = chave[i];
        chave[i] = chave[j];
        chave[j] = guardada;
    }
    return chave;
}

std::string inverterChave(const std::string& chave) {
    std::string inversa(TAMANHO_ALFABETO, 'a');
    for (int i = 0; i < TAMANHO_ALFABETO; i++) {
        inversa[chave[i] - 'a'] = static_cast<char>('a' + i);
    }
    return inversa;
}

std::string cifrar(const std::string& texto, const std::string& chave) {
    std::string resultado = normalizarParaCifra(texto);
    for (char& letra : resultado) {
        if (letra >= 'a' && letra <= 'z') {
            letra = chave[letra - 'a'];
        }
    }
    return resultado;
}

std::string decifrar(const std::string& texto, const std::string& chave) {
    return cifrar(texto, inverterChave(chave));
}
