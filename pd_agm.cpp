// Arvore Geradora Minima - Programacao Dinamica sobre subconjuntos
// Compilar: g++ -std=c++17 -O2 pd_agm.cpp -o pd_agm
// Executar: pd_agm arquivo.txt
//
// Mesmo formato de entrada do Kruskal:
//   primeira linha: V E          (numero de vertices e de arestas)
//   proximas E linhas: u v peso  (vertices numerados de 0 a V-1)
//
// Ideia: dp[S] = peso da arvore geradora minima que liga exatamente os
// vertices do conjunto S (S sempre contem o vertice 0).
// O conjunto S e guardado como uma mascara de bits: o bit v ligado
// significa que o vertice v esta no conjunto. Ex.: S = 1011 = {0, 1, 3}.

#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;

const long long INF = 1000000000000000LL;  // "infinito": conjunto que nao da para ligar
const int MAX_V = 25;                       // 2^25 estados ja ocupam centenas de MB

int V, E;
long long peso[MAX_V][MAX_V];   // peso[u][v] = menor aresta entre u e v (INF se nao existe)

// ------------------------------------------------------------
// Aresta mais barata que liga o vertice v a algum vertice do conjunto "resto".
// Devolve o peso e guarda em "vizinho" qual vertice do resto foi usado.
// ------------------------------------------------------------
long long arestaMaisBarata(int resto, int v, int& vizinho){
    long long melhor = INF;
    vizinho = -1;
    for (int u = 0; u < V; u++){
        if ((resto & (1 << u)) && peso[u][v] < melhor){
            melhor = peso[u][v];
            vizinho = u;
        }
    }
    return melhor;
}

// ------------------------------------------------------------
// 1) Recursao simples (sem guardar nada)
// Recorrencia: f(S) = min, para cada v de S (v != 0), de
//              f(S sem v) + aresta mais barata de v ate (S sem v)
// Caso basico: f({0}) = 0
// ------------------------------------------------------------
long long chamadasRecursao = 0;

long long recursaoSimples(int S){
    chamadasRecursao++;
    if (S == 1) return 0;                    // caso basico: so o vertice 0

    long long melhor = INF;
    for (int v = 1; v < V; v++){
        if (!(S & (1 << v))) continue;       // v precisa estar em S
        int resto = S ^ (1 << v);            // decisao: v e a ultima folha ligada
        int vizinho;
        long long aresta = arestaMaisBarata(resto, v, vizinho);
        if (aresta == INF) continue;         // v nao tem aresta para o resto
        long long sub = recursaoSimples(resto);   // resolve o subproblema DE NOVO
        if (sub != INF && sub + aresta < melhor) melhor = sub + aresta;
    }
    return melhor;
}

// ------------------------------------------------------------
// 2) Top-Down com memorizacao
// Mesma recorrencia, mas cada f(S) e calculado uma vez e guardado em memo[S].
// ------------------------------------------------------------
long long* memo;                 // memo[S] = -1 enquanto ainda nao foi calculado
long long chamadasTopDown = 0;
long long acertosCache = 0;      // quantas vezes o resultado ja estava guardado

long long topDown(int S){
    chamadasTopDown++;
    if (memo[S] != -1){                      // consulta ao cache
        acertosCache++;
        return memo[S];
    }

    long long melhor;
    if (S == 1) melhor = 0;                  // caso basico
    else {
        melhor = INF;
        for (int v = 1; v < V; v++){
            if (!(S & (1 << v))) continue;
            int resto = S ^ (1 << v);
            int vizinho;
            long long aresta = arestaMaisBarata(resto, v, vizinho);
            if (aresta == INF) continue;
            long long sub = topDown(resto);  // chamada recursiva (pode vir do cache)
            if (sub != INF && sub + aresta < melhor) melhor = sub + aresta;
        }
    }
    memo[S] = melhor;                        // armazenamento
    return melhor;
}

// ------------------------------------------------------------
// 3) Bottom-Up com tabulacao
// Preenche dp[S] em ordem crescente de S: como (S sem v) < S,
// todo subproblema ja esta pronto quando e consultado.
// folha[S] e vizinho[S] guardam a decisao, para reconstruir a arvore.
// ------------------------------------------------------------
long long* dp;
unsigned char* folha;
unsigned char* vizinhoFolha;

void bottomUp(){
    int total = 1 << V;
    for (int S = 0; S < total; S++) dp[S] = INF;
    dp[1] = 0;                               // caso basico: {0}

    for (int S = 3; S < total; S += 2){      // S impar = conjuntos que contem o vertice 0
        for (int v = 1; v < V; v++){
            if (!(S & (1 << v))) continue;
            int resto = S ^ (1 << v);
            if (dp[resto] == INF) continue;  // resto nao da para ligar
            int vizinho;
            long long aresta = arestaMaisBarata(resto, v, vizinho);
            if (aresta == INF) continue;
            if (dp[resto] + aresta < dp[S]){ // combinacao: guarda a melhor decisao
                dp[S] = dp[resto] + aresta;
                folha[S] = v;
                vizinhoFolha[S] = vizinho;
            }
        }
    }
}

// Escreve o conjunto S como {0,1,3}
void imprimirConjunto(int S){
    cout << "{";
    bool primeiro = true;
    for (int v = 0; v < V; v++){
        if (S & (1 << v)){
            if (!primeiro) cout << ",";
            cout << v;
            primeiro = false;
        }
    }
    cout << "}";
}

