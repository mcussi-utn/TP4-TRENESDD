# Especificación matemática del TP4 — Trenes

Vale para las dos versiones: `trenescont` (contigua) y `treneslink` (enlazada). La especificación no depende de cómo se guardan los datos: ambas implementan las mismas funciones con los mismos prototipos.

## Simbología

- N: naturales (incluido el 0). Z: enteros. R⁺: reales. B: booleanos. Σ*: cadenas.
- A × B: producto cartesiano. A\*: secuencia de elementos de A.
- `1`: sin valor de retorno (la función solo produce efecto, por ejemplo escribir en `cout`).
- Una función con parámetro por referencia modifica ese parámetro; se marca con ⟲ (por ejemplo `F ⟲`).

## Conjuntos

| Símbolo | Conjunto |
|---|---|
| T | Tipos de recurso: {LC, LP, P, M, K, C} = locomotora de carga, locomotora de pasajeros, vagón de pasajeros, comedor, cocina, vagón de carga |
| R | Recursos = T × Σ* × N × Σ* (tipo, id, capacidad, conductor) |
| F | Formaciones = Σ* × R\* (id, recursos en orden de enganche; el primero es siempre una locomotora) |
| P | Pool de recursos = multiconjunto de R (sin orden relevante) |
| FS | Colección de formaciones = F\* de largo máximo 14, con ids distintos |
| E | Resultados = {Ok, Incompatible, SinRecurso, SinLocomotora, FormaciónLlena, ValorInválido, ExcedeCapacidad} |

## Constantes del dominio

| Constante | Valor |
|---|---|
| Toneladas por vagón de carga | 50 |
| Pasajeros por vagón (viaje corto, hasta 50 km) | 150 |
| Pasajeros por vagón (viaje largo, más de 50 km) | 50 |
| Vagones por locomotora (carga) | 10 |
| Locomotoras máximas: carga / pasajeros | 2 / 1 |
| Vagones máximos: carga / pasajeros | 20 / 10 |
| Toneladas máximas / distancia máxima | 1000 / 600 km |
| Formaciones máximas (servicios) | 14 |

## Invariantes

1. Una formación empieza con una locomotora y es toda de carga o toda de pasajeros (no se mezclan).
2. Una formación de carga acepta solo vagones C; una de pasajeros acepta P, M y K.
3. Carga: hasta 10 vagones por locomotora (2 locomotoras → hasta 20 vagones). Pasajeros: 1 locomotora y hasta 10 vagones.
4. Viaje largo (más de 50 km): la formación lleva además una cocina y un comedor.
5. Un recurso está en el pool o en una formación, nunca en las dos. Al eliminar una formación vuelven todos al pool.
6. Los ids de formación no se repiten.

## Funciones

Convención de la cátedra: las funciones **internas** tienen precondición (el que llama garantiza que se cumple) y las **externas** tienen postcondición (verifican y informan con `E` o `B`).

### Recurso

| Función | Especificación | Descripción |
|---|---|---|
| `EsLocomotora` | T → B | true si el tipo es LC o LP |
| `CapacidadBase` | T → N | 50 para C, 150 para P, 0 para el resto |
| `CrearRecurso` | T × Σ* → R | Recurso con ese tipo e id y su capacidad base |
| `LeerTipo` | Σ* → T × B | Convierte el texto de un vagón (CARGA, PASAJERO, COMEDOR, COCINA) a T. B indica si el texto era válido |

### PoolDeRecursos

| Función | Especificación | Pre / Post |
|---|---|---|
| `Agregar` | P ⟲ × R → B | Post: el pool tiene un recurso más. Falla (false) solo si el pool contiguo está lleno |
| `Contar` | P × T → N | Post: cantidad de recursos del tipo T en el pool |
| `Sacar` | P ⟲ × T → R × B | Quita y devuelve el primer recurso de tipo T. Post: B = false y pool intacto si no hay ninguno |
| `Devolver` | P ⟲ × F → 1 | Agrega al pool todos los recursos de la formación, con su capacidad base |
| `CargarTipo` | P ⟲ × T × Σ* × N → 1 | Agrega N recursos de un tipo con ids prefijo1, prefijo2, … |
| `CargarPool` | P ⟲ → 1 | Carga el pool fijo: 4 LC, 5 LP, 80 C, 60 P, 10 M, 10 K |
| `MostrarPool` | P → 1 | Imprime la cantidad disponible de cada tipo |

### Formación

