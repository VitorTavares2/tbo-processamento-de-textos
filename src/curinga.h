#ifndef CURINGA_H
#define CURINGA_H

#include <string>
#include <vector>

const char CURINGA = '*';

bool temCuringa(const std::string& padrao);

std::vector<int> buscarComCuringa(const std::string& texto, const std::string& padrao);

#endif
