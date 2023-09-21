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

void lecturaParametros(const std::string &nombreArchivo);

std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>>
ingestaDeDatos(const std::string &nombreArchivo, int &tamannoMatriz);

void algoritmoGreedy(std::string &nombreArchivo);

std::pair<std::vector<int>, int> PMDLBit(const std::string &nombreArchivo);

std::pair<std::vector<int>, int> PMDLBitrandom(const std::string &nombreArchivo);

int funcionObjetivo(const std::vector<int> &p, const std::vector<std::vector<int>> &flujo,
                    const std::vector<std::vector<int>> &distancia);

std::vector<int> intercambio(const std::vector<int> &p, int r, int s);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "main::Uso: " << argv[0] << " <nombre del archivoParametros de parametros>" << std::endl;
        return 1;
    }

    std::string archivoParametros = argv[1];

    lecturaParametros(archivoParametros);

    return 0;
}

std::vector<int> semillas;

void lecturaParametros(const std::string &nombreArchivo) {

    std::ifstream paramFile(nombreArchivo);
    if (!paramFile.is_open()) {
        std::cerr << "lecturaParametros::No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return;
    }

    std::map<std::string, std::string> parametros;

    std::string line;
    while (std::getline(paramFile, line)) {

        if (!line.empty() && (line[0] == '#' || (line.size() >= 2 && line[0] == '/' && line[1] == '/'))) {
            continue;
        }

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

    if (parametros.find("semilla") != parametros.end()) {
        std::string seeds_string = parametros["semilla"];
        std::stringstream ss(seeds_string);
        std::string seed;
        while (std::getline(ss, seed, ',')) {
            semillas.push_back(std::stoi(seed));
        }
    }

    std::string archivoDatos = parametros["nombre_del_archivo"];

    if (parametros["algoritmo"] == "greedy" || parametros["algoritmo"] == "1") {
        algoritmoGreedy(archivoDatos);
        return;
    }

    if (parametros["algoritmo"] == "bit" || parametros["algoritmo"] == "2") {
        PMDLBit(archivoDatos);
        return;
    }

    if (parametros["algoritmo"] == "bitr" || parametros["algoritmo"] == "3") {
        PMDLBitrandom(archivoDatos);
        return;
    }

    std::cout << "Archivo " << nombreArchivo << " leido correctamente. No se ha indicado ninguna funcion conocida";
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

    std::cout << "\nArchivo " << nombreArchivo << " procesado correctamente." << std::endl;

    return std::make_pair(flujo, distancias);
}

void algoritmoGreedy(std::string &nombreArchivo) {

    std::cout << "\nResultados del algoritmo Greedy" << std::endl << "---------------------------------"
              << std::endl;

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

std::pair<std::vector<int>, int> PMDLBit(const std::string &nombreArchivo) {

    std::cout << "\nResultados del algoritmo PMDLBit" << std::endl << "--------------------------------"
              << std::endl;

    int tamannoMatriz;
    auto matrices = ingestaDeDatos(nombreArchivo, tamannoMatriz);

    std::vector<std::vector<int>> flujo = matrices.first;
    std::vector<std::vector<int>> distancia = matrices.second;

    std::vector<int> solucion_inicial(tamannoMatriz);
    for (int i = 0; i < tamannoMatriz; i++) {
        solucion_inicial[i] = i;
    }

    // Solucion actual y mejor solucion encontrada
    std::vector<int> solucion_actual = solucion_inicial;
    std::vector<int> mejor_solucion = solucion_inicial;
    int coste_mejor_solucion = funcionObjetivo(mejor_solucion, flujo, distancia);

    // Inicializar mascara DLB con todos los bits a 1
    std::vector<int> DLB(tamannoMatriz, 1);

    int iteraciones = 0;

    while (std::accumulate(DLB.begin(), DLB.end(), 0) > 0 && iteraciones < 1000) {
        // Bucle de recorrido
        for (int i = 0; i < tamannoMatriz; ++i) {
            // Si el bit DLB para el elemento i esta a 0, lo saltamos
            if (DLB[i] == 0) {
                continue;
            }

            // Bucle de intercambio
            bool mejora = false;
            for (int j = 0; j < tamannoMatriz; ++j) {
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
                    break;  // Sale del bucle de exploracion del vecindario
                }
            }

            // Si no se ha encontrado mejora para el elemento i, establecer su bit a 0 en DLB
            if (!mejora) {
                DLB[i] = 0;
            }
        }
    }

    std::cout << "| Unidad >> Pos |" << std::endl;
    std::cout << "|---------------|" << std::endl;

    for (int i = 0; i < tamannoMatriz; i++) {
        std::cout << "|   " << std::setw(5) << i + 1
                  << " >> " << std::setw(2) << mejor_solucion[i] + 1 << " |" << std::endl;
    }
    std::cout << "|---------------|" << std::endl;

    return {mejor_solucion, coste_mejor_solucion};
}

std::pair<std::vector<int>, int> PMDLBitrandom(const std::string &nombreArchivo) {


    std::cout << "\nResultados del algoritmo PMDLBitrandom" << std::endl << "--------------------------------"
              << std::endl;

    int tamannoMatriz;
    auto matrices = ingestaDeDatos(nombreArchivo, tamannoMatriz);

    std::vector<std::vector<int>> flujo = matrices.first;
    std::vector<std::vector<int>> distancia = matrices.second;

    std::vector<int> solucion_inicial(tamannoMatriz);
    for (int i = 0; i < tamannoMatriz; i++) {
        solucion_inicial[i] = i;
    }

    std::vector<int> mejor_solucion_global = solucion_inicial;
    int coste_mejor_solucion_global = INT_MAX;

    if (semillas.empty()) {
        semillas.push_back((int) std::random_device{}());
    }

    for (int semilla: semillas) {
        std::mt19937 gen(semilla);

        std::vector<int> solucion_actual = solucion_inicial;
        std::vector<int> mejor_solucion = solucion_inicial;
        int coste_mejor_solucion = funcionObjetivo(mejor_solucion, flujo, distancia);

        // Inicializa la mascara DLB con todos los bits a 1
        std::vector<int> DLB(tamannoMatriz, 1);

        std::uniform_int_distribution<> dis(0, tamannoMatriz - 1);

        int iteraciones = 0;
        while (iteraciones < 1000) {
            bool global_mejora = false;
            int inicio = dis(gen);  // Genera un índice aleatorio como punto de inicio
            for (int k = 0; k < tamannoMatriz; ++k) {
                int i = (inicio + k) % tamannoMatriz;  // Calcula el índice actual de manera cíclica
                if (DLB[i] == 0) {
                    continue;
                }

                bool mejora = false;
                for (int j = 0; j < tamannoMatriz; ++j) {
                    if (i == j || DLB[j] == 0) {
                        continue;
                    }

                    std::vector<int> solucion_vecina = intercambio(solucion_actual, i, j);
                    int coste_vecina = funcionObjetivo(solucion_vecina, flujo, distancia);
                    iteraciones++;

                    if (coste_vecina < coste_mejor_solucion) {
                        coste_mejor_solucion = coste_vecina;
                        mejor_solucion = solucion_vecina;
                        solucion_actual = solucion_vecina;
                        std::fill(DLB.begin(), DLB.end(), 1);  // Resetear DLB
                        DLB[j] = 0;  // Establecer el bit del último elemento intercambiado a 0
                        mejora = true;
                        global_mejora = true;
                        break;  // Salir del bucle de exploración del vecindario
                    }
                }

                if (!mejora) {
                    DLB[i] = 0;
                }
            }

            if (!global_mejora) break;  // Si no se ha encontrado mejora global, finalizar
        }

        std::cout << "\nCoste para semilla " << semilla << ": " << coste_mejor_solucion;

        if (coste_mejor_solucion < coste_mejor_solucion_global) {
            mejor_solucion_global = mejor_solucion;
            coste_mejor_solucion_global = coste_mejor_solucion;
        }
    }

    std::cout << "\n\nMejor resultado global: " << coste_mejor_solucion_global << std::endl;

    std::cout << "| Unidad >> Pos |" << std::endl;
    std::cout << "|---------------|" << std::endl;
    for (int i = 0; i < tamannoMatriz; i++) {
        std::cout << "|   " << std::setw(5) << i + 1
                  << " >> " << std::setw(2) << mejor_solucion_global[i] + 1 << " |" << std::endl;
    }
    std::cout << "|---------------|" << std::endl;

    return {mejor_solucion_global, coste_mejor_solucion_global};
}


