#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <limits>
#include <algorithm>

int ingestaDeDatos(const std::string &nombreArchivo, std::vector<std::vector<int>> &sumas, int &tam);

int lecturaParametros(const std::string &nombreArchivo);

int algoritmoGreedy(std::string &nombreArchivo);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "main::Uso: " << argv[0] << " <nombre del archivoParametros de parametros>" << std::endl;
        return 1;
    }

    std::string archivoParametros = argv[1];

    lecturaParametros(archivoParametros);

    return 0;
}

int ingestaDeDatos(const std::string &nombreArchivo, std::vector<std::vector<int>> &sumas, int &tam) {
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "ingestaDeDatos::No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return 1;
    }

    int n;
    while (archivo >> n) {
        archivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::vector<std::vector<int>> flujo(n, std::vector<int>(n));
        std::vector<std::vector<int>> distancias(n, std::vector<int>(n));
        std::vector<int> sumatorioFlujos(n);
        std::vector<int> sumatorioDistancias(n);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                archivo >> flujo[i][j];
            }
        }

        archivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                archivo >> distancias[i][j];
            }
        }

        std::cout << "Tamanno: " << n << std::endl;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                sumatorioFlujos[i] += flujo[i][j];

            }
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                sumatorioDistancias[i] += distancias[i][j];
            }
        }

        sumas.push_back(sumatorioFlujos);
        sumas.push_back(sumatorioDistancias);
        tam = n;
    }

    archivo.close();
    return 0;
}

int lecturaParametros(const std::string &nombreArchivo) {

    std::ifstream paramFile(nombreArchivo);
    if (!paramFile.is_open()) {
        std::cerr << "lecturaParametros::No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return 1;
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

    if (parametros["otros_parametros"] == "ingesta") {
        if (parametros["algoritmo"] == "greedy") {
            return algoritmoGreedy(parametros["nombre_del_archivo"]);
        }
    }

    return 0;
}

int algoritmoGreedy(std::string &nombreArchivo) {
    int tam;
    std::vector<std::vector<int>> sumas;
    int result = ingestaDeDatos(nombreArchivo, sumas, tam);
    if (result != 0) {
        return result;
    }

    std::vector<std::pair<int, int>> sumasIndizadas[2];

    for (int j = 0; j < 2; j++) {
        for (size_t i = 0; i < sumas[j].size(); i++) {
            sumasIndizadas[j].emplace_back(sumas[j][i], i);
        }
    }

    std::sort(sumasIndizadas[0].begin(), sumasIndizadas[0].end());

    std::sort(sumasIndizadas[1].begin(), sumasIndizadas[1].end(),
              [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
                  return a.first > b.first;
              });

    std::cout << "------------------------------------------" << std::endl;
    for (size_t i = 0; i < sumasIndizadas[0].size(); i++) {
        std::cout << "Unidad " << sumasIndizadas[0][i].second + 1 << " (F=" << sumasIndizadas[0][i].first << ")"
                  << " --> "
                  << "Localizacion: " << sumasIndizadas[1][i].second + 1 << " (D=" << sumasIndizadas[1][i].first
                  << ")"
                  << std::endl;
    }

    return 0;
}