// Arvore Geradora Minima - Divisao e Conquista (vertices divididos ao meio)
// Compilar: g++ -std=c++17 -O2 dc_agm.cpp -o dc_agm
// Executar: dc_agm arquivo.txt
//
// Mesmo formato de entrada do Kruskal e da PD:
//   primeira linha: V E          (numero de vertices e de arestas)
//   proximas E linhas: u v peso  (vertices numerados de 0 a V-1)
//
// Ideia:
//   Dividir:     os vertices [ini, fim] sao separados em duas metades,
//                [ini, meio] e [meio+1, fim]. As arestas vao para a metade
//                esquerda, para a metade direita ou ficam como "cruzadas".
//   Conquistar:  resolve a AGM de cada metade, recursivamente.
//   Caso base:   um vertice sozinho ja e uma arvore (sem arestas).
//   Combinar:    junta as arvores das duas metades com as arestas cruzadas,
//                da mais leve para a mais pesada, descartando as que formam
//                ciclo (Union-Find).
//
// Por que a combinacao funciona (propriedade do ciclo): uma aresta de uma
// metade que ficou fora da AGM daquela metade e a mais pesada de um ciclo
// dentro dela, entao tambem nao entra na AGM do grafo inteiro. Por isso basta
// combinar as duas arvores com as arestas cruzadas.

#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
using namespace std;

struct Aresta {
    int origem;
    int destino;
    int peso;
};

// ------------------------------------------------------------
// Merge Sort (o mesmo do Kruskal) - ordena as arestas pelo peso uma unica vez.
// Como a divisao das arestas mantem a ordem, todas as listas da recursao
// ja ficam ordenadas.
// ------------------------------------------------------------
void conquer(Aresta* a, Aresta* aux, int low, int middle, int high){
    for (int k = low; k <= high; k++) aux[k] = a[k];
    int i = low, j = middle + 1;
    for (int k = low; k <= high; k++){
        if      (i > middle)                a[k] = aux[j++];
        else if (j > high)                  a[k] = aux[i++];
        else if (aux[j].peso < aux[i].peso) a[k] = aux[j++];
        else                                a[k] = aux[i++];
    }
}

void divide(Aresta* a, Aresta* aux, int low, int high){
    if (low >= high) return;
    int middle = (low + high) / 2;
    divide(a, aux, low, middle);
    divide(a, aux, middle + 1, high);
    conquer(a, aux, low, middle, high);
}

void mergeSort(Aresta* a, int n){
    if (n <= 1) return;
    Aresta* aux = new Aresta[n];
    divide(a, aux, 0, n - 1);
    delete[] aux;
}

// ------------------------------------------------------------
// Union-Find, usado so na etapa de combinacao
// ------------------------------------------------------------
int* pai;
int* altura;

int encontrar(int v){
    if (pai[v] != v) pai[v] = encontrar(pai[v]);   // compressao de caminho
    return pai[v];
}

void unir(int raizA, int raizB){                    // uniao por altura
    if      (altura[raizA] < altura[raizB]) pai[raizA] = raizB;
    else if (altura[raizA] > altura[raizB]) pai[raizB] = raizA;
    else {
        pai[raizB] = raizA;
        altura[raizA]++;
    }
}

// ------------------------------------------------------------
// Contadores para a analise
// ------------------------------------------------------------
long long chamadas = 0;           // chamadas recursivas
long long divisoes = 0;           // vezes que um problema foi dividido
int profundidadeMaxima = 0;       // nivel mais fundo da arvore de recursao
long long totalCruzadas = 0;      // arestas analisadas nas combinacoes (cruzadas)
long long descartadasCombinacao = 0;
long long arestasVivas = 0;       // arestas guardadas em listas neste momento
long long picoArestas = 0;        // maior valor de arestasVivas
bool mostrarPassos = false;       // so para grafos pequenos

void guardar(long long qtd){
    arestasVivas += qtd;
    if (arestasVivas > picoArestas) picoArestas = arestasVivas;
}

void recuar(int profundidade){ for (int i = 0; i < profundidade; i++) cout << "    "; }

void imprimirLista(const vector<Aresta>& lista){
    if (lista.empty()){ cout << "nenhuma"; return; }
    for (size_t i = 0; i < lista.size(); i++){
        if (i) cout << ", ";
        cout << lista[i].origem << "-" << lista[i].destino << " (" << lista[i].peso << ")";
    }
}

long long somaPesos(const vector<Aresta>& lista){
    long long s = 0;
    for (const Aresta& a : lista) s += a.peso;
    return s;
}

