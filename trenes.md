# **Especificación matemática del TP4 — Trenes**

## Respecto de la simbología matemática:
- N: Números Naturales (incluido el 0).
- Z: Números Enteros.
- B: Booleanos (Verdadero o Falso).
- Σ*: Cadenas de caracteres.
- Ø: Sin valor de retorno.
- ⟲: Parámetro por referencia, la función lo modifica.

## Respecto de nuestros tipos:
- T: Tipos de recurso: (LC, LP, P, M, K, C) = locomotora de carga, locomotora de pasajeros, vagón de pasajeros, comedor, cocina, vagón de carga.
- R: Recursos = T x Σ* x N (tipo, id, capacidad).
- F: Formaciones = Σ* x R* (id y recursos en orden de enganche; el primero es una locomotora).
- P: Pool de recursos disponibles.
- FS: Colección de formaciones (hasta 14).

## Especificación Matemática de todas las funciones utilizadas:

### Recurso
- **CrearRecurso:** T x Σ* ---> R
- **LeerTipo:** Σ* ---> T x B

### PoolDeRecursos
- **Agregar:** P ⟲ x R ---> Ø
- **Sacar:** P ⟲ x T ---> R x B
- **Contar:** P x T ---> N
- **CargarTipo:** P ⟲ x T x Σ* x N ---> Ø
- **CargarPool:** P ⟲ ---> Ø
- **MostrarPool:** P ---> Ø

### Formación
- **Crear:** R x Σ* ---> F
- **EsDeCarga:** F ---> B
- **GetCantidadLocomotoras:** F ---> N
- **GetCantidadVagones:** F ---> N
- **GetCapacidad:** F ---> N
- **AddLocomotora:** F ⟲ x R ---> B
- **AddVagón:** F ⟲ x R ---> B
- **Enganchar:** F ⟲ x P ⟲ x T ---> B (saca el vagón del pool y lo agrega)
- **Armar:** P ⟲ x Σ* x B x N ---> F x B (B: carga o pasajeros; N: toneladas o pasajeros)
- **Mostrar:** F ---> Ø

### Colección de formaciones
- **Buscar:** FS x Σ* ---> Z (índice, o −1 si no existe)
