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

// Variables globales

std::vector<int> semillas;

// Funciones
void lecturaParametros(const std::string &nombre_archivo);

std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>>
ingestaDeDatos(const std::string &nombre_archivo, int &tamanno_matriz);

void algoritmoGreedy(std::string &nombreArchivo);

std::pair<std::vector<int>, int> PrimeroMejorDLB(const std::string &nombre_archivo);

std::pair<std::vector<int>, int> PrimeroMejorDBLRandom(const std::string &nombre_archivo);

int funcion_objetivo(const std::vector<int> &p, const std::vector<std::vector<int>> &flujo,
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

void lecturaParametros(const std::string &nombre_archivo) {

    std::ifstream paramFile(nombre_archivo);
    if (!paramFile.is_open()) {
        std::cerr << "lecturaParametros::No se pudo abrir el archivo " << nombre_archivo << std::endl;
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

    std::string archivo_datos = parametros["nombre_del_archivo"];

    if (parametros["algoritmo"] == "greedy" || parametros["algoritmo"] == "1") {
        algoritmoGreedy(archivo_datos);
        return;
    }

    if (parametros["algoritmo"] == "bit" || parametros["algoritmo"] == "2") {
        PrimeroMejorDLB(archivo_datos);
        return;
    }

    if (parametros["algoritmo"] == "bitr" || parametros["algoritmo"] == "3") {
        PrimeroMejorDBLRandom(archivo_datos);
        return;
    }

    std::cout << "Archivo " << nombre_archivo << " leido correctamente. No se ha indicado ninguna funcion conocida";
}

std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>>
ingestaDeDatos(const std::string &nombre_archivo, int &tamanno_matriz) {
    std::ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        std::cerr << "ingestaDeDatos::No se pudo abrir el archivo " << nombre_archivo << std::endl;
        return std::make_pair(std::vector<std::vector<int>>(), std::vector<std::vector<int>>());
    }

    archivo >> tamanno_matriz;
    archivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<std::vector<int>> flujo(tamanno_matriz, std::vector<int>(tamanno_matriz));
    std::vector<std::vector<int>> distancias(tamanno_matriz, std::vector<int>(tamanno_matriz));

    for (int i = 0; i < tamanno_matriz; ++i) {
        for (int j = 0; j < tamanno_matriz; ++j) {
            archivo >> flujo[i][j];
        }
    }

    archivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (int i = 0; i < tamanno_matriz; ++i) {
        for (int j = 0; j < tamanno_matriz; ++j) {
            archivo >> distancias[i][j];
        }
    }

    archivo.close();

    std::cout << "\nArchivo " << nombre_archivo << " procesado correctamente." << std::endl;

    return std::make_pair(flujo, distancias);
}

void algoritmoGreedy(std::string &nombreArchivo) {

    std::cout << "\nResultados del algoritmo Greedy" << std::endl << "---------------------------------"
              << std::endl;

    int tamanno_matriz;
    auto matrices = ingestaDeDatos(nombreArchivo, tamanno_matriz);
    auto flujo = matrices.first;
    auto distancias = matrices.second;

    std::vector<int> sumatorio_flujos(tamanno_matriz, 0);
    std::vector<int> sumatorio_distancias(tamanno_matriz, 0);

    for (int i = 0; i < tamanno_matriz; ++i) {
        for (int j = 0; j < tamanno_matriz; ++j) {
            sumatorio_flujos[i] += flujo[i][j];
            sumatorio_distancias[i] += distancias[i][j];
        }
    }

    auto funcion_ordenar = [](const std::vector<int> &sumas, bool descending = false) {
        std::vector<std::pair<int, int>> sumas_indizadas;
        for (int i = 0; i < sumas.size(); i++) {
            sumas_indizadas.emplace_back(sumas[i], i);
        }
        if (descending) {
            std::sort(sumas_indizadas.begin(), sumas_indizadas.end(),
                      [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
                          return a.first > b.first;
                      });
        } else {
            std::sort(sumas_indizadas.begin(), sumas_indizadas.end());
        }
        return sumas_indizadas;
    };

    auto flujos_indizados = funcion_ordenar(sumatorio_flujos);
    auto distancias_indizadas = funcion_ordenar(sumatorio_distancias, true);

    std::cout << "| Unidad >> Pos | Flujo | Dist |" << std::endl;
    std::cout << "|---------------|-------|------|" << std::endl;

    for (int i = 0; i < flujos_indizados.size(); i++) {
        std::cout << "| " << std::setw(6) << flujos_indizados[i].second + 1
                  << " >> " << std::setw(3) << distancias_indizadas[i].second + 1
                  << " | " << std::setw(5) << flujos_indizados[i].first
                  << " | " << std::setw(4) << distancias_indizadas[i].first << " |" << std::endl;
    }
    std::cout << "|------------------------------|" << std::endl;
}

int funcion_objetivo(const std::vector<int> &p, const std::vector<std::vector<int>> &flujo,
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

int delta_coste(const std::vector<int> &p, const std::vector<std::vector<int>> &flujo,
                const std::vector<std::vector<int>> &distancia, int r, int s) {
    size_t n = p.size();
    int delta = 0;

    for (int k = 0; k < n; ++k) {
        if (k != r && k != s) {
            delta += flujo[r][k] * (distancia[p[s]][p[k]] - distancia[p[r]][p[k]])
                     + flujo[s][k] * (distancia[p[r]][p[k]] - distancia[p[s]][p[k]])
                     + flujo[k][r] * (distancia[p[k]][p[s]] - distancia[p[k]][p[r]])
                     + flujo[k][s] * (distancia[p[k]][p[r]] - distancia[p[k]][p[s]]);
        }
    }
    return delta;
}


std::vector<int> intercambio(const std::vector<int> &p, int r, int s) {
    std::vector<int> p_copia = p;
    std::swap(p_copia[r], p_copia[s]);
    return p_copia;
}

std::pair<std::vector<int>, int> PrimeroMejorDLB(const std::string &nombre_archivo) {

    std::cout << "\nResultados del algoritmo PrimeroMejorDLB" << std::endl << "--------------------------------"
              << std::endl;

    int tamanno_matriz;
    auto matrices = ingestaDeDatos(nombre_archivo, tamanno_matriz);

    std::vector<std::vector<int>> flujo = matrices.first;
    std::vector<std::vector<int>> distancia = matrices.second;

    // Solucion "estandar"
    std::vector<int> solucion_inicial(tamanno_matriz);
    for (int i = 0; i < tamanno_matriz; i++) {
        solucion_inicial[i] = i;
    }

    // Solucion actual y mejor solucion encontrada
    std::vector<int> solucion_actual = solucion_inicial;
    std::vector<int> mejor_solucion = solucion_inicial;
    int coste_actual = funcion_objetivo(solucion_actual, flujo, distancia);
    int coste_mejor_solucion = coste_actual;

    // Inicializar mascara DLB con todos los bits a 1
    std::vector<int> DLB(tamanno_matriz, 1);

    int iteraciones = 0;

    while (std::accumulate(DLB.begin(), DLB.end(), 0) > 0 && iteraciones < 1000) {
        // Bucle de recorrido
        for (int i = 0; i < tamanno_matriz; ++i) {
            // Si el bit DLB para el elemento i esta a 0, lo saltamos
            if (DLB[i] == 0) {
                continue;
            }

            // Bucle de intercambio
            bool mejora = false;
            for (int j = 0; j < tamanno_matriz; ++j) {
                if (i == j) {
                    continue;
                }

                int delta = delta_coste(solucion_actual, flujo, distancia, i, j);
                iteraciones++;

                if (delta < 0) {  // Si el intercambio mejora el coste
                    std::swap(solucion_actual[i], solucion_actual[j]);
                    coste_actual += delta;
                    std::fill(DLB.begin(), DLB.end(), 1);  // Resetear DLB
                    DLB[j] = 1;  // Establecer el bit del último elemento intercambiado a 1
                    mejora = true;
                    if (coste_actual < coste_mejor_solucion) {
                        mejor_solucion = solucion_actual;
                        coste_mejor_solucion = coste_actual;
                    }
                    break;  // Salir del bucle de exploración del vecindario
                }
            }

            if (!mejora) {
                DLB[i] = 0;
            }
        }
    }

    std::cout << "\nCoste de la mejor solucion: " << coste_mejor_solucion << std::endl;
    std::cout << "| Unidad >> Pos |" << std::endl;
    std::cout << "|---------------|" << std::endl;

    for (int i = 0; i < tamanno_matriz; i++) {
        std::cout << "|   " << std::setw(5) << i + 1
                  << " >> " << std::setw(2) << mejor_solucion[i] + 1 << " |" << std::endl;
    }
    std::cout << "|---------------|" << std::endl;

    return {mejor_solucion, coste_mejor_solucion};
}

std::pair<std::vector<int>, int> PrimeroMejorDBLRandom(const std::string &nombre_archivo) {


    std::cout << "\nResultados del algoritmo PrimeroMejorDBLRandom" << std::endl << "--------------------------------"
              << std::endl;

    int tamanno_matriz;
    auto matrices = ingestaDeDatos(nombre_archivo, tamanno_matriz);

    std::vector<std::vector<int>> flujo = matrices.first;
    std::vector<std::vector<int>> distancia = matrices.second;

    std::vector<int> solucion_inicial(tamanno_matriz);
    for (int i = 0; i < tamanno_matriz; i++) {
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
        int coste_mejor_solucion = funcion_objetivo(mejor_solucion, flujo, distancia);

        // Inicializa la mascara DLB con todos los bits a 1
        std::vector<int> DLB(tamanno_matriz, 1);

        std::uniform_int_distribution<> dis(0, tamanno_matriz - 1);

        int iteraciones = 0;
        while (iteraciones < 1000) {
            bool global_mejora = false;
            int inicio = dis(gen);  // Genera un índice aleatorio como punto de inicio
            for (int k = 0; k < tamanno_matriz; ++k) {
                int i = (inicio + k) % tamanno_matriz;  // Calcula el índice actual de manera cíclica
                if (DLB[i] == 0) {
                    continue;
                }

                bool mejora = false;
                for (int j = 0; j < tamanno_matriz; ++j) {
                    if (i == j || DLB[j] == 0) {
                        continue;
                    }

                    std::vector<int> solucion_vecina = intercambio(solucion_actual, i, j);
                    int coste_vecina = funcion_objetivo(solucion_vecina, flujo, distancia);
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
        std::cout << "\n\nNumero de iteraciones: " << iteraciones << std::endl;

        std::cout << "\nCoste para semilla " << semilla << ": " << coste_mejor_solucion;

        if (coste_mejor_solucion < coste_mejor_solucion_global) {
            mejor_solucion_global = mejor_solucion;
            coste_mejor_solucion_global = coste_mejor_solucion;
        }
    }

    std::cout << "\n\nMejor resultado global: " << coste_mejor_solucion_global << std::endl;

    std::cout << "| Unidad >> Pos |" << std::endl;
    std::cout << "|---------------|" << std::endl;
    for (int i = 0; i < tamanno_matriz; i++) {
        std::cout << "|   " << std::setw(5) << i + 1
                  << " >> " << std::setw(2) << mejor_solucion_global[i] + 1 << " |" << std::endl;
    }
    std::cout << "|---------------|" << std::endl;

    return {mejor_solucion_global, coste_mejor_solucion_global};
}