#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <limits>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>

std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>>
ingestaDeDatos(const std::string &nombreArchivo, int &tamannoMatriz);

void lecturaParametros(const std::string &nombreArchivo);

void algoritmoGreedy(std::string &nombreArchivo);

std::pair<std::vector<int>, int>
PMDLBit(const std::vector<int> &solucion_inicial, const std::vector<std::vector<int>> &flujo,
        const std::vector<std::vector<int>> &distancia);

std::pair<std::vector<int>, int>
PMDLBrandom(const std::vector<int> &solucion_inicial, const std::vector<std::vector<int>> &flujo,
            const std::vector<std::vector<int>> &distancia, unsigned int semilla);

int funcionObjetivo(const std::vector<int> &p, const std::vector<std::vector<int>> &flujo,
                    const std::vector<std::vector<int>> &distancia);

std::vector<int> intercambio(const std::vector<int> &p, int r, int s);

void test_PMDLBit();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "main::Uso: " << argv[0] << " <nombre del archivoParametros de parametros>" << std::endl;
        return 1;
    }

    std::string archivoParametros = argv[1];

    lecturaParametros(archivoParametros);

    test_PMDLBit();

    return 0;
}

std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>>
ingestaDeDatos(const std::string &nombreArchivo, int &tamannoMatriz) {
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "ingestaDeDatos::No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return std::make_pair(std::vector<std::vector<int>>(), std::vector<std::vector<int>>());
    }

    archivo >> tamannoMatriz;
    archivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<std::vector<int>> flujo(tamannoMatriz, std::vector<int>(tamannoMatriz));
    std::vector<std::vector<int>> distancias(tamannoMatriz, std::vector<int>(tamannoMatriz));

    for (int i = 0; i < tamannoMatriz; ++i) {
        for (int j = 0; j < tamannoMatriz; ++j) {
            archivo >> flujo[i][j];
        }
    }

    archivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (int i = 0; i < tamannoMatriz; ++i) {
        for (int j = 0; j < tamannoMatriz; ++j) {
            archivo >> distancias[i][j];
        }
    }

    archivo.close();

    return std::make_pair(flujo, distancias);
}

void lecturaParametros(const std::string &nombreArchivo) {

    std::ifstream paramFile(nombreArchivo);
    if (!paramFile.is_open()) {
        std::cerr << "lecturaParametros::No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return;
    }

    std::map<std::string, std::string> parametros;

    std::string line;
    while (std::getline(paramFile, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            key.erase(0, key.find_first_not_of(' '));
            key.erase(key.find_last_not_of(' ') + 1);
            value.erase(0, value.find_first_not_of(' '));
            value.erase(value.find_last_not_of(' ') + 1);
            parametros[key] = value;
        }
    }

    paramFile.close();

    if (parametros["algoritmo"] == "greedy") {
        algoritmoGreedy(parametros["nombre_del_archivo"]);
    }

    if (parametros["algoritmo"] == "bit") {
        // PMDLBit(nombreArchivo,
    }
}

void algoritmoGreedy(std::string &nombreArchivo) {

    int tamannoMatriz;
    auto matrices = ingestaDeDatos(nombreArchivo, tamannoMatriz);
    auto flujo = matrices.first;
    auto distancias = matrices.second;

    std::vector<int> sumatorioFlujos(tamannoMatriz, 0);
    std::vector<int> sumatorioDistancias(tamannoMatriz, 0);

    for (int i = 0; i < tamannoMatriz; ++i) {
        for (int j = 0; j < tamannoMatriz; ++j) {
            sumatorioFlujos[i] += flujo[i][j];
            sumatorioDistancias[i] += distancias[i][j];
        }
    }

    auto funcionOrdenar = [](const std::vector<int> &sumas, bool descending = false) {
        std::vector<std::pair<int, int>> sumasIndizadas;
        for (int i = 0; i < sumas.size(); i++) {
            sumasIndizadas.emplace_back(sumas[i], i);
        }
        if (descending) {
            std::sort(sumasIndizadas.begin(), sumasIndizadas.end(),
                      [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
                          return a.first > b.first;
                      });
        } else {
            std::sort(sumasIndizadas.begin(), sumasIndizadas.end());
        }
        return sumasIndizadas;
    };

    auto flujosIndizados = funcionOrdenar(sumatorioFlujos);
    auto distanciasIndizadas = funcionOrdenar(sumatorioDistancias, true);

    std::cout << "\nResultados algoritmo Greedy:" << std::endl;
    std::cout << "| Unidad >> Pos | Flujo | Dist |" << std::endl;
    std::cout << "|---------------|-------|------|" << std::endl;

    for (int i = 0; i < flujosIndizados.size(); i++) {
        std::cout << "| " << std::setw(6) << flujosIndizados[i].second + 1
                  << " >> " << std::setw(3) << distanciasIndizadas[i].second + 1
                  << " | " << std::setw(5) << flujosIndizados[i].first
                  << " | " << std::setw(4) << distanciasIndizadas[i].first << " |" << std::endl;
    }
    std::cout << "|------------------------------|" << std::endl;
}

int funcionObjetivo(const std::vector<int> &p, const std::vector<std::vector<int>> &flujo,
                    const std::vector<std::vector<int>> &distancia) {
    size_t n = p.size();
    int coste = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            coste += flujo[i][j] * distancia[p[i]][p[j]];
        }
    }
    return coste;
}

std::vector<int> intercambio(const std::vector<int> &p, int r, int s) {
    std::vector<int> p_copia = p;
    std::swap(p_copia[r], p_copia[s]);
    return p_copia;
}

