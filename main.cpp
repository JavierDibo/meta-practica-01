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
#include <filesystem>
#include <deque>

// Variables globales

std::vector<int> semillas;
bool echo = false;
bool loggear = false;
int numIteraciones = 0;
int tenenciaTabu = 0;

// Funciones
void lectura_parametros(const std::string &nombre_archivo);

std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>>
ingesta_datos(const std::string &nombre_archivo, int &tamanno_matriz);

void algoritmo_greedy(std::string &nombre_archivo);

std::pair<std::vector<int>, int> primero_mejor_DLB(const std::string &nombre_archivo);

int funcion_objetivo(const std::vector<int> &vec, const std::vector<std::vector<int>> &flujo,
                     const std::vector<std::vector<int>> &distancia);

void escribir_log(std::ofstream &archivo_log, int iteraciones, int i, int j, int coste_actual, int delta);

std::ofstream inicializar_log(int semilla, const std::string &nombre_archivo);

int delta_coste(const std::vector<int> &vec, const std::vector<std::vector<int>> &flujo,
                const std::vector<std::vector<int>> &distancia, int r, int s);

void imprimir_resumen_semilla_PM(int semilla, int iteraciones, int coste_actual);

void imprimir_resumen_global_PM(int coste_mejor_solucion, int mejor_semilla, int mejor_iteraciones,
                                const std::vector<int> &mejor_solucion);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre del archivo de parametros>" << std::endl;
        return 1;
    }

    std::string archivo_parametros = argv[1];

    lectura_parametros(archivo_parametros);

    return 0;
}

