#include "quebra.h"
#include "cifra.h"
#include <cmath>

const int TOTAL_QUADRIGRAMAS = TAMANHO_ALFABETO * TAMANHO_ALFABETO * TAMANHO_ALFABETO * TAMANHO_ALFABETO;
const size_t LETRAS_DA_AMOSTRA = 20000;
const int REINICIOS = 6;
const double CONTAGEM_MINIMA = 0.01;

std::vector<int> extrairLetras(const std::string& textoNormalizado, size_t limite) {
    std::vector<int> letras;
    for (char caractere : textoNormalizado) {
        if (caractere >= 'a' && caractere <= 'z') {
            letras.push_back(caractere - 'a');
            if (letras.size() == limite) {
                break;
            }
        }
    }
    return letras;
}

std::string ordenarPorFrequencia(const std::vector<long long>& contagem) {
    std::string ordem;
    for (int i = 0; i < TAMANHO_ALFABETO; i++) {
        char letra = static_cast<char>('a' + i);
        int posicao = static_cast<int>(ordem.size());
        ordem += letra;
        while (posicao > 0 && contagem[ordem[posicao - 1] - 'a'] < contagem[letra - 'a']) {
            ordem[posicao] = ordem[posicao - 1];
            posicao--;
        }
        ordem[posicao] = letra;
    }
    return ordem;
}

void ModeloQuadrigramas::treinar(const std::string& texto) {
    std::vector<int> letras = extrairLetras(normalizarParaCifra(texto), texto.size());
    std::vector<long long> contagem(TOTAL_QUADRIGRAMAS, 0);
    contagemLetras.assign(TAMANHO_ALFABETO, 0);
    totalLetras = static_cast<long long>(letras.size());

    int indice = 0;
    for (size_t i = 0; i < letras.size(); i++) {
        contagemLetras[letras[i]]++;
        indice = (indice * TAMANHO_ALFABETO + letras[i]) % TOTAL_QUADRIGRAMAS;
        if (i >= 3) {
            contagem[indice]++;
        }
    }

    double totalQuadrigramas = static_cast<double>(letras.size() >= 3 ? letras.size() - 3 : 1);
    logProbabilidades.assign(TOTAL_QUADRIGRAMAS, std::log10(CONTAGEM_MINIMA / totalQuadrigramas));
    for (int i = 0; i < TOTAL_QUADRIGRAMAS; i++) {
        if (contagem[i] > 0) {
            logProbabilidades[i] = std::log10(contagem[i] / totalQuadrigramas);
        }
    }
}

double ModeloQuadrigramas::pontuar(const std::vector<int>& cifradas,
                                   const int* mapa) const {
    double pontuacao = 0;
    int indice = 0;
    for (size_t i = 0; i < cifradas.size(); i++) {
        indice = (indice * TAMANHO_ALFABETO + mapa[cifradas[i]]) % TOTAL_QUADRIGRAMAS;
        if (i >= 3) {
            pontuacao += logProbabilidades[indice];
        }
    }
    return pontuacao;
}

std::string ModeloQuadrigramas::letrasPorFrequencia() const {
    return ordenarPorFrequencia(contagemLetras);
}

long long ModeloQuadrigramas::letrasDeTreino() const {
    return totalLetras;
}

double subirMorro(const std::vector<int>& cifradas, const ModeloQuadrigramas& modelo,
                  int* mapa, int& trocasAceitas) {
    double atual = modelo.pontuar(cifradas, mapa);
    bool melhorou = true;
    while (melhorou) {
        melhorou = false;
        for (int a = 0; a < TAMANHO_ALFABETO; a++) {
            for (int b = a + 1; b < TAMANHO_ALFABETO; b++) {
                int guardada = mapa[a];
                mapa[a] = mapa[b];
                mapa[b] = guardada;
                double nova = modelo.pontuar(cifradas, mapa);
                if (nova > atual) {
                    atual = nova;
                    melhorou = true;
                    trocasAceitas++;
                } else {
                    mapa[b] = mapa[a];
                    mapa[a] = guardada;
                }
            }
        }
    }
    return atual;
}

ResultadoQuebra quebrarCifra(const std::string& cifrado, const ModeloQuadrigramas& modelo,
                             std::mt19937& gerador) {
    std::vector<int> cifradas = extrairLetras(normalizarParaCifra(cifrado), LETRAS_DA_AMOSTRA);

    std::vector<long long> contagemCifrada(TAMANHO_ALFABETO, 0);
    for (int letra : cifradas) {
        contagemCifrada[letra]++;
    }
    std::string ordemCifrada = ordenarPorFrequencia(contagemCifrada);
    std::string ordemNormal = modelo.letrasPorFrequencia();

    ResultadoQuebra resultado;
    resultado.letrasUsadas = static_cast<int>(cifradas.size());
    resultado.trocasAceitas = 0;
    resultado.pontuacao = 0;
    int melhorMapa[TAMANHO_ALFABETO];

    for (int reinicio = 0; reinicio < REINICIOS; reinicio++) {
        int mapa[TAMANHO_ALFABETO];
        if (reinicio == 0) {
            for (int i = 0; i < TAMANHO_ALFABETO; i++) {
                mapa[ordemCifrada[i] - 'a'] = ordemNormal[i] - 'a';
            }
        } else {
            std::string sorteada = gerarChave(gerador);
            for (int i = 0; i < TAMANHO_ALFABETO; i++) {
                mapa[i] = sorteada[i] - 'a';
            }
        }

        double pontuacao = subirMorro(cifradas, modelo, mapa, resultado.trocasAceitas);
        if (reinicio == 0 || pontuacao > resultado.pontuacao) {
            resultado.pontuacao = pontuacao;
            for (int i = 0; i < TAMANHO_ALFABETO; i++) {
                melhorMapa[i] = mapa[i];
            }
        }
    }

    resultado.chave = std::string(TAMANHO_ALFABETO, 'a');
    for (int cifrada = 0; cifrada < TAMANHO_ALFABETO; cifrada++) {
        resultado.chave[melhorMapa[cifrada]] = static_cast<char>('a' + cifrada);
    }
    return resultado;
}
