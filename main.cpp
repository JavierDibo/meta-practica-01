#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <limits>

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Uso: " << argv[0] << " <nombre del archivo> [numero de matrices]" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << argv[1] << "." << std::endl;
        return 1;
    }

    int max_matrices = (argc == 3) ? std::atoi(argv[2]) : std::numeric_limits<int>::max();
    int matrices_read = 0;

    int n;
    while (file >> n && matrices_read < max_matrices) {
        matrices_read++;

        // Leer la línea en blanco después del número
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::vector<std::vector<int>> flujo(n, std::vector<int>(n));
        std::vector<std::vector<int>> distancias(n, std::vector<int>(n));

        // Leer la matriz de flujo
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                file >> flujo[i][j];
            }
        }

        // Leer la línea en blanco entre las matrices
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Leer la matriz de distancias
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                file >> distancias[i][j];
            }
        }

        // Imprimir las matrices para verificar
        std::cout << "Tamaño: " << n << std::endl;
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

    file.close();
    return 0;
}