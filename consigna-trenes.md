# Trenes (TP4) — consigna

Transcripción de la sección 42 de `Aed Assignments.pdf` (pp. 135-137).

## 42.1. Objetivos

- Diseñar e implementar tipos.
- Aplicar composición de tipos.
- Demostrar capacidad de simular estructuras dinámicas.
- Estructuras enlazadas.

## 42.2. Temas

- Tipos.
- Estructuras.
- Flujos.

## 42.3. Problema

Dado una necesidad de servicios de trenes a satisfacer, se requiere armar las formaciones que permiten satisfacer los servicios de carga y de pasajero.

## 42.4. Restricciones

- Los flujos a usar son los estándar: `cin` y `cout`.
- Deben crearse dos versiones de la solución: contigua y enlazada.
- Los programas deben llamarse `trenescont` y `treneslink`.
- Los programas deben poder ejecutarse desde la línea de comando de la siguiente forma:

  ```
  trenescont < comandos.txt > resultados.txt
  ```

- Los comandos indican como crear las formaciones.
- Se deben especificar e implementar los siguientes tipos con sus operaciones:

  **Formación**
  - Crear : Recurso → Formación
  - AddLocomotora : Formación x Recurso → Formación
  - AddVagón : Formación x Recurso → Formación
  - GetCapaciddad : Formación → N
  - …

  **PoolDeRecursos**
  - Agregar : …
  - Sacar : …
  - …

## 42.5. Código de Partida

```cpp
#include<array>
#include<string>
using namespace std::literals;

struct Recurso{
  static const unsigned CapacidadCarga{17};
  static const unsigned CapacidadPasaje{21};

  enum struct Tipo{Locomotora, Pasajero, Comedor, Cocina, Carga};

  Tipo tipo;
  std::string id;
  unsigned capacidad;//despues lo vemos
};

struct Formación{
  static const unsigned LongitudMáxima{10};
  std::string id;
  std::array<Recurso, LongitudMáxima> links;
};
int main() {

  Recurso primero{ Recurso::Tipo::Locomotora, "L001"s, 2};
}
```

## 42.6. Productos

```
DD-Polígonos
  |-- readme.md
  |-- trenes.md // especificación
  |-- trenescont.cpp
  `-- treneslink.cpp
```
