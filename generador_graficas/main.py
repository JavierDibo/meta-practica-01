import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import zipfile


def generar_grafica(archivo, directorio_salida, alg):
    # Cargar datos
    global tit
    data = pd.read_csv(archivo)

    # Extraer columnas
    iteracion = data['Iteracion']
    coste = data['Coste solucion']

    # Valor mínimo del coste
    min_coste = coste.min()
    min_coste_iteraciones = iteracion[coste == min_coste]

    # Extraer nombre y semilla para el título
    patron = rf"(\w+)_{alg}_(\d+).csv"
    coincidencia = re.search(patron, archivo)
    nombre = coincidencia.group(1)
    semilla = int(coincidencia.group(2))

    if alg == "tabu":
        tit = "Tabu"
    if alg == "pm":
        tit = "PMDLB"
    if alg == "grasp":
        tit = "GRASP"

    titulo = f"{tit} {nombre} -- Semilla {semilla}"

    # Crear gráfica
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.plot(iteracion, coste, color='grey', label='Coste por Iteración')
    ax.scatter(min_coste_iteraciones, [min_coste] * len(min_coste_iteraciones), color='blue', s=60, zorder=5,
               label=f'Mínimo coste: {min_coste}', marker='x')
    ax.set_title(titulo)
    ax.set_xlabel('Iteración')
    ax.set_ylabel('Coste')
    ax.legend()
    ax.grid(True, linestyle='--', linewidth=0.5)
    plt.tight_layout()

    # Guardar imagen
    img_path = os.path.join(directorio_salida, f"{nombre}_{alg}_{semilla}.png")
    plt.savefig(img_path, dpi=300, format='png')
    plt.close()
    return img_path


def main():
    alg = "grasp"

    # Directorio donde están los archivos CSV
    directorio_entrada = (f"../cmake-build-debug/logs/{alg}")
    # archivos están en otro lugar
    # Directorio para guardar las imágenes
    directorio_salida = f"./graficas/{alg}"
    os.makedirs(directorio_salida, exist_ok=True)

    # Lista de archivos CSV en el directorio de entrada
    archivos = [f for f in os.listdir(directorio_entrada) if f.endswith('.csv')]

    # Generar gráficas para todos los archivos
    for archivo in archivos:
        generar_grafica(os.path.join(directorio_entrada, archivo), directorio_salida, alg)

    print(f"Gráficas guardadas en {directorio_salida}")


if __name__ == "__main__":
    main()
