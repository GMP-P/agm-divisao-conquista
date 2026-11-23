# Árvore Geradora Mínima - Divisão e Conquista x Programação Dinâmica x Kruskal

Atividade de Análise e Projeto de Algoritmos (Divisão e Conquista), terceira parte do estudo da AGM, depois de Algoritmos Gulosos (Kruskal) e Programação Dinâmica.

## Arquivos

- `dc_agm.cpp` - AGM por Divisão e Conquista: divide os vértices ao meio, resolve cada metade recursivamente e combina as duas árvores com as arestas que cruzam entre elas (inclui, para comparação, a versão ingênua que liga as metades só pela aresta cruzada mais leve)
- `kruskal.cpp` - implementação gulosa (primeira atividade)
- `pd_agm.cpp` - Programação Dinâmica sobre subconjuntos (segunda atividade)
- `gerador.cpp` - gera grafos conexos aleatórios (semente fixa 42)
- `teste1_pequeno.txt` - 4 vértices (mostra todas as chamadas recursivas)
- `teste1b_cormen.txt` - 9 vértices, exemplo do Cormen (peso 37)
- `teste2_medio.txt` - 20 vértices, 60 arestas (peso 2741)
- `teste_empate.txt` e `teste_desconexo.txt` - casos extras
- `rodar_testes.bat` - compila, gera as instâncias grandes e roda os três algoritmos (Windows)

## Formato da entrada

```
V E
u v peso
...
```

Vértices numerados de 0 a V-1. É o mesmo formato das atividades anteriores, então os três programas rodam sobre os mesmos arquivos.

## Como compilar e rodar

```
g++ -std=c++17 -O2 dc_agm.cpp -o dc_agm
dc_agm teste1_pequeno.txt
```

Para grafos de até 10 vértices o programa mostra todas as chamadas recursivas; para até 20, as arestas da árvore.
