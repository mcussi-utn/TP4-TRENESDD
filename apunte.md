# Apunte TP4 — Trenes (`trenescont` y `treneslink`)

Apunte interno para defender el TP. Lo que se entrega (comandos, reglas, restricciones, cómo ejecutar) está en `readme.md` y las firmas de las funciones en `trenes.md`. Acá va cómo funciona el código, paso a paso.

## 1. Qué hace el programa

Lee comandos por `cin` hasta que se acaba la entrada. Cada comando arma o modifica una formación sacando recursos del pool, y el resultado sale por `cout`. Las dos versiones tienen las mismas funciones y dan la misma salida: solo cambia cómo se guardan los recursos.

## 2. Estructura de los archivos

1. `#include`.
2. Tipos: `Recurso`, `Formación`, `PoolDeRecursos`, `Formaciones` (en la enlazada, además, `Nodo`).
3. Prototipos solo de lo que usa `main` (como en `Dim3Extra.cpp`); el resto está definido antes de usarse.
4. `main`: `Pruebas()`, crea el pool y las formaciones, `CargarPool` y `LeerComandos`. En la enlazada, al final, `LiberarTodo`.
5. Funciones agrupadas por tipo: Recurso, Pool, Formación, Formaciones, Comandos, Pruebas.

## 3. Conceptos que se usan

**Ya vistos en TPs anteriores**
- **Composición de tipos:** `Recurso` dentro de `Formación`, y `Formación` dentro de `Formaciones`.
- **`static const` dentro de un `struct`:** constantes compartidas por todas las variables del tipo (no ocupan espacio en cada una).
- **`enum struct`:** se escribe `Recurso::Tipo::Carga`, sin que se mezcle con otros nombres.
- **`std::array` + `.at()`:** el arreglo siempre tiene todas sus posiciones; `.at()` avisa si el índice se pasa de rango.
- **Referencias:** `&` modifica el original; `const &` solo lee, sin copiar. Un parámetro por referencia también sirve de salida: `Sacar(p, t, salida)` devuelve `bool` y deja el recurso en `salida`.
- **Estilo guarda:** `AddVagón` y `AddLocomotora` salen con `return false` antes de tocar nada.
- **`static_cast`:** `Buscar` devuelve `int` (para el −1) pero recorre con `unsigned`, así que convierte.
- **Flujos estándar:** `for(std::string comando; std::cin >> comando;)` lee hasta que la entrada se acaba (mismo patrón de `Dim1.cpp`).

**Propios de este TP**
- **`cantidad` + postincremento:** `links.at(cantidad++) = r` escribe en la primera posición libre y *después* sube el contador.
- **Redondeo hacia arriba con enteros:** `(toneladas + 50 − 1) / 50`. Con 120 t: (120 + 49) / 50 = 3 vagones.
- **Cortocircuito de `&&`:** si la primera condición es falsa, las siguientes no se evalúan. `LeerComandos` lo usa: `Armar` solo se ejecuta si el id no existe *y* hay lugar para otra formación.
- **`assert`:** en `Pruebas()`, corta el programa si algo no se cumple; si todo está bien no imprime nada.

**Nuevos (solo `treneslink`)**
- **Nodo y lista enlazada:** un `Nodo` guarda un `Recurso` y un puntero `siguiente` al próximo. Es puntero (`Nodo*`) porque un `Nodo` no puede contener otro `Nodo` entero. El último apunta a `nullptr`.
- **`new` / `delete`:** `new Nodo{...}` pide memoria y devuelve su dirección; `delete` la devuelve. Cada `new` tiene que tener su `delete`.
- **`->`:** accede a un campo a través de un puntero: `n->siguiente` es `(*n).siguiente`.
- **`Nodo*&`:** referencia a un puntero, para que la función pueda cambiar el `primero` de quien la llama.

## 4. Los tipos

| Tipo | Contigua | Enlazada |
|---|---|---|
| `Recurso` | `tipo`, `id`, `capacidad` | igual |
| `Formación` | `std::array<Recurso, 22> links` + `cantidad` | `Nodo* primero` + `cantidad` |
| `PoolDeRecursos` | `std::array<Recurso, 200> recursos` + `cantidad` | `Nodo* primero`, sin tope |
| `Formaciones` | `std::array<Formación, 14> items` + `cantidad` | igual (el arreglo guarda las cabezas de las listas) |

## 5. Versión contigua, función por función

**Recurso**
- `CrearRecurso`: arma el `Recurso`; la capacidad es 50 para carga, 150 para pasajeros y 0 para el resto.
- `LeerTipo`: convierte el texto (`"CARGA"`, `"PASAJERO"`, `"COMEDOR"`, `"COCINA"`) a `Tipo`; devuelve `false` si no lo conoce.

**Pool**
- `Agregar`: escribe al final (`recursos.at(cantidad++) = r`).
- `Contar`: recorre y cuenta los de un tipo.
- `Sacar`: busca el primero del tipo, lo copia a `salida`, corre un lugar los siguientes para tapar el hueco y baja `cantidad`. Si no hay ninguno, `false` y el pool queda igual.
- `CargarTipo` / `CargarPool`: llenan el pool inicial con ids `LC1, LC2…`, `VC1…`, etc.
- `MostrarPool`: imprime cuántos hay de cada tipo.

**Formación**
- `Crear`: la locomotora va en la posición 0 y `cantidad` queda en 1.
- `EsDeCarga`: mira el tipo de la posición 0.
- `GetCantidadLocomotoras`, `GetCantidadVagones` (`cantidad` − locomotoras) y `GetCapacidad` (suma de capacidades).
- `AddLocomotora`: solo en formaciones de carga, solo locomotoras de carga, hasta 2.
- `AddVagón`: el vagón tiene que ser compatible (carga acepta C; pasajeros acepta P, M y K) y no pasarse del máximo: en carga, 10 por cada locomotora que ya tiene; en pasajeros, 10.
- `Enganchar`: `Sacar` del pool + `AddVagón`. Si el vagón no entra, vuelve al pool (`Agregar`).
- `Armar`: ver abajo.
- `Mostrar`: imprime id, clase, locomotoras, vagones, capacidad y composición.

**`Armar(p, id, carga, cantidad, f)`** — paso a paso:
1. Según `carga` elige los tipos (locomotora, vagón) y la capacidad por vagón (50 o 150).
2. `vagones` = `cantidad` / capacidad, redondeado hacia arriba.
3. `locomotoras` = 2 si `vagones` > 10, si no 1.
4. **Verifica antes de sacar nada:** que `vagones` no pase del máximo (20 en carga, 10 en pasajeros) y que el pool tenga las locomotoras y los vagones. Si no, `return false` sin tocar el pool.
5. Saca la locomotora y crea la formación con `Crear`. Si son 2, saca la segunda y la agrega con `AddLocomotora` *antes* de los vagones, porque `AddVagón` calcula el máximo según cuántas locomotoras hay.
6. Llama a `Enganchar` una vez por vagón.

**Formaciones**
- `Buscar`: recorre `items` y devuelve el índice con ese id, o −1.

**`LeerComandos`**: lee la palabra del comando y despacha.
- `CARGA` / `PASAJEROS`: lee id y cantidad. Si el id no existe, hay lugar y `Armar` funciona, guarda la formación y la muestra; si no, imprime "No se pudo armar".
- `VAGON`: busca la formación, convierte el texto con `LeerTipo` y llama a `Enganchar`; imprime OK o "no se pudo enganchar".
- `MOSTRAR` / `POOL`: muestran una formación / el pool.
- Cualquier otra palabra: "Comando desconocido".

**`Pruebas`**: arma una formación de 120 t, verifica sus 3 vagones y 150 de capacidad, que un comedor no entre en carga, y que 1200 t (24 vagones) no se pueda armar.

## 6. Versión enlazada: qué cambia

Lo único que cambia es cómo se guardan, recorren y modifican las listas. `Armar`, `Enganchar`, `Buscar`, `LeerComandos` y `Pruebas` son iguales.

- **`Enlazar(primero, r)`**: crea un nodo con `new Nodo{r, nullptr}`. Si la lista está vacía, el nodo es el `primero`; si no, camina hasta el último (`while(n->siguiente)`) y lo engancha ahí. Recibe `Nodo*&` porque puede tener que cambiar `primero`. Agregar al final mantiene el mismo orden que la versión contigua.
- **Agregar** (`Agregar`, `Crear`, `AddLocomotora`, `AddVagón`): llaman a `Enlazar`; las formaciones además suben `cantidad`.
- **Recorrer** (`Contar`, `GetCantidadLocomotoras`, `GetCapacidad`, `Mostrar`): `for(const Nodo* i = primero; i; i = i->siguiente)`. El ciclo termina cuando `i` es `nullptr`.
- **`EsDeCarga`**: `f.primero->recurso.tipo`. El primer nodo siempre existe porque `Crear` lo agrega.
- **`Sacar`**: recorre llevando un puntero `anterior`. Al encontrar el nodo: copia su recurso a `salida`, **desconecta** el nodo (`anterior->siguiente = n->siguiente`, o `p.primero = n->siguiente` si era el primero) y recién entonces hace `delete n`. Si se borrara antes de desconectar, la lista quedaría apuntando a memoria liberada.
- **`LiberarLista`**: mientras haya primero, se guarda el siguiente, borra el nodo y avanza.
- **`LiberarTodo`**: libera la lista de cada formación guardada y la del pool. `main` lo llama al final. Así cada `new` tiene su `delete`: `Sacar` borra el nodo que saca y `LiberarTodo` borra el resto.
- **Guardar una formación**: `fs.items.at(...) = f` copia los punteros; desde ahí la lista es de `fs` y la `f` local no libera nada.

## 7. Flujo de principio a fin

Entrada:

```text
POOL
CARGA C1 120
CARGA C2 600
PASAJEROS P1 300
VAGON P1 COMEDOR
VAGON P1 COCINA
VAGON C1 COMEDOR
MOSTRAR P1
POOL
```

Salida (idéntica en las dos versiones):

```text
Pool: 4 LC, 5 LP, 80 C, 60 P, 10 M, 10 K
Formacion C1 [carga] | Locomotoras: 1 | Vagones: 3 | Capacidad: 150
  Composicion: LC1 VC1 VC2 VC3
Formacion C2 [carga] | Locomotoras: 2 | Vagones: 12 | Capacidad: 600
  Composicion: LC2 LC3 VC4 VC5 VC6 VC7 VC8 VC9 VC10 VC11 VC12 VC13 VC14 VC15
Formacion P1 [pasajeros] | Locomotoras: 1 | Vagones: 2 | Capacidad: 300
  Composicion: LP1 VP1 VP2
Vagon COMEDOR -> P1: OK
Vagon COCINA -> P1: OK
Vagon COMEDOR -> C1: no se pudo enganchar
Formacion P1 [pasajeros] | Locomotoras: 1 | Vagones: 4 | Capacidad: 300
  Composicion: LP1 VP1 VP2 VM1 VK1
Pool: 1 LC, 4 LP, 65 C, 58 P, 9 M, 9 K
```

Qué pasa en cada paso:

1. **`main`** corre `Pruebas()`, crea el pool vacío y las formaciones vacías, y `CargarPool` lo llena (4 + 5 + 80 + 60 + 10 + 10 recursos).
2. **`POOL`**: `MostrarPool` cuenta cada tipo con `Contar`.
3. **`CARGA C1 120`**: `Buscar` no encuentra `C1` (−1) y hay lugar, así que se llama a `Armar`: 120 t → (120 + 49) / 50 = 3 vagones, 1 locomotora. Verifica el pool, saca `LC1`, crea la formación y engancha `VC1`, `VC2` y `VC3`. Se guarda en `items` (posición 0) y se muestra.
4. **`CARGA C2 600`**: 12 vagones > 10, así que lleva 2 locomotoras: saca `LC2`, crea la formación, saca `LC3` y la agrega con `AddLocomotora`, y después engancha 12 vagones (el máximo ahora es 10 × 2 = 20).
5. **`PASAJEROS P1 300`**: 300 / 150 = 2 vagones y 1 locomotora (`LP1`, `VP1`, `VP2`).
6. **`VAGON P1 COMEDOR`** (y `COCINA`): `Buscar` encuentra `P1`, `LeerTipo` convierte el texto, `Enganchar` saca `VM1` del pool y `AddVagón` lo acepta (pasajeros admite comedor y cocina). Imprime OK.
7. **`VAGON C1 COMEDOR`**: `Enganchar` saca `VM2`, pero `AddVagón` lo rechaza (carga solo acepta C); `Enganchar` lo devuelve al pool y el comando imprime "no se pudo enganchar".
8. **`MOSTRAR P1`**: muestra `P1` con sus 4 vagones.
9. **`POOL`**: 1 LC (4 − 1 − 2), 4 LP (5 − 1), 65 C (80 − 3 − 12), 58 P (60 − 2), 9 M y 9 K (uno de cada uno quedó en `P1`).
10. **Fin de la entrada:** `std::cin >> comando` falla y el `for` termina. En la enlazada, `main` llama a `LiberarTodo`.
