@echo off
REM Compila, gera as instancias grandes e roda todos os testes
REM (Divisao e Conquista, Kruskal e Programacao Dinamica).
REM Saida completa vai para resultados.txt

g++ -std=c++17 -O2 dc_agm.cpp -o dc_agm.exe
g++ -std=c++17 -O2 kruskal.cpp -o kruskal.exe
g++ -std=c++17 -O2 pd_agm.cpp -o pd_agm.exe
g++ -std=c++17 -O2 gerador.cpp -o gerador.exe

REM Serie A: grafos pequenos (E = 3V), onde a PD ainda consegue rodar
gerador.exe 10 30 grande_v10.txt
gerador.exe 12 36 grande_v12.txt
gerador.exe 14 42 grande_v14.txt
gerador.exe 16 48 grande_v16.txt
gerador.exe 18 54 grande_v18.txt
gerador.exe 20 60 grande_v20.txt
gerador.exe 22 66 grande_v22.txt
gerador.exe 24 72 grande_v24.txt

REM Serie B: os mesmos grafos grandes do Kruskal (E = 5V)
gerador.exe 2000 10000 grande_e10000.txt
gerador.exe 20000 100000 grande_e100000.txt
gerador.exe 100000 500000 grande_e500000.txt
gerador.exe 200000 1000000 grande_e1000000.txt
gerador.exe 400000 2000000 grande_e2000000.txt
gerador.exe 1000000 5000000 grande_e5000000.txt

echo ===== Teste 1 - pequeno (4 vertices) - DC ===== > resultados.txt
dc_agm.exe teste1_pequeno.txt >> resultados.txt
echo ===== Teste 1b - Cormen (9 vertices) - DC ===== >> resultados.txt
dc_agm.exe teste1b_cormen.txt >> resultados.txt
echo ===== Teste 1b - Cormen (9 vertices) - Kruskal ===== >> resultados.txt
kruskal.exe teste1b_cormen.txt >> resultados.txt
echo ===== Teste 2 - medio (20 vertices) - DC ===== >> resultados.txt
dc_agm.exe teste2_medio.txt >> resultados.txt
echo ===== Teste 2 - medio (20 vertices) - Kruskal ===== >> resultados.txt
kruskal.exe teste2_medio.txt >> resultados.txt
echo ===== Teste 2 - medio (20 vertices) - PD ===== >> resultados.txt
pd_agm.exe teste2_medio.txt >> resultados.txt
echo ===== Extra - empate - DC ===== >> resultados.txt
dc_agm.exe teste_empate.txt >> resultados.txt
echo ===== Extra - desconexo - DC ===== >> resultados.txt
dc_agm.exe teste_desconexo.txt >> resultados.txt

for %%V in (10 12 14 16 18 20 22 24) do (
    for %%R in (1 2 3) do (
        echo ===== Teste 3A - V=%%V execucao %%R - DC ===== >> resultados.txt
        dc_agm.exe grande_v%%V.txt >> resultados.txt
        echo ===== Teste 3A - V=%%V execucao %%R - Kruskal ===== >> resultados.txt
        kruskal.exe grande_v%%V.txt >> resultados.txt
        echo ===== Teste 3A - V=%%V execucao %%R - PD ===== >> resultados.txt
        pd_agm.exe grande_v%%V.txt >> resultados.txt
    )
)

for %%E in (10000 100000 500000 1000000 2000000 5000000) do (
    for %%R in (1 2 3) do (
        echo ===== Teste 3B - E=%%E execucao %%R - DC ===== >> resultados.txt
        dc_agm.exe grande_e%%E.txt >> resultados.txt
        echo ===== Teste 3B - E=%%E execucao %%R - Kruskal ===== >> resultados.txt
        kruskal.exe grande_e%%E.txt >> resultados.txt
    )
)

echo Pronto. Veja resultados.txt