// ------------------------------------------------------------
// Combinar: intercala as tres listas ordenadas (como o "conquer" do
// Merge Sort) e aceita cada aresta que nao forma ciclo.
// ------------------------------------------------------------
vector<Aresta> combinar(int ini, int fim, const vector<Aresta>& arvoreEsq,
                        const vector<Aresta>& arvoreDir, const vector<Aresta>& cruzadas){
    for (int v = ini; v <= fim; v++){ pai[v] = v; altura[v] = 0; }

    vector<Aresta> resultado;
    size_t i = 0, j = 0, k = 0;
    int maximo = fim - ini;                   // uma arvore com n vertices tem n-1 arestas
    while ((int)resultado.size() < maximo &&
           (i < arvoreEsq.size() || j < arvoreDir.size() || k < cruzadas.size())){
        // escolhe a mais leve entre as tres listas
        const Aresta* a = nullptr;
        int origemLista = 0;
        if (i < arvoreEsq.size()) { a = &arvoreEsq[i]; origemLista = 1; }
        if (j < arvoreDir.size() && (!a || arvoreDir[j].peso < a->peso)) { a = &arvoreDir[j]; origemLista = 2; }
        if (k < cruzadas.size()  && (!a || cruzadas[k].peso < a->peso))  { a = &cruzadas[k];  origemLista = 3; }
        if (origemLista == 1) i++; else if (origemLista == 2) j++; else { k++; totalCruzadas++; }

        int raizOrigem = encontrar(a->origem), raizDestino = encontrar(a->destino);
        if (raizOrigem != raizDestino){
            unir(raizOrigem, raizDestino);
            resultado.push_back(*a);
        } else descartadasCombinacao++;          // fecharia um ciclo
    }
    return resultado;
}

// ------------------------------------------------------------
// Divisao e Conquista: AGM dos vertices [ini, fim] usando so as arestas recebidas
// ------------------------------------------------------------
vector<Aresta> agmDivisao(int ini, int fim, const vector<Aresta>& arestas, int profundidade){
    chamadas++;
    if (profundidade > profundidadeMaxima) profundidadeMaxima = profundidade;

    if (ini == fim){                              // caso base: um vertice, nenhuma aresta
        if (mostrarPassos){ recuar(profundidade); cout << "[" << ini << "] caso base: arvore vazia\n"; }
        return vector<Aresta>();
    }

    // ---------- dividir ----------
    divisoes++;
    int meio = (ini + fim) / 2;
    vector<Aresta> esquerda, direita, cruzadas;
    for (const Aresta& a : arestas){
        bool origemEsq = a.origem <= meio, destinoEsq = a.destino <= meio;
        if (origemEsq && destinoEsq)        esquerda.push_back(a);
        else if (!origemEsq && !destinoEsq) direita.push_back(a);
        else                                cruzadas.push_back(a);
    }
    guardar(esquerda.size() + direita.size() + cruzadas.size());

    if (mostrarPassos){
        recuar(profundidade);
        cout << "[" << ini << ".." << fim << "] divide em [" << ini << ".." << meio << "] e ["
             << meio + 1 << ".." << fim << "] | cruzadas: ";
        imprimirLista(cruzadas);
        cout << "\n";
    }

    // ---------- conquistar (as duas metades sao independentes) ----------
    vector<Aresta> arvoreEsq = agmDivisao(ini, meio, esquerda, profundidade + 1);
    vector<Aresta> arvoreDir = agmDivisao(meio + 1, fim, direita, profundidade + 1);
    guardar(-(long long)(esquerda.size() + direita.size()));
    vector<Aresta>().swap(esquerda);              // libera a memoria das metades
    vector<Aresta>().swap(direita);

    // ---------- combinar ----------
    vector<Aresta> resultado = combinar(ini, fim, arvoreEsq, arvoreDir, cruzadas);
    guardar(-(long long)cruzadas.size());

    if (mostrarPassos){
        recuar(profundidade);
        cout << "[" << ini << ".." << fim << "] combina: ";
        imprimirLista(resultado);
        cout << " = peso " << somaPesos(resultado) << "\n";
    }
    return resultado;
}

