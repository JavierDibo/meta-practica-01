# meta-practica-01

## Algoritmo tabu -- Pseudocodigo

```
FUNCION tabu_mar(tamanno_matriz, flujo, distancia):
    tiempo_inicio = obtener_tiempo_actual()

    // Generar la solución inicial
    solucion_inicial = generar_solucion_inicial(tamanno_matriz)

    // Variables para mantener la mejor solución encontrada
    mejor_solucion = solucion_inicial
    coste_mejor_solucion = calcular_coste(mejor_solucion, flujo, distancia)

    // Bucle para diferentes semillas (diferentes soluciones iniciales)
    PARA CADA semilla EN semillas:
        solucion_actual = solucion_inicial
        coste_actual = calcular_coste(solucion_actual, flujo, distancia)
        iteraciones = 0

        // Inicialización de estructuras de memoria a corto y largo plazo
        lista_tabu = nueva_lista_tabu(tamanno_tabu)
        memoria_largo_plazo = inicializar_memoria_largo_plazo(tamanno_matriz)

        // Bucle principal de búsqueda
        MIENTRAS no_se_cumpla_criterio_de_parada(iteraciones, solucion_actual, coste_actual):
            mejor_vecino = solucion_actual
            mejor_coste_vecino = coste_actual
            DLB = lista_de_tamanno(tamanno_matriz, inicializado_a=0)

            // Explorar vecindario
            PARA CADA indice i EN solucion_actual:
                SI DLB[i] == 1:
                    CONTINUAR

                PARA CADA indice j EN solucion_actual, j != i:
                    delta = calcular_delta_coste(solucion_actual, flujo, distancia, i, j)

                    // Criterio de aspiración: aceptar si es mejor que la mejor conocida
                    SI (delta < 0) O (es_mejor_que_la_mejor_solucion(solucion_actual, mejor_solucion)):
                        SI no_esta_en_lista_tabu(lista_tabu, movimiento(i, j)):
                            // Actualizar la mejor solución vecina
                            mejor_vecino = aplicar_movimiento(solucion_actual, i, j)
                            mejor_coste_vecino = coste_actual + delta

            // Actualizar solución actual
            solucion_actual = mejor_vecino
            coste_actual = mejor_coste_vecino
            iteraciones = iteraciones + 1

            // Actualizar mejor solución global
            SI coste_actual < coste_mejor_solucion:
                mejor_solucion = solucion_actual
                coste_mejor_solucion = coste_actual

            // Actualizar lista tabú
            lista_tabu = actualizar_lista_tabu(lista_tabu, movimiento(i, j))

            // Actualizar memoria a largo plazo y posiblemente reiniciar búsqueda (opcional)
            memoria_largo_plazo = actualizar_memoria_largo_plazo(memoria_largo_plazo, solucion_actual)
            SI condicion_de_reinicio(memoria_largo_plazo):
                solucion_actual, coste_actual = reiniciar_busqueda(memoria_largo_plazo)

        // Opcional: guardar o imprimir resultados de la semilla actual

    // Opcional: guardar o imprimir resultados globales

    tiempo_fin = obtener_tiempo_actual()
    imprimir("Tiempo de ejecución:", tiempo_fin - tiempo_inicio)

    RETORNAR mejor_solucion, coste_mejor_solucion
FIN FUNCION
```

## Funciones a implementar

1. **`generar_solucion_inicial(tamanno_matriz)`**
    - **Objetivo**: Generar una solución inicial para comenzar la búsqueda.
    - **Entrada**: Tamaño de la matriz (dimensiones del problema).
    - **Salida**: Un vector que representa una solución inicial.

2. **`calcular_coste(solucion, flujo, distancia)`**
    - **Objetivo**: Calcular el coste de una solución dada.
    - **Entrada**: La solución a evaluar y las matrices de flujo y distancia.
    - **Salida**: Un valor escalar que representa el coste de la solución.

3. **`nueva_lista_tabu(tamanno_tabu)`**
    - **Objetivo**: Inicializar una nueva lista tabú.
    - **Entrada**: Tamaño de la lista tabú.
    - **Salida**: Una estructura de datos que representa la lista tabú.

4. **`inicializar_memoria_largo_plazo(tamanno_matriz)`**
    - **Objetivo**: Inicializar la memoria a largo plazo.
    - **Entrada**: Tamaño de la matriz (dimensiones del problema).
    - **Salida**: Una estructura de datos para almacenar información para la memoria a largo plazo.

5. **`no_se_cumpla_criterio_de_parada(iteraciones, solucion_actual, coste_actual)`**
    - **Objetivo**: Determinar si se debe continuar la búsqueda.
    - **Entrada**: Número de iteraciones, solución actual y su coste.
    - **Salida**: Verdadero/Falso dependiendo de si se debe continuar la búsqueda.

6. **`calcular_delta_coste(solucion_actual, flujo, distancia, i, j)`**
    - **Objetivo**: Calcular el cambio en el coste al intercambiar dos elementos.
    - **Entrada**: Solución actual, matrices de flujo y distancia, índices a intercambiar.
    - **Salida**: Valor del cambio en el coste.

7. **`es_mejor_que_la_mejor_solucion(solucion_actual, mejor_solucion)`**
    - **Objetivo**: Determinar si la solución actual es mejor que la mejor conocida.
    - **Entrada**: Solución actual y la mejor solución conocida.
    - **Salida**: Verdadero/Falso.

8. **`no_esta_en_lista_tabu(lista_tabu, movimiento)`**
    - **Objetivo**: Verificar si un movimiento está en la lista tabú.
    - **Entrada**: Lista tabú, movimiento a verificar.
    - **Salida**: Verdadero/Falso.

9. **`aplicar_movimiento(solucion_actual, i, j)`**
    - **Objetivo**: Aplicar un movimiento a la solución actual.
    - **Entrada**: Solución actual, índices a intercambiar.
    - **Salida**: Nueva solución.

10. **`actualizar_lista_tabu(lista_tabu, movimiento)`**
    - **Objetivo**: Actualizar la lista tabú con un nuevo movimiento.
    - **Entrada**: Lista tabú, movimiento a añadir.
    - **Salida**: Lista tabú actualizada.

11. **`actualizar_memoria_largo_plazo(memoria_largo_plazo, solucion_actual)`**
    - **Objetivo**: Actualizar la memoria a largo plazo basándose en la solución actual.
    - **Entrada**: Memoria a largo plazo, solución actual.
    - **Salida**: Memoria a largo plazo actualizada.

12. **`condicion_de_reinicio(memoria_largo_plazo)`**
    - **Objetivo**: Determinar si se debe reiniciar la búsqueda basándose en la memoria a largo plazo.
    - **Entrada**: Memoria a largo plazo.
    - **Salida**: Verdadero/Falso.

13. **`reiniciar_busqueda(memoria_largo_plazo)`**
    - **Objetivo**: Reiniciar la búsqueda utilizando la memoria a largo plazo.
    - **Entrada**: Memoria a largo plazo.
    - **Salida**: Nueva solución y su coste para reiniciar la búsqueda.