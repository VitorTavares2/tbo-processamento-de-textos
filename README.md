# Projeto 2 — Busca e Ordenação em textos

Programa em C++17 com as tarefas 1, 2, 3, 4, 5, 6 e 10 do enunciado:

| Opção | Tarefa | Técnica |
|---|---|---|
| 1 | Buscar uma palavra, uma ocorrência por vez, com coringa `*` | KMP |
| 2 | Buscar várias palavras ao mesmo tempo | Aho-Corasick |
| 3 | Comparar desempenho | ingênua × KMP × Aho-Corasick |
| 4, 5 | Cifrar e decifrar com chave | substituição por permutação do alfabeto |
| 6 | Quebrar a cifra sem a chave | quadrigramas + subida de encosta |
| 7 | Marcar erros e sugerir correções | busca binária + distância de edição |
| 8 | Comprimir e descomprimir | código de Huffman |

## Compilar e rodar

Na pasta do projeto:

```powershell
g++ -O2 -std=c++17 -static src\*.cpp -o textos.exe
.\textos.exe
```

O programa lê a pasta `dados`, então precisa ser executado de dentro da pasta do projeto.

## Dados

- `dom_casmurro.txt`: texto principal das buscas (Machado de Assis, Projeto Gutenberg, domínio público).
- `outros_livros.txt`: sete livros usados para treinar a quebra da cifra e para medir o desempenho.
- `dicionario.txt`: 261 mil palavras do português, com plurais e verbos conjugados (lista br.ispell).
- `redacao.txt`: texto atual com erros de ortografia de propósito.

## Documentação

- [Relatório](documentacao/Relatorio.pdf)
- [Apresentação](documentacao/Apresentacao.pdf)