// ------------------------------------------------------------
// Versao ingenua (so para comparacao): junta as duas arvores
// usando apenas a aresta cruzada mais leve. Nao garante a AGM.
// ------------------------------------------------------------
vector<Aresta> agmIngenua(int ini, int fim, const vector<Aresta>& arestas){
    if (ini == fim) return vector<Aresta>();
    int meio = (ini + fim) / 2;
    vector<Aresta> esquerda, direita, cruzadas;
    for (const Aresta& a : arestas){
        bool origemEsq = a.origem <= meio, destinoEsq = a.destino <= meio;
        if (origemEsq && destinoEsq)        esquerda.push_back(a);
        else if (!origemEsq && !destinoEsq) direita.push_back(a);
        else                                cruzadas.push_back(a);
    }
    vector<Aresta> resultado = agmIngenua(ini, meio, esquerda);
    vector<Aresta> arvoreDir = agmIngenua(meio + 1, fim, direita);
    resultado.insert(resultado.end(), arvoreDir.begin(), arvoreDir.end());
    if (!cruzadas.empty()) resultado.push_back(cruzadas[0]);   // a cruzada mais leve
    return resultado;
}

int main(int argc, char* argv[]){
    if (argc < 2){
        cout << "Uso: dc_agm arquivo.txt\n";
        return 1;
    }

    ifstream arquivo(argv[1]);
    if (!arquivo){
        cout << "Erro ao abrir " << argv[1] << "\n";
        return 1;
    }

    // ---------- leitura ----------
    auto t0 = chrono::steady_clock::now();
    int V, E;
    arquivo >> V >> E;
    Aresta* lidas = new Aresta[E];
    for (int i = 0; i < E; i++)
        arquivo >> lidas[i].origem >> lidas[i].destino >> lidas[i].peso;
    arquivo.close();

    // ---------- ordenacao (uma vez so) ----------
    auto t1 = chrono::steady_clock::now();
    mergeSort(lidas, E);
    vector<Aresta> arestas(lidas, lidas + E);
    delete[] lidas;
    guardar(E);

    // ---------- divisao e conquista ----------
    pai = new int[V];
    altura = new int[V];
    mostrarPassos = (V <= 10);
    if (mostrarPassos) cout << "Chamadas recursivas (cada nivel de recuo = um nivel da arvore de recursao):\n";
    auto t2 = chrono::steady_clock::now();
    vector<Aresta> arvore = agmDivisao(0, V - 1, arestas, 0);
    auto t3 = chrono::steady_clock::now();

    // ---------- versao ingenua, para comparacao ----------
    vector<Aresta> ingenua = agmIngenua(0, V - 1, arestas);

    // ---------- resultado ----------
    long long pesoTotal = somaPesos(arvore);
    if (V <= 20){
        cout << "\nArestas da arvore geradora minima:\n";
        for (const Aresta& a : arvore)
            cout << "  (" << a.origem << " - " << a.destino << ") peso " << a.peso << "\n";
    }

    double tLeitura   = chrono::duration<double, milli>(t1 - t0).count();
    double tOrdenacao = chrono::duration<double, milli>(t2 - t1).count();
    double tDivisao   = chrono::duration<double, milli>(t3 - t2).count();
    double memoriaMB  = picoArestas * (double)sizeof(Aresta) / (1024.0 * 1024.0);

    cout << "\nVertices: " << V << " | Arestas: " << E << "\n";
    cout << "Arestas na arvore: " << arvore.size() << " | Peso total: " << pesoTotal << "\n";
    if ((int)arvore.size() < V - 1)
        cout << "Atencao: o grafo e desconexo, o resultado e uma floresta geradora minima ("
             << V - arvore.size() << " componentes).\n";
    cout << "Chamadas recursivas: " << chamadas << " | Divisoes: " << divisoes
         << " | Profundidade maxima: " << profundidadeMaxima << "\n";
    cout << "Arestas cruzadas analisadas na combinacao: " << totalCruzadas
         << " | Descartadas na combinacao (ciclo): " << descartadasCombinacao << "\n";
    cout << "Pico de arestas em memoria: " << picoArestas << " (" << memoriaMB << " MB)\n";
    cout << "Versao ingenua (so a cruzada mais leve): " << ingenua.size() << " arestas, peso "
         << somaPesos(ingenua);
    if ((int)ingenua.size() < (int)arvore.size()) cout << " (nao liga todos os vertices)";
    else if (somaPesos(ingenua) > pesoTotal) cout << " (mais pesada que a AGM)";
    else cout << " (coincidiu com a AGM)";
    cout << "\n";
    cout << "Tempo leitura: " << tLeitura << " ms | ordenacao: " << tOrdenacao
         << " ms | divisao e conquista: " << tDivisao << " ms\n";

    delete[] pai;
    delete[] altura;
    return 0;
}
