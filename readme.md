# Trabajo Práctico 4: Trenes

Armamos las **formaciones de tren** que satisfacen servicios de carga y de pasajeros, tomando locomotoras y vagones de un **pool de recursos**, en dos versiones: **contigua** (`std::array`) y **enlazada** (nodos con `new` / `delete`).

## Objetivos
* Diseñar e implementar tipos
* Aplicar composición de tipos
* Demostrar capacidad de simular estructuras dinámicas
* Estructuras enlazadas

## Archivos

1. **Especificación (`trenes.md`):** especificación matemática de los tipos y de todas las funciones. Vale para las dos versiones
2. **Versión contigua (`trenescont.cpp`):** formaciones y pool guardados en `std::array` de tamaño fijo, con un contador `cantidad` de posiciones ocupadas
3. **Versión enlazada (`treneslink.cpp`):** formaciones y pool guardados en listas de `Nodo`. Tiene las mismas funciones con los mismos prototipos y da la misma salida
4. **Datos de prueba (`comandos.txt`):** comandos para probar las dos versiones

## Comandos

Se leen desde `cin`, separados por espacios o saltos de línea:

| Comando | Qué hace |
|---|---|
| `CARGA <id> <toneladas>` | Arma una formación de carga para esas toneladas |
| `PASAJEROS <id> <pasajeros>` | Arma una formación de pasajeros para esos pasajeros |
| `VAGON <id> <tipo>` | Engancha un vagón del pool: `CARGA`, `PASAJERO`, `COMEDOR` o `COCINA` |
| `MOSTRAR <id>` | Muestra una formación |
| `POOL` | Muestra los recursos disponibles |

## Reglas
* **Carga:** vagones de 50 toneladas, una locomotora cada 10 vagones, hasta 2 locomotoras (20 vagones)
* **Pasajeros:** vagones de 150 pasajeros, una locomotora y hasta 10 vagones; el comedor y la cocina se agregan con `VAGON`
* **Pool inicial:** 4 locomotoras de carga (LC), 5 de pasajeros (LP), 80 vagones de carga (C), 60 de pasajeros (P), 10 comedores (M) y 10 cocinas (K)
* **Formaciones:** hasta 14, con ids distintos

## Restricciones
* **Flujos estándar:** los datos entran por `cin` y los resultados salen por `cout`
* **Redireccionamiento:** los programas se ejecutan con archivos de entrada y salida
* **`std::array<T,N>`:** se usa en la versión contigua en lugar de arreglos `T[N]`
* **Acceso seguro:** se usa `.at()` en lugar del operador `[]`
* **Estructuras enlazadas:** en `treneslink`, cada `Nodo` guarda un `Recurso` y un puntero al siguiente. Es un puntero (`Nodo*`) porque un `Nodo` no puede contener otro `Nodo` entero. Cada `new` tiene su `delete`: al sacar un recurso del pool y al final del programa (`LiberarTodo`)

## Diferencias entre las versiones

| | Contigua | Enlazada |
|---|---|---|
| Agregar | `links.at(cantidad++) = r` | `Enlazar`: `new Nodo` al final de la lista |
| Sacar del pool | Busca y corre los siguientes un lugar | Busca, desconecta el nodo y hace `delete` |
| Recorrer | `for(unsigned i{}; i < cantidad; ++i)` | `for(const Nodo* i = primero; i; i = i->siguiente)` |
| Memoria | Se libera sola | `LiberarTodo` al final de `main` |
| Funciones propias | — | `Enlazar`, `LiberarLista` y `LiberarTodo` (crean y liberan los nodos; no cambian la especificación) |

## Pruebas
* Al empezar, `main` llama a `Pruebas()`, que verifica con `assert` el armado de una formación, su capacidad y un vagón incompatible. Si todo está bien, no imprime nada
* Con `comandos.txt` las dos versiones dan exactamente la misma salida

## Ejecución

```bash
g++ -std=c++17 trenescont.cpp -o trenescont
g++ -std=c++17 treneslink.cpp -o treneslink
./trenescont < comandos.txt > resultados.txt
./treneslink < comandos.txt > resultados.txt
```

## Estructura del Proyecto

```text
DD-Trenes
 |-- readme.md
 |-- trenes.md
 |-- comandos.txt
 |-- trenescont.cpp
 `-- treneslink.cpp
```

## Autor
* **Grupo:** 10
* **Materia:** Algoritmos y Estructuras de Datos (AyED)
* **Año:** 2026