int main(int argc, char* argv[]){
    if (argc < 2){
        cout << "Uso: pd_agm arquivo.txt\n";
        return 1;
    }

    ifstream arquivo(argv[1]);
    if (!arquivo){
        cout << "Erro ao abrir " << argv[1] << "\n";
        return 1;
    }

    // ---------- leitura (matriz de adjacencia) ----------
    arquivo >> V >> E;
    if (V < 1 || V > MAX_V){
        cout << "A PD por subconjuntos aceita de 1 a " << MAX_V << " vertices (o grafo tem " << V << ").\n";
        return 1;
    }
    for (int u = 0; u < V; u++)
        for (int v = 0; v < V; v++) peso[u][v] = INF;
    for (int i = 0; i < E; i++){
        int u, v;
        long long p;
        arquivo >> u >> v >> p;
        if (p < peso[u][v]){                 // arestas repetidas: fica a mais leve
            peso[u][v] = p;
            peso[v][u] = p;
        }
    }
    arquivo.close();

    int total = 1 << V;
    int todos = total - 1;                   // conjunto com todos os vertices
    long long estados = total / 2;           // conjuntos que contem o vertice 0

    // ---------- 3) Bottom-Up ----------
    dp = new long long[total];
    folha = new unsigned char[total];
    vizinhoFolha = new unsigned char[total];
    auto t0 = chrono::steady_clock::now();
    bottomUp();
    auto t1 = chrono::steady_clock::now();
    double tBottomUp = chrono::duration<double, milli>(t1 - t0).count();

    if (dp[todos] == INF){
        cout << "O grafo e desconexo: nao existe arvore geradora.\n";
        return 0;
    }

    // tabela completa so para grafos bem pequenos
    if (V <= 5){
        cout << "Tabela da PD (Bottom-Up, em ordem de preenchimento):\n";
        cout << "  dp[{0}] = 0 (caso basico)\n";
        for (int S = 3; S < total; S += 2){
            cout << "  dp[";
            imprimirConjunto(S);
            cout << "] = ";
            if (dp[S] == INF) cout << "INF (nao da para ligar)\n";
            else {
                cout << dp[S] << "  <- dp[";
                imprimirConjunto(S ^ (1 << folha[S]));
                cout << "] + aresta (" << (int)vizinhoFolha[S] << " - " << (int)folha[S]
                     << ") peso " << peso[vizinhoFolha[S]][folha[S]] << "\n";
            }
        }
        cout << "\n";
    }

    // reconstrucao: desfaz as decisoes a partir do conjunto completo
    // (as arestas so sao listadas em grafos de ate 20 vertices)
    if (V <= 20){
        int qtd = 0;
        int arvoreU[MAX_V], arvoreV[MAX_V];
        for (int S = todos; S != 1; S ^= (1 << folha[S])){
            arvoreU[qtd] = vizinhoFolha[S];
            arvoreV[qtd] = folha[S];
            qtd++;
        }
        cout << "Arestas da arvore geradora minima (na ordem em que a arvore cresce a partir do 0):\n";
        for (int i = qtd - 1; i >= 0; i--)
            cout << "  (" << arvoreU[i] << " - " << arvoreV[i] << ") peso " << peso[arvoreU[i]][arvoreV[i]] << "\n";
        cout << "\n";
    }

    // ---------- 2) Top-Down ----------
    memo = new long long[total];
    for (int S = 0; S < total; S++) memo[S] = -1;
    t0 = chrono::steady_clock::now();
    long long resTopDown = topDown(todos);
    t1 = chrono::steady_clock::now();
    double tTopDown = chrono::duration<double, milli>(t1 - t0).count();
    delete[] memo;

    // ---------- 1) Recursao simples (so para grafos pequenos) ----------
    long long resRecursao = -1;
    double tRecursao = 0;
    if (V <= 12){
        t0 = chrono::steady_clock::now();
        resRecursao = recursaoSimples(todos);
        t1 = chrono::steady_clock::now();
        tRecursao = chrono::duration<double, milli>(t1 - t0).count();
    }

    // ---------- resultado ----------
    double memoriaMB = total * (8.0 + 2.0) / (1024.0 * 1024.0);
    cout << "Vertices: " << V << " | Arestas: " << E << " | Estados (conjuntos com o 0): " << estados << "\n";
    cout << "Peso total (Bottom-Up): " << dp[todos] << " | (Top-Down): " << resTopDown;
    if (resRecursao != -1) cout << " | (Recursao simples): " << resRecursao;
    cout << "\n";
    cout << "Chamadas Top-Down: " << chamadasTopDown << " | Acertos no cache: " << acertosCache << "\n";
    if (resRecursao != -1) cout << "Chamadas da recursao simples: " << chamadasRecursao << "\n";
    else cout << "Recursao simples: nao executada (V > 12 levaria tempo demais)\n";
    cout << "Memoria da tabela: " << memoriaMB << " MB\n";
    cout << "Tempo Bottom-Up: " << tBottomUp << " ms | Top-Down: " << tTopDown << " ms";
    if (resRecursao != -1) cout << " | Recursao simples: " << tRecursao << " ms";
    cout << "\n";

    delete[] dp;
    delete[] folha;
    delete[] vizinhoFolha;
    return 0;
}
