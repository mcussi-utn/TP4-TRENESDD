# Apunte TP4 — Trenes (`trenescont` y `treneslink`)

Apunte interno para defender el TP. Lo que se entrega está en `readme.md` (comandos, reglas, restricciones, ejecución); acá va lo que conviene saber explicar.

## 1. Qué hace el programa

Lee comandos por `cin`, arma formaciones de tren para un servicio (toneladas o pasajeros) sacando recursos de un pool fijo, y escribe el resultado por `cout`.

| Comando | Qué hace |
|---|---|
| `CARGA <id> <toneladas>` | `Armar` de carga: ⌈toneladas / 50⌉ vagones, 2 locomotoras si pasa de 10 |
| `PASAJEROS <id> <pasajeros>` | `Armar` de pasajeros: ⌈pasajeros / 150⌉ vagones, hasta 10 |
| `VAGON <id> <tipo>` | `Enganchar` un vagón más (así se agregan comedor y cocina) |
| `MOSTRAR <id>` / `POOL` | `Mostrar` una formación / `MostrarPool` |

## 2. Estructura (igual en los dos archivos)

1. `#include` (sin comentario de encabezado: el uso y los comandos están en `readme.md`, como en DD-Dims).
2. Tipos: `Recurso`, `Formación`, `PoolDeRecursos`, `Formaciones` (en la enlazada, además, `Nodo`).
3. Prototipos solo de lo que usa `main` (como en `Dim3Extra.cpp`); el resto está definido antes de usarse.
4. `main`: `Pruebas`, carga el pool y `LeerComandos`. En la enlazada, al final, `LiberarTodo`.
5. Funciones agrupadas por tipo, `LeerComandos` y `Pruebas` (4 `assert`, como en 02-Recur).

## 3. Los tipos

| Tipo | Contigua | Enlazada |
|---|---|---|
| `Recurso` | `tipo`, `id`, `capacidad` | igual |
| `Formación` | `std::array<Recurso, 22> links` + `cantidad` | `Nodo* primero` + `cantidad` |
| `PoolDeRecursos` | `std::array<Recurso, 200> recursos` + `cantidad` | `Nodo* primero`, sin tope |
| `Formaciones` | `std::array<Formación, 14> items` + `cantidad` | igual |

- **Contigua:** `cantidad` dice cuántas posiciones están ocupadas. Agregar es `links.at(cantidad++) = r`.
- **Enlazada:** cada `Nodo` guarda un `Recurso` y un puntero `siguiente` (puntero porque un `Nodo` no puede contener otro `Nodo` entero). `new` al agregar, `delete` al sacar. `Enlazar` camina hasta el último nodo para agregar al final, así el orden es el mismo que en la contigua y las salidas coinciden.
- **`Buscar`** devuelve el índice o −1 (como `BuscarFormacion` de lula). El `static_cast<int>` hace falta porque `i` es `unsigned`, igual que `cantidad`; ya lo usamos en `Dim3Extra.cpp`.

## 4. Las funciones clave

- **`AddVagón`:** carga acepta C (10 por locomotora); pasajeros acepta P, M y K (hasta 10). Si no puede, `false` sin tocar nada.
- **`AddLocomotora`:** solo carga, hasta 2.
- **`Enganchar`:** `Sacar` del pool + `AddVagón`; si no entra, el vagón vuelve al pool.
- **`Armar`:** calcula vagones y locomotoras, **primero verifica** con `Contar` que el pool alcance y recién después saca. Así nunca queda una formación a medias.
- **`Sacar`:** la que más cambia entre versiones (contigua corre los siguientes; enlazada desconecta el nodo y hace `delete`).

## 5. Diferencias entre las dos versiones

| Función | Contigua | Enlazada |
|---|---|---|
| `Agregar` / `AddVagón` | `links.at(cantidad++) = r` | `Enlazar`: `new Nodo` y lo engancha al final |
| `Sacar` | busca y corre los siguientes un lugar | busca, desconecta el nodo y hace `delete` |
| Recorridos | `for(unsigned i{}; i < cantidad; ++i)` | `for(const Nodo* i = primero; i; i = i->siguiente)` |
| Memoria | se libera sola | `LiberarTodo` al final de `main` |

## 6. Qué se simplificó (2026-11-17)

Se sacaron `ELIMINAR`, `FORMACION`, `FORMACIONES`, los comentarios `#` en la entrada, el enum `Resultado` con sus mensajes (ahora todo devuelve `bool`), el conductor, el viaje corto/largo, los topes de 1000 t y 600 km, la segunda locomotora automática en `Enganchar`, el puntero `ultimo` de la enlazada y casi todos los `assert`. Quedaron 20 funciones en lugar de unas 40. Los comentarios de uso y de comandos pasaron de los `.cpp` al `readme.md`.

Limitación conocida, aceptada para no sumar validaciones: las cantidades se leen como `unsigned`, así que un número negativo en `CARGA` o `PASAJEROS` no da error: el cálculo da la vuelta y se arma una formación con solo la locomotora. `comandos.txt` no lo prueba.

## 7. Pendiente

- [ ] Confirmar con el docente que está bien no usar `std::vector` y que `std::array` para la contigua es lo esperado.
- [ ] Confirmar las reglas de negocio (50 t, 150 pasajeros, límites): salieron de `lula.cpp`; la consigna solo trae valores de ejemplo (17 y 21).
- [ ] Revisar fugas de memoria de `treneslink` con una herramienta (acá no se pudo: solo se verificó por lectura que cada `new` tiene su `delete`).
- [ ] Copiar `readme.md`, `trenes.md`, `comandos.txt`, `trenescont.cpp` y `treneslink.cpp` al repo (`DD-Trenes`), reemplazando lo que hay ahí.