std::pair<std::vector<int>, int>
PMDLBit(const std::vector<int> &solucion_inicial, const std::vector<std::vector<int>> &flujo,
        const std::vector<std::vector<int>> &distancia) {
    size_t n = solucion_inicial.size();

    // Solucion actual y mejor solucion encontrada
    std::vector<int> solucion_actual = solucion_inicial;
    std::vector<int> mejor_solucion = solucion_inicial;
    int coste_mejor_solucion = funcionObjetivo(mejor_solucion, flujo, distancia);

    // Inicializar mascara DLB con todos los bits a 1
    std::vector<int> DLB(n, 1);

    int iteraciones = 0;

    while (std::accumulate(DLB.begin(), DLB.end(), 0) > 0 && iteraciones < 1000) {
        for (int i = 0; i < n; ++i) {
            // Si el bit DLB para el elemento i esta a 0, lo saltamos
            if (DLB[i] == 0) {
                continue;
            }

            bool mejora = false;
            for (int j = 0; j < n; ++j) {
                if (i == j) {  // No intercambiar el mismo elemento
                    continue;
                }

                // Intercambiar elementos i y j
                std::vector<int> solucion_vecina = intercambio(solucion_actual, i, j);
                int coste_vecina = funcionObjetivo(solucion_vecina, flujo, distancia);
                iteraciones++;

                // Si la solucion vecina es mejor
                if (coste_vecina < coste_mejor_solucion) {
                    coste_mejor_solucion = coste_vecina;
                    mejor_solucion = solucion_vecina;
                    solucion_actual = solucion_vecina;
                    std::fill(DLB.begin(), DLB.end(), 0);  // Resetear DLB
                    DLB[j] = 1;  // Establecer el bit del ultimo elemento intercambiado a 1
                    mejora = true;
                    break;  // Salir del bucle de exploracion del vecindario
                }
            }

            // Si no se ha encontrado mejora para el elemento i, establecer su bit a 0 en DLB
            if (!mejora) {
                DLB[i] = 0;
            }
        }
    }

    return {mejor_solucion, coste_mejor_solucion};
}

std::pair<std::vector<int>, int>
PMDLBrandom(const std::vector<int> &solucion_inicial, const std::vector<std::vector<int>> &flujo,
            const std::vector<std::vector<int>> &distancia, unsigned int semilla) {
    size_t n = solucion_inicial.size();

    // Solucion actual y mejor solucion encontrada
    std::vector<int> solucion_actual = solucion_inicial;
    std::vector<int> mejor_solucion = solucion_inicial;
    int coste_mejor_solucion = funcionObjetivo(mejor_solucion, flujo, distancia);

    // Inicializar mascara DLB con todos los bits a 1
    std::vector<int> DLB(n, 1);

    int iteraciones = 0;

    // Generador de numeros aleatorios con semilla
    std::mt19937 generador(semilla);

    while (std::accumulate(DLB.begin(), DLB.end(), 0) > 0 && iteraciones < 1000) {
        // Elegir aleatoriamente una posicion con bit a 1 en DLB usando el generador con semilla
        std::vector<int> indices_activos;
        for (int idx = 0; idx < n; ++idx) {
            if (DLB[idx] == 1) {
                indices_activos.push_back(idx);
            }
        }
        std::uniform_int_distribution<int> distribucion(0, (int) indices_activos.size() - 1);
        int i = indices_activos[distribucion(generador)];

        bool mejora = false;
        for (int j = 0; j < n; ++j) {
            if (i == j) {  // No intercambiar el mismo elemento
                continue;
            }

            // Intercambiar elementos i y j
            std::vector<int> solucion_vecina = intercambio(solucion_actual, i, j);
            int coste_vecina = funcionObjetivo(solucion_vecina, flujo, distancia);
            iteraciones++;

            // Si la solucion vecina es mejor
            if (coste_vecina < coste_mejor_solucion) {
                coste_mejor_solucion = coste_vecina;
                mejor_solucion = solucion_vecina;
                solucion_actual = solucion_vecina;
                std::fill(DLB.begin(), DLB.end(), 0);  // Resetear DLB
                DLB[j] = 1;  // Establecer el bit del ultimo elemento intercambiado a 1
                mejora = true;
                break;  // Salir del bucle de exploracion del vecindario
            }
        }

        // Si no se ha encontrado mejora para el elemento i, establecer su bit a 0 en DLB
        if (!mejora) {
            DLB[i] = 0;
        }
    }

    return {mejor_solucion, coste_mejor_solucion};
}

void test_PMDLBit() {
    // 1. Generar datos de prueba
    std::vector<std::vector<int>> flujo = {
            {0, 3, 8, 3},
            {3, 0, 2, 4},
            {8, 2, 0, 5},
            {3, 4, 5, 0}
    };

    std::vector<std::vector<int>> distancia = {
            {0,  12, 6, 4},
            {12, 0,  6, 8},
            {6,  6,  0, 7},
            {4,  8,  7, 0}
    };

    // 2. Generar solucion inicial
    std::vector<int> solucion_inicial = {0, 1, 2,
                                         3};  // Esta es una solucion inicial simple (identidad). Puede generarse aleatoriamente si se prefiere.

    // 3. Invocar PMDLBit
    auto resultado = PMDLBit(solucion_inicial, flujo, distancia);

    // 4. Mostrar los resultados
    std::cout << "Solucion inicial: ";
    for (int val: solucion_inicial) {
        std::cout << val << " ";
    }
    std::cout << "\nCoste de la solucion inicial: " << funcionObjetivo(solucion_inicial, flujo, distancia) << std::endl;

    std::cout << "\nMejor solucion encontrada: ";
    for (int val: resultado.first) {
        std::cout << val << " ";
    }
    std::cout << "\nCoste de la mejor solucion: " << resultado.second << std::endl;
}


