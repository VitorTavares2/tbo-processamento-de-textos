#include "huffman.h"
#include <stdexcept>

const int BYTES_POR_FREQUENCIA = 4;
const int TAMANHO_CABECALHO = TOTAL_SIMBOLOS * BYTES_POR_FREQUENCIA;

std::vector<long long> contarSimbolos(const std::string& dados) {
    std::vector<long long> frequencias(TOTAL_SIMBOLOS, 0);
    for (char caractere : dados) {
        frequencias[static_cast<unsigned char>(caractere)]++;
    }
    return frequencias;
}

bool menorPeso(const std::vector<NoHuffman>& nos, int primeiro, int segundo) {
    if (nos[primeiro].peso != nos[segundo].peso) {
        return nos[primeiro].peso < nos[segundo].peso;
    }
    return primeiro < segundo;
}

void inserirNoHeap(std::vector<int>& heap, const std::vector<NoHuffman>& nos, int no) {
    heap.push_back(no);
    size_t filho = heap.size() - 1;
    while (filho > 0) {
        size_t pai = (filho - 1) / 2;
        if (!menorPeso(nos, heap[filho], heap[pai])) {
            break;
        }
        int guardado = heap[pai];
        heap[pai] = heap[filho];
        heap[filho] = guardado;
        filho = pai;
    }
}

int removerMenor(std::vector<int>& heap, const std::vector<NoHuffman>& nos) {
    int menor = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    size_t pai = 0;
    while (true) {
        size_t esquerda = 2 * pai + 1;
        size_t direita = 2 * pai + 2;
        size_t escolhido = pai;
        if (esquerda < heap.size() && menorPeso(nos, heap[esquerda], heap[escolhido])) {
            escolhido = esquerda;
        }
        if (direita < heap.size() && menorPeso(nos, heap[direita], heap[escolhido])) {
            escolhido = direita;
        }
        if (escolhido == pai) {
            break;
        }
        int guardado = heap[pai];
        heap[pai] = heap[escolhido];
        heap[escolhido] = guardado;
        pai = escolhido;
    }
    return menor;
}

ArvoreHuffman montarArvoreHuffman(const std::vector<long long>& frequencias) {
    ArvoreHuffman arvore;
    arvore.raiz = -1;
    std::vector<int> heap;
    for (int simbolo = 0; simbolo < TOTAL_SIMBOLOS; simbolo++) {
        if (frequencias[simbolo] > 0) {
            NoHuffman folha;
            folha.peso = frequencias[simbolo];
            folha.simbolo = simbolo;
            folha.esquerda = -1;
            folha.direita = -1;
            arvore.nos.push_back(folha);
            inserirNoHeap(heap, arvore.nos, static_cast<int>(arvore.nos.size()) - 1);
        }
    }
    if (heap.empty()) {
        return arvore;
    }

    while (heap.size() > 1) {
        int primeiro = removerMenor(heap, arvore.nos);
        int segundo = removerMenor(heap, arvore.nos);
        NoHuffman juncao;
        juncao.peso = arvore.nos[primeiro].peso + arvore.nos[segundo].peso;
        juncao.simbolo = -1;
        juncao.esquerda = primeiro;
        juncao.direita = segundo;
        arvore.nos.push_back(juncao);
        inserirNoHeap(heap, arvore.nos, static_cast<int>(arvore.nos.size()) - 1);
    }
    arvore.raiz = heap[0];
    return arvore;
}

void percorrer(const ArvoreHuffman& arvore, int no, const std::string& caminho, std::vector<std::string>& codigos) {
    const NoHuffman& atual = arvore.nos[no];
    if (atual.simbolo != -1) {
        codigos[atual.simbolo] = caminho.empty() ? "0" : caminho;
        return;
    }
    percorrer(arvore, atual.esquerda, caminho + "0", codigos);
    percorrer(arvore, atual.direita, caminho + "1", codigos);
}

std::vector<std::string> gerarCodigos(const ArvoreHuffman& arvore) {
    std::vector<std::string> codigos(TOTAL_SIMBOLOS);
    if (arvore.raiz != -1) {
        percorrer(arvore, arvore.raiz, "", codigos);
    }
    return codigos;
}

std::string comprimir(const std::string& dados) {
    std::vector<long long> frequencias = contarSimbolos(dados);
    std::vector<std::string> codigos = gerarCodigos(montarArvoreHuffman(frequencias));

    std::string saida;
    for (int simbolo = 0; simbolo < TOTAL_SIMBOLOS; simbolo++) {
        unsigned long long valor = static_cast<unsigned long long>(frequencias[simbolo]);
        for (int i = 0; i < BYTES_POR_FREQUENCIA; i++) {
            saida += static_cast<char>((valor >> (8 * i)) & 0xFF);
        }
    }

    unsigned char byteAtual = 0;
    int bitsNoByte = 0;
    for (char caractere : dados) {
        for (char bit : codigos[static_cast<unsigned char>(caractere)]) {
            byteAtual = static_cast<unsigned char>(byteAtual << 1);
            if (bit == '1') {
                byteAtual |= 1;
            }
            bitsNoByte++;
            if (bitsNoByte == 8) {
                saida += static_cast<char>(byteAtual);
                byteAtual = 0;
                bitsNoByte = 0;
            }
        }
    }
    if (bitsNoByte > 0) {
        saida += static_cast<char>(byteAtual << (8 - bitsNoByte));
    }
    return saida;
}

std::string descomprimir(const std::string& comprimido) {
    if (comprimido.size() < static_cast<size_t>(TAMANHO_CABECALHO)) {
        throw std::runtime_error("arquivo comprimido incompleto");
    }
    std::vector<long long> frequencias(TOTAL_SIMBOLOS, 0);
    long long total = 0;
    for (int simbolo = 0; simbolo < TOTAL_SIMBOLOS; simbolo++) {
        unsigned long long valor = 0;
        for (int i = 0; i < BYTES_POR_FREQUENCIA; i++) {
            unsigned char byte = static_cast<unsigned char>(comprimido[simbolo * BYTES_POR_FREQUENCIA + i]);
            valor |= static_cast<unsigned long long>(byte) << (8 * i);
        }
        frequencias[simbolo] = static_cast<long long>(valor);
        total += frequencias[simbolo];
    }

    ArvoreHuffman arvore = montarArvoreHuffman(frequencias);
    std::string saida;
    if (arvore.raiz == -1) {
        return saida;
    }
    if (arvore.nos[arvore.raiz].simbolo != -1) {
        return std::string(static_cast<size_t>(total), static_cast<char>(arvore.nos[arvore.raiz].simbolo));
    }

    saida.reserve(static_cast<size_t>(total));
    int no = arvore.raiz;
    long long produzidos = 0;
    for (size_t i = TAMANHO_CABECALHO; i < comprimido.size() && produzidos < total; i++) {
        unsigned char byte = static_cast<unsigned char>(comprimido[i]);
        for (int bit = 7; bit >= 0 && produzidos < total; bit--) {
            bool um = (byte >> bit) & 1;
            no = um ? arvore.nos[no].direita : arvore.nos[no].esquerda;
            if (arvore.nos[no].simbolo != -1) {
                saida += static_cast<char>(arvore.nos[no].simbolo);
                produzidos++;
                no = arvore.raiz;
            }
        }
    }
    return saida;
}