| Función | Especificación | Pre / Post |
|---|---|---|
| `Crear` | R × Σ* → F | Pre: el recurso es una locomotora. Post: formación con solo esa locomotora |
| `EsDeCarga` | F → B | true si la primera locomotora es LC |
| `MaxVagones` | F → N | 20 si es de carga, 10 si es de pasajeros |
| `MaxLocomotoras` | F → N | 2 si es de carga, 1 si es de pasajeros |
| `GetCantidadLocomotoras` | F → N | Cantidad de locomotoras de la formación |
| `GetCantidadVagones` | F → N | Longitud de la formación menos sus locomotoras |
| `GetCapacidad` | F → N | Suma de las capacidades de sus recursos (toneladas o pasajeros) |
| `AddLocomotora` | F ⟲ × R → B | Post: false y sin cambios si no hay lugar, si ya está el máximo de locomotoras o si no es del mismo tipo que la inicial |
| `EsCompatible` | F × T → B | Carga acepta C; pasajeros acepta P, M y K |
| `AddVagón` | F ⟲ × R → B | Agrega al final. Post: false y sin cambios si no hay lugar, si el recurso es locomotora o si es incompatible |
| `Enganchar` | F ⟲ × P ⟲ × T → E | Saca un vagón de tipo T del pool y lo agrega. En carga suma una locomotora si el vagón nuevo pasa de 10 por locomotora. Post: si el resultado no es Ok, formación y pool quedan intactos |
| `ArmarCarga` | P ⟲ × Σ* × R⁺ → F × E | Calcula vagones = ⌈toneladas / 50⌉ y locomotoras = 1 si vagones ≤ 10, si no 2. ValorInválido si toneladas ≤ 0; ExcedeCapacidad si pasa de 1000 |
| `ArmarPasajeros` | P ⟲ × Σ* × Z × Z → F × E | Si km > 50 es viaje largo: vagones = ⌈pasajeros / 50⌉ más cocina y comedor; si no, ⌈pasajeros / 150⌉. ValorInválido si pasajeros o km ≤ 0; ExcedeCapacidad si km > 600 o si vagones + servicio > 10 |
| `Mostrar` | F → 1 | Imprime id, clase, cantidades, capacidad y composición |
| `Mensaje` | E → Σ* | Texto del resultado |

`ArmarCarga` y `ArmarPasajeros` validan todo **antes** de sacar recursos: si el resultado no es Ok, el pool queda intacto.

### Colección de formaciones

| Función | Especificación | Descripción |
|---|---|---|
| `Buscar` | FS × Σ* → Z | Índice de la formación con ese id, o −1 si no existe |
| `PuedeCrear` | FS × Σ* → B | false si el id ya existe o si ya hay 14 formaciones |
| `Registrar` | FS ⟲ × F × E → 1 | Si E = Ok agrega la formación a la colección; si no, informa el error |
| `Eliminar` | FS ⟲ × P ⟲ × N → 1 | Pre: el índice existe. Devuelve los recursos al pool y quita la formación |

### Solo en `treneslink` (detalle de implementación)

`Enlazar` (agrega un nodo al final de una lista), `LiberarLista`, `Liberar` y `LiberarTodo` (hacen `delete` de los nodos). No cambian la especificación: son la forma de guardar `R\*` y `P` con nodos.

### Complejidad (n = recursos del pool o de la formación)

| Operación | Contigua | Enlazada |
|---|---|---|
| `Agregar` (pool), `AddVagón`, `AddLocomotora` | O(1) (escribe en `cantidad`) | O(1) (`Enlazar` usa `ultimo`, no recorre) |
| `Contar`, `GetCapacidad`, `GetCantidadLocomotoras`, `Mostrar` | O(n) | O(n) |
| `Sacar` (pool) | O(n): busca y corre los siguientes | O(n): busca y desconecta el nodo |
| Longitud de la formación | O(1) (`cantidad`) | O(1) (`cantidad` guardada; sin ella sería O(n)) |

## Pruebas

`Pruebas()` se ejecuta al empezar `main` en las dos versiones (usa solo la interfaz, por eso es idéntica) y verifica con `assert` el pool y las formaciones. No imprime nada. Los `assert` de precondición están en `Crear` y `Eliminar`. Con `#define NDEBUG` antes de `<cassert>` se desactivan todos.

## Comandos de entrada

`trenescont < comandos.txt > resultados.txt` (ídem `treneslink`). Los comandos se separan con espacios o saltos de línea; una línea que empieza con `#` es comentario.

| Comando | Qué hace |
|---|---|
| `CARGA <id> <toneladas>` | `ArmarCarga` y registra la formación |
| `PASAJEROS <id> <pasajeros> <km>` | `ArmarPasajeros` y registra la formación |
| `FORMACION <id> <C\|P>` | `Crear` con una locomotora del pool (carga o pasajeros) |
| `VAGON <id> <tipo>` | `Enganchar`; tipo: CARGA, PASAJERO, COMEDOR o COCINA |
| `ELIMINAR <id>` | `Eliminar`: desarma la formación y devuelve los recursos |
| `MOSTRAR <id>` | `Mostrar` de una formación |
| `FORMACIONES` | `Mostrar` de todas |
| `POOL` | `MostrarPool` |

Un comando desconocido o con argumentos inválidos imprime un error y el programa sigue con el siguiente.
