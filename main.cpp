#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <limits>

int ingestaDeDatos(const std::string &nombreArchivo);

int lecturaParametros(const std::string &nombreArchivo);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre del archivoParametros de parametros>" << std::endl;
        return 1;
    }

    std::string archivoParametros = argv[1];

    std::ifstream paramFile(archivoParametros);

    if (!paramFile.is_open()) {
        std::cerr << "No se pudo abrir el archivoParametros " << archivoParametros << "." << std::endl;
        return 1;
    }

    lecturaParametros(archivoParametros);

    return 0;
}

int ingestaDeDatos(const std::string &nombreArchivo) {
    std::ifstream dataFile(nombreArchivo);
    if (!dataFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << nombreArchivo << "." << std::endl;
        return 1;
    }

    int n;
    while (dataFile >> n) {
        dataFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::vector<std::vector<int>> flujo(n, std::vector<int>(n));
        std::vector<std::vector<int>> distancias(n, std::vector<int>(n));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                dataFile >> flujo[i][j];
            }
        }

        dataFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                dataFile >> distancias[i][j];
            }
        }

        std::cout << "Tamanno: " << n << std::endl;
        std::cout << "Matriz de flujo:" << std::endl;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cout << flujo[i][j] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "\nMatriz de distancias:" << std::endl;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cout << distancias[i][j] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "----------------------------------\n";
    }

    dataFile.close();
    return 0;
}

int lecturaParametros(const std::string &nombreArchivo) {

    std::ifstream paramFile(nombreArchivo);
    if (!paramFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << nombreArchivo << "." << std::endl;
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
        int result = ingestaDeDatos(parametros["nombre_del_archivo"]);
        if (result != 0) {
            return result;
        }
    }

    return 0;
}