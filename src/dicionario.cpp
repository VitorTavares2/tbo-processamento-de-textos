#include "dicionario.h"
#include "texto.h"

const int DISTANCIA_MAXIMA = 2;
const size_t QUANTIDADE_DE_SUGESTOES = 3;

std::vector<PalavraNoTexto> separarPalavras(const std::string& texto) {
    std::vector<PalavraNoTexto> palavras;
    size_t i = 0;
    while (i < texto.size()) {
        if (!ehLetra(texto[i])) {
            i++;
            continue;
        }
        size_t inicio = i;
        while (i < texto.size() && ehLetra(texto[i])) {
            i++;
        }
        PalavraNoTexto encontrada;
        encontrada.palavra = minusculas(texto.substr(inicio, i - inicio));
        encontrada.posicao = static_cast<int>(inicio);
        palavras.push_back(encontrada);
    }
    return palavras;
}

void intercalar(std::vector<std::string>& palavras, std::vector<std::string>& auxiliar, size_t inicio, size_t meio, size_t fim) {
    size_t esquerda = inicio;
    size_t direita = meio;
    size_t destino = inicio;
    while (esquerda < meio && direita < fim) {
        if (palavras[direita] < palavras[esquerda]) {
            auxiliar[destino++] = palavras[direita++];
        } else {
            auxiliar[destino++] = palavras[esquerda++];
        }
    }
    while (esquerda < meio) {
        auxiliar[destino++] = palavras[esquerda++];
    }
    while (direita < fim) {
        auxiliar[destino++] = palavras[direita++];
    }
    for (size_t i = inicio; i < fim; i++) {
        palavras[i] = auxiliar[i];
    }
}

void ordenarPalavras(std::vector<std::string>& palavras) {
    std::vector<std::string> auxiliar(palavras.size());
    for (size_t largura = 1; largura < palavras.size(); largura *= 2) {
        for (size_t inicio = 0; inicio + largura < palavras.size(); inicio += 2 * largura) {
            size_t meio = inicio + largura;
            size_t fim = meio + largura < palavras.size() ? meio + largura : palavras.size();
            intercalar(palavras, auxiliar, inicio, meio, fim);
        }
    }
}

int menorDeTres(int a, int b, int c) {
    int menor = a < b ? a : b;
    return menor < c ? menor : c;
}

int distanciaDeEdicao(const std::string& primeira, const std::string& segunda, int limite) {
    std::vector<int> anterior(segunda.size() + 1);
    std::vector<int> atual(segunda.size() + 1);
    for (size_t j = 0; j <= segunda.size(); j++) {
        anterior[j] = static_cast<int>(j);
    }
    for (size_t i = 1; i <= primeira.size(); i++) {
        atual[0] = static_cast<int>(i);
        int menorDaLinha = atual[0];
        for (size_t j = 1; j <= segunda.size(); j++) {
            int troca = primeira[i - 1] == segunda[j - 1] ? 0 : 1;
            atual[j] = menorDeTres(anterior[j] + 1, atual[j - 1] + 1, anterior[j - 1] + troca);
            if (atual[j] < menorDaLinha) {
                menorDaLinha = atual[j];
            }
        }
        if (menorDaLinha > limite) {
            return limite + 1;
        }
        anterior.swap(atual);
    }
    return anterior[segunda.size()];
}

void Dicionario::carregar(const std::string& conteudo) {
    std::vector<std::string> todas;
    for (const std::string& linha : dividir(conteudo, '\n')) {
        std::string palavra = minusculas(aparar(linha));
        if (!palavra.empty()) {
            todas.push_back(palavra);
        }
    }
    ordenarPalavras(todas);

    lista.clear();
    for (const std::string& palavra : todas) {
        if (lista.empty() || lista.back() != palavra) {
            lista.push_back(palavra);
        }
    }
    frequencias.assign(lista.size(), 0);
}

int Dicionario::posicao(const std::string& palavra) const {
    int inicio = 0;
    int fim = static_cast<int>(lista.size()) - 1;
    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        if (lista[meio] == palavra) {
            return meio;
        }
        if (lista[meio] < palavra) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    return -1;
}

bool Dicionario::contem(const std::string& palavra) const {
    return posicao(palavra) != -1;
}

void Dicionario::contarFrequencias(const std::string& texto) {
    for (const PalavraNoTexto& encontrada : separarPalavras(texto)) {
        int indice = posicao(encontrada.palavra);
        if (indice != -1) {
            frequencias[indice]++;
        }
    }
}

bool vemAntes(const Sugestao& primeira, const Sugestao& segunda) {
    if (primeira.distancia != segunda.distancia) {
        return primeira.distancia < segunda.distancia;
    }
    return primeira.frequencia > segunda.frequencia;
}

std::vector<Sugestao> Dicionario::sugerir(const std::string& palavra) const {
    std::vector<Sugestao> melhores;
    int tamanho = static_cast<int>(palavra.size());
    for (size_t i = 0; i < lista.size(); i++) {
        int diferenca = static_cast<int>(lista[i].size()) - tamanho;
        if (diferenca > DISTANCIA_MAXIMA || diferenca < -DISTANCIA_MAXIMA) {
            continue;
        }
        int distancia = distanciaDeEdicao(palavra, lista[i], DISTANCIA_MAXIMA);
        if (distancia > DISTANCIA_MAXIMA) {
            continue;
        }

        Sugestao candidata;
        candidata.palavra = lista[i];
        candidata.distancia = distancia;
        candidata.frequencia = frequencias[i];

        if (melhores.size() < QUANTIDADE_DE_SUGESTOES) {
            melhores.push_back(candidata);
        } else if (vemAntes(candidata, melhores.back())) {
            melhores.back() = candidata;
        } else {
            continue;
        }
        size_t j = melhores.size() - 1;
        while (j > 0 && vemAntes(melhores[j], melhores[j - 1])) {
            Sugestao guardada = melhores[j];
            melhores[j] = melhores[j - 1];
            melhores[j - 1] = guardada;
            j--;
        }
    }
    return melhores;
}

size_t Dicionario::tamanho() const {
    return lista.size();
}

const std::vector<std::string>& Dicionario::palavras() const {
    return lista;
}
