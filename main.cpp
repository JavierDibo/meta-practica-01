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
#include <set>

// Typedefs

using vector = std::vector<int>;
using string = std::string;
using movimiento = std::pair<int, int>;
using matriz = std::vector<std::vector<int>>;
using mapa = std::map<string, string>;

// Definiciones

#define ALGORITMO_GREEDY "GREEDY"
#define ALGORITMO_PRIMERO_MEJOR "PRIMERO_MEJOR"
#define ALGORITMO_TABU "TABU"
#define ALGORITMO_GRASP "GRASP"
#define MILISEGUNDOS 1000
#define INTENSIFICAR 1
#define DIVERSIFICAR 0

// Variables globales

vector SEMILLAS;
bool ECHO;
bool LOG;
[[maybe_unused]] bool PARALELIZACION;
bool STATS;
int TENENCIA_TABU;
int ITERACIONES_PARA_ESTANCAMIENTO;
int NUM_ITERACIONES_GRASP;
int MAX_ITERACIONES;
int NUMERO_MAX_VECINOS;
int INFINITO_POSITIVO = std::numeric_limits<int>::max();
int INFINITO_NEGATIVO = std::numeric_limits<int>::min();
int VENTANA_GRASP;
double PROBABILIDAD_OSCILAR;
double PORCENTAJE_CEROS_DLB;
std::vector<string> ARCHIVOS_DATOS;

/// Greedy -------------------------------------------------------------------------------------------------------------

void algoritmo_greedy(int tamanno_matriz, const matriz &flujo, const matriz &distancia);

std::vector<movimiento> ordenar_vector_greedy(const vector &sumas, bool descending = false);

/// Primero el mejor ---------------------------------------------------------------------------------------------------

std::pair<vector, int>
primero_mejor_DLB(int tamanno_matriz, const matriz &flujo, const matriz &distancia, const string &archivo_datos,
                  const int &semilla, std::chrono::duration<double> &tiempo_transcurrido);

bool is_DLB_llena(const vector &DLB);

/// Algoritmo tabu -----------------------------------------------------------------------------------------------------

std::pair<vector, int> tabu_mar(int tamanno_matriz, matriz &flujo, matriz &distancia, const string &archivo_datos,
                                const int &semilla, std::chrono::duration<double> &tiempo_transcurrido);

void limpiar_memorias(std::vector<movimiento> &lista_tabu_implicita, matriz &lista_tabu_explicita,
                      matriz &memoria_largo_plazo);

vector diversificar(const matriz &memoria_largo_plazo, const int &semilla, const int &veces, const int &tam);

vector intensificar(const matriz &memoria_largo_plazo, const int &semilla, const int &veces, const int &tam);

std::pair<movimiento, int>
generar_vecinos(const vector &solucion, int tam, const matriz &flujo, const matriz &distancia,
                const std::vector<movimiento> &lista_tabu, const int &semilla);

bool condicion_estancamiento(const vector &registro_costes, const std::pair<vector, int> &mejor_local);

void actualizar_registro_costes(int ultimo_coste, vector &registro_costes);

bool movimiento_en_lista_tabu(const std::vector<movimiento> &lista_tabu, const movimiento &mov);

void actualizar_lista_tabu_implicita(std::vector<movimiento> &lista_tabu_implicita, const movimiento &mov);

void actualizar_lista_tabu_explicita(matriz &lista_tabu_explicita, const vector &solucion);

void realizar_movimiento(vector &solucion, const movimiento &mov, std::vector<movimiento> &lista_tabu_implicita,
                         matriz &lista_tabu_explicita, matriz &memoria_largo_plazo, int &iteraciones);

/// Algoritmo GRASP ----------------------------------------------------------------------------------------------------

std::pair<vector, int>
grasp(int tamanno_matriz, matriz &flujo, matriz &distancia, const string &archivo_datos, const int &semilla,
      std::chrono::duration<double> &tiempo_transcurrido);

std::vector<int> algoritmo_greedy_aletatorizado(int tamanno_matriz, const matriz &flujo, const matriz &distancia,
                                                const int &semilla, const int &mint);

std::pair<vector, int> tabu_grasp(int tamanno_matriz, matriz &flujo, matriz &distancia, const string &archivo_datos,
                                  const vector &solucion_inicial, const int &semilla, const int &mint);

/// Funciones de evaluacion --------------------------------------------------------------------------------------------

int calcular_coste_solucion(const vector &solucion, const matriz &flujo, const matriz &distancia);

int delta_coste(const vector &vec, const matriz &flujo, const matriz &distancia, movimiento mov);

/// IO -----------------------------------------------------------------------------------------------------------------

void imprimir_resumen_semilla(const int &semilla, const int &coste_actual);

[[maybe_unused]] void imprimir_solucion(const vector &sol);

void imprimir_datos_estadisticos(const string &archivo_datos, mapa parametros, const std::vector<double> &valores,
                                 const int &mejor_coste, const double &tiempo_total,
                                 const std::vector<double> &tiempos);

/// Logging ------------------------------------------------------------------------------------------------------------

std::ofstream
inicializar_log_primero_mejor(int semilla, const std::string &nombre_archivo, const std::string &algoritmo);

void escribir_log_primero_mejor(std::ofstream &archivo_log, int iteraciones, movimiento mov, int coste, int delta,
                                const vector &solucion);

std::ofstream inicializar_log_tabu(int semilla, const std::string &nombre_archivo, const std::string &algoritmo);

void escribir_log_tabu(std::ofstream &archivo_log, int iteraciones, movimiento mov, int coste, int delta,
                       const string &tag, const int &coste_mejor_local, const vector &solucion);

std::ofstream
inicializar_log_grasp(int semilla, const std::string &nombre_archivo, const std::string &algoritmo, const int &mint);

void escribir_log_grasp(std::ofstream &archivo_log, int iteraciones, movimiento mov, int coste, int delta,
                        const string &tag, const int &coste_mejor_local, const vector &solucion);

/// Funciones auxiliares -----------------------------------------------------------------------------------------------

vector vector_aleatorio(int tamanno_matriz, int semilla);

vector randomizar_DLB(int semilla, int tam, int &num_reseteos_DLB);

int random_cero_uno(int semilla, int &veces);

movimiento reverse_mov(const movimiento &mov);

vector iniciar_registro_costes();

/// Funciones datos estadisticos ---------------------------------------------------------------------------------------

double calcular_desviacion_tipica(const std::vector<double> &valores);

double calcular_media(const std::vector<double> &valores);

double calcular_coeficiente_variacion(const std::vector<double> &valores);

void
actualizar_datos_estadisticos(const int &coste, int &menor_coste, double &tiempo_total, std::vector<double> &valores,
                              double tiempo, std::vector<double> &tiempos);

/// Funciones main -----------------------------------------------------------------------------------------------------

mapa lectura_parametros(const string &nombre_archivo);

void leer_matrices(const string &nombre_archivo, int &tamanno_matriz, matriz &flujo, matriz &distancia);

void lanzar_algoritmo(mapa parametros);

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre del archivo de parametros>" << std::endl;
        return 1;
    }

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    string archivo_parametros = argv[1];

    mapa parametros = lectura_parametros(archivo_parametros);

    lanzar_algoritmo(parametros);

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;
    if (ECHO) {
        std::cout << "Tiempo TOTAL de ejecucion: " << tiempo_transcurrido.count() * MILISEGUNDOS << " milisegundos."
                  << std::endl;
    }

    return 0;
}

std::vector<int> algoritmo_greedy_aletatorizado(int tamanno_matriz, const matriz &flujo, const matriz &distancia,
                                                const int &semilla, const int &mint) {

    std::vector<int> solucion(tamanno_matriz, -1);
    std::set<int> pendientes;
    for (int i = 0; i < tamanno_matriz; ++i) {
        pendientes.insert(i);
    }
    std::default_random_engine random(semilla + mint);

    while (!pendientes.empty()) {

        int unidad_actual = *std::next(pendientes.begin(), static_cast<int>(random() % pendientes.size()));

        std::vector<std::pair<int, int>> costos;
        for (int localizacion_actual = 0; localizacion_actual < tamanno_matriz; ++localizacion_actual) {
            if (solucion[localizacion_actual] == -1) {
                int costo = 0;
                for (int i = 0; i < tamanno_matriz; ++i) {
                    if (solucion[i] != -1) {
                        costo += flujo[unidad_actual][i] * distancia[localizacion_actual][solucion[i]];
                    }
                }
                costos.emplace_back(costo, localizacion_actual);
            }
        }

        std::sort(costos.begin(), costos.end());

        int indice_aleatorio = static_cast<int>(random() % std::min(VENTANA_GRASP, static_cast<int>(costos.size())));
        solucion[costos[indice_aleatorio].second] = unidad_actual;

        pendientes.erase(unidad_actual);
    }

    int coste = calcular_coste_solucion(solucion, flujo, distancia);

    return solucion;
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

    auto flujos_indizados = ordenar_vector_greedy(sumatorio_flujos);
    auto distancias_indizadas = ordenar_vector_greedy(sumatorio_distancias, true);

    std::vector<int> solucion(tamanno_matriz);
    for (int i = 0; i < flujos_indizados.size(); i++) {
        solucion[flujos_indizados[i].second] = distancias_indizadas[i].second;
    }

    int coste = calcular_coste_solucion(solucion, flujo, distancia);

    auto tiempo_fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_transcurrido = tiempo_fin - tiempo_inicio;

    if (ECHO) {
        std::cout << "Coste: " << coste << std::endl;
        std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() * MILISEGUNDOS << " milisegundos."
                  << std::endl;
    }
}

std::ofstream
inicializar_log_grasp(int semilla, const std::string &nombre_archivo, const std::string &algoritmo, const int &mint) {

    std::filesystem::path path(nombre_archivo);
    std::string nombre = path.stem().string();

    std::string nombre_log =
            "logs/" + algoritmo + "/" + nombre + "_" + algoritmo + "_" + std::to_string(mint) + "_" +
            std::to_string(semilla) + ".csv";

    std::filesystem::path directorio = std::filesystem::path(nombre_log).parent_path();
    if (!std::filesystem::exists(directorio)) {
        std::filesystem::create_directories(directorio);
    }

    std::ofstream archivo_log(nombre_log);

    if (!archivo_log.is_open()) {
        std::cerr << "inicializar_log::no se pudo abrir el archivo " + nombre_log << std::endl;
        throw;
    }

    if (archivo_log.is_open()) {
        archivo_log
                << "Iteracion,Movimiento i,Movimiento j,Coste solucion,Delta,Etiqueta,Coste mejor local,Solucion,"
                   "Max vecinos,Iteraciones estancamiento,Oscilacion,Tenencia,Iteraciones Max,Iteraciones GRASP,Ventana GRASP\n";
    }

    return archivo_log;
}

void escribir_log_grasp(std::ofstream &archivo_log, int iteraciones, movimiento mov, int coste, int delta,
                        const string &tag, const int &coste_mejor_local, const vector &solucion) {

    archivo_log << iteraciones << "," << mov.first << "," << mov.second << "," << coste << "," << delta
                << "," << tag << "," << coste_mejor_local << ",";

    for (const auto dato: solucion) {
        archivo_log << dato << " ";
    }

    archivo_log << "," << NUMERO_MAX_VECINOS << "," << ITERACIONES_PARA_ESTANCAMIENTO << ","
                << PROBABILIDAD_OSCILAR << "," << TENENCIA_TABU << "," << MAX_ITERACIONES << ","
                << NUM_ITERACIONES_GRASP << "," << VENTANA_GRASP;

    archivo_log << "\n";
}

std::pair<vector, int> tabu_grasp(int tamanno_matriz, matriz &flujo, matriz &distancia, const string &archivo_datos,
                                  const vector &solucion_inicial, const int &semilla, const int &mint) {

    // Logging
    std::ofstream log_file;
    if (LOG) {
        log_file = inicializar_log_grasp(semilla, archivo_datos, "grasp", mint);
    }

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
    std::vector<movimiento> lista_tabu_implicita;
    matriz lista_tabu_explicita(tamanno_matriz);
    vector DLB(tamanno_matriz, 0);

    // Empezar en indice aleatorio sin saltarse ninguno
    vector indices = vector_aleatorio(tamanno_matriz, semilla);

    while (iteraciones < MAX_ITERACIONES) {

        if (condicion_estancamiento(registro_costes, mejor_local)) {

            int oscilador = random_cero_uno(semilla, num_oscilaciones);

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
            limpiar_memorias(lista_tabu_implicita, lista_tabu_explicita, memoria_largo_plazo);
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
                    if (movimiento_en_lista_tabu(lista_tabu_implicita, movimiento_actual)) {
                        continue;
                    }

                    if (delta_actual < 0) {
                        realizar_movimiento(solucion_actual, movimiento_actual, lista_tabu_implicita,
                                            lista_tabu_explicita, memoria_largo_plazo, iteraciones);

                        coste_actual += delta_actual;

                        if (coste_actual < mejor_local.second) {
                            mejor_local.first = solucion_actual;
                            mejor_local.second = coste_actual;
                        }

                        DLB[mov_j] = 1;
                        mejora = true;

                        if (LOG) {
                            escribir_log_grasp(log_file, iteraciones, movimiento_actual, coste_actual, delta_actual,
                                               "DLB", mejor_local.second, solucion_actual);
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

        auto mejor_vecino = generar_vecinos(solucion_actual, tamanno_matriz, flujo, distancia, lista_tabu_implicita,
                                            semilla);

        int delta = mejor_vecino.second;

        realizar_movimiento(solucion_actual, mejor_vecino.first, lista_tabu_implicita, lista_tabu_explicita,
                            memoria_largo_plazo, iteraciones);

        coste_actual += delta;

        actualizar_registro_costes(coste_actual, registro_costes);

        if (LOG) {
            escribir_log_grasp(log_file, iteraciones, mejor_vecino.first, coste_actual, delta, "Empeoramiento",
                               mejor_local.second, solucion_actual);
        }
    }


    if (LOG) {
        log_file.close();
    }

    return mejor_local;
}

void purge_grasp_logs(const int &mejor, const int &semilla, const string &archivo_datos) {
    // Obtener el valor de num a partir de archivo_datos

    size_t start = archivo_datos.find("ford0") + 5;
    size_t end = archivo_datos.find(".dat");
    string num = archivo_datos.substr(start, end - start);

    // Construir la lista de nombres de archivos posibles
    for (int i = 0; i <= MILISEGUNDOS; i++) {
        string filename =
                "logs/grasp/ford0" + num + "_grasp_" + std::to_string(i) + "_" + std::to_string(semilla) + ".csv";

        // Si i no es igual a "mejor", eliminamos el archivo
        if (i != mejor) {
            if (std::filesystem::exists(filename)) {
                std::filesystem::remove(filename);
            }
        } else {
            // Renombrar el archivo que coincide con "mejor"
            string newFilename = "logs/grasp/ford0" + num + "_grasp_" + std::to_string(semilla) + ".csv";
            if (std::filesystem::exists(filename)) {
                std::filesystem::rename(filename, newFilename);
            }
        }
    }
}

std::pair<vector, int> tabu_mar(int tamanno_matriz, matriz &flujo, matriz &distancia, const string &archivo_datos,
                                const int &semilla, std::chrono::duration<double> &tiempo_transcurrido) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    // Solucion inicial aleatoria
    vector solucion_inicial = vector_aleatorio(tamanno_matriz, semilla);

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
    std::vector<movimiento> lista_tabu_implicita;
    matriz lista_tabu_explicita(tamanno_matriz);
    vector DLB(tamanno_matriz, 0);

    // Empezar en indice aleatorio sin saltarse ninguno
    vector indices = vector_aleatorio(tamanno_matriz, semilla);

    // Logging
    std::ofstream log_file;
    if (LOG) { log_file = inicializar_log_tabu(semilla, archivo_datos, "tabu"); }

    while (iteraciones < MAX_ITERACIONES) {

        if (condicion_estancamiento(registro_costes, mejor_local)) {

            int oscilador = random_cero_uno(semilla, num_oscilaciones);

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
            limpiar_memorias(lista_tabu_implicita, lista_tabu_explicita, memoria_largo_plazo);
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
                    if (movimiento_en_lista_tabu(lista_tabu_implicita, movimiento_actual)) {
                        continue;
                    }

                    if (delta_actual < 0) {
                        realizar_movimiento(solucion_actual, movimiento_actual, lista_tabu_implicita,
                                            lista_tabu_explicita,
                                            memoria_largo_plazo, iteraciones);

                        coste_actual += delta_actual;

                        if (coste_actual < mejor_local.second) {
                            mejor_local.first = solucion_actual;
                            mejor_local.second = coste_actual;
                        }

                        DLB[mov_j] = 1;
                        mejora = true;

                        if (LOG) {
                            escribir_log_tabu(log_file, iteraciones, movimiento_actual, coste_actual, delta_actual,
                                              "DLB", mejor_local.second, solucion_actual);
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

        auto mejor_vecino = generar_vecinos(solucion_actual, tamanno_matriz, flujo, distancia, lista_tabu_implicita,
                                            semilla);

        int delta = mejor_vecino.second;

        realizar_movimiento(solucion_actual, mejor_vecino.first, lista_tabu_implicita, lista_tabu_explicita,
                            memoria_largo_plazo, iteraciones);

        coste_actual += delta;

        actualizar_registro_costes(coste_actual, registro_costes);

        if (LOG) {
            escribir_log_tabu(log_file, iteraciones, mejor_vecino.first, coste_actual, delta, "Empeoramiento",
                              mejor_local.second, solucion_actual);
        }
    }

    if (LOG) { log_file.close(); }

    auto tiempo_fin = std::chrono::high_resolution_clock::now();

    tiempo_transcurrido = tiempo_fin - tiempo_inicio;

#pragma omp critical
    if (ECHO) {
        std::cout << "Algoritmo Tabu: " << std::endl;
        imprimir_resumen_semilla(semilla, mejor_local.second);
        std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() * MILISEGUNDOS << " milisegundos."
                  << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }

    return mejor_local;
}

std::pair<vector, int> primero_mejor_DLB(int tamanno_matriz, const matriz &flujo, const matriz &distancia,
                                         const string &archivo_datos, const int &semilla,
                                         std::chrono::duration<double> &tiempo_transcurrido) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    vector solucion_inicial = vector_aleatorio(tamanno_matriz, semilla);

    std::vector<int> solucion_actual = solucion_inicial;
    int coste_actual = calcular_coste_solucion(solucion_actual, flujo, distancia);

    std::vector<int> DLB(tamanno_matriz, 0);
    int iteraciones = 0;

    std::vector<int> indices(tamanno_matriz);
    for (int i = 0; i < tamanno_matriz; i++) {
        indices[i] = i;
    }

    std::default_random_engine random(semilla);
    std::shuffle(indices.begin(), indices.end(), random);

    std::ofstream log_file;
    if (LOG) { log_file = inicializar_log_primero_mejor(semilla, archivo_datos, "pm"); }

    while (std::accumulate(DLB.begin(), DLB.end(), 0) < tamanno_matriz && iteraciones < MAX_ITERACIONES) {
        for (int index: indices) {
            int i = index;

            if (DLB[i] == 1) {
                continue;
            }

            bool mejora = false;
            for (int j = i + 1; j < tamanno_matriz; ++j) {

                movimiento movimiento_actual = std::make_pair(i, j);
                int delta = delta_coste(solucion_actual, flujo, distancia, movimiento_actual);

                if (delta < 0) {
                    std::swap(solucion_actual[i], solucion_actual[j]);
                    iteraciones++;
                    coste_actual += delta;
                    DLB[j] = 1;
                    mejora = true;

                    if (LOG) {
                        escribir_log_primero_mejor(log_file, iteraciones, movimiento_actual, coste_actual, delta,
                                                   solucion_actual);
                    }
                    break;
                }
            }

            if (!mejora) {
                DLB[i] = 1;
            }
        }
    }

    if (LOG) { log_file.close(); }

    auto tiempo_fin = std::chrono::high_resolution_clock::now();

    tiempo_transcurrido = tiempo_fin - tiempo_inicio;

    if (ECHO) {
        std::cout << "Algoritmo primero el mejor: " << std::endl;
        imprimir_resumen_semilla(semilla, coste_actual);
        std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() * MILISEGUNDOS << " milisegundos."
                  << std::endl;
        std::cout << "-----------------------------" << std::endl;
    }

    return std::make_pair(solucion_actual, coste_actual);
}

double calcular_desviacion_tipica(const std::vector<double> &valores) {

    int n = static_cast<int>(valores.size());
    if (n == 0) {
        std::cerr << "Error: El vector de valores está vacío." << std::endl;
        return 0.0;
    }

    double suma = 0.0;
    for (const double &valor: valores) {
        suma += valor;
    }
    double media = suma / n;

    double diferencias_al_cuadrado = 0.0;
    for (const double &valor: valores) {
        double diferencia = valor - media;
        diferencias_al_cuadrado += diferencia * diferencia;
    }

    double desviacion_tipica = std::sqrt(diferencias_al_cuadrado / n);

    return desviacion_tipica;
}

double calcular_media(const std::vector<double> &valores) {

    int n = static_cast<int>(valores.size());
    if (n == 0) {
        std::cerr << "Error: El vector de valores está vacío." << std::endl;
        return 0.0;
    }

    double suma = 0.0;
    for (const double &valor: valores) {
        suma += valor;
    }

    double media = suma / n;
    return media;
}

double calcular_coeficiente_variacion(const std::vector<double> &valores) {
    double desviacion = calcular_desviacion_tipica(valores);
    double media = calcular_media(valores);

    if (media == 0.0) {
        std::cerr << "Error: La media es igual a cero." << std::endl;
        return 0.0;
    }

    double coeficienteVariacion = (desviacion / media) * 100.0;
    return coeficienteVariacion;
}

void
actualizar_datos_estadisticos(const int &coste, int &menor_coste, double &tiempo_total, std::vector<double> &valores,
                              double tiempo, std::vector<double> &tiempos) {
    if (coste < menor_coste)
        menor_coste = coste;
    tiempo_total += tiempo;
    valores.push_back(coste);
    tiempos.push_back(tiempo);
}

void imprimir_datos_estadisticos(const string &archivo_datos, mapa parametros, const std::vector<double> &valores,
                                 const int &mejor_coste, const double &tiempo_total,
                                 const std::vector<double> &tiempos) {
    std::cout << archivo_datos << " con algoritmo " << parametros["algoritmo"] << std::endl;
    std::cout << "Media coste: " << calcular_media(valores) << " -- Mejor coste: " << mejor_coste
              << " -- Desviacion tipica coste: "
              << calcular_desviacion_tipica(valores) << " -- Coeficiente de variacion coste: ";

    printf("%.2f%%\n", calcular_coeficiente_variacion(valores));

    std::cout << "Media tiempos: " << calcular_media(tiempos) << " -- Tiempo total: " << tiempo_total
              << " -- Desviacion tipica tiempo: "
              << calcular_desviacion_tipica(tiempos) << " -- Coeficiente de variacion tiempo: ";

    printf("%.2f%%", calcular_coeficiente_variacion(tiempos));

    std::cout << std::endl;
}

void lanzar_algoritmo(mapa parametros) {

    int tamanno_matriz;
    matriz flujo, distancia;

    for (const auto &archivo_datos: ARCHIVOS_DATOS) {

        std::vector<double> valores, tiempos;
        int coste = 0, menor_coste = INFINITO_POSITIVO;
        double tiempo_total = 0;
        std::chrono::duration<double> tiempo_transcurrido{};

        leer_matrices(archivo_datos, tamanno_matriz, flujo, distancia);

        string algoritmo = parametros["algoritmo"];

        if (algoritmo == ALGORITMO_GREEDY) {
            algoritmo_greedy(tamanno_matriz, flujo, distancia);
            continue;
        } else if (algoritmo == ALGORITMO_PRIMERO_MEJOR) {
            for (const auto &semilla: SEMILLAS) {
                coste = primero_mejor_DLB(tamanno_matriz, flujo, distancia, archivo_datos, semilla,
                                          tiempo_transcurrido).second;
                actualizar_datos_estadisticos(coste, menor_coste, tiempo_total, valores,
                                              tiempo_transcurrido.count() * MILISEGUNDOS, tiempos);
            }
        } else if (algoritmo == ALGORITMO_TABU) {
#pragma omp parallel for default(none) shared(tamanno_matriz, flujo, distancia, archivo_datos, SEMILLAS, coste, menor_coste, valores, tiempos, tiempo_total) private(tiempo_transcurrido) if(PARALELIZACION)
            for (const auto &semilla: SEMILLAS) {
                coste = tabu_mar(tamanno_matriz, flujo, distancia, archivo_datos, semilla, tiempo_transcurrido).second;
                actualizar_datos_estadisticos(coste, menor_coste, tiempo_total, valores,
                                              tiempo_transcurrido.count() * MILISEGUNDOS, tiempos);
            }
        } else if (algoritmo == ALGORITMO_GRASP) {
            for (const auto &semilla: SEMILLAS) {
                coste = grasp(tamanno_matriz, flujo, distancia, archivo_datos, semilla, tiempo_transcurrido).second;
                actualizar_datos_estadisticos(coste, menor_coste, tiempo_total, valores,
                                              tiempo_transcurrido.count() * MILISEGUNDOS, tiempos);
            }
        }

        if (STATS) {
            imprimir_datos_estadisticos(archivo_datos, parametros, valores, menor_coste, tiempo_total, tiempos);
        }
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
            SEMILLAS.push_back(std::stoi(seed));
        }
    }

    if (SEMILLAS.empty()) {
        SEMILLAS.push_back(12345678);
        SEMILLAS.push_back(23456781);
        SEMILLAS.push_back(34567812);
    }

    if (parametros.find("algoritmo") != parametros.end()) {

        if (parametros["algoritmo"] == "1" || parametros["algoritmo"] == "greedy") {
            parametros["algoritmo"] = "GREEDY";
        }

        if (parametros["algoritmo"] == "2" || parametros["algoritmo"] == "pm") {
            parametros["algoritmo"] = "PRIMERO_MEJOR";
        }

        if (parametros["algoritmo"] == "3" || parametros["algoritmo"] == "tabu") {
            parametros["algoritmo"] = "TABU";
        }

        if (parametros["algoritmo"] == "4" || parametros["algoritmo"] == "grasp") {
            parametros["algoritmo"] = "GRASP";
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
    }

    if (parametros.find("num_max_vecinos") != parametros.end()) {
        NUMERO_MAX_VECINOS = std::stoi(parametros["num_max_vecinos"]);
    }

    if (parametros.find("iteraciones_para_estancamiento") != parametros.end()) {
        ITERACIONES_PARA_ESTANCAMIENTO = std::stoi(parametros["iteraciones_para_estancamiento"]);
    }

    if (parametros.find("datos") != parametros.end()) {
        string archivos_string = parametros["datos"];
        std::stringstream ss(archivos_string);
        string archivo;
        while (getline(ss, archivo, ',')) {
            ARCHIVOS_DATOS.push_back(archivo);
        }
    }

    if (parametros.find("porcentaje_oscilar") != parametros.end()) {
        PROBABILIDAD_OSCILAR = std::stod(parametros["porcentaje_oscilar"]);
    }

    if (parametros.find("iteracion_grasp") != parametros.end()) {
        NUM_ITERACIONES_GRASP = std::stoi(parametros["iteracion_grasp"]);
    }

    if (parametros.find("ventana_grasp") != parametros.end()) {
        VENTANA_GRASP = std::stoi(parametros["ventana_grasp"]);
    }

    if (parametros.find("paralelizacion") != parametros.end()) {
        PARALELIZACION = (parametros["paralelizacion"] == "true");
    }

    if (parametros.find("statistics") != parametros.end()) {
        STATS = (parametros["statistics"] == "true");
    }

    if (parametros.find("porcetaje_ceros_DLB") != parametros.end()) {
        PORCENTAJE_CEROS_DLB = std::stod(parametros["porcetaje_ceros_DLB"]);
    }

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

int calcular_coste_solucion(const vector &solucion, const matriz &flujo, const matriz &distancia) {
    int n = static_cast<int>(solucion.size());
    int coste = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            coste += flujo[i][j] * distancia[solucion[i]][solucion[j]];
        }
    }
    return coste;
}

std::vector<movimiento> ordenar_vector_greedy(const vector &sumas, bool descending) {

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

void imprimir_resumen_semilla(const int &semilla, const int &coste_actual) {
    std::cout << "Semilla: " << semilla << std::endl;
    std::cout << "Coste de la solucion para esta semilla: " << coste_actual << std::endl;
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
    std::bernoulli_distribution distribucion(PORCENTAJE_CEROS_DLB);
    vector vec;

    for (int i = 0; i < tam; i++) {
        vec.push_back(distribucion(random));
    }

    num_reseteos_DLB++;

    return vec;
}

int random_cero_uno(const int semilla, int &veces) {
    std::default_random_engine random(semilla + veces);
    std::bernoulli_distribution distribucion(PROBABILIDAD_OSCILAR);

    int num = distribucion(random);
    veces++;

    return num;
}

bool is_DLB_llena(const vector &DLB) {
    return (std::accumulate(DLB.begin(), DLB.end(), 0) == DLB.size());
}

movimiento reverse_mov(const movimiento &mov) {
    return {mov.second, mov.first};
}

bool movimiento_en_lista_tabu(const std::vector<movimiento> &lista_tabu, const movimiento &mov) {

    for (const auto &m: lista_tabu) {
        if (m == mov) return true;
        if (m == reverse_mov(mov)) return true;
    }
    return false;
}

void actualizar_lista_tabu_implicita(std::vector<movimiento> &lista_tabu_implicita, const movimiento &mov) {

    lista_tabu_implicita.push_back(mov);
    lista_tabu_implicita.push_back(reverse_mov((mov)));

    if (lista_tabu_implicita.size() > TENENCIA_TABU * 2) {
        lista_tabu_implicita.erase(lista_tabu_implicita.begin());
        lista_tabu_implicita.erase(lista_tabu_implicita.begin());
    }
}

void actualizar_lista_tabu_explicita(matriz &lista_tabu_explicita, const vector &solucion) {

    lista_tabu_explicita.push_back(solucion);

    if (lista_tabu_explicita.size() > TENENCIA_TABU) {
        lista_tabu_explicita.erase(lista_tabu_explicita.begin());
    }
}

void realizar_movimiento(vector &solucion, const movimiento &mov, std::vector<movimiento> &lista_tabu_implicita,
                         matriz &lista_tabu_explicita, matriz &memoria_largo_plazo, int &iteraciones) {

    if (mov.first >= solucion.size() || mov.second >= solucion.size()) {
        std::cerr << "realizar_movimiento::Error: Indices de movimiento fuera de rango." << std::endl;
        throw;
    }

    // Aumenta los indices apropiados
    memoria_largo_plazo[solucion[mov.first]][mov.second]++;
    memoria_largo_plazo[solucion[mov.second]][mov.first]++;

    // Realiza el movimiento
    std::swap(solucion[mov.first], solucion[mov.second]);
    iteraciones++;

    // Añade el movimiento a la lista tabú implicito y su simetrico
    actualizar_lista_tabu_implicita(lista_tabu_implicita, mov);
    actualizar_lista_tabu_implicita(lista_tabu_implicita, reverse_mov(mov));

    // Añade la solucion modificada a la lista tabu explicita
    actualizar_lista_tabu_explicita(lista_tabu_explicita, solucion);
}

std::pair<movimiento, int>
generar_vecinos(const vector &solucion, const int tam, const matriz &flujo, const matriz &distancia,
                const std::vector<movimiento> &lista_tabu, const int &semilla) {

    int mejor_delta = INFINITO_POSITIVO;
    movimiento mejor_mov;
    int contador_vecinos = 0;

    std::default_random_engine random(semilla + contador_vecinos);
    std::uniform_int_distribution<int> dist(0, tam - 1);
    int indice_random = dist(random);

    // Cicla desde el indice hasta tam
    for (int i = indice_random; i < tam && contador_vecinos < NUMERO_MAX_VECINOS; ++i) {
        for (int j = (i + 1) % tam; j != i && contador_vecinos < NUMERO_MAX_VECINOS; j = (j + 1) % tam) {
            movimiento mov(i, j);
            if (movimiento_en_lista_tabu(lista_tabu, mov))
                continue;
            int delta_actual = delta_coste(solucion, flujo, distancia, mov);
            if (delta_actual < mejor_delta) {
                mejor_delta = delta_actual;
                mejor_mov = mov;
            }
            ++contador_vecinos;
        }
    }

    // Cicla desde 0 al indice
    for (int i = 0; i < indice_random && contador_vecinos < NUMERO_MAX_VECINOS; ++i) {
        for (int j = (i + 1) % tam; j != i && contador_vecinos < NUMERO_MAX_VECINOS; j = (j + 1) % tam) {
            movimiento mov(i, j);
            if (movimiento_en_lista_tabu(lista_tabu, mov))
                continue;
            int delta_actual = delta_coste(solucion, flujo, distancia, mov);
            if (delta_actual < mejor_delta) {
                mejor_delta = delta_actual;
                mejor_mov = mov;
            }
            ++contador_vecinos;
        }
    }

    return {mejor_mov, mejor_delta};
}

[[maybe_unused]] void imprimir_solucion(const vector &sol) {

    for (const auto dato: sol) {
        std::cout << dato << " ";
    }
    std::cout << std::endl;
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

void limpiar_memorias(std::vector<movimiento> &lista_tabu_implicita, matriz &lista_tabu_explicita,
                      matriz &memoria_largo_plazo) {
    lista_tabu_implicita.clear();
    lista_tabu_explicita.clear();
    int tam = static_cast<int>(memoria_largo_plazo.size());
    memoria_largo_plazo = matriz(tam, std::vector<int>(tam, 0));
}

vector iniciar_registro_costes() {
    vector vec(ITERACIONES_PARA_ESTANCAMIENTO, INFINITO_NEGATIVO);
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
                temp[indice] = INFINITO_POSITIVO;
                indice = static_cast<int>(std::distance(temp.begin(), std::min_element(temp.begin(), temp.end())));
            }
            resultado.push_back(indice);
            usados[indice] = true;
        }
    }

    return resultado;
}

std::ofstream
inicializar_log_primero_mejor(int semilla, const std::string &nombre_archivo, const std::string &algoritmo) {

    std::filesystem::path path(nombre_archivo);
    std::string nombre = path.stem().string();

    std::string nombre_log =
            "logs/" + algoritmo + "/" + nombre + "_" + algoritmo + "_" + std::to_string(semilla) + ".csv";

    std::filesystem::path directorio = std::filesystem::path(nombre_log).parent_path();
    if (!std::filesystem::exists(directorio)) {
        std::filesystem::create_directories(directorio);
    }

    std::ofstream archivo_log(nombre_log);

    if (!archivo_log.is_open()) {
        std::cerr << "inicializar_log::no se pudo abrir el archivo " + nombre_log << std::endl;
        throw;
    }

    if (archivo_log.is_open()) {
        archivo_log << "Iteracion,Movimiento_i,Movimiento_j,Coste solucion,Delta,Max_Iteraciones,Solucion\n";
    }

    return archivo_log;
}

void escribir_log_primero_mejor(std::ofstream &archivo_log, int iteraciones, movimiento mov, int coste, int delta,
                                const vector &solucion) {

    archivo_log << iteraciones << "," << mov.first << "," << mov.second << "," << coste << "," << delta
                << "," << MAX_ITERACIONES << ",";

    for (const auto dato: solucion) {
        archivo_log << dato << " ";
    }

    archivo_log << "\n";
}

std::ofstream inicializar_log_tabu(int semilla, const std::string &nombre_archivo, const std::string &algoritmo) {

    std::filesystem::path path(nombre_archivo);
    std::string nombre = path.stem().string();

    std::string nombre_log =
            "logs/" + algoritmo + "/" + nombre + "_" + algoritmo + "_" + std::to_string(semilla) + ".csv";

    std::filesystem::path directorio = std::filesystem::path(nombre_log).parent_path();
    if (!std::filesystem::exists(directorio)) {
        std::filesystem::create_directories(directorio);
    }

    std::ofstream archivo_log(nombre_log);

    if (!archivo_log.is_open()) {
        std::cerr << "inicializar_log::no se pudo abrir el archivo " + nombre_log << std::endl;
        throw;
    }

    if (archivo_log.is_open()) {
        archivo_log
                << "Iteracion,Movimiento i,Movimiento j,Coste solucion,Delta,Etiqueta,Coste mejor local,Solucion,"
                   "Max vecinos,Iteraciones estancamiento,Oscilacion,Tenencia,Iteraciones Max\n";
    }

    return archivo_log;
}

void escribir_log_tabu(std::ofstream &archivo_log, int iteraciones, movimiento mov, int coste, int delta,
                       const string &tag, const int &coste_mejor_local, const vector &solucion) {

    archivo_log << iteraciones << "," << mov.first << "," << mov.second << "," << coste << "," << delta
                << "," << tag << "," << coste_mejor_local << ",";

    for (const auto dato: solucion) {
        archivo_log << dato << " ";
    }

    archivo_log << "," << NUMERO_MAX_VECINOS << "," << ITERACIONES_PARA_ESTANCAMIENTO << ","
                << PROBABILIDAD_OSCILAR << "," << TENENCIA_TABU << "," << MAX_ITERACIONES;

    archivo_log << "\n";
}

std::pair<vector, int> grasp(int tamanno_matriz, matriz &flujo, matriz &distancia, const string &archivo_datos,
                             const int &semilla, std::chrono::duration<double> &tiempo_transcurrido) {

    auto tiempo_inicio = std::chrono::high_resolution_clock::now();

    std::pair<vector, int> mejor_solucion = {vector(tamanno_matriz, 0), INFINITO_POSITIVO};
    int mejor = -1;

#pragma omp parallel for default(none) shared(tamanno_matriz, flujo, distancia, semilla, NUM_ITERACIONES_GRASP, archivo_datos, mejor_solucion, mejor) if (PARALELIZACION)
    for (int iteracion = 0; iteracion < NUM_ITERACIONES_GRASP; ++iteracion) {

        vector solucion_inicial = algoritmo_greedy_aletatorizado(tamanno_matriz, flujo, distancia, semilla,
                                                                 iteracion);

        std::pair<vector, int> solucion_mejorada = tabu_grasp(tamanno_matriz, flujo, distancia, archivo_datos,
                                                              solucion_inicial, semilla, iteracion);
#pragma omp critical
        if (solucion_mejorada.second < mejor_solucion.second) {
            mejor_solucion = solucion_mejorada;
            mejor = iteracion;
        }
    }

    auto tiempo_fin = std::chrono::high_resolution_clock::now();

    if (LOG) { purge_grasp_logs(mejor, semilla, archivo_datos); }

    tiempo_transcurrido = tiempo_fin - tiempo_inicio;

    if (ECHO) {
        std::cout << "Algoritmo GRASP: " << std::endl;
        imprimir_resumen_semilla(semilla, mejor_solucion.second);
        std::cout << "Tiempo de ejecucion: " << tiempo_transcurrido.count() * MILISEGUNDOS << " milisegundos."
                  << std::endl;
        std::cout << "---------------------------------------" << std::endl;
    }

    return mejor_solucion;
}
