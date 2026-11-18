#include <array>
#include <cassert>
#include <iostream>
#include <string>

struct Recurso{
    static const unsigned CapacidadCarga = 50;      // toneladas por vagón de carga
    static const unsigned CapacidadPasaje = 150;    // pasajeros por vagón de pasajeros
    enum struct Tipo{LocomotoraCarga, LocomotoraPasajeros, Pasajero, Comedor, Cocina, Carga};
    Tipo tipo{};
    std::string id;
    unsigned capacidad{};
};

struct Formación{
    static const unsigned VagonesPorLocomotora = 10;      // carga: 1 locomotora cada 10 vagones
    static const unsigned LocomotorasMáximasCarga = 2;
    static const unsigned VagonesMáximosPasajeros = 10;
    static const unsigned LongitudMáxima = 22;            // 2 locomotoras + 20 vagones
    std::string id;
    std::array<Recurso, LongitudMáxima> links;           // ocupadas: las primeras 'cantidad'
    unsigned cantidad{};
};

struct PoolDeRecursos{
    static const unsigned Máximo = 200;
    std::array<Recurso, Máximo> recursos;
    unsigned cantidad{};
};

struct Formaciones{
    static const unsigned Máximo = 14;
    std::array<Formación, Máximo> items;
    unsigned cantidad{};
};

void Pruebas();
void CargarPool(PoolDeRecursos& p);
void LeerComandos(PoolDeRecursos& p, Formaciones& fs);

int main(){
    Pruebas();
    PoolDeRecursos pool;
    Formaciones formaciones;
    CargarPool(pool);
    LeerComandos(pool, formaciones);
}

// ---------- Recurso ----------
Recurso CrearRecurso(Recurso::Tipo tipo, const std::string& id){
    Recurso r{tipo, id, 0};   // locomotoras, comedor y cocina no transportan carga ni pasajeros
    if(tipo == Recurso::Tipo::Carga) r.capacidad = Recurso::CapacidadCarga;
    if(tipo == Recurso::Tipo::Pasajero) r.capacidad = Recurso::CapacidadPasaje;
    return r;
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

// ---------- PoolDeRecursos ----------
void Agregar(PoolDeRecursos& p, const Recurso& r){
    p.recursos.at(p.cantidad++) = r;   // postincremento: escribe y recién después sube el contador
}

unsigned Contar(const PoolDeRecursos& p, Recurso::Tipo t){
    unsigned n{};
    for(unsigned i{}; i < p.cantidad; ++i)
        if(p.recursos.at(i).tipo == t) ++n;
    return n;
}

// Saca el primer recurso del tipo pedido; lo devuelve por 'salida'
bool Sacar(PoolDeRecursos& p, Recurso::Tipo t, Recurso& salida){
    for(unsigned i{}; i < p.cantidad; ++i){
        if(p.recursos.at(i).tipo == t){
            salida = p.recursos.at(i);
            for(unsigned j = i; j + 1 < p.cantidad; ++j)   // tapa el hueco corriendo los siguientes
                p.recursos.at(j) = p.recursos.at(j + 1);
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

void CargarPool(PoolDeRecursos& p){
    using T = Recurso::Tipo;
    CargarTipo(p, T::LocomotoraCarga, "LC", 4);
    CargarTipo(p, T::LocomotoraPasajeros, "LP", 5);
    CargarTipo(p, T::Carga, "VC", 80);
    CargarTipo(p, T::Pasajero, "VP", 60);
    CargarTipo(p, T::Comedor, "VM", 10);
    CargarTipo(p, T::Cocina, "VK", 10);
}

void MostrarPool(const PoolDeRecursos& p){
    using T = Recurso::Tipo;
    std::cout << "Pool: " << Contar(p, T::LocomotoraCarga) << " LC, "
              << Contar(p, T::LocomotoraPasajeros) << " LP, "
              << Contar(p, T::Carga) << " C, " << Contar(p, T::Pasajero) << " P, "
              << Contar(p, T::Comedor) << " M, " << Contar(p, T::Cocina) << " K\n";
}

// ---------- Formación ----------
Formación Crear(const Recurso& locomotora, const std::string& id){
    Formación f;
    f.id = id;
    f.links.at(0) = locomotora;   // la locomotora ocupa la posición 0
    f.cantidad = 1;
    return f;
}

bool EsDeCarga(const Formación& f){
    return f.links.at(0).tipo == Recurso::Tipo::LocomotoraCarga;
}

unsigned GetCantidadLocomotoras(const Formación& f){
    unsigned n{};
    for(unsigned i{}; i < f.cantidad; ++i)
        if(f.links.at(i).tipo == Recurso::Tipo::LocomotoraCarga ||
           f.links.at(i).tipo == Recurso::Tipo::LocomotoraPasajeros) ++n;
    return n;
}

unsigned GetCantidadVagones(const Formación& f){
    return f.cantidad - GetCantidadLocomotoras(f);
}

unsigned GetCapacidad(const Formación& f){
    unsigned total{};
    for(unsigned i{}; i < f.cantidad; ++i)
        total += f.links.at(i).capacidad;
    return total;
}

// Solo las de carga llevan una segunda locomotora
bool AddLocomotora(Formación& f, const Recurso& r){
    if(!EsDeCarga(f) || r.tipo != Recurso::Tipo::LocomotoraCarga ||
       GetCantidadLocomotoras(f) == Formación::LocomotorasMáximasCarga) return false;
    f.links.at(f.cantidad++) = r;
    return true;
}

// Carga acepta C (10 por locomotora); pasajeros acepta P, M y K (hasta 10)
bool AddVagón(Formación& f, const Recurso& r){
    using T = Recurso::Tipo;
    bool compatible = EsDeCarga(f) ? r.tipo == T::Carga
                                   : r.tipo == T::Pasajero || r.tipo == T::Comedor || r.tipo == T::Cocina;
    unsigned máximo = EsDeCarga(f) ? Formación::VagonesPorLocomotora * GetCantidadLocomotoras(f)
                                   : Formación::VagonesMáximosPasajeros;
    if(!compatible || GetCantidadVagones(f) == máximo) return false;
    f.links.at(f.cantidad++) = r;
    return true;
}

// Saca un vagón del pool y lo agrega; si no entra, vuelve al pool
bool Enganchar(Formación& f, PoolDeRecursos& p, Recurso::Tipo t){
    Recurso r;
    if(!Sacar(p, t, r)) return false;
    if(AddVagón(f, r)) return true;
    Agregar(p, r);
    return false;
}

// Arma la formación para 'cantidad' toneladas (carga) o pasajeros.
// Primero verifica que alcance el pool; recién después saca los recursos.
bool Armar(PoolDeRecursos& p, const std::string& id, bool carga, unsigned cantidad, Formación& f){
    using T = Recurso::Tipo;
    T locomotora = carga ? T::LocomotoraCarga : T::LocomotoraPasajeros;
    T vagón = carga ? T::Carga : T::Pasajero;
    unsigned capacidad = carga ? Recurso::CapacidadCarga : Recurso::CapacidadPasaje;
    unsigned vagones = (cantidad + capacidad - 1) / capacidad;   // redondeo hacia arriba
    unsigned locomotoras = vagones > Formación::VagonesPorLocomotora ? 2 : 1;
    unsigned máximo = carga ? Formación::VagonesPorLocomotora * Formación::LocomotorasMáximasCarga
                            : Formación::VagonesMáximosPasajeros;

    if(vagones > máximo || Contar(p, locomotora) < locomotoras || Contar(p, vagón) < vagones)
        return false;

    Recurso r;
    Sacar(p, locomotora, r);
    f = Crear(r, id);
    if(locomotoras == 2){
        Sacar(p, locomotora, r);
        AddLocomotora(f, r);
    }
    for(unsigned i{}; i < vagones; ++i)
        Enganchar(f, p, vagón);
    return true;
}

void Mostrar(const Formación& f){
    std::cout << "Formacion " << f.id << (EsDeCarga(f) ? " [carga]" : " [pasajeros]")
              << " | Locomotoras: " << GetCantidadLocomotoras(f)
              << " | Vagones: " << GetCantidadVagones(f)
              << " | Capacidad: " << GetCapacidad(f) << '\n'
              << "  Composicion:";
    for(unsigned i{}; i < f.cantidad; ++i)
        std::cout << ' ' << f.links.at(i).id;
    std::cout << '\n';
}

// ---------- Formaciones ----------
// Devuelve el índice de la formación o -1 si no existe
int Buscar(const Formaciones& fs, const std::string& id){
    for(unsigned i{}; i < fs.cantidad; ++i)
        if(fs.items.at(i).id == id) return static_cast<int>(i);
    return -1;
}

// ---------- Comandos ----------
void LeerComandos(PoolDeRecursos& p, Formaciones& fs){
    for(std::string comando, id; std::cin >> comando;){
        if(comando == "CARGA" || comando == "PASAJEROS"){
            unsigned cantidad{};
            std::cin >> id >> cantidad;
            Formación f;
            if(Buscar(fs, id) == -1 && fs.cantidad < Formaciones::Máximo &&
               Armar(p, id, comando == "CARGA", cantidad, f)){
                fs.items.at(fs.cantidad++) = f;
                Mostrar(f);
            }
            else std::cout << "No se pudo armar la formacion " << id << '\n';
        }
        else if(comando == "VAGON"){
            std::string tipo;
            std::cin >> id >> tipo;
            int i = Buscar(fs, id);
            Recurso::Tipo t{};
            bool ok = i != -1 && LeerTipo(tipo, t) && Enganchar(fs.items.at(i), p, t);
            std::cout << "Vagon " << tipo << " -> " << id << (ok ? ": OK" : ": no se pudo enganchar") << '\n';
        }
        else if(comando == "MOSTRAR"){
            std::cin >> id;
            int i = Buscar(fs, id);
            if(i == -1) std::cout << "No existe la formacion " << id << '\n';
            else Mostrar(fs.items.at(i));
        }
        else if(comando == "POOL") MostrarPool(p);
        else std::cout << "Comando desconocido: " << comando << '\n';
    }
}

// ---------- Pruebas ----------
void Pruebas(){
    PoolDeRecursos p;
    CargarPool(p);
    Formación f;
    assert(Armar(p, "C1", true, 120, f));                              // 120 t: 3 vagones
    assert(GetCantidadVagones(f) == 3 && GetCapacidad(f) == 150);
    assert(!AddVagón(f, CrearRecurso(Recurso::Tipo::Comedor, "VM1")));  // incompatible con carga
    assert(!Armar(p, "C2", true, 1200, f));                            // 24 vagones: no entran
}