std::pair<std::vector<int>, int> tabu_martes(const std::string &nombre_archivo) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    if (echo) {
        std::cout << "Algoritmo Tabu: " << std::endl;
    }

    int tamanno_matriz;
    auto matrices = ingesta_datos(nombre_archivo, tamanno_matriz);
    std::vector<std::vector<int>> flujo = matrices.first;
    std::vector<std::vector<int>> distancia = matrices.second;

    std::vector<int> solucion_inicial(tamanno_matriz);
    for (int i = 0; i < tamanno_matriz; i++) {
        solucion_inicial[i] = tamanno_matriz - i - 1;
    }

    std::vector<int> mejor_solucion = solucion_inicial;
    int coste_mejor_solucion = funcion_objetivo(mejor_solucion, flujo, distancia);
    int mejor_semilla = 0;
    int mejor_iteraciones = 0;

    for (int semilla: semillas) {
        std::vector<int> solucion_actual = solucion_inicial;
        int coste_actual = funcion_objetivo(solucion_actual, flujo, distancia);

        std::vector<int> DLB(tamanno_matriz, 0);
        std::vector<std::vector<int>> memoria_largo_plazo(tamanno_matriz, std::vector<int>(tamanno_matriz, 0));
        std::deque<std::pair<int, int>> tabu_list(7, {-1, -1});  // Inicializamos con pares dummy
        std::vector<std::vector<int>> matriz_tabu(tamanno_matriz, std::vector<int>(tamanno_matriz, 0));

        int iteraciones = 0;

        std::vector<int> indices(tamanno_matriz);
        for (int i = 0; i < tamanno_matriz; i++) {
            indices[i] = i;
        }

        std::default_random_engine random(semilla);
        std::shuffle(indices.begin(), indices.end(), random);

        std::ofstream log_file;
        if (loggear)
            log_file = inicializar_log(semilla, nombre_archivo);

        while (std::accumulate(DLB.begin(), DLB.end(), 0) < tamanno_matriz && iteraciones < numIteraciones) {
            for (int index: indices) {
                int i = index;

                if (DLB[i] == 1) {
                    continue;
                }

                bool mejora = false;
                for (int j = i + 1; j < tamanno_matriz; ++j) {
                    int delta = delta_coste(solucion_actual, flujo, distancia, i, j);

                    if (delta < 0) {
                        std::swap(solucion_actual[i], solucion_actual[j]);
                        iteraciones++;
                        coste_actual += delta;
                        DLB[j] = 0;
                        mejora = true;

                        if (loggear) {
                            escribir_log(log_file, iteraciones, i, j, coste_actual, delta);
                        }

                        if (coste_actual < coste_mejor_solucion) {
                            mejor_solucion = solucion_actual;
                            coste_mejor_solucion = coste_actual;
                            mejor_semilla = semilla;
                            mejor_iteraciones = iteraciones;
                        }
                        break;
                    }
                }

                if (!mejora) {
                    DLB[i] = 1;
                }
            }
        }

        if(std::accumulate(DLB.begin(), DLB.end(), 0) == tamanno_matriz) {
            int mejor_peor_coste = INT_MAX;
            std::pair<int, int> mejor_peor_movimiento = {-1, -1};

            for(int i = 0; i < tamanno_matriz; ++i) {
                for(int j = i + 1; j < tamanno_matriz; ++j) {

                    // Si esta en la lista
                    if(std::find(tabu_list.begin(), tabu_list.end(), std::make_pair(i, j)) != tabu_list.end()) {
                        continue;
                    }

                    int delta = delta_coste(solucion_actual, flujo, distancia, i, j);

                    if(delta > 0 && delta < mejor_peor_coste) {
                        mejor_peor_coste = delta;
                        mejor_peor_movimiento = {i, j};
                    }
                }
            }

            if(mejor_peor_movimiento.first != -1 && mejor_peor_movimiento.second != -1) {
                std::swap(solucion_actual[mejor_peor_movimiento.first], solucion_actual[mejor_peor_movimiento.second]);
                coste_actual += mejor_peor_coste;
                iteraciones++;

                if(tabu_list.size() >= tenenciaTabu) {
                    tabu_list.pop_front();
                }
                tabu_list.push_back(mejor_peor_movimiento);
            }

            std::bernoulli_distribution distribucion(0.5);

            for(int i = 0; i < tamanno_matriz; ++i) {
                DLB[i] = distribucion(random) ? 1 : 0;
            }
        }

        if (loggear)
            log_file.close();

        if (echo) {
            imprimir_resumen_semilla_PM(semilla, iteraciones, coste_actual);
        }
    }

    if (echo)
        imprimir_resumen_global_PM(coste_mejor_solucion, mejor_semilla, mejor_iteraciones, mejor_solucion);

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;
    std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() << " segundos." << std::endl;

    return {mejor_solucion, coste_mejor_solucion};
}


std::pair<std::vector<int>, int> primero_mejor_DLB(const std::string &nombre_archivo) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    if (echo) {
        std::cout << "Algoritmo Primero el Mejor (DLB): " << std::endl;
    }

    int tamanno_matriz;
    auto matrices = ingesta_datos(nombre_archivo, tamanno_matriz);

    std::vector<std::vector<int>> flujo = matrices.first;
    std::vector<std::vector<int>> distancia = matrices.second;

    std::vector<int> solucion_inicial(tamanno_matriz);
    for (int i = 0; i < tamanno_matriz; i++) {
        solucion_inicial[i] = tamanno_matriz - i - 1;
    }

    std::vector<int> mejor_solucion = solucion_inicial;
    int coste_mejor_solucion = funcion_objetivo(mejor_solucion, flujo, distancia);
    int mejor_semilla = 0;
    int mejor_iteraciones = 0;

    for (int semilla: semillas) {
        std::vector<int> solucion_actual = solucion_inicial;
        int coste_actual = funcion_objetivo(solucion_actual, flujo, distancia);

        std::vector<int> DLB(tamanno_matriz, 0);
        int iteraciones = 0;

        std::vector<int> indices(tamanno_matriz);
        for (int i = 0; i < tamanno_matriz; i++) {
            indices[i] = i;
        }

        std::default_random_engine random(semilla);
        std::shuffle(indices.begin(), indices.end(), random);

        std::ofstream log_file;
        if (loggear)
            log_file = inicializar_log(semilla, nombre_archivo);

        while (std::accumulate(DLB.begin(), DLB.end(), 0) < tamanno_matriz && iteraciones < 1000) {
            for (int index: indices) {
                int i = index;

                if (DLB[i] == 1) {
                    continue;
                }

                bool mejora = false;
                int pasos = 0;
                for (int j = (i + 1) % tamanno_matriz; pasos < tamanno_matriz; j = (j + 1) % tamanno_matriz, ++pasos) {
                    int delta = delta_coste(solucion_actual, flujo, distancia, i, j);

                    if (delta < 0) {
                        std::swap(solucion_actual[i], solucion_actual[j]);
                        iteraciones++;
                        coste_actual += delta;
                        DLB[j] = 0;
                        mejora = true;

                        if (loggear) {
                            escribir_log(log_file, iteraciones, i, j, coste_actual, delta);
                        }

                        if (coste_actual < coste_mejor_solucion) {
                            mejor_solucion = solucion_actual;
                            coste_mejor_solucion = coste_actual;
                            mejor_semilla = semilla;
                            mejor_iteraciones = iteraciones;
                        }
                        break;
                    }
                }

                if (!mejora) {
                    DLB[i] = 1;
                }
            }
        }

        if (loggear)
            log_file.close();

        if (echo) {
            imprimir_resumen_semilla_PM(semilla, iteraciones, coste_actual);
        }
    }

    if (echo)
        imprimir_resumen_global_PM(coste_mejor_solucion, mejor_semilla, mejor_iteraciones, mejor_solucion);

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;
    std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() << " segundos." << std::endl;

    return {mejor_solucion, coste_mejor_solucion};
}


void lectura_parametros(const std::string &nombre_archivo) {

    std::ifstream archivo_parametros(nombre_archivo);
    if (!archivo_parametros.is_open()) {
        std::cerr << "lectura_parametros::No se pudo abrir el archivo " << nombre_archivo << std::endl;
        return;
    }

    std::map<std::string, std::string> parametros;

    std::string line;
    while (std::getline(archivo_parametros, line)) {
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

    archivo_parametros.close();

    if (parametros.find("semilla") != parametros.end()) {
        std::string seeds_string = parametros["semilla"];
        std::stringstream ss(seeds_string);
        std::string seed;
        while (std::getline(ss, seed, ',')) {
            semillas.push_back(std::stoi(seed));
        }
    }

    if (parametros.find("echo") != parametros.end()) {
        echo = (parametros["echo"] == "true");
    }

    if (parametros.find("log") != parametros.end()) {
        loggear = (parametros["log"] == "true");
    }

    if (parametros.find("numIteraciones") != parametros.end()) {
        numIteraciones = std::stoi(parametros["numIteraciones"]);
    }

    if (parametros.find("tenenciaTabu") != parametros.end()) {
        tenenciaTabu = std::stoi(parametros["tenenciaTabu"]);
    }

    std::string archivo_datos = parametros["nombre_del_archivo"];

    if (parametros["algoritmo"] == "greedy" || parametros["algoritmo"] == "1") {
        algoritmo_greedy(archivo_datos);
        return;
    }

    if (parametros["algoritmo"] == "pm" || parametros["algoritmo"] == "2") {
        primero_mejor_DLB(archivo_datos);
        return;
    }

    if (parametros["algoritmo"] == "tabu" || parametros["algoritmo"] == "3") {
        tabu_martes(archivo_datos);
        return;
    }



    std::cout << "Archivo " << nombre_archivo << " leido correctamente. No se ha indicado ninguna funcion conocida";
}

std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>>
ingesta_datos(const std::string &nombre_archivo, int &tamanno_matriz) {
    std::ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        std::cerr << "ingesta_datos::No se pudo abrir el archivo " << nombre_archivo << std::endl;
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

    if (echo)
        std::cout << "\nArchivo " << nombre_archivo << " procesado correctamente." << std::endl;

    return std::make_pair(flujo, distancias);
}

void algoritmo_greedy(std::string &nombre_archivo) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    if (echo) {
        std::cout << "\nResultados del algoritmo Greedy" << std::endl << "---------------------------------"
                  << std::endl;
    }

    int tamanno_matriz;
    auto matrices = ingesta_datos(nombre_archivo, tamanno_matriz);
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

    std::vector<int> solucion(tamanno_matriz);
    for (int i = 0; i < flujos_indizados.size(); i++) {
        solucion[flujos_indizados[i].second] = distancias_indizadas[i].second;
    }

    int coste = funcion_objetivo(solucion, flujo, distancias);

    if (echo) {
        std::cout << "Coste de la solucion: " << coste << std::endl;

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

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;
    if (echo) {
        std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() << " segundos." << std::endl;
    }
}

int funcion_objetivo(const std::vector<int> &vec, const std::vector<std::vector<int>> &flujo,
                     const std::vector<std::vector<int>> &distancia) {
    size_t n = vec.size();
    int coste = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            coste += flujo[i][j] * distancia[vec[i]][vec[j]];
        }
    }
    return coste;
}

int delta_coste(const std::vector<int> &vec, const std::vector<std::vector<int>> &flujo,
                const std::vector<std::vector<int>> &distancia, int r, int s) {
    size_t n = vec.size();
    int delta = 0;

    for (int k = 0; k < n; ++k) {
        if (k != r && k != s) {
            delta += flujo[r][k] * (distancia[vec[s]][vec[k]] - distancia[vec[r]][vec[k]])
                     + flujo[s][k] * (distancia[vec[r]][vec[k]] - distancia[vec[s]][vec[k]])
                     + flujo[k][r] * (distancia[vec[k]][vec[s]] - distancia[vec[k]][vec[r]])
                     + flujo[k][s] * (distancia[vec[k]][vec[r]] - distancia[vec[k]][vec[s]]);
        }
    }
    return delta;
}

void imprimir_resumen_semilla_PM(int semilla, int iteraciones, int coste_actual) {
    std::cout << "Semilla: " << semilla << std::endl;
    std::cout << "Numero de iteraciones: " << iteraciones << std::endl;
    std::cout << "Coste de la solucion para esta semilla: " << coste_actual << std::endl;
    std::cout << "---------------------------------------" << std::endl;
}

void imprimir_resumen_global_PM(int coste_mejor_solucion, int mejor_semilla, int mejor_iteraciones,
                                const std::vector<int> &mejor_solucion) {

    std::cout << "\nCoste de la mejor solucion: " << coste_mejor_solucion << std::endl;
    std::cout << "Mejor semilla: " << mejor_semilla << std::endl;
    std::cout << "Numero de iteraciones de la mejor solucion: " << mejor_iteraciones << std::endl;
    std::cout << "| Unidad >> Pos |" << std::endl;
    std::cout << "|---------------|" << std::endl;

    for (int i = 0; i < mejor_solucion.size(); i++) {
        std::cout << "|   " << std::setw(5) << i + 1
                  << " >> " << std::setw(2) << mejor_solucion[i] + 1 << " |" << std::endl;
    }
    std::cout << "|---------------|" << std::endl;
}

std::ofstream inicializar_log(int semilla, const std::string &nombre_archivo) {
    std::filesystem::create_directory("logs");

    std::filesystem::path path(nombre_archivo);
    std::string nombre = path.stem().string();

    std::string nombre_log = "logs/" + nombre + "_PM_" + std::to_string(semilla) + ".csv";

    std::ofstream archivo_log(nombre_log);
    if (archivo_log.is_open()) {
        archivo_log << "Iteracion,Movimiento_i,Movimiento_j,Coste Actual,Delta\n";
    }

    return archivo_log;
}

void escribir_log(std::ofstream &archivo_log, int iteraciones, int i, int j, int coste_actual, int delta) {
    if (loggear)
        archivo_log << iteraciones << "," << i << "," << j << "," << coste_actual << "," << delta << "\n";
}