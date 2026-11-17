# Apunte TP4 — Trenes (`trenescont` y `treneslink`)

Archivos: `trenescont.cpp`, `treneslink.cpp`, `trenes.md` (especificación) y `comandos.txt` (prueba). Se compilan con `g++ -std=c++17` y se usan así: `trenescont < comandos.txt > resultados.txt`.

## 1. Qué hace el programa

Lee comandos por `cin`, arma formaciones de tren sacando recursos de un pool fijo y escribe el resultado por `cout`. No simula vías ni viajes: solo arma, muestra y desarma formaciones (hasta 14, como los 14 servicios del enunciado).

## 2. Estructura general (igual en los dos archivos)

1. Comentario con los comandos.
2. `#include`: solo `<array>`, `<iostream>` y `<string>`.
3. Tipos y arreglos: `Recurso`, `Formación`, `PoolDeRecursos`, `Formaciones`, `Resultado`.
4. Declaración de funciones (prototipos), agrupadas por tipo.
5. `main`: crea el pool y las formaciones, carga el pool, llama a `LeerComandos`. En la enlazada, además, a `LiberarTodo`.
6. Detalle de las funciones, en el mismo orden que los prototipos.

## 3. Los tipos y sus arreglos

| Tipo | Contigua | Enlazada |
|---|---|---|
| `Recurso` | `tipo`, `id`, `capacidad`, `conductor` | igual |
| `Formación` | `std::array<Recurso, 22> links` + `cantidad` | `Nodo* primero`, `Nodo* ultimo` + `cantidad` |
| `PoolDeRecursos` | `std::array<Recurso, 200> recursos` + `cantidad` | lista de nodos (`primero`, `ultimo`, `cantidad`), sin tope |
| `Formaciones` | `std::array<Formación, 14> items` + `cantidad` | igual (el arreglo guarda solo las cabezas de las listas) |

- **Contigua:** el arreglo siempre tiene todas sus posiciones; `cantidad` dice cuántas están ocupadas (lo visto el 28/10). La locomotora va en la posición 0. Agregar es `links[cantidad++] = r` (postincremento: escribe y después sube el contador).
- **Enlazada:** cada `Nodo` guarda un `Recurso` y un puntero `siguiente`. Es puntero y no un `Nodo` entero porque un tipo no puede contenerse a sí mismo. `new` pide memoria al agregar y `delete` la devuelve al sacar.
- El `static const` de `Formación` (máximos, constantes del dominio) no ocupa espacio en cada variable: es compartido por todas.

## 4. Las funciones por partes

- **Recurso:** `EsLocomotora`, `CapacidadBase`, `CrearRecurso`, `LeerTipo` (texto → tipo).
- **Pool:** `Agregar`, `Contar`, `Sacar` (la que más cambia entre versiones), `CargarTipo`/`CargarPool` (pool hardcodeado: 4 LC, 5 LP, 80 C, 60 P, 10 M, 10 K), `MostrarPool`, `Devolver`.
- **Formación:** `Crear`, `AddLocomotora`, `AddVagón`, `Get…`, `Enganchar` (agrega un vagón desde el pool y suma locomotora si hace falta), `ArmarCarga`, `ArmarPasajeros`, `Mostrar`, `Mensaje`.
- **Formaciones:** `Buscar` (índice o −1), `PuedeCrear`, `Registrar`, `Eliminar`.
- **Comandos:** un `Comando…` por cada comando y `LeerComandos`, que solo despacha. Así `main` queda corto y cada comando se defiende por separado.

Reglas de negocio (de `lula.cpp`): carga con 50 t por vagón, 1 locomotora cada 10 vagones y máximo 2; pasajeros con 1 locomotora, 150 por vagón (50 si el viaje pasa de 50 km, que además lleva cocina y comedor), máximo 10 vagones.

## 5. Diferencias entre las dos versiones

Solo cambia cómo se recorre y se modifica el almacenamiento. Las funciones tienen los mismos prototipos y las mismas salidas.

