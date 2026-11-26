#include "texto.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string lerArquivo(const std::string& caminho) {
    std::ifstream arquivo(caminho, std::ios::binary);
    if (!arquivo) {
        throw std::runtime_error("nao foi possivel abrir " + caminho);
    }
    std::ostringstream conteudo;
    conteudo << arquivo.rdbuf();
    return conteudo.str();
}

void gravarArquivo(const std::string& caminho, const std::string& conteudo) {
    std::ofstream arquivo(caminho, std::ios::binary);
    if (!arquivo) {
        throw std::runtime_error("nao foi possivel gravar " + caminho);
    }
    arquivo << conteudo;
}

bool arquivoExiste(const std::string& caminho) {
    std::ifstream arquivo(caminho, std::ios::binary);
    return static_cast<bool>(arquivo);
}

bool ehContinuacao(unsigned char byte) {
    return byte >= 0x80 && byte <= 0xBF;
}

char pontuacaoSimples(unsigned char terceiroByte) {
    if (terceiroByte == 0x93 || terceiroByte == 0x94) {
        return '-';
    }
    if (terceiroByte == 0x98 || terceiroByte == 0x99) {
        return '\'';
    }
    if (terceiroByte == 0x9C || terceiroByte == 0x9D) {
        return '"';
    }
    return '?';
}

std::string utf8ParaLatin1(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.size());
    size_t i = 0;
    while (i < texto.size()) {
        unsigned char byte = static_cast<unsigned char>(texto[i]);
        bool temSegundo = i + 1 < texto.size() && ehContinuacao(static_cast<unsigned char>(texto[i + 1]));
        bool temTerceiro = i + 2 < texto.size() && ehContinuacao(static_cast<unsigned char>(texto[i + 2]));

        if (byte < 0x80) {
            resultado += static_cast<char>(byte);
            i++;
        } else if ((byte == 0xC2 || byte == 0xC3) && temSegundo) {
            unsigned char segundo = static_cast<unsigned char>(texto[i + 1]);
            resultado += static_cast<char>(byte == 0xC2 ? segundo : segundo + 0x40);
            i += 2;
        } else if (byte >= 0xE0 && byte <= 0xEF && temSegundo && temTerceiro) {
            resultado += pontuacaoSimples(static_cast<unsigned char>(texto[i + 2]));
            i += 3;
        } else {
            resultado += static_cast<char>(byte);
            i++;
        }
    }
    return resultado;
}

std::string latin1ParaUtf8(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.size() + texto.size() / 8);
    for (char caractere : texto) {
        unsigned char byte = static_cast<unsigned char>(caractere);
        if (byte < 0x80) {
            resultado += caractere;
        } else {
            resultado += static_cast<char>(0xC0 | (byte >> 6));
            resultado += static_cast<char>(0x80 | (byte & 0x3F));
        }
    }
    return resultado;
}

std::string minusculas(std::string texto) {
    for (char& caractere : texto) {
        unsigned char byte = static_cast<unsigned char>(caractere);
        bool maiusculaSimples = byte >= 'A' && byte <= 'Z';
        bool maiusculaAcentuada = byte >= 0xC0 && byte <= 0xDE && byte != 0xD7;
        if (maiusculaSimples || maiusculaAcentuada) {
            caractere = static_cast<char>(byte + 32);
        }
    }
    return texto;
}

std::string semAcentos(std::string texto) {
    const std::string LETRA_BASE = "aaaaaaaceeeeiiii?nooooo?ouuuuy?y";
    for (char& caractere : texto) {
        unsigned char byte = static_cast<unsigned char>(caractere);
        if (byte >= 0xE0 && LETRA_BASE[byte - 0xE0] != '?') {
            caractere = LETRA_BASE[byte - 0xE0];
        }
    }
    return texto;
}

bool ehLetra(char caractere) {
    unsigned char byte = static_cast<unsigned char>(caractere);
    if ((byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z')) {
        return true;
    }
    return byte >= 0xC0 && byte != 0xD7 && byte != 0xF7;
}

std::vector<std::string> dividir(const std::string& texto, char delimitador) {
    std::vector<std::string> partes;
    std::string atual;
    for (char caractere : texto) {
        if (caractere == delimitador) {
            partes.push_back(atual);
            atual.clear();
        } else {
            atual += caractere;
        }
    }
    partes.push_back(atual);
    return partes;
}

bool ehEspaco(char caractere) {
    return caractere == ' ' || caractere == '\r' || caractere == '\n' || caractere == '\t';
}

std::string aparar(const std::string& texto) {
    size_t inicio = 0;
    size_t fim = texto.size();
    while (inicio < fim && ehEspaco(texto[inicio])) {
        inicio++;
    }
    while (fim > inicio && ehEspaco(texto[fim - 1])) {
        fim--;
    }
    return texto.substr(inicio, fim - inicio);
}

bool converterInteiro(const std::string& texto, int& valor) {
    const size_t MAXIMO_DE_DIGITOS = 9;
    std::string limpo = aparar(texto);
    if (limpo.empty() || limpo.size() > MAXIMO_DE_DIGITOS) {
        return false;
    }
    int resultado = 0;
    for (char caractere : limpo) {
        if (caractere < '0' || caractere > '9') {
            return false;
        }
        resultado = resultado * 10 + (caractere - '0');
    }
    valor = resultado;
    return true;
}

std::string formatarNumero(long long numero) {
    std::string digitos = std::to_string(numero < 0 ? -numero : numero);
    std::string resultado;
    int contador = 0;
    for (size_t i = digitos.size(); i > 0; i--) {
        if (contador > 0 && contador % 3 == 0) {
            resultado = "." + resultado;
        }
        resultado = digitos[i - 1] + resultado;
        contador++;
    }
    return numero < 0 ? "-" + resultado : resultado;
}
