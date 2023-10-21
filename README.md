# Metaheurísticas

## Práctica 1

Realizado por Javier Francisco Dibo Gómez y María Torres Gutiérrez

### Parámetros

### Parámetros Configurables

A continuación, se presentan todos los parámetros que se pueden modificar en el archivo `parametros.txt`.

**semilla**
* Descripción: Representa las semillas utilizadas para generar números aleatorios en el programa.
* Uso: Se puede establecer una única semilla o varias separadas por comas. Si no se especifica una semilla, el programa utiliza un conjunto de semillas por defecto.
* Ejemplo: `semilla=3475,3718,3264`

**algoritmo**
* Descripción: Determina el algoritmo que se utilizará para resolver el problema.
* Uso: Puede ser uno entre `greedy`, `pm`, `tabu`, o `grasp`. También se pueden usar números para representar estos algoritmos: `1`, `2`, `3`, o `4` respectivamente.
* Ejemplos:
  - `algoritmo=3` (que representa `tabu`)
  - `algoritmo=grasp`

**echo**
* Descripción: Si está activado, el programa imprimirá datos por consola.
* Valores: `true` o `false`

**statistics**
* Descripción: Si está activado, el programa mostrará datos estadísticos por consola independientemente del valor de `echo`.
* Valores: `true` o `false`

**paralelizacion**
* Descripción: Activa la paralelización para mejorar el rendimiento del programa usando OpenMP.
* Valores: `true` o `false`

**log**
* Descripción: Si está activado, el programa creará archivos log con información detallada de las operaciones y las guardará en la carpeta correspondiente.
* Valores: `true` o `false`

**porcentaje_oscilar**
* Descripción: Controla la proporción entre diversificación e intensificación en el proceso de búsqueda. Un valor cercano a `1` implica más diversificación, mientras que un valor cercano a `0` implica más intensificación.
* Valores: Un número decimal entre `0` y `1`. Por defecto en `0.5`

**datos**
* Descripción: Especifica los archivos de datos que el programa usará.
* Uso: Se pueden especificar múltiples archivos separados por comas.
* Ejemplo: `datos=ford01.dat,ford02.dat,ford03.dat,ford04.dat`

**max_iteraciones**
* Descripción: Define el número máximo de iteraciones que el algoritmo realizará.
* Valores: Un número entero positivo.

**tenencia_tabu**
* Descripción: Especifica la tenencia tabú, que es la cantidad de iteraciones durante las cuales una solución es considerada "tabú" y no puede ser revisada nuevamente.
* Valores: Un número entero positivo.

**num_max_vecinos**
* Descripción: Define el número máximo de vecinos que se considerarán a la hora de buscar soluciones si se llena la DLB.
* Valores: Un número entero positivo.

**iteraciones_para_estancamiento**
* Descripción: Establece el número de iteraciones sin mejora después del cual se considera que el algoritmo está estancado y se procede a diversificar o intensificar.
* Valores: Un número entero positivo.

**iteracion_grasp**
* Descripción: Determina el número de ejecuciones independientes de la búsqueda local que ejecuta el algoritmo GRASP.
* Valores: Un número entero positivo.

**ventana_grasp**
* Descripción: Define el tamaño de la ventana para el método GRASP, lo que afecta la generación de las soluciones iniciales a partir del algoritmo greedy aleatorizado.
* Valores: Un número entero positivo.
