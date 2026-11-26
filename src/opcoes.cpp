#include "opcoes.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "ahocorasick.h"
#include "cifra.h"
#include "curinga.h"
#include "entrada.h"
#include "huffman.h"
#include "ingenua.h"
#include "kmp.h"
#include "texto.h"

const int CONTEXTO = 35;
const size_t TAMANHO_MAXIMO_DO_PADRAO = 100;
const size_t LIMITE_DE_LINHAS = 15;
const size_t LIMITE_DO_TEXTO_MARCADO = 5000;
const size_t LIMITE_DE_SUGESTOES = 20;
const size_t LETRAS_MOSTRADAS = 400;
const size_t LETRAS_MINIMAS_PARA_QUEBRA = 2000;
const int REPETICOES = 3;
const int TAMANHO_DO_PIOR_CASO = 1000000;
const int PADRAO_DO_PIOR_CASO = 100;
const size_t TAMANHO_MINIMO_DO_PADRAO_MEDIDO = 4;
const int SIMBOLOS_MOSTRADOS = 8;

const std::string ARQUIVO_CIFRADO = "dados/cifrado.txt";
const std::string ARQUIVO_CHAVE = "dados/chave.txt";
const std::string ARQUIVO_DECIFRADO = "dados/decifrado.txt";
const std::string ARQUIVO_COMPRIMIDO = "dados/comprimido.huf";
const std::string ARQUIVO_DESCOMPRIMIDO = "dados/descomprimido.txt";
const std::string ARQUIVO_PRINCIPAL = "dados/dom_casmurro.txt";
const std::string ARQUIVO_REDACAO = "dados/redacao.txt";

double milissegundosDesde(std::chrono::steady_clock::time_point inicio) {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - inicio).count();
}

std::string milissegundos(double valor) {
    std::ostringstream texto;
    texto << std::fixed << std::setprecision(valor < 10 ? 3 : 1) << valor << " ms";
    return texto.str();
}

std::string porcentagem(double parte, double total) {
    std::ostringstream texto;
    texto << std::fixed << std::setprecision(1) << (total > 0 ? 100.0 * parte / total : 0.0) << "%";
    return texto.str();
}

bool palavraInteira(const std::string& texto, int posicao, int tamanho) {
    bool antesLivre = posicao == 0 || !ehLetra(texto[posicao - 1]);
    bool depoisLivre = posicao + tamanho >= static_cast<int>(texto.size()) || !ehLetra(texto[posicao + tamanho]);
    return antesLivre && depoisLivre;
}

int numeroDaLinha(const std::string& texto, int posicao) {
    int linha = 1;
    for (int i = 0; i < posicao; i++) {
        if (texto[i] == '\n') {
            linha++;
        }
    }
    return linha;
}

std::string trechoAoRedor(const std::string& texto, int posicao, int tamanho) {
    int inicio = posicao - CONTEXTO > 0 ? posicao - CONTEXTO : 0;
    int fim = posicao + tamanho + CONTEXTO;
    if (fim > static_cast<int>(texto.size())) {
        fim = static_cast<int>(texto.size());
    }
    std::string trecho = texto.substr(inicio, posicao - inicio) + "[" + texto.substr(posicao, tamanho) + "]" +
                         texto.substr(posicao + tamanho, fim - posicao - tamanho);
    for (char& caractere : trecho) {
        if (caractere == '\n' || caractere == '\r') {
            caractere = ' ';
        }
    }
    return latin1ParaUtf8(trecho);
}

bool lerPadrao(const std::string& pergunta, std::string& padrao) {
    while (true) {
        if (!lerLinha(pergunta, padrao)) {
            return false;
        }
        if (!padrao.empty() && padrao.size() <= TAMANHO_MAXIMO_DO_PADRAO) {
            padrao = minusculas(padrao);
            return true;
        }
        std::cout << "Erro: digite de 1 a " << TAMANHO_MAXIMO_DO_PADRAO << " caracteres." << std::endl;
    }
}

bool lerCaminho(const std::string& pergunta, const std::string& padrao, std::string& caminho) {
    while (true) {
        if (!lerLinha(pergunta + " (Enter = " + padrao + "): ", caminho)) {
            return false;
        }
        if (caminho.empty()) {
            caminho = padrao;
        }
        if (arquivoExiste(caminho)) {
            return true;
        }
        std::cout << "Erro: o arquivo " << latin1ParaUtf8(caminho) << " nao existe. Confira o nome e a pasta." << std::endl;
    }
}

bool mostrarEContinuar(const std::string& texto, int posicao, int tamanho, int numero, int total) {
    std::cout << "Ocorrencia " << numero << " de " << total << " (linha " << numeroDaLinha(texto, posicao) << "): "
              << trechoAoRedor(texto, posicao, tamanho) << std::endl;
    if (numero == total) {
        std::cout << "Fim do texto: todas as ocorrencias foram mostradas." << std::endl;
        return false;
    }
    std::string resposta;
    if (!lerLinha("  Enter = proxima, P = parar: ", resposta)) {
        return false;
    }
    return minusculas(resposta) != "p";
}

void buscarUmaPalavra(const Base& base) {
    std::string padrao;
    bool soInteira = false;
    if (!lerPadrao("Palavra (pode usar * como coringa): ", padrao) || !lerSimOuNao("So palavra inteira (S/N)? ", soInteira)) {
        return;
    }
    int tamanho = static_cast<int>(padrao.size());
    bool comCuringa = temCuringa(padrao);

    auto inicio = std::chrono::steady_clock::now();
    std::vector<int> todas = comCuringa ? buscarComCuringa(base.textoMinusculo, padrao) : buscarTodasKmp(base.textoMinusculo, padrao);
    std::vector<int> ocorrencias;
    for (int posicao : todas) {
        if (!soInteira || palavraInteira(base.texto, posicao, tamanho)) {
            ocorrencias.push_back(posicao);
        }
    }
    double tempo = milissegundosDesde(inicio);
    int total = static_cast<int>(ocorrencias.size());

    std::cout << "Total no texto: " << formatarNumero(total) << " ocorrencias ("
              << (comCuringa ? "KMP por pedacos, com coringa" : "KMP") << ", " << milissegundos(tempo) << ")" << std::endl;
    if (total == 0) {
        std::cout << "Dica: a busca nao diferencia maiusculas; tente sem acento ou com * no lugar da letra duvidosa." << std::endl;
        return;
    }

    if (comCuringa) {
        for (int i = 0; i < total; i++) {
            if (!mostrarEContinuar(base.texto, ocorrencias[i], tamanho, i + 1, total)) {
                return;
            }
        }
        return;
    }

    BuscaKmp busca = iniciarBuscaKmp(padrao);
    int numero = 0;
    int posicao = proximaOcorrencia(busca, base.textoMinusculo);
    while (posicao != -1) {
        if (!soInteira || palavraInteira(base.texto, posicao, tamanho)) {
            numero++;
            if (!mostrarEContinuar(base.texto, posicao, tamanho, numero, total)) {
                return;
            }
        }
        posicao = proximaOcorrencia(busca, base.textoMinusculo);
    }
}

void buscarVariasPalavras(const Base& base) {
    std::string linha;
    bool soInteira = false;
    if (!lerLinha("Palavras separadas por virgula: ", linha)) {
        return;
    }
    std::vector<std::string> palavras;
    for (const std::string& parte : dividir(linha, ',')) {
        std::string palavra = minusculas(aparar(parte));
        bool repetida = false;
        for (const std::string& existente : palavras) {
            repetida = repetida || existente == palavra;
        }
        if (!palavra.empty() && !repetida) {
            palavras.push_back(palavra);
        }
    }
    if (palavras.empty()) {
        std::cout << "Erro: nenhuma palavra digitada. Exemplo: capitu, bentinho, escobar" << std::endl;
        return;
    }
    if (!lerSimOuNao("So palavra inteira (S/N)? ", soInteira)) {
        return;
    }

    auto inicioMontagem = std::chrono::steady_clock::now();
    AhoCorasick automato;
    for (const std::string& palavra : palavras) {
        automato.adicionar(palavra);
    }
    automato.montar();
    double tempoMontagem = milissegundosDesde(inicioMontagem);

    auto inicioBusca = std::chrono::steady_clock::now();
    std::vector<OcorrenciaAho> todas = automato.buscar(base.textoMinusculo);
    std::vector<OcorrenciaAho> ocorrencias;
    for (const OcorrenciaAho& ocorrencia : todas) {
        if (!soInteira || palavraInteira(base.texto, ocorrencia.posicao, automato.tamanhoDoPadrao(ocorrencia.padrao))) {
            ocorrencias.push_back(ocorrencia);
        }
    }
    double tempoBusca = milissegundosDesde(inicioBusca);

    std::cout << "Automato com " << automato.quantidadeEstados() << " estados, montado em " << milissegundos(tempoMontagem) << std::endl;
    std::cout << "Busca de todas as palavras em uma unica passada pelo texto: " << milissegundos(tempoBusca) << std::endl;
    std::vector<int> contagem(palavras.size(), 0);
    for (const OcorrenciaAho& ocorrencia : ocorrencias) {
        contagem[ocorrencia.padrao]++;
    }
    for (size_t i = 0; i < palavras.size(); i++) {
        std::cout << "  " << latin1ParaUtf8(palavras[i]) << ": " << formatarNumero(contagem[i]) << " ocorrencias" << std::endl;
    }
    if (ocorrencias.empty()) {
        return;
    }

    std::cout << "Primeiras ocorrencias, na ordem do texto:" << std::endl;
    size_t limite = ocorrencias.size() < LIMITE_DE_LINHAS ? ocorrencias.size() : LIMITE_DE_LINHAS;
    for (size_t i = 0; i < limite; i++) {
        const OcorrenciaAho& ocorrencia = ocorrencias[i];
        std::cout << "  linha " << numeroDaLinha(base.texto, ocorrencia.posicao) << " - " << latin1ParaUtf8(palavras[ocorrencia.padrao])
                  << ": " << trechoAoRedor(base.texto, ocorrencia.posicao, automato.tamanhoDoPadrao(ocorrencia.padrao)) << std::endl;
    }
    if (ocorrencias.size() > limite) {
        std::cout << "  ... e mais " << formatarNumero(static_cast<long long>(ocorrencias.size() - limite)) << std::endl;
    }
}

long long contarComIngenua(const std::string& texto, const std::vector<std::string>& padroes) {
    long long total = 0;
    for (const std::string& padrao : padroes) {
        total += static_cast<long long>(buscarTodasIngenua(texto, padrao).size());
    }
    return total;
}

long long contarComKmp(const std::string& texto, const std::vector<std::string>& padroes) {
    long long total = 0;
    for (const std::string& padrao : padroes) {
        total += static_cast<long long>(buscarTodasKmp(texto, padrao).size());
    }
    return total;
}

long long contarComAho(const std::string& texto, const std::vector<std::string>& padroes) {
    AhoCorasick automato;
    for (const std::string& padrao : padroes) {
        automato.adicionar(padrao);
    }
    automato.montar();
    return automato.contar(texto);
}

double medirMediana(long long (*metodo)(const std::string&, const std::vector<std::string>&),
                    const std::string& texto, const std::vector<std::string>& padroes, long long& ocorrencias) {
    double tempos[REPETICOES];
    for (int i = 0; i < REPETICOES; i++) {
        auto inicio = std::chrono::steady_clock::now();
        ocorrencias = metodo(texto, padroes);
        tempos[i] = milissegundosDesde(inicio);
    }
    for (int i = 1; i < REPETICOES; i++) {
        double atual = tempos[i];
        int j = i;
        while (j > 0 && tempos[j - 1] > atual) {
            tempos[j] = tempos[j - 1];
            j--;
        }
        tempos[j] = atual;
    }
    return tempos[REPETICOES / 2];
}

void imprimirLinhaDeMedida(const std::string& rotulo, double ingenua, double kmp, double aho, bool iguais, long long ocorrencias) {
    std::cout << std::left << std::setw(12) << rotulo << std::right << std::setw(14) << milissegundos(ingenua)
              << std::setw(14) << milissegundos(kmp) << std::setw(16) << milissegundos(aho)
              << std::setw(14) << formatarNumero(ocorrencias) << (iguais ? "" : "  (DIFERENTES!)") << std::endl;
}

void compararDesempenho(const Base& base) {
    const int QUANTIDADES[] = {1, 10, 100, 1000};
    std::string texto = base.textoMinusculo + "\n" + minusculas(base.outrosLivros);

    std::vector<std::string> candidatas;
    for (const std::string& palavra : base.dicionario.palavras()) {
        if (palavra.size() >= TAMANHO_MINIMO_DO_PADRAO_MEDIDO) {
            candidatas.push_back(palavra);
        }
    }

    std::cout << "Texto: " << formatarNumero(static_cast<long long>(texto.size())) << " caracteres (os 8 livros). Mediana de "
              << REPETICOES << " execucoes. Medindo, aguarde..." << std::endl;
    std::cout << std::left << std::setw(12) << "Palavras" << std::right << std::setw(14) << "Ingenua" << std::setw(14) << "KMP"
              << std::setw(16) << "Aho-Corasick" << std::setw(14) << "Ocorrencias" << std::endl;

    for (int quantidade : QUANTIDADES) {
        std::vector<std::string> padroes;
        size_t passo = candidatas.size() / quantidade;
        for (int i = 0; i < quantidade; i++) {
            padroes.push_back(candidatas[i * passo + passo / 2]);
        }
        long long ocorrenciasIngenua = 0;
        long long ocorrenciasKmp = 0;
        long long ocorrenciasAho = 0;
        double ingenua = medirMediana(contarComIngenua, texto, padroes, ocorrenciasIngenua);
        double kmp = medirMediana(contarComKmp, texto, padroes, ocorrenciasKmp);
        double aho = medirMediana(contarComAho, texto, padroes, ocorrenciasAho);
        bool iguais = ocorrenciasIngenua == ocorrenciasKmp && ocorrenciasKmp == ocorrenciasAho;
        imprimirLinhaDeMedida(std::to_string(quantidade), ingenua, kmp, aho, iguais, ocorrenciasAho);
    }

    std::string pior(TAMANHO_DO_PIOR_CASO, 'a');
    std::vector<std::string> padraoPior;
    padraoPior.push_back(std::string(PADRAO_DO_PIOR_CASO - 1, 'a') + "b");
    long long ocorrenciasIngenua = 0;
    long long ocorrenciasKmp = 0;
    long long ocorrenciasAho = 0;
    double ingenua = medirMediana(contarComIngenua, pior, padraoPior, ocorrenciasIngenua);
    double kmp = medirMediana(contarComKmp, pior, padraoPior, ocorrenciasKmp);
    double aho = medirMediana(contarComAho, pior, padraoPior, ocorrenciasAho);
    std::cout << std::endl << "Pior caso da busca ingenua: texto com " << formatarNumero(TAMANHO_DO_PIOR_CASO)
              << " letras 'a', padrao com " << PADRAO_DO_PIOR_CASO - 1 << " letras 'a' e um 'b' no fim" << std::endl;
    imprimirLinhaDeMedida("1", ingenua, kmp, aho, ocorrenciasIngenua == ocorrenciasKmp && ocorrenciasKmp == ocorrenciasAho, ocorrenciasAho);
}

void imprimirChave(const std::string& chave) {
    std::cout << "  letra normal:  abcdefghijklmnopqrstuvwxyz" << std::endl;
    std::cout << "  letra cifrada: " << chave << std::endl;
}

bool lerChave(const std::string& pergunta, bool aceitaVazia, Base& base, std::string& chave) {
    while (true) {
        if (!lerLinha(pergunta, chave)) {
            return false;
        }
        chave = minusculas(chave);
        if (chave.empty() && aceitaVazia) {
            chave = gerarChave(base.gerador);
            return true;
        }
        if (chaveValida(chave)) {
            return true;
        }
        std::cout << "Erro: a chave precisa ter as 26 letras de a a z, cada uma uma vez so. Exemplo: qwertyuiopasdfghjklzxcvbnm" << std::endl;
    }
}

void cifrarOuDecifrarMensagem(Base& base) {
    std::string operacao;
    while (true) {
        if (!lerLinha("Cifrar (C) ou decifrar (D)? ", operacao)) {
            return;
        }
        operacao = minusculas(operacao);
        if (operacao == "c" || operacao == "d") {
            break;
        }
        std::cout << "Erro: digite C ou D." << std::endl;
    }
    std::string mensagem;
    if (!lerLinha("Mensagem: ", mensagem)) {
        return;
    }
    std::string chave;
    if (operacao == "c") {
        if (!lerChave("Chave com 26 letras (Enter = gerar uma aleatoria): ", true, base, chave)) {
            return;
        }
        std::cout << "Chave usada:" << std::endl;
        imprimirChave(chave);
        std::cout << "Mensagem cifrada: " << latin1ParaUtf8(cifrar(mensagem, chave)) << std::endl;
    } else {
        if (!lerChave("Chave com 26 letras: ", false, base, chave)) {
            return;
        }
        std::cout << "Mensagem decifrada: " << latin1ParaUtf8(decifrar(mensagem, chave)) << std::endl;
    }
}

void cifrarArquivo(Base& base) {
    std::string caminho;
    std::string chave;
    if (!lerCaminho("Arquivo para cifrar", ARQUIVO_PRINCIPAL, caminho) ||
        !lerChave("Chave com 26 letras (Enter = gerar uma aleatoria): ", true, base, chave)) {
        return;
    }
    std::string cifrado = cifrar(utf8ParaLatin1(lerArquivo(caminho)), chave);
    gravarArquivo(ARQUIVO_CIFRADO, latin1ParaUtf8(cifrado));
    gravarArquivo(ARQUIVO_CHAVE, chave + "\n");

    std::cout << "Chave usada (gravada em " << ARQUIVO_CHAVE << "):" << std::endl;
    imprimirChave(chave);
    std::cout << "Texto cifrado gravado em " << ARQUIVO_CIFRADO << ". Comeco:" << std::endl;
    std::cout << latin1ParaUtf8(cifrado.substr(0, LETRAS_MOSTRADAS)) << std::endl;
}

void conferirComChaveReal(const std::string& cifrado, const std::string& chaveEncontrada, const std::string& chaveReal) {
    int letrasCertas = 0;
    std::string erradas;
    for (int i = 0; i < TAMANHO_ALFABETO; i++) {
        if (chaveEncontrada[i] == chaveReal[i]) {
            letrasCertas++;
        } else {
            erradas += static_cast<char>('a' + i);
        }
    }
    std::string certo = decifrar(cifrado, chaveReal);
    std::string obtido = decifrar(cifrado, chaveEncontrada);
    long long letrasDoTexto = 0;
    long long letrasIguais = 0;
    for (size_t i = 0; i < certo.size(); i++) {
        if (certo[i] >= 'a' && certo[i] <= 'z') {
            letrasDoTexto++;
            if (certo[i] == obtido[i]) {
                letrasIguais++;
            }
        }
    }
    std::cout << "Conferencia com a chave verdadeira (" << ARQUIVO_CHAVE << "):" << std::endl;
    std::cout << "  letras da chave certas: " << letrasCertas << " de " << TAMANHO_ALFABETO;
    if (!erradas.empty()) {
        std::cout << " (erradas: " << erradas << ")";
    }
    std::cout << std::endl;
    std::cout << "  letras do texto decifradas certo: " << formatarNumero(letrasIguais) << " de " << formatarNumero(letrasDoTexto)
              << " (" << porcentagem(static_cast<double>(letrasIguais), static_cast<double>(letrasDoTexto)) << "; erradas: "
              << formatarNumero(letrasDoTexto - letrasIguais) << ")" << std::endl;
}

void quebrarCifraDeArquivo(Base& base) {
    std::string caminho;
    if (!lerCaminho("Arquivo cifrado", ARQUIVO_CIFRADO, caminho)) {
        return;
    }
    std::string cifrado = utf8ParaLatin1(lerArquivo(caminho));
    size_t letras = extrairLetras(normalizarParaCifra(cifrado), LETRAS_MINIMAS_PARA_QUEBRA).size();
    if (letras < LETRAS_MINIMAS_PARA_QUEBRA) {
        std::cout << "Aviso: o texto tem so " << letras << " letras. A quebra funciona melhor com textos grandes." << std::endl;
    }

    std::cout << "Quebrando a cifra, aguarde..." << std::endl;
    auto inicio = std::chrono::steady_clock::now();
    ResultadoQuebra resultado = quebrarCifra(cifrado, base.modelo, base.gerador);
    std::string decifrado = decifrar(cifrado, resultado.chave);
    double tempo = milissegundosDesde(inicio);
    gravarArquivo(ARQUIVO_DECIFRADO, latin1ParaUtf8(decifrado));

    std::cout << "Amostra analisada: " << formatarNumero(resultado.letrasUsadas) << " letras; trocas aceitas na subida: "
              << resultado.trocasAceitas << "; tempo: " << milissegundos(tempo) << std::endl;
    std::cout << "Chave encontrada:" << std::endl;
    imprimirChave(resultado.chave);
    std::cout << "Texto decifrado gravado em " << ARQUIVO_DECIFRADO << ". Comeco:" << std::endl;
    std::cout << latin1ParaUtf8(decifrado.substr(0, LETRAS_MOSTRADAS)) << std::endl;

    if (caminho == ARQUIVO_CIFRADO && arquivoExiste(ARQUIVO_CHAVE)) {
        std::string chaveReal = aparar(lerArquivo(ARQUIVO_CHAVE));
        if (chaveValida(chaveReal)) {
            conferirComChaveReal(cifrado, resultado.chave, chaveReal);
        }
    }
}

struct PalavraErrada {
    std::string palavra;
    int primeiraPosicao;
    int vezes;
};

void marcarErrosOrtograficos(const Base& base) {
    std::string caminho;
    if (!lerCaminho("Arquivo de texto", ARQUIVO_REDACAO, caminho)) {
        return;
    }
    std::string texto = utf8ParaLatin1(lerArquivo(caminho));

    auto inicio = std::chrono::steady_clock::now();
    std::vector<PalavraNoTexto> palavras = separarPalavras(texto);
    std::vector<PalavraNoTexto> foraDoDicionario;
    for (const PalavraNoTexto& encontrada : palavras) {
        if (!base.dicionario.contem(encontrada.palavra)) {
            foraDoDicionario.push_back(encontrada);
        }
    }
    double tempoVerificacao = milissegundosDesde(inicio);

    std::vector<PalavraErrada> diferentes;
    for (const PalavraNoTexto& encontrada : foraDoDicionario) {
        bool jaVista = false;
        for (PalavraErrada& errada : diferentes) {
            if (errada.palavra == encontrada.palavra) {
                errada.vezes++;
                jaVista = true;
                break;
            }
        }
        if (!jaVista) {
            PalavraErrada nova;
            nova.palavra = encontrada.palavra;
            nova.primeiraPosicao = encontrada.posicao;
            nova.vezes = 1;
            diferentes.push_back(nova);
        }
    }

    std::cout << "Palavras no texto: " << formatarNumero(static_cast<long long>(palavras.size()))
              << " | fora do dicionario: " << formatarNumero(static_cast<long long>(foraDoDicionario.size()))
              << " (" << formatarNumero(static_cast<long long>(diferentes.size())) << " diferentes) | verificacao: "
              << milissegundos(tempoVerificacao) << std::endl;

    if (texto.size() <= LIMITE_DO_TEXTO_MARCADO && !foraDoDicionario.empty()) {
        std::string marcado;
        size_t copiado = 0;
        for (const PalavraNoTexto& errada : foraDoDicionario) {
            marcado += texto.substr(copiado, errada.posicao - copiado);
            marcado += "[" + texto.substr(errada.posicao, errada.palavra.size()) + "]";
            copiado = errada.posicao + errada.palavra.size();
        }
        marcado += texto.substr(copiado);
        std::cout << std::endl << "Texto com as palavras fora do dicionario entre [ ]:" << std::endl;
        std::cout << latin1ParaUtf8(marcado) << std::endl;
    }
    if (diferentes.empty()) {
        return;
    }

    std::cout << "Sugestoes (distancia de edicao ate 2; no empate vem a palavra mais usada nos livros):" << std::endl;
    auto inicioSugestoes = std::chrono::steady_clock::now();
    size_t limite = diferentes.size() < LIMITE_DE_SUGESTOES ? diferentes.size() : LIMITE_DE_SUGESTOES;
    for (size_t i = 0; i < limite; i++) {
        std::cout << "  linha " << numeroDaLinha(texto, diferentes[i].primeiraPosicao) << " - " << latin1ParaUtf8(diferentes[i].palavra) << " -> ";
        std::vector<Sugestao> sugestoes = base.dicionario.sugerir(diferentes[i].palavra);
        if (sugestoes.empty()) {
            std::cout << "sem sugestao";
        }
        for (size_t j = 0; j < sugestoes.size(); j++) {
            std::cout << (j > 0 ? ", " : "") << latin1ParaUtf8(sugestoes[j].palavra) << " (" << sugestoes[j].distancia << ")";
        }
        std::cout << std::endl;
    }
    if (diferentes.size() > limite) {
        std::cout << "  ... e mais " << formatarNumero(static_cast<long long>(diferentes.size() - limite)) << " palavras diferentes" << std::endl;
    }
    std::cout << "Tempo das sugestoes: " << milissegundos(milissegundosDesde(inicioSugestoes)) << std::endl;
}

std::string nomeDoSimbolo(int simbolo) {
    if (simbolo == ' ') {
        return "espaco";
    }
    if (simbolo == '\n') {
        return "quebra de linha";
    }
    if (simbolo > ' ' && simbolo < 127) {
        return std::string("'") + static_cast<char>(simbolo) + "'";
    }
    std::ostringstream texto;
    texto << "byte " << simbolo;
    return texto.str();
}

void comprimirArquivo() {
    std::string caminho;
    if (!lerCaminho("Arquivo para comprimir", ARQUIVO_PRINCIPAL, caminho)) {
        return;
    }
    std::string dados = lerArquivo(caminho);

    auto inicioCompressao = std::chrono::steady_clock::now();
    std::string comprimido = comprimir(dados);
    double tempoCompressao = milissegundosDesde(inicioCompressao);
    gravarArquivo(ARQUIVO_COMPRIMIDO, comprimido);

    auto inicioDescompressao = std::chrono::steady_clock::now();
    std::string volta = descomprimir(lerArquivo(ARQUIVO_COMPRIMIDO));
    double tempoDescompressao = milissegundosDesde(inicioDescompressao);
    gravarArquivo(ARQUIVO_DESCOMPRIMIDO, volta);

    std::vector<long long> frequencias = contarSimbolos(dados);
    std::vector<std::string> codigos = gerarCodigos(montarArvoreHuffman(frequencias));
    double totalBits = 0;
    double entropia = 0;
    int simbolosUsados = 0;
    for (int simbolo = 0; simbolo < TOTAL_SIMBOLOS; simbolo++) {
        if (frequencias[simbolo] > 0) {
            double probabilidade = static_cast<double>(frequencias[simbolo]) / static_cast<double>(dados.size());
            totalBits += static_cast<double>(frequencias[simbolo]) * static_cast<double>(codigos[simbolo].size());
            entropia -= probabilidade * std::log2(probabilidade);
            simbolosUsados++;
        }
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Original:     " << formatarNumero(static_cast<long long>(dados.size())) << " bytes (" << simbolosUsados << " simbolos diferentes)" << std::endl;
    std::cout << "Comprimido:   " << formatarNumero(static_cast<long long>(comprimido.size())) << " bytes ("
              << porcentagem(static_cast<double>(comprimido.size()), static_cast<double>(dados.size())) << " do original, gravado em "
              << ARQUIVO_COMPRIMIDO << ")" << std::endl;
    std::cout << "Bits por simbolo: " << (dados.empty() ? 0.0 : totalBits / static_cast<double>(dados.size()))
              << " (antes: 8; limite teorico pela entropia: " << entropia << ")" << std::endl;
    std::cout << "Tempo: compressao " << milissegundos(tempoCompressao) << ", descompressao " << milissegundos(tempoDescompressao) << std::endl;
    std::cout << "Descomprimido igual ao original: " << (volta == dados ? "sim" : "NAO") << " (" << ARQUIVO_DESCOMPRIMIDO << ")" << std::endl;
    std::cout.unsetf(std::ios::fixed);

    std::cout << "Simbolos mais frequentes e seus codigos:" << std::endl;
    std::vector<bool> mostrado(TOTAL_SIMBOLOS, false);
    for (int vez = 0; vez < SIMBOLOS_MOSTRADOS && vez < simbolosUsados; vez++) {
        int maior = -1;
        for (int simbolo = 0; simbolo < TOTAL_SIMBOLOS; simbolo++) {
            if (!mostrado[simbolo] && frequencias[simbolo] > 0 && (maior == -1 || frequencias[simbolo] > frequencias[maior])) {
                maior = simbolo;
            }
        }
        mostrado[maior] = true;
        std::cout << "  " << std::left << std::setw(16) << nomeDoSimbolo(maior) << std::right << std::setw(10)
                  << formatarNumero(frequencias[maior]) << " vezes  codigo " << codigos[maior] << std::endl;
    }
}
