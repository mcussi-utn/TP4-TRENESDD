// treneslink - versión ENLAZADA (nodos con new / delete)
// Uso: treneslink < comandos.txt > resultados.txt
// Especificación: trenes.md (la misma para trenescont y treneslink)
// Misma interfaz y mismos comandos que trenescont; cambia solo cómo se guardan los recursos.
//
// Comandos (uno o más por línea, separados por espacios; '#' comenta la línea):
//   CARGA <id> <toneladas>            arma una formación de carga completa
//   PASAJEROS <id> <pasajeros> <km>   arma una formación de pasajeros completa
//   FORMACION <id> <C|P>              crea una formación con solo la locomotora
//   VAGON <id> <tipo>                 engancha un vagón del pool (CARGA, PASAJERO, COMEDOR, COCINA)
//   ELIMINAR <id>                     desarma la formación y devuelve los recursos al pool
//   MOSTRAR <id> | FORMACIONES | POOL
// #define NDEBUG   // descomentar para desactivar todos los assert (va antes de <cassert>)
#include <array>
#include <cassert>
#include <iostream>
#include <string>

// ---------- Tipos y arreglos ----------
struct Recurso{
    static const unsigned CapacidadCarga = 50;          // toneladas por vagón de carga
    static const unsigned CapacidadPasajeCorto = 150;   // pasajeros por vagón (viaje corto)
    static const unsigned CapacidadPasajeLargo = 50;    // pasajeros por vagón (viaje largo)
    enum struct Tipo{LocomotoraCarga, LocomotoraPasajeros, Pasajero, Comedor, Cocina, Carga};
    Tipo tipo{};
    std::string id;
    unsigned capacidad{};
    std::string conductor;   // solo lo usan las locomotoras
};

// Nodo de la lista enlazada: un recurso y el puntero al siguiente.
// Es puntero (Nodo*) porque un Nodo no puede contener otro Nodo entero.
struct Nodo{
    Recurso recurso;
    Nodo* siguiente{nullptr};
};

struct Formación{
    static const unsigned LocomotorasMáximasCarga = 2;
    static const unsigned VagonesMáximosCarga = 20;
    static const unsigned LocomotorasMáximasPasajeros = 1;
    static const unsigned VagonesMáximosPasajeros = 10;
    static const unsigned LongitudMáxima = LocomotorasMáximasCarga + VagonesMáximosCarga;
    static const unsigned VagonesPorLocomotora = 10;    // carga: 1 locomotora cada 10 vagones
    static const unsigned ToneladasMáximas = 1000;
    static const unsigned DistanciaMáxima = 600;        // km
    static const unsigned DistanciaLarga = 50;          // más de 50 km: cocina y comedor
    std::string id;
    Nodo* primero{nullptr};   // locomotoras y vagones, en orden de enganche
    Nodo* ultimo{nullptr};    // para agregar al final sin recorrer la lista
    unsigned cantidad{};
};

struct PoolDeRecursos{
    Nodo* primero{nullptr};   // sin tope: crece con new
    Nodo* ultimo{nullptr};
    unsigned cantidad{};
};

struct Formaciones{
    static const unsigned Máximo = 14;                  // los 14 servicios del enunciado
    std::array<Formación, Máximo> items;
    unsigned cantidad{};
};

enum struct Resultado{Ok, Incompatible, SinRecurso, SinLocomotora, FormaciónLlena,
                      ValorInválido, ExcedeCapacidad};

