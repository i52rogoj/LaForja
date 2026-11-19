#include "GestorCICT.h"
#include <fstream>
#include <cstring>

GestorCICT::GestorCICT(const std::string& rutaEquipos, const std::string& rutaReservas)
    : rutaEquipos_(rutaEquipos), rutaReservas_(rutaReservas) {
    inicializarDatosSiNoExisten();
}

// Si es la primera vez que se ejecuta, crea equipos.dat con datos de prueba
void GestorCICT::inicializarDatosSiNoExisten() {
    std::ifstream check(rutaEquipos_, std::ios::binary);
    if (check.good()) {
        check.close();
        return;
    }

    std::ofstream out(rutaEquipos_, std::ios::binary);
    if (!out) return;

    Equipo e1(1, "Microscopio Electronico", "Rabanales", "Disponible");
    Equipo e2(2, "Espectrometro de Masas", "Rabanales", "Disponible");
    Equipo e3(3, "Cromatografo Liquido", "Menendez Pidal", "Averiado");

    out.write(reinterpret_cast<const char*>(&e1), sizeof(Equipo));
    out.write(reinterpret_cast<const char*>(&e2), sizeof(Equipo));
    out.write(reinterpret_cast<const char*>(&e3), sizeof(Equipo));
    out.close();
}

int GestorCICT::obtenerSiguienteIdReserva() {
    std::ifstream in(rutaReservas_, std::ios::binary | std::ios::ate);
    if (!in) return 1;
    std::streamsize tamano = in.tellg();
    return static_cast<int>(tamano / sizeof(Reserva)) + 1;
}

// Lee equipos.dat y devuelve los que están en estado "Disponible"
std::vector<Equipo> GestorCICT::obtenerEquiposDisponibles() {
    std::vector<Equipo> disponibles;
    std::ifstream in(rutaEquipos_, std::ios::binary);
    if (!in) return disponibles;

    Equipo eq;
    while (in.read(reinterpret_cast<char*>(&eq), sizeof(Equipo))) {
        if (std::strcmp(eq.estado, "Disponible") == 0) {
            disponibles.push_back(eq);
        }
    }
    in.close();
    return disponibles;
}

// Recorre reservas.dat buscando si ya existe una reserva para ese equipo, fecha y franja
bool GestorCICT::verificarDisponibilidadFranja(int idEquipo, const std::string& fecha, const std::string& franja) {
    std::ifstream in(rutaReservas_, std::ios::binary);
    if (!in) return true; // Si aún no existe reservas.dat, está libre

    Reserva res;
    while (in.read(reinterpret_cast<char*>(&res), sizeof(Reserva))) {
        if (res.idEquipo == idEquipo &&
            std::strcmp(res.fecha, fecha.c_str()) == 0 &&
            std::strcmp(res.franja, franja.c_str()) == 0) {
            in.close();
            return false; // Franja ocupada
        }
    }
    in.close();
    return true;
}

// Implementación principal de HU-01 (Escenarios 1 y 2)
bool GestorCICT::crearReservaInterna(int idUsuario, int idEquipo, const std::string& fecha, const std::string& franja) {
    // 1. Validar que el equipo existe y está "Disponible"
    std::ifstream inEq(rutaEquipos_, std::ios::binary);
    if (!inEq) return false;

    bool equipoValido = false;
    Equipo eq;
    while (inEq.read(reinterpret_cast<char*>(&eq), sizeof(Equipo))) {
        if (eq.id == idEquipo && std::strcmp(eq.estado, "Disponible") == 0) {
            equipoValido = true;
            break;
        }
    }
    inEq.close();

    if (!equipoValido) {
        return false;
    }

    // 2. Escenario 2 (Unhappy flow): Bloquear si la franja ya está ocupada
    if (!verificarDisponibilidadFranja(idEquipo, fecha, franja)) {
        return false;
    }

    // 3. Escenario 1 (Happy flow): Añadir la reserva al final de reservas.dat (ios::app)
    int nuevoId = obtenerSiguienteIdReserva();
    Reserva nueva(nuevoId, idUsuario, idEquipo, fecha, franja);

    std::ofstream outRes(rutaReservas_, std::ios::binary | std::ios::app);
    if (!outRes) return false;

    outRes.write(reinterpret_cast<const char*>(&nueva), sizeof(Reserva));
    outRes.close();

    return true;
}