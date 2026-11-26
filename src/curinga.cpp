#include "curinga.h"
#include "kmp.h"

bool temCuringa(const std::string& padrao) {
    for (char caractere : padrao) {
        if (caractere == CURINGA) {
            return true;
        }
    }
    return false;
}

std::vector<int> buscarComCuringa(const std::string& texto,
                                  const std::string& padrao) {
    std::vector<int> encontradas;
    if (padrao.empty() || padrao.size() > texto.size()) {
        return encontradas;
    }
    int ultimoInicio = static_cast<int>(texto.size() - padrao.size());
    std::vector<int> pedacosCasados(ultimoInicio + 1, 0);
    int totalPedacos = 0;

    size_t i = 0;
    while (i < padrao.size()) {
        if (padrao[i] == CURINGA) {
            i++;
            continue;
        }
        size_t inicioPedaco = i;
        while (i < padrao.size() && padrao[i] != CURINGA) {
            i++;
        }
        std::string pedaco = padrao.substr(inicioPedaco, i - inicioPedaco);
        totalPedacos++;

        for (int posicao : buscarTodasKmp(texto, pedaco)) {
            int inicio = posicao - static_cast<int>(inicioPedaco);
            if (inicio >= 0 && inicio <= ultimoInicio) {
                pedacosCasados[inicio]++;
            }
        }
    }

    for (int inicio = 0; inicio <= ultimoInicio; inicio++) {
        if (pedacosCasados[inicio] == totalPedacos) {
            encontradas.push_back(inicio);
        }
    }
    return encontradas;
}