// ---------- Declaración de funciones ----------
// Recurso
bool EsLocomotora(Recurso::Tipo t);
unsigned CapacidadBase(Recurso::Tipo t);
Recurso CrearRecurso(Recurso::Tipo tipo, const std::string& id);
bool LeerTipo(const std::string& s, Recurso::Tipo& tipo);
// Lista enlazada (la usan Pool y Formación)
void Enlazar(Nodo*& primero, Nodo*& ultimo, const Recurso& r);
void LiberarLista(Nodo*& primero, Nodo*& ultimo);
// PoolDeRecursos
bool Agregar(PoolDeRecursos& p, const Recurso& r);
unsigned Contar(const PoolDeRecursos& p, Recurso::Tipo t);
bool Sacar(PoolDeRecursos& p, Recurso::Tipo t, Recurso& salida);
void CargarTipo(PoolDeRecursos& p, Recurso::Tipo t, const std::string& prefijo, unsigned n);
void CargarPool(PoolDeRecursos& p);
void MostrarPool(const PoolDeRecursos& p);
void Devolver(PoolDeRecursos& p, const Formación& f);
void Liberar(PoolDeRecursos& p);
// Formación
Formación Crear(const Recurso& locomotora, const std::string& id);
bool EsDeCarga(const Formación& f);
unsigned MaxVagones(const Formación& f);
unsigned MaxLocomotoras(const Formación& f);
unsigned GetCantidadLocomotoras(const Formación& f);
unsigned GetCantidadVagones(const Formación& f);
unsigned GetCapacidad(const Formación& f);
bool AddLocomotora(Formación& f, const Recurso& r);
bool EsCompatible(const Formación& f, Recurso::Tipo t);
bool AddVagón(Formación& f, const Recurso& r);
Resultado Enganchar(Formación& f, PoolDeRecursos& p, Recurso::Tipo t);
Resultado ArmarCarga(PoolDeRecursos& p, const std::string& id, double toneladas, Formación& salida);
Resultado ArmarPasajeros(PoolDeRecursos& p, const std::string& id, int pasajeros, int km, Formación& salida);
void Mostrar(const Formación& f);
void Liberar(Formación& f);
const char* Mensaje(Resultado r);
// Formaciones
int Buscar(const Formaciones& fs, const std::string& id);
bool PuedeCrear(const Formaciones& fs, const std::string& id);
void Registrar(Formaciones& fs, const Formación& f, Resultado r);
void Eliminar(Formaciones& fs, PoolDeRecursos& p, unsigned i);
void LiberarTodo(Formaciones& fs, PoolDeRecursos& p);
// Comandos
void Descartar();
void ComandoCarga(PoolDeRecursos& p, Formaciones& fs);
void ComandoPasajeros(PoolDeRecursos& p, Formaciones& fs);
void ComandoFormacion(PoolDeRecursos& p, Formaciones& fs);
void ComandoVagon(PoolDeRecursos& p, Formaciones& fs);
void ComandoEliminar(PoolDeRecursos& p, Formaciones& fs);
void ComandoMostrar(const Formaciones& fs);
void LeerComandos(PoolDeRecursos& p, Formaciones& fs);
// Pruebas
void Pruebas();

int main(){
    Pruebas();   // no imprime nada: si algo falla, el assert corta el programa
    PoolDeRecursos pool;
    Formaciones formaciones;
    CargarPool(pool);
    LeerComandos(pool, formaciones);
    LiberarTodo(formaciones, pool);   // devuelve toda la memoria pedida con new
}

// ---------- Recurso ----------
bool EsLocomotora(Recurso::Tipo t){
    return t == Recurso::Tipo::LocomotoraCarga || t == Recurso::Tipo::LocomotoraPasajeros;
}

unsigned CapacidadBase(Recurso::Tipo t){
    if(t == Recurso::Tipo::Carga) return Recurso::CapacidadCarga;
    if(t == Recurso::Tipo::Pasajero) return Recurso::CapacidadPasajeCorto;
    return 0;   // locomotoras, comedor y cocina no transportan carga ni pasajeros
}

Recurso CrearRecurso(Recurso::Tipo tipo, const std::string& id){
    return Recurso{tipo, id, CapacidadBase(tipo), ""};
}

// Devuelve true si el texto es un tipo de vagón; el tipo sale por 'tipo'
bool LeerTipo(const std::string& s, Recurso::Tipo& tipo){
    using T = Recurso::Tipo;
    if(s == "PASAJERO") tipo = T::Pasajero;
    else if(s == "COMEDOR") tipo = T::Comedor;
    else if(s == "COCINA") tipo = T::Cocina;
    else if(s == "CARGA") tipo = T::Carga;
    else return false;
    return true;
}

// ---------- Lista enlazada ----------
// Agrega un nodo nuevo al final de la lista
void Enlazar(Nodo*& primero, Nodo*& ultimo, const Recurso& r){
    Nodo* nuevo = new Nodo{r, nullptr};
    if(ultimo) ultimo->siguiente = nuevo;   // lista con elementos: engancha al último
    else primero = nuevo;                   // lista vacía: el nuevo es el primero
    ultimo = nuevo;
}

// Libera todos los nodos de la lista (un delete por cada new)
void LiberarLista(Nodo*& primero, Nodo*& ultimo){
    while(primero){
        Nodo* n = primero;
        primero = n->siguiente;   // primero se guarda el siguiente, después se borra
        delete n;
    }
    ultimo = nullptr;
}

// ---------- PoolDeRecursos ----------
bool Agregar(PoolDeRecursos& p, const Recurso& r){
    Enlazar(p.primero, p.ultimo, r);
    ++p.cantidad;
    return true;   // sin tope: nunca falla (mismo prototipo que la versión contigua)
}

unsigned Contar(const PoolDeRecursos& p, Recurso::Tipo t){
    unsigned n{};
    for(const Nodo* i = p.primero; i; i = i->siguiente)
        if(i->recurso.tipo == t) ++n;
    return n;
}

// Saca el primer recurso del tipo pedido; lo devuelve por 'salida'
bool Sacar(PoolDeRecursos& p, Recurso::Tipo t, Recurso& salida){
    Nodo* anterior{nullptr};
    for(Nodo* n = p.primero; n; anterior = n, n = n->siguiente){
        if(n->recurso.tipo == t){
            salida = n->recurso;
            if(anterior) anterior->siguiente = n->siguiente;   // desconecta el nodo de la cadena
            else p.primero = n->siguiente;
            if(n == p.ultimo) p.ultimo = anterior;
            delete n;                                          // y recién ahí lo libera
            --p.cantidad;
            return true;
        }
    }
    return false;
}

// Agrega 'n' recursos del mismo tipo con ids prefijo1, prefijo2, ...
void CargarTipo(PoolDeRecursos& p, Recurso::Tipo t, const std::string& prefijo, unsigned n){
    for(unsigned i = 1; i <= n; ++i)
        Agregar(p, CrearRecurso(t, prefijo + std::to_string(i)));
}

// Pool hardcodeado (no sale de ningún flujo)
void CargarPool(PoolDeRecursos& p){
    using T = Recurso::Tipo;
    const std::array<std::string, 4> conductoresCarga{"Carlos", "Mariano", "Jorge", "Pedro"};
    const std::array<std::string, 5> conductoresPasajeros{"Ana", "Lucia", "Martin", "Sofia", "Diego"};

    unsigned n{};
    for(const auto& conductor : conductoresCarga){
        Recurso r = CrearRecurso(T::LocomotoraCarga, "LC" + std::to_string(++n));
        r.conductor = conductor;
        Agregar(p, r);
    }
    n = 0;
    for(const auto& conductor : conductoresPasajeros){
        Recurso r = CrearRecurso(T::LocomotoraPasajeros, "LP" + std::to_string(++n));
        r.conductor = conductor;
        Agregar(p, r);
    }
    CargarTipo(p, T::Carga, "VC", 80);
    CargarTipo(p, T::Pasajero, "VP", 60);
    CargarTipo(p, T::Comedor, "VM", 10);
    CargarTipo(p, T::Cocina, "VK", 10);
}

void MostrarPool(const PoolDeRecursos& p){
    using T = Recurso::Tipo;
    std::cout << "Recursos disponibles:\n"
              << "  Locomotoras de carga: " << Contar(p, T::LocomotoraCarga) << '\n'
              << "  Vagones de carga: " << Contar(p, T::Carga) << '\n'
              << "  Locomotoras de pasajeros: " << Contar(p, T::LocomotoraPasajeros) << '\n'
              << "  Vagones de pasajeros: " << Contar(p, T::Pasajero) << '\n'
              << "  Cocinas: " << Contar(p, T::Cocina) << '\n'
              << "  Comedores: " << Contar(p, T::Comedor) << '\n';
}

// Devuelve al pool una copia de cada recurso de la formación (con su capacidad original)
void Devolver(PoolDeRecursos& p, const Formación& f){
    for(const Nodo* i = f.primero; i; i = i->siguiente){
        Recurso r = i->recurso;
        r.capacidad = CapacidadBase(r.tipo);
        Agregar(p, r);
    }
}

void Liberar(PoolDeRecursos& p){
    LiberarLista(p.primero, p.ultimo);
    p.cantidad = 0;
}

// ---------- Formación ----------
Formación Crear(const Recurso& locomotora, const std::string& id){
    assert(EsLocomotora(locomotora.tipo));   // precondición: la llama código propio
    Formación f;
    f.id = id;
    Enlazar(f.primero, f.ultimo, locomotora);   // la locomotora es el primer nodo
    f.cantidad = 1;
    return f;
}

bool EsDeCarga(const Formación& f){
    return f.primero->recurso.tipo == Recurso::Tipo::LocomotoraCarga;
}

unsigned MaxVagones(const Formación& f){
    return EsDeCarga(f) ? Formación::VagonesMáximosCarga : Formación::VagonesMáximosPasajeros;
}

unsigned MaxLocomotoras(const Formación& f){
    return EsDeCarga(f) ? Formación::LocomotorasMáximasCarga : Formación::LocomotorasMáximasPasajeros;
}

unsigned GetCantidadLocomotoras(const Formación& f){
    unsigned n{};
    for(const Nodo* i = f.primero; i; i = i->siguiente)
        if(EsLocomotora(i->recurso.tipo)) ++n;
    return n;
}

unsigned GetCantidadVagones(const Formación& f){
    return f.cantidad - GetCantidadLocomotoras(f);
}

unsigned GetCapacidad(const Formación& f){
    unsigned total{};
    for(const Nodo* i = f.primero; i; i = i->siguiente)
        total += i->recurso.capacidad;
    return total;
}

// Estilo "guarda": si algo falla se sale antes de tocar la formación
bool AddLocomotora(Formación& f, const Recurso& r){
    if(f.cantidad == Formación::LongitudMáxima) return false;
    if(GetCantidadLocomotoras(f) == MaxLocomotoras(f)) return false;
    if(r.tipo != f.primero->recurso.tipo) return false;   // misma clase que la inicial
    Enlazar(f.primero, f.ultimo, r);
    ++f.cantidad;
    return true;
}

bool EsCompatible(const Formación& f, Recurso::Tipo t){
    using T = Recurso::Tipo;
    if(EsDeCarga(f)) return t == T::Carga;
    return t == T::Pasajero || t == T::Comedor || t == T::Cocina;
}

bool AddVagón(Formación& f, const Recurso& r){
    if(f.cantidad == Formación::LongitudMáxima) return false;
    if(GetCantidadVagones(f) == MaxVagones(f)) return false;
    if(EsLocomotora(r.tipo) || !EsCompatible(f, r.tipo)) return false;
    Enlazar(f.primero, f.ultimo, r);
    ++f.cantidad;
    return true;
}

// Engancha un vagón sacándolo del pool; en carga suma una locomotora si hace falta.
// Valida todo antes de sacar: si no devuelve Ok, el pool queda intacto.
Resultado Enganchar(Formación& f, PoolDeRecursos& p, Recurso::Tipo t){
    if(EsLocomotora(t) || !EsCompatible(f, t)) return Resultado::Incompatible;
    if(GetCantidadVagones(f) == MaxVagones(f)) return Resultado::FormaciónLlena;

    bool requiereLocomotora = EsDeCarga(f) &&
        GetCantidadVagones(f) + 1 > Formación::VagonesPorLocomotora * GetCantidadLocomotoras(f);

    if(requiereLocomotora && GetCantidadLocomotoras(f) == MaxLocomotoras(f))
        return Resultado::FormaciónLlena;
    if(Contar(p, t) == 0) return Resultado::SinRecurso;
    if(requiereLocomotora && Contar(p, Recurso::Tipo::LocomotoraCarga) == 0)
        return Resultado::SinLocomotora;

    Recurso vagón;
    Sacar(p, t, vagón);
    AddVagón(f, vagón);
    if(requiereLocomotora){
        Recurso locomotora;
        Sacar(p, Recurso::Tipo::LocomotoraCarga, locomotora);
        AddLocomotora(f, locomotora);
    }
    return Resultado::Ok;
}

// Arma una formación de carga. Valida todo antes de sacar del pool.
Resultado ArmarCarga(PoolDeRecursos& p, const std::string& id, double toneladas, Formación& salida){
    if(toneladas <= 0) return Resultado::ValorInválido;
    if(toneladas > Formación::ToneladasMáximas) return Resultado::ExcedeCapacidad;

    unsigned vagones = static_cast<unsigned>(toneladas / Recurso::CapacidadCarga);
    if(toneladas > vagones * Recurso::CapacidadCarga) ++vagones;   // redondeo hacia arriba
    unsigned locomotoras = vagones <= Formación::VagonesPorLocomotora ? 1u : 2u;

    if(Contar(p, Recurso::Tipo::LocomotoraCarga) < locomotoras) return Resultado::SinLocomotora;
    if(Contar(p, Recurso::Tipo::Carga) < vagones) return Resultado::SinRecurso;

    Recurso r;
    Sacar(p, Recurso::Tipo::LocomotoraCarga, r);
    salida = Crear(r, id);
    for(unsigned i = 1; i < locomotoras; ++i){
        Sacar(p, Recurso::Tipo::LocomotoraCarga, r);
        AddLocomotora(salida, r);
    }
    for(unsigned i{}; i < vagones; ++i){
        Sacar(p, Recurso::Tipo::Carga, r);
        AddVagón(salida, r);
    }
    return Resultado::Ok;
}

// Arma una formación de pasajeros. Más de 50 km: viaje largo (vagones de 50 + cocina + comedor).
Resultado ArmarPasajeros(PoolDeRecursos& p, const std::string& id, int pasajeros, int km, Formación& salida){
    if(pasajeros <= 0 || km <= 0) return Resultado::ValorInválido;
    if(km > static_cast<int>(Formación::DistanciaMáxima)) return Resultado::ExcedeCapacidad;

    bool largo = km > static_cast<int>(Formación::DistanciaLarga);
    unsigned capacidad = largo ? Recurso::CapacidadPasajeLargo : Recurso::CapacidadPasajeCorto;
    unsigned vagones = (static_cast<unsigned>(pasajeros) + capacidad - 1) / capacidad;   // redondeo hacia arriba
    unsigned servicio = largo ? 2u : 0u;   // cocina + comedor

    if(vagones + servicio > Formación::VagonesMáximosPasajeros) return Resultado::ExcedeCapacidad;
    if(Contar(p, Recurso::Tipo::LocomotoraPasajeros) == 0) return Resultado::SinLocomotora;
    if(Contar(p, Recurso::Tipo::Pasajero) < vagones ||
       (largo && (Contar(p, Recurso::Tipo::Cocina) == 0 || Contar(p, Recurso::Tipo::Comedor) == 0)))
        return Resultado::SinRecurso;

    Recurso r;
    Sacar(p, Recurso::Tipo::LocomotoraPasajeros, r);
    salida = Crear(r, id);
    if(largo){
        Sacar(p, Recurso::Tipo::Cocina, r);
        AddVagón(salida, r);
        Sacar(p, Recurso::Tipo::Comedor, r);
        AddVagón(salida, r);
    }
    for(unsigned i{}; i < vagones; ++i){
        Sacar(p, Recurso::Tipo::Pasajero, r);
        r.capacidad = capacidad;   // la capacidad depende del tipo de viaje
        AddVagón(salida, r);
    }
    return Resultado::Ok;
}

void Mostrar(const Formación& f){
    std::cout << "Formacion " << f.id << (EsDeCarga(f) ? " [carga]" : " [pasajeros]") << '\n'
              << "  Locomotoras: " << GetCantidadLocomotoras(f)
              << " | Vagones: " << GetCantidadVagones(f)
              << " | Capacidad: " << GetCapacidad(f)
              << (EsDeCarga(f) ? " toneladas" : " pasajeros") << '\n'
              << "  Composicion:";
    for(const Nodo* i = f.primero; i; i = i->siguiente){
        std::cout << ' ' << i->recurso.id;
        if(EsLocomotora(i->recurso.tipo)) std::cout << '(' << i->recurso.conductor << ')';
    }
    std::cout << '\n';
}

// Libera los nodos de la formación (Devolver ya copió sus recursos al pool)
void Liberar(Formación& f){
    LiberarLista(f.primero, f.ultimo);
    f.cantidad = 0;
}

const char* Mensaje(Resultado r){
    switch(r){
        case Resultado::Ok: return "OK";
        case Resultado::Incompatible: return "vagon incompatible con la formacion";
        case Resultado::SinRecurso: return "no hay suficientes recursos en el pool";
        case Resultado::SinLocomotora: return "no hay locomotora disponible en el pool";
        case Resultado::FormaciónLlena: return "se supera el maximo de la formacion";
        case Resultado::ValorInválido: return "los valores deben ser mayores que cero";
        case Resultado::ExcedeCapacidad: return "supera la capacidad maxima permitida";
    }
    return "";
}

// ---------- Formaciones ----------
// Devuelve el índice de la formación o -1 si no existe
int Buscar(const Formaciones& fs, const std::string& id){
    for(unsigned i{}; i < fs.cantidad; ++i)
        if(fs.items.at(i).id == id) return static_cast<int>(i);
    return -1;
}

// Verifica que se pueda dar de alta 'id'; si no, informa el motivo
bool PuedeCrear(const Formaciones& fs, const std::string& id){
    if(Buscar(fs, id) != -1){
        std::cout << "Error: ya existe la formacion " << id << '\n';
        return false;
    }
    if(fs.cantidad == Formaciones::Máximo){
        std::cout << "Error: no caben mas formaciones\n";
        return false;
    }
    return true;
}

// Guarda la formación armada (si el resultado fue Ok) e informa.
// La copia comparte los nodos con 'f', pero 'f' es una variable local que se descarta.
void Registrar(Formaciones& fs, const Formación& f, Resultado r){
    if(r != Resultado::Ok){
        std::cout << "Error al crear " << f.id << ": " << Mensaje(r) << '\n';
        return;
    }
    fs.items.at(fs.cantidad++) = f;
    std::cout << "Formacion " << f.id << " creada\n";
    Mostrar(f);
}

// Desarma la formación 'i': devuelve sus recursos, libera sus nodos y compacta el arreglo
void Eliminar(Formaciones& fs, PoolDeRecursos& p, unsigned i){
    assert(i < fs.cantidad);   // precondición: Buscar ya verificó el índice
    Devolver(p, fs.items.at(i));
    Liberar(fs.items.at(i));
    for(unsigned j = i; j + 1 < fs.cantidad; ++j)
        fs.items.at(j) = fs.items.at(j + 1);   // mueve las cabezas de lista (se copian punteros)
    --fs.cantidad;
    fs.items.at(fs.cantidad) = Formación{};   // limpia el hueco para no dejar un puntero repetido
}

void LiberarTodo(Formaciones& fs, PoolDeRecursos& p){
    for(unsigned i{}; i < fs.cantidad; ++i)
        Liberar(fs.items.at(i));
    Liberar(p);
}

// ---------- Comandos ----------
// Tras una lectura fallida: limpia el error y descarta el resto de la línea
void Descartar(){
    std::cin.clear();
    std::string resto;
    std::getline(std::cin, resto);
}

void ComandoCarga(PoolDeRecursos& p, Formaciones& fs){   // CARGA <id> <toneladas>
    Formación f;
    double toneladas{};
    std::cin >> f.id >> toneladas;
    if(!std::cin){
        Descartar();
        std::cout << "Error: argumentos invalidos para CARGA\n";
    }
    else if(PuedeCrear(fs, f.id)){
        Resultado r = ArmarCarga(p, f.id, toneladas, f);
        Registrar(fs, f, r);
    }
}

void ComandoPasajeros(PoolDeRecursos& p, Formaciones& fs){   // PASAJEROS <id> <pasajeros> <km>
    Formación f;
    int pasajeros{}, km{};
    std::cin >> f.id >> pasajeros >> km;
    if(!std::cin){
        Descartar();
        std::cout << "Error: argumentos invalidos para PASAJEROS\n";
    }
    else if(PuedeCrear(fs, f.id)){
        Resultado r = ArmarPasajeros(p, f.id, pasajeros, km, f);
        Registrar(fs, f, r);
    }
}

void ComandoFormacion(PoolDeRecursos& p, Formaciones& fs){   // FORMACION <id> <C|P>
    std::string id, clase;
    std::cin >> id >> clase;
    Recurso loc;
    if(clase != "C" && clase != "P")
        std::cout << "Error: la clase debe ser C o P\n";
    else if(PuedeCrear(fs, id)){
        Recurso::Tipo t = clase == "C" ? Recurso::Tipo::LocomotoraCarga
                                       : Recurso::Tipo::LocomotoraPasajeros;
        if(!Sacar(p, t, loc))
            std::cout << "Error: no hay locomotora disponible para " << id << '\n';
        else{
            fs.items.at(fs.cantidad++) = Crear(loc, id);
            std::cout << "Formacion " << id << " creada con locomotora " << loc.id << '\n';
        }
    }
}

void ComandoVagon(PoolDeRecursos& p, Formaciones& fs){   // VAGON <idFormacion> <tipo>
    std::string id, tipo;
    std::cin >> id >> tipo;
    int i = Buscar(fs, id);
    Recurso::Tipo t;
    if(i == -1) std::cout << "Error: no existe la formacion " << id << '\n';
    else if(!LeerTipo(tipo, t)) std::cout << "Error: tipo desconocido " << tipo << '\n';
    else std::cout << "Vagon " << tipo << " -> " << id << ": "
                   << Mensaje(Enganchar(fs.items.at(i), p, t)) << '\n';
}

void ComandoEliminar(PoolDeRecursos& p, Formaciones& fs){   // ELIMINAR <id>
    std::string id;
    std::cin >> id;
    int i = Buscar(fs, id);
    if(i == -1) std::cout << "Error: no existe la formacion " << id << '\n';
    else{
        Eliminar(fs, p, static_cast<unsigned>(i));
        std::cout << "Formacion " << id << " eliminada; recursos devueltos al pool\n";
    }
}

void ComandoMostrar(const Formaciones& fs){   // MOSTRAR <id>
    std::string id;
    std::cin >> id;
    int i = Buscar(fs, id);
    if(i == -1) std::cout << "Error: no existe la formacion " << id << '\n';
    else Mostrar(fs.items.at(i));
}

void LeerComandos(PoolDeRecursos& p, Formaciones& fs){
    for(std::string comando; std::cin >> comando;){
        if(comando.at(0) == '#') Descartar();
        else if(comando == "CARGA") ComandoCarga(p, fs);
        else if(comando == "PASAJEROS") ComandoPasajeros(p, fs);
        else if(comando == "FORMACION") ComandoFormacion(p, fs);
        else if(comando == "VAGON") ComandoVagon(p, fs);
        else if(comando == "ELIMINAR") ComandoEliminar(p, fs);
        else if(comando == "MOSTRAR") ComandoMostrar(fs);
        else if(comando == "FORMACIONES"){
            if(fs.cantidad == 0) std::cout << "No hay formaciones creadas\n";
            for(unsigned i{}; i < fs.cantidad; ++i)
                Mostrar(fs.items.at(i));
        }
        else if(comando == "POOL") MostrarPool(p);
        else std::cout << "Comando desconocido: " << comando << '\n';
    }
}

// ---------- Pruebas ----------
// Usan solo la interfaz (Agregar, Sacar, Crear, AddVagón...), así que son las mismas
// en trenescont y en treneslink. Con NDEBUG definido no hacen nada.
void Pruebas(){
    using T = Recurso::Tipo;
    PoolDeRecursos p;
    assert(Contar(p, T::Carga) == 0);
    Agregar(p, CrearRecurso(T::Carga, "VC1"));
    Agregar(p, CrearRecurso(T::Carga, "VC2"));
    assert(Contar(p, T::Carga) == 2);

    Recurso r;
    assert(Sacar(p, T::Carga, r) && r.id == "VC1");   // saca el primero que encuentra
    assert(Contar(p, T::Carga) == 1);
    assert(!Sacar(p, T::Cocina, r));                  // no hay: devuelve false y el pool no cambia
    assert(Contar(p, T::Carga) == 1);

    Formación f = Crear(CrearRecurso(T::LocomotoraCarga, "LC1"), "F1");
    assert(GetCantidadLocomotoras(f) == 1 && GetCantidadVagones(f) == 0);
    assert(AddVagón(f, CrearRecurso(T::Carga, "VC3")));
    assert(!AddVagón(f, CrearRecurso(T::Comedor, "VM1")));   // incompatible: no cambia nada
    assert(GetCantidadVagones(f) == 1 && GetCapacidad(f) == Recurso::CapacidadCarga);
    assert(Enganchar(f, p, T::Carga) == Resultado::Ok);       // toma VC2 del pool
    assert(Enganchar(f, p, T::Carga) == Resultado::SinRecurso);
    assert(GetCantidadVagones(f) == 2);
    Liberar(f);   // las pruebas también devuelven lo que pidieron con new
    Liberar(p);
}
