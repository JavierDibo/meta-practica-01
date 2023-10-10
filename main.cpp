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

using vector = std::vector<int>;
using string = std::string;
using movimiento = std::pair<int, int>;
using matriz = std::vector<std::vector<int>>;
using mapa = std::map<string, string>;

// Variables globales

vector semillas;
bool echo = false;
bool loggear = false;
int max_iteraciones = 0;
int tenencia_tabu = 0;
string archivo_datos;

// Funciones

std::pair<vector, int> tabu_v1(int tamanno_matriz, matriz &flujo, matriz &distancia);

mapa lectura_parametros(const string &nombre_archivo) {

    std::ifstream archivo_parametros(nombre_archivo);
    if (!archivo_parametros.is_open()) {
        std::cerr << "lectura_parametros::No se pudo abrir el archivo " << nombre_archivo << std::endl;
        return {};
    }

    std::map<string, string> parametros;

    string line;
    while (std::getline(archivo_parametros, line)) {
        if (!line.empty() && (line[0] == '#' || (line.size() >= 2 && line[0] == '/' && line[1] == '/'))) {
            continue;
        }

        size_t pos = line.find('=');
        if (pos != string::npos) {
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);
            key.erase(0, key.find_first_not_of(' '));
            key.erase(key.find_last_not_of(' ') + 1);
            value.erase(0, value.find_first_not_of(' '));
            value.erase(value.find_last_not_of(' ') + 1);
            parametros[key] = value;
        }
    }

    archivo_parametros.close();

    if (parametros.find("semilla") != parametros.end()) {
        string seeds_string = parametros["semilla"];
        std::stringstream ss(seeds_string);
        string seed;
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

    if (parametros.find("max_iteraciones") != parametros.end()) {
        max_iteraciones = std::stoi(parametros["max_iteraciones"]);
    }

    if (parametros.find("tenencia_tabu") != parametros.end()) {
        tenencia_tabu = std::stoi(parametros["tenencia_tabu"]);
    }

    archivo_datos = parametros["nombre_del_archivo"];

    std::cout << "Archivo " << nombre_archivo << " leido correctamente. No se ha indicado ninguna funcion conocida";

    return parametros;
}

void leer_matrices(const string &nombre_archivo, int &tamanno_matriz, matriz &flujo, matriz &distancia) {

    std::ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        std::cerr << "leer_matrices::No se pudo abrir el archivo " << nombre_archivo << std::endl;
        return;
    }

    archivo >> tamanno_matriz;
    archivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    flujo = matriz(tamanno_matriz, vector(tamanno_matriz));
    distancia = matriz(tamanno_matriz, vector(tamanno_matriz));

    for (int i = 0; i < tamanno_matriz; ++i) {
        for (int j = 0; j < tamanno_matriz; ++j) {
            archivo >> flujo[i][j];
        }
    }

    archivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (int i = 0; i < tamanno_matriz; ++i) {
        for (int j = 0; j < tamanno_matriz; ++j) {
            archivo >> distancia[i][j];
        }
    }

    archivo.close();

    if (echo) { std::cout << "\nArchivo " << nombre_archivo << " procesado correctamente." << std::endl; }
}

int coste_solucion(const vector &vec, const matriz &flujo, const matriz &distancia) {
    size_t n = vec.size();
    int coste = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            coste += flujo[i][j] * distancia[vec[i]][vec[j]];
        }
    }
    return coste;
}

void escribir_log(std::ofstream &archivo_log, int iteraciones, int i, int j, int coste_actual, int delta) {
    if (loggear)
        archivo_log << iteraciones << "," << i << "," << j << "," << coste_actual << "," << delta << "\n";
}

std::ofstream inicializar_log(int semilla, const string &nombre_archivo, const string &algoritmo) {
    std::filesystem::create_directory("logs");

    std::filesystem::path path(nombre_archivo);
    string nombre = path.stem().string();

    string nombre_log = "logs/" + nombre + "_" + algoritmo + "_" + std::to_string(semilla) + ".csv";

    std::ofstream archivo_log(nombre_log);
    if (archivo_log.is_open()) {
        archivo_log << "Iteracion,Movimiento_i,Movimiento_j,Coste Actual,Delta\n";
    }

    return archivo_log;
}

int delta_coste(const vector &vec, const matriz &flujo, const matriz &distancia, int r, int s) {
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
                                const vector &mejor_solucion) {

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

void lanzar_algoritmo(mapa parametros) {

    int tamanno_matriz;
    matriz flujo, distancia;
    leer_matrices(archivo_datos, tamanno_matriz, flujo, distancia);

    if (parametros["algoritmo"] == "tabu" || parametros["algoritmo"] == "3") {
        tabu_v1(tamanno_matriz, flujo, distancia);
        return;
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre del archivo de parametros>" << std::endl;
        return 1;
    }

    string archivo_parametros = argv[1];

    mapa parametros = lectura_parametros(archivo_parametros);

    lanzar_algoritmo(parametros);

    return 0;
}

std::pair<vector, int> tabu_v1(int tamanno_matriz, matriz &flujo, matriz &distancia) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    if (echo) { std::cout << "Algoritmo tabu_v1: " << std::endl; }

    // Solucion inicial aleatoria
    vector solucion_inicial(tamanno_matriz);
    for (int i = 0; i < tamanno_matriz; i++) {
        solucion_inicial[i] = i;
    }
    std::default_random_engine sol_incial_random(semillas[0]);
    std::shuffle(solucion_inicial.begin(), solucion_inicial.end(), sol_incial_random);

    // Variables
    vector mejor_solucion = solucion_inicial;
    int coste_mejor_solucion = coste_solucion(mejor_solucion, flujo, distancia);
    int mejor_semilla = 0, mejor_iteraciones = 0;

    // Loop de semillas
    for (int semilla: semillas) {

        // Iniciacion de variables para el algoritmo
        vector solucion_actual = solucion_inicial;
        int coste_actual = coste_solucion(solucion_actual, flujo, distancia), iteraciones = 0;

        matriz memoria_largo_plazo(tamanno_matriz, vector(tamanno_matriz, 0));

        std::vector<movimiento> lista_tabu;

        vector DLB(tamanno_matriz, 0), indices(tamanno_matriz);

        // Empezar en indice aleatorio sin saltarse ninguno
        for (int i = 0; i < tamanno_matriz; i++) {
            indices[i] = i;
        }
        std::default_random_engine random(semilla);
        std::shuffle(indices.begin(), indices.end(), random);

        // Logging
        std::ofstream log_file;
        if (loggear) { log_file = inicializar_log(semilla, archivo_datos, "tabu"); }

        // Mientras la DLB tenga candidatos || queden iteraciones
        while (std::accumulate(DLB.begin(), DLB.end(), 0) < tamanno_matriz && iteraciones < max_iteraciones) {
            for (int index: indices) {
                int i = index;

                if (DLB[i] == 1) {
                    continue;
                }

                bool mejora = false;
                int pasos = 0;
                for (int j = (i + 1) % tamanno_matriz; pasos < tamanno_matriz; j = (j + 1) % tamanno_matriz, ++pasos) {
                    int delta = delta_coste(solucion_actual, flujo, distancia, i, j);
                    // int movimientos_empeoramiento = 0;

                    // Lógica para movimientos de empeoramiento
                    if (std::accumulate(DLB.begin(), DLB.end(), 0) == tamanno_matriz) {
                        // realizar_el_mejor_de_los_peores_movimientos(solucion_actual, flujo, distancia, coste_actual);
                        // reiniciar_DLB(DLB);
                        // movimientos_empeoramiento++;
                    }

                    // Lógica para reinicio con memoria a largo plazo
                    /*if (movimientos_de_empeoramiento_durante_5_por_ciento_de_iteraciones(movimientos_empeoramiento,
                                                                                         iteraciones)) {
                        reiniciar_con_memoria_largo_plazo(solucion_actual, memoria_largo_plazo);
                        movimientos_empeoramiento = 0;
                    }*/

                    // movimiento movimiento_actual = std::make_pair(i, j);

                    // Lógica de lista tabú
                    if (// !movimiento_en_lista_tabu(lista_tabu, movimiento_actual) ||
                            (delta < 0)) {

                        std::swap(solucion_actual[i], solucion_actual[j]);
                        iteraciones++;
                        coste_actual += delta;
                        DLB[j] = 0;
                        mejora = true;

                        if (loggear) { escribir_log(log_file, iteraciones, i, j, coste_actual, delta); }

                        if (coste_actual < coste_mejor_solucion) {
                            mejor_solucion = solucion_actual;
                            coste_mejor_solucion = coste_actual;
                            mejor_semilla = semilla;
                            mejor_iteraciones = iteraciones;
                        }

                        // Añade el movimiento a la lista tabú
                        // actualizar_lista_tabu(lista_tabu, movimiento_actual);

                        break;  // Sale del bucle una vez que se ha realizado un movimiento
                    }
                }

                if (!mejora) {
                    DLB[i] = 1;
                }
            }
        }

        if (loggear) { log_file.close(); }

        if (echo) { imprimir_resumen_semilla_PM(semilla, iteraciones, coste_actual); }
    }

    if (echo) { imprimir_resumen_global_PM(coste_mejor_solucion, mejor_semilla, mejor_iteraciones, mejor_solucion); }

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;
    std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() << " segundos." << std::endl;

    return {mejor_solucion, coste_mejor_solucion};
}

