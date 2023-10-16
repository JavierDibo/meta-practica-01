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

// Keywords

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

void tabu_v1(int tamanno_matriz, matriz &flujo, matriz &distancia);

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
        tenencia_tabu *= 2; // Para tener en cuenta los movimientos simetricos
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

int calcular_coste_solucion(const vector &vec, const matriz &flujo, const matriz &distancia) {
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
    archivo_log << iteraciones << "," << i << "," << j << "," << coste_actual << "," << delta << "\n";
}

void escribir_log_DLB(std::ofstream &archivo_log, int iteraciones, int i, int j, int coste_actual, int delta,
                      const std::vector<int> &DLB) {

    archivo_log << iteraciones << "," << i << "," << j << "," << coste_actual << "," << delta << ",";

    for (int dato: DLB) {
        archivo_log << dato;
    }

    archivo_log << "\n";
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

std::ofstream inicializar_log_DLB(int semilla, const string &nombre_archivo, const string &algoritmo) {
    std::filesystem::create_directory("logs");

    std::filesystem::path path(nombre_archivo);
    string nombre = path.stem().string();

    string nombre_log = "logs/" + nombre + "_" + algoritmo + "_" + std::to_string(semilla) + ".csv";

    std::ofstream archivo_log(nombre_log);
    if (archivo_log.is_open()) {
        archivo_log << "Iteracion,Movimiento_i,Movimiento_j,Coste Actual,Delta,DLB\n";
    }

    return archivo_log;
}


int delta_coste(const vector &vec, const matriz &flujo, const matriz &distancia, movimiento mov) {
    size_t n = vec.size();
    int delta = 0;

    int r = mov.first;
    int s = mov.second;

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

void imprimir_resumen_semilla(int semilla, int iteraciones, int coste_actual) {
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

vector vector_aleatorio(int tamanno_matriz, int semilla) {

    vector vec(tamanno_matriz);
    for (int i = 0; i < tamanno_matriz; i++) {
        vec[i] = i;
    }
    std::default_random_engine random(semilla);
    std::shuffle(vec.begin(), vec.end(), random);

    return vec;
}

vector randomizar_DLB(int semilla, int tam, int num_reseteos_DLB) {
    std::default_random_engine random(semilla + num_reseteos_DLB);
    std::bernoulli_distribution distribucion(0.5);
    vector vec;

    for (int i = 0; i < tam; i++) {
        vec.push_back(distribucion(random));
    }

    return vec;
}

bool is_DLB_llena(const vector &DLB) {
    return (std::accumulate(DLB.begin(), DLB.end(), 0) == DLB.size());
}

bool movimiento_en_lista_tabu(const std::vector<movimiento> &lista_tabu, const movimiento &mov) {

    for (const auto &m: lista_tabu) {
        if (m == mov) return true;
    }
    return false;
}

movimiento reverse_mov(movimiento mov) {
    return {mov.second, mov.first};
}

void actualizar_lista_tabu(std::vector<movimiento> &lista_tabu, const movimiento &mov) {

    lista_tabu.push_back(mov);
    lista_tabu.push_back(reverse_mov((mov)));

    if (lista_tabu.size() > tenencia_tabu) {
        lista_tabu.erase(lista_tabu.begin());
    }
}

void realizar_movimiento(vector &vec, const movimiento &mov, std::vector<movimiento> &lista_tabu,
                         matriz &memoria_largo_plazo, int &iteraciones) {

    if (mov.first >= vec.size() || mov.second >= vec.size()) {
        std::cerr << "realizar_movimiento::Error: Indices de movimiento fuera de rango." << std::endl;
        return;
    }

    // Realiza el movimiento
    std::swap(vec[mov.first], vec[mov.second]);
    iteraciones++;

    // Añade el movimiento a la lista tabú
    actualizar_lista_tabu(lista_tabu, mov);

    // Aumenta los indices apropiados
    memoria_largo_plazo[mov.first][mov.second]++;
    memoria_largo_plazo[mov.second][mov.first]++;
}

std::pair<movimiento, int>
generar_vecinos(const vector &solucion, const int tam, const matriz &flujo, const matriz &distancia) {

    int mejor_delta = std::numeric_limits<int>::max();
    movimiento mejor_mov;

    for (int i = 0; i < tam; ++i) {
        for (int j = i + 1; j < tam; ++j) {
            movimiento mov(i, j);
            int delta_actual = delta_coste(solucion, flujo, distancia, mov);
            if (delta_actual < mejor_delta) {
                mejor_delta = delta_actual;
                mejor_mov = mov;
            }
        }
    }

    return {mejor_mov, mejor_delta};
}

bool condicion_estancamiento(const vector &registro_deltas) {

    if (registro_deltas.size() < 50) {
        return false;
    }

    // Comprueba los ultimos 50 elementos
    for (size_t i = registro_deltas.size() - 50; i < registro_deltas.size(); ++i) {
        if (registro_deltas[i] < 0) {
            return false;
        }
    }

    return true;
}

void tabu_v1(int tamanno_matriz, matriz &flujo, matriz &distancia) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    if (echo) { std::cout << "Algoritmo tabu_v1: " << std::endl; }

    // Solucion inicial aleatoria
    vector solucion_inicial = vector_aleatorio(tamanno_matriz, semillas[0]);

    // Loop de semillas
    for (int semilla: semillas) {

        // Iniciacion de variables para el algoritmo
        vector solucion_actual = solucion_inicial;
        int coste_actual = calcular_coste_solucion(solucion_actual, flujo, distancia);
        int iteraciones = 0;
        int num_reseteos_DLB = 0;

        // Estructuras de memoria
        matriz memoria_largo_plazo(tamanno_matriz, vector(tamanno_matriz, 0));
        std::vector<movimiento> lista_tabu;
        vector DLB(tamanno_matriz, 0);

        // Empezar en indice aleatorio sin saltarse ninguno
        vector indices = vector_aleatorio(tamanno_matriz, semilla);

        // Logging
        std::ofstream log_file;
        if (loggear) { log_file = inicializar_log_DLB(semilla, archivo_datos, "tabu"); }

        while (iteraciones < max_iteraciones) {

            while (!is_DLB_llena(DLB)) {
                // Explorar
                for (int index: indices) {
                    int mov_i = index;

                    // Si el movimiento ya esta explorado
                    if (DLB[mov_i] == 1) {
                        continue;
                    }

                    bool mejora = false;

                    // Recorrer el rango de mov_j e mov_i entero
                    int mov_j = (mov_i + 1) % tamanno_matriz;
                    for (int pasos = 0;
                         pasos < tamanno_matriz && mov_i != mov_j; mov_j = (mov_j + 1) % tamanno_matriz, ++pasos) {

                        // Datos actuales
                        movimiento movimiento_actual = std::make_pair(mov_i, mov_j);
                        int delta_actual = delta_coste(solucion_actual, flujo, distancia, movimiento_actual);

                        // Lógica de lista tabu
                        if (movimiento_en_lista_tabu(lista_tabu, movimiento_actual)) {
                            continue;
                        }

                        if (delta_actual < 0) {
                            realizar_movimiento(solucion_actual, movimiento_actual, lista_tabu,
                                                memoria_largo_plazo, iteraciones);
                            coste_actual += delta_actual;
                            DLB[mov_j] = 1;
                            mejora = true;

                            if (loggear) {
                                escribir_log_DLB(log_file, iteraciones, mov_i, mov_j, coste_actual, delta_actual,
                                                 DLB);
                            }

                            vector registro_deltas(max_iteraciones);
                            registro_deltas.emplace_back(delta_actual);




                            // Salir del bucle una vez que se ha realizado un movimiento
                            break;
                        }
                    }

                    // Si hemos explorado las mejores soluciones
                    if (!mejora) {
                        DLB[mov_i] = 1;
                    }
                }
            }

            // Si todos los DLB son 1, moverse hacia el mejor de los peores y generar un DLB aleatorio
            DLB = randomizar_DLB(semilla, tamanno_matriz, num_reseteos_DLB++);

            std::pair<movimiento, int> mejor_vecino =
                    generar_vecinos(solucion_actual, tamanno_matriz, flujo, distancia);

            realizar_movimiento(solucion_actual, mejor_vecino.first, lista_tabu, memoria_largo_plazo, iteraciones);

            int delta_vecino = mejor_vecino.second;
            coste_actual += delta_vecino;
            vector registro_deltas((int) (max_iteraciones * 0.05));
            registro_deltas.emplace_back(delta_vecino);

            if (loggear) {
                escribir_log_DLB(log_file, iteraciones, mejor_vecino.first.first, mejor_vecino.first.second,
                                 coste_actual, delta_vecino, DLB);
            }

            if (condicion_estancamiento(registro_deltas)) {

                //TODO implementar logica de oscilacion estrategica
                std::cout << "Nos hemos estancado" << std::endl;
            }
        }

        // Equivalente a numero de iteraciones vacias
        std::cout << "Numero de veces que se ha resetado la DLB (50%): " << num_reseteos_DLB << std::endl;

        if (loggear) { log_file.close(); }

        if (echo) { imprimir_resumen_semilla(semilla, iteraciones, coste_actual); }
    }

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;
    std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() << " segundos." << std::endl;
}