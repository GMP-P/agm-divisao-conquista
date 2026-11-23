// Gerador de grafos conexos aleatorios para testar o Kruskal
// Compilar: g++ -std=c++17 -O2 gerador.cpp -o gerador
// Executar: gerador V E arquivo.txt
//
// Gera V vertices e E arestas (E >= V-1) com pesos de 1 a 1000.
// As primeiras V-1 arestas ligam cada vertice v a um vertice anterior
// qualquer, o que garante que o grafo e conexo. O resto e sorteado.

#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]){
    if (argc < 4){
        cout << "Uso: gerador V E arquivo.txt\n";
        return 1;
    }
    int V = atoi(argv[1]);
    int E = atoi(argv[2]);
    if (V < 2 || E < V - 1){
        cout << "Precisa de V >= 2 e E >= V-1\n";
        return 1;
    }

    // semente fixa: o mesmo comando sempre gera o mesmo grafo
    // (rand() no MinGW so vai ate 32767, por isso o mt19937)
    mt19937 rng(42);
    uniform_int_distribution<int> sorteiaPeso(1, 1000);

    ofstream arquivo(argv[3]);
    arquivo << V << " " << E << "\n";

    // V-1 arestas que deixam o grafo conexo
    for (int v = 1; v < V; v++){
        int u = uniform_int_distribution<int>(0, v - 1)(rng);
        arquivo << u << " " << v << " " << sorteiaPeso(rng) << "\n";
    }

    // arestas restantes, sem laco (u != v)
    uniform_int_distribution<int> sorteiaVertice(0, V - 1);
    for (int i = V - 1; i < E; i++){
        int u = sorteiaVertice(rng);
        int v = sorteiaVertice(rng);
        while (v == u) v = sorteiaVertice(rng);
        arquivo << u << " " << v << " " << sorteiaPeso(rng) << "\n";
    }

    arquivo.close();
    cout << "Grafo com " << V << " vertices e " << E << " arestas salvo em " << argv[3] << "\n";
    return 0;
}