| Función | Contigua | Enlazada |
|---|---|---|
| `Agregar` / `AddVagón` | escribe en `links[cantidad++]` | `Enlazar`: `new Nodo` y engancha al final |
| `Sacar` (pool) | busca y corre los siguientes un lugar | busca, desconecta el nodo y hace `delete` |
| Recorridos | `for(unsigned i{0}; i < cantidad; ++i)` | `for(const Nodo* i{primero}; i; i = i->siguiente)` |
| `Eliminar` | devuelve recursos y compacta el arreglo | devuelve recursos, hace `Liberar` de los nodos y compacta |
| Memoria | se libera sola | `LiberarTodo` al final de `main` |

## 6. Temas importantes del TP reflejados en el código

- **Tipos y composición:** `Recurso` dentro de `Formación`, y `Formación` dentro de `Formaciones`.
- **Pasaje por referencia:** `AddVagón(Formación&, const Recurso&)` modifica la formación original; `const &` en lo que solo se lee (visto el 14/10 y el 28/10).
- **Estilo guarda:** `AddLocomotora` y `AddVagón` salen con `return false` antes de tocar nada.
- **Pre y postcondiciones:** las funciones de más abajo (`Crear`, `Eliminar`) asumen que quien las llama validó; las que están expuestas al comando (`Enganchar`, `Armar…`) verifican y devuelven `Resultado`. Todo se valida antes de sacar del pool, así que si falla, el pool queda intacto.
- **Interfaz vs. implementación:** el mismo `trenes.md` sirve para las dos versiones.
- **Listas enlazadas:** `Nodo`, `new`/`delete`, desconectar un nodo antes de borrarlo, puntero al último para agregar sin recorrer.
- **Flujos estándar:** `cin >> comando` en un `for` (el mismo patrón de `Dim1.cpp`) y `cout` para los resultados. `for(const auto& …)` para recorrer los arreglos de conductores.

## 7. Antes y ahora

| Antes | Ahora |
|---|---|
| `lula.cpp`: menú interactivo, `vector`, `<sstream>`, `<iomanip>`, `<limits>`, `<cassert>`, IDs generados, 6 pools separados | Comandos por `cin`; solo `array` y `string`; un único pool con tipo por recurso |
| `fabri.cpp`: base sólida pero con reglas propias (20 t, 40 vagones), sin conductor ni `CARGA`/`PASAJEROS` armados de una vez, sin `ELIMINAR` ni `POOL` | Se mantiene su estructura y sus nombres; se suman las reglas de `lula`, los comandos que faltaban y `Devolver` |
| `trenescont.cpp` y `treneslink.cpp` anteriores: traían un `MODIFICAR` transaccional, con `Clonar` del pool, y `main` con todo el `if` adentro | `main` de 4 líneas, un `Comando…` por comando y sin `Clonar` |
| `trenes.md`: 13 funciones, sin pool ni comandos ni pre/post | Todas las funciones del código, conjuntos, constantes, invariantes y comandos |

Cosas que se sacaron a propósito para simplificar: `MODIFICAR` (editar una formación = `ELIMINAR` y volver a crearla), los IDs generados automáticamente (el id lo da el comando) y los menús.

## 8. Qué se probó y qué falta validar

Probado el 2026-11-08: compilan sin warnings con `g++ -std=c++17 -Wall -Wextra`; con `comandos.txt` y con una prueba de 11 vagones (para forzar la segunda locomotora) las dos versiones dan **exactamente la misma salida**, y al eliminar las formaciones el pool vuelve a su estado inicial.

Pendiente:
- [ ] Confirmar con el docente que está bien no usar `std::vector` (es una deducción) y que `std::array` para la contigua es lo esperado.
- [ ] Confirmar las reglas de negocio (50 t, 150/50 pasajeros, límites): las sacamos de `lula.cpp`; la consigna solo trae valores de ejemplo (17 y 21).
- [ ] Revisar fugas de memoria de `treneslink` con valgrind u otra herramienta: acá no pude (falta `libubsan` en MinGW), solo comprobé que cada `new` tiene su `delete` por lectura.
- [ ] Decidir si se agrega `MODIFICAR` (o `SacarVagón`, que se vio en clase) si el docente lo pide.
- [ ] Decidir si se activan `assert` en las funciones internas (en clase: `#define NDEBUG` antes de `<cassert>`). No se agregaron para mantener el código simple.
- [ ] Copiar los archivos al repo (`DD-Trenes`) con los nombres que pide la consigna y completar `readme.md` (hoy tiene secciones vacías).
- [ ] Revisar con todo el equipo que `trenes.md` y el código usen los mismos nombres.
