#include <cmath>
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

// Typedefs

using vector = std::vector<int>;
using string = std::string;
using movimiento = std::pair<int, int>;
using matriz = std::vector<std::vector<int>>;
using mapa = std::map<string, string>;

// Variables globales

vector semillas;
bool ECHO = false;
bool LOG = false;
int TENENCIA_TABU = 0;
string archivo_datos;
int ITERACIONES_PARA_ESTANCAMIENTO;
double PORCENTAJE_ESTANCAMIENTO;
int MAX_ITERACIONES = 0;
int INTENSIFICAR = 1;
int DIVERSIFICAR = 0;
int NUMERO_MAX_VECINOS;
int INFINITO_POSITIVO = std::numeric_limits<int>::max();
int INFINITO_NEGATIVO = std::numeric_limits<int>::min();

// Funciones

void tabu_mar(int tamanno_matriz, matriz &flujo, matriz &distancia);

void algoritmo_greedy(int tamanno_matriz, const matriz &flujo, const matriz &distancia);

mapa lectura_parametros(const string &nombre_archivo);

void leer_matrices(const string &nombre_archivo, int &tamanno_matriz, matriz &flujo, matriz &distancia);

int calcular_coste_solucion(const vector &vec, const matriz &flujo, const matriz &distancia);

void escribir_log(std::ofstream &archivo_log, int iteraciones, int i, int j, int coste_actual, int delta);

void escribir_log_DLB(std::ofstream &archivo_log, int iteraciones, int i, int j, int coste_actual, int delta,
                      const std::vector<int> &DLB, const string &tag);

std::ofstream inicializar_log(int semilla, const string &nombre_archivo, const string &algoritmo);

std::ofstream inicializar_log_DLB(int semilla, const string &nombre_archivo, const string &algoritmo);

int delta_coste(const vector &vec, const matriz &flujo, const matriz &distancia, movimiento mov);

void imprimir_resumen_semilla(int semilla, int iteraciones, int coste_actual);

void imprimir_resumen_global_PM(int coste_mejor_solucion, int mejor_semilla, int mejor_iteraciones,
                                const vector &mejor_solucion);

void lanzar_algoritmo(mapa parametros);

vector vector_aleatorio(int tamanno_matriz, int semilla);

vector randomizar_DLB(int semilla, int tam, int &num_reseteos_DLB);

int random_cero_uno(int semilla, int veces);

bool is_DLB_llena(const vector &DLB);

movimiento reverse_mov(movimiento mov);

bool movimiento_en_lista_tabu(const std::vector<movimiento> &lista_tabu, const movimiento &mov);

void actualizar_lista_tabu(std::vector<movimiento> &lista_tabu, const movimiento &mov);

void realizar_movimiento(vector &vec, const movimiento &mov, std::vector<movimiento> &lista_tabu,
                         matriz &memoria_largo_plazo, int &iteraciones);

std::pair<movimiento, int>
generar_vecinos(const vector &solucion, int tam, const matriz &flujo, const matriz &distancia,
                const std::vector<movimiento> &lista_tabu);

bool condicion_estancamiento(const vector &registro_costes, const std::pair<vector, int> &mejor_local);

void actualizar_registro_costes(int ultimo_coste, vector &registro_costes);

void resetar_memoria(std::vector<movimiento> &lista_tabu, matriz &memoria_largo_plazo);

vector iniciar_registro_costes();

vector diversificar(const matriz &memoria_largo_plazo, const int &semilla, const int &veces, const int &tam);

vector intensificar(const matriz &memoria_largo_plazo, const int &semilla, const int &veces, const int &tam);

std::vector<movimiento> funcion_ordenar_greedy(const vector &sumas, bool descending = false);

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

void algoritmo_greedy(int tamanno_matriz, const matriz &flujo, const matriz &distancia) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    if (ECHO) { std::cout << "\nAlgoritmo greedy:" << std::endl; }

    std::vector<int> sumatorio_flujos(tamanno_matriz, 0);
    std::vector<int> sumatorio_distancias(tamanno_matriz, 0);

    for (int i = 0; i < tamanno_matriz; ++i) {
        for (int j = 0; j < tamanno_matriz; ++j) {
            sumatorio_flujos[i] += flujo[i][j];
            sumatorio_distancias[i] += distancia[i][j];
        }
    }

    auto flujos_indizados = funcion_ordenar_greedy(sumatorio_flujos);
    auto distancias_indizadas = funcion_ordenar_greedy(sumatorio_distancias, true);

    std::vector<int> solucion(tamanno_matriz);
    for (int i = 0; i < flujos_indizados.size(); i++) {
        solucion[flujos_indizados[i].second] = distancias_indizadas[i].second;
    }

    int coste = calcular_coste_solucion(solucion, flujo, distancia);

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;

    if (ECHO) { std::cout << "Coste: " << coste << std::endl; }
    if (ECHO) { std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() << " segundos." << std::endl; }
}

void lanzar_algoritmo(mapa parametros) {

    int tamanno_matriz;
    matriz flujo, distancia;
    leer_matrices(archivo_datos, tamanno_matriz, flujo, distancia);

    if (parametros["algoritmo"] == "tabu" || parametros["algoritmo"] == "3") {
        tabu_mar(tamanno_matriz, flujo, distancia);
        return;
    }

    if (parametros["algoritmo"] == "greedy" || parametros["algoritmo"] == "1") {
        algoritmo_greedy(tamanno_matriz, flujo, distancia);
        return;
    }

    if (parametros["algoritmo"] == "pm" || parametros["algoritmo"] == "2") {
        tabu_mar(tamanno_matriz, flujo, distancia);
        return;
    }
}


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
        ECHO = (parametros["echo"] == "true");
    }

    if (parametros.find("log") != parametros.end()) {
        LOG = (parametros["log"] == "true");
    }

    if (parametros.find("max_iteraciones") != parametros.end()) {
        MAX_ITERACIONES = std::stoi(parametros["max_iteraciones"]);
    }

    if (parametros.find("tenencia_tabu") != parametros.end()) {
        TENENCIA_TABU = std::stoi(parametros["tenencia_tabu"]);
        TENENCIA_TABU *= 2; // Para tener en cuenta los movimientos simetricos
    }

    if (parametros.find("num_max_vecinos") != parametros.end()) {
        NUMERO_MAX_VECINOS = std::stoi(parametros["num_max_vecinos"]);
    }

    if (parametros.find("porcetanje_estancamiento") != parametros.end()) {
        PORCENTAJE_ESTANCAMIENTO = std::stof(parametros["porcetanje_estancamiento"]);
        ITERACIONES_PARA_ESTANCAMIENTO = static_cast<int>(std::round(MAX_ITERACIONES * PORCENTAJE_ESTANCAMIENTO));
    }

    archivo_datos = parametros["nombre_del_archivo"];

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

    if (ECHO) { std::cout << "\nArchivo " << nombre_archivo << " procesado correctamente." << std::endl; }
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
                      const std::vector<int> &DLB, const string &tag) {

    archivo_log << iteraciones << "," << i << "," << j << "," << coste_actual << "," << delta << "," + tag + ",";

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
        archivo_log << "Iteracion,Movimiento_i,Movimiento_j,Coste Actual,Delta,Tag\n";
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
        archivo_log << "Iteracion,Movimiento_i,Movimiento_j,Coste Actual,Delta,Tag,DLB\n";
    }

    return archivo_log;
}

std::vector<movimiento> funcion_ordenar_greedy(const vector &sumas, bool descending) {
    std::vector<movimiento> sumas_indizadas;
    for (int i = 0; i < sumas.size(); i++) {
        sumas_indizadas.emplace_back(sumas[i], i);
    }
    if (descending) {
        std::sort(sumas_indizadas.begin(), sumas_indizadas.end(),
                  [](const movimiento &a, const movimiento &b) {
                      return a.first > b.first;
                  });
    } else {
        std::sort(sumas_indizadas.begin(), sumas_indizadas.end());
    }
    return sumas_indizadas;
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

vector vector_aleatorio(int tamanno_matriz, int semilla) {

    vector vec(tamanno_matriz);
    for (int i = 0; i < tamanno_matriz; i++) {
        vec[i] = i;
    }
    std::default_random_engine random(semilla);
    std::shuffle(vec.begin(), vec.end(), random);

    return vec;
}

vector randomizar_DLB(const int semilla, const int tam, int &num_reseteos_DLB) {
    std::default_random_engine random(semilla + num_reseteos_DLB);
    std::bernoulli_distribution distribucion(0.5);
    vector vec;

    for (int i = 0; i < tam; i++) {
        vec.push_back(distribucion(random));
    }

    num_reseteos_DLB++;

    return vec;
}

int random_cero_uno(const int semilla, int veces) {
    std::default_random_engine random(semilla + veces);
    std::bernoulli_distribution distribucion(0.5);

    int num = distribucion(random);

    return num;
}

bool is_DLB_llena(const vector &DLB) {
    return (std::accumulate(DLB.begin(), DLB.end(), 0) == DLB.size());
}

movimiento reverse_mov(movimiento mov) {
    return {mov.second, mov.first};
}

bool movimiento_en_lista_tabu(const std::vector<movimiento> &lista_tabu, const movimiento &mov) {

    for (const auto &m: lista_tabu) {
        if (m == mov) return true;
        if (m == reverse_mov(mov)) return true;
    }
    return false;
}

void actualizar_lista_tabu(std::vector<movimiento> &lista_tabu, const movimiento &mov) {

    lista_tabu.push_back(mov);
    lista_tabu.push_back(reverse_mov((mov)));

    if (lista_tabu.size() > TENENCIA_TABU) {
        lista_tabu.erase(lista_tabu.begin());
        lista_tabu.erase(lista_tabu.begin());
    }
}

void realizar_movimiento(vector &vec, const movimiento &mov, std::vector<movimiento> &lista_tabu,
                         matriz &memoria_largo_plazo, int &iteraciones) {

    if (mov.first >= vec.size() || mov.second >= vec.size()) {
        std::cerr << "realizar_movimiento::Error: Indices de movimiento fuera de rango." << std::endl;
        return;
    }

    // Aumenta los indices apropiados
    memoria_largo_plazo[vec[mov.first]][mov.second]++;
    memoria_largo_plazo[vec[mov.second]][mov.first]++;

    // Realiza el movimiento
    std::swap(vec[mov.first], vec[mov.second]);
    iteraciones++;

    // Añade el movimiento a la lista tabú
    actualizar_lista_tabu(lista_tabu, mov);
    actualizar_lista_tabu(lista_tabu, reverse_mov(mov));
}

std::pair<movimiento, int>
generar_vecinos(const vector &solucion, const int tam, const matriz &flujo, const matriz &distancia,
                const std::vector<movimiento> &lista_tabu) {

    int mejor_delta = INFINITO_POSITIVO;
    movimiento mejor_mov;
    int contador_vecinos = 0;

    for (int i = 0; i < tam && contador_vecinos < NUMERO_MAX_VECINOS; ++i) {
        for (int j = i + 1; j < tam && contador_vecinos < NUMERO_MAX_VECINOS; ++j) {
            movimiento mov(i, j);
            if (movimiento_en_lista_tabu(lista_tabu, mov))
                continue;
            int delta_actual = delta_coste(solucion, flujo, distancia, mov);
            if (delta_actual < mejor_delta && delta_actual >= 0) {
                mejor_delta = delta_actual;
                mejor_mov = mov;
            }
            ++contador_vecinos;
        }
    }

    return {mejor_mov, mejor_delta};
}

bool condicion_estancamiento(const vector &registro_costes, const std::pair<vector, int> &mejor_local) {

    for (const auto &coste: registro_costes) {
        if (coste <= mejor_local.second) {
            return false;
        }
    }
    return true;
}

void actualizar_registro_costes(const int ultimo_coste, vector &registro_costes) {

    registro_costes.push_back(ultimo_coste);

    if (registro_costes.size() > ITERACIONES_PARA_ESTANCAMIENTO) {
        registro_costes.erase(registro_costes.begin());
    }
}

void resetar_memoria(std::vector<movimiento> &lista_tabu, matriz &memoria_largo_plazo) {
    lista_tabu.clear();
    int tam = static_cast<int>(memoria_largo_plazo.size());
    memoria_largo_plazo = matriz(tam, std::vector<int>(tam, 0));
}

vector iniciar_registro_costes() {
    int tam = static_cast<int>(MAX_ITERACIONES * PORCENTAJE_ESTANCAMIENTO);
    vector vec(tam, INFINITO_NEGATIVO);
    return vec;
}

vector diversificar(const matriz &memoria_largo_plazo, const int &semilla, const int &veces, const int &tam) {

    std::default_random_engine random(semilla + veces);
    std::uniform_int_distribution<int> distribution(0, tam - 1);

    int indice = distribution(random);

    vector resultado;
    std::vector<bool> usados(tam, false);

    for (const auto &fila: memoria_largo_plazo) {
        indice = static_cast<int>(std::distance(fila.begin(), std::max_element(fila.begin(), fila.end())));

        if (!usados[indice]) {
            resultado.push_back(indice);
            usados[indice] = true;
        } else {
            std::vector<int> temp = fila;
            while (usados[indice]) {
                temp[indice] = std::numeric_limits<int>::min();
                indice = static_cast<int>(std::distance(temp.begin(), std::max_element(temp.begin(), temp.end())));
            }
            resultado.push_back(indice);
            usados[indice] = true;
        }
    }

    return resultado;
}

vector intensificar(const matriz &memoria_largo_plazo, const int &semilla, const int &veces, const int &tam) {

    std::default_random_engine random(semilla + veces);
    std::uniform_int_distribution<int> distribution(0, tam - 1);

    int indice = distribution(random);

    std::vector<int> resultado;
    std::vector<bool> usados(tam, false);

    for (const auto &fila: memoria_largo_plazo) {
        indice = static_cast<int>(std::distance(fila.begin(), std::min_element(fila.begin(), fila.end())));

        if (!usados[indice]) {
            resultado.push_back(indice);
            usados[indice] = true;
        } else {
            std::vector<int> temp = fila;
            while (usados[indice]) {
                temp[indice] = std::numeric_limits<int>::max();
                indice = static_cast<int>(std::distance(temp.begin(), std::min_element(temp.begin(), temp.end())));
            }
            resultado.push_back(indice);
            usados[indice] = true;
        }
    }

    return resultado;
}

void tabu_mar(int tamanno_matriz, matriz &flujo, matriz &distancia) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    if (ECHO) { std::cout << "Algoritmo tabu_mar: " << std::endl; }

    // Solucion inicial aleatoria
    vector solucion_inicial = vector_aleatorio(tamanno_matriz, semillas[0]);

    // Loop de semillas
    for (int semilla: semillas) {

        // Iniciacion de variables para el algoritmo
        vector solucion_actual = solucion_inicial;
        int coste_actual = calcular_coste_solucion(solucion_actual, flujo, distancia);
        int iteraciones = 0;
        int num_reseteos_DLB = 0;
        int num_oscilaciones = 0;
        vector registro_costes = iniciar_registro_costes();
        std::pair<vector, int> mejor_local = {solucion_inicial, coste_actual};

        // Estructuras de memoria
        matriz memoria_largo_plazo(tamanno_matriz, vector(tamanno_matriz, 0));
        std::vector<movimiento> lista_tabu;
        vector DLB(tamanno_matriz, 0);

        // Empezar en indice aleatorio sin saltarse ninguno
        vector indices = vector_aleatorio(tamanno_matriz, semilla);

        // Logging
        std::ofstream log_file;
        if (LOG) { log_file = inicializar_log_DLB(semilla, archivo_datos, "tabu"); }

        while (iteraciones < MAX_ITERACIONES) {

            if (condicion_estancamiento(registro_costes, mejor_local)) {

                int oscilador = random_cero_uno(semilla, num_oscilaciones++);

                if (oscilador == DIVERSIFICAR) {
                    /// Si tengo que diversificar adopto la solucion (empezando desde un indice aleatorio) y
                    /// maximizo en la memoria a largo plazo
                    solucion_actual = diversificar(memoria_largo_plazo, semilla, num_oscilaciones, tamanno_matriz);

                } else if (oscilador == INTENSIFICAR) {
                    /// Si tengo que intensificar adopto la solucion (empezando desde un indice aleatorio) y
                    /// minimizo en la memoria a largo plazo
                    solucion_actual = intensificar(memoria_largo_plazo, semilla, num_oscilaciones, tamanno_matriz);
                }

                coste_actual = calcular_coste_solucion(solucion_actual, flujo, distancia);
                registro_costes = iniciar_registro_costes();
                resetar_memoria(lista_tabu, memoria_largo_plazo);
                DLB = vector(tamanno_matriz, 0);
            }

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

                            if (coste_actual < mejor_local.second) {
                                mejor_local.first = solucion_actual;
                                mejor_local.second = coste_actual;
                            }

                            DLB[mov_j] = 1;
                            mejora = true;

                            if (LOG) {
                                escribir_log_DLB(log_file, iteraciones, mov_i, mov_j, coste_actual, delta_actual,
                                                 DLB, "DLB");
                            }

                            actualizar_registro_costes(coste_actual, registro_costes);

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
            DLB = randomizar_DLB(semilla, tamanno_matriz, num_reseteos_DLB);

            auto mejor_vecino = generar_vecinos(solucion_actual, tamanno_matriz, flujo, distancia, lista_tabu);

            int delta = mejor_vecino.second;

            realizar_movimiento(solucion_actual, mejor_vecino.first, lista_tabu, memoria_largo_plazo, iteraciones);

            coste_actual += delta;

            actualizar_registro_costes(coste_actual, registro_costes);

            if (LOG) {
                escribir_log_DLB(log_file, iteraciones, mejor_vecino.first.first, mejor_vecino.first.second,
                                 coste_actual, delta, DLB, "EMP");
            }
        }

        if (LOG) { log_file.close(); }

        if (ECHO) { imprimir_resumen_semilla(semilla, iteraciones, mejor_local.second); }
    }

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;
    std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() << " segundos." << std::endl;
}