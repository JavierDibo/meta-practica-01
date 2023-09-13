#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>

int main() {
    std::ifstream file("data.txt"); // Asume que el archivo de entrada se llama "data.txt"
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return 1;
    }

    int n;
    while (file >> n) {
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
