#include <iostream>
#include <string>
#include "GestorCICT.h"

void menuInvestigadorUCO(GestorCICT& gestor, int idUsuarioUCO) {
    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n--- PANEL INVESTIGADOR UCO (HU-01) ---\n";
        std::cout << "1. Ver catalogo de equipos disponibles\n";
        std::cout << "2. Reservar franja horaria de equipo\n";
        std::cout << "0. Salir\n";
        std::cout << "Opcion: ";
        std::cin >> opcion;

        if (opcion == 1) {
            auto equipos = gestor.obtenerEquiposDisponibles();
            std::cout << "\nEQUIPOS DISPONIBLES:\n";
            for (const auto& eq : equipos) {
                std::cout << "ID: " << eq.id
                          << " | " << eq.nombre
                          << " (" << eq.campus << ")\n";
            }
        } else if (opcion == 2) {
            int idEquipo;
            std::string fecha, franja;

            std::cout << "ID del equipo a reservar: ";
            std::cin >> idEquipo;
            std::cout << "Fecha (YYYY-MM-DD): ";
            std::cin >> fecha;
            std::cout << "Franja horaria de 2h (ej. 09:00-11:00): ";
            std::cin >> franja;

            if (gestor.crearReservaInterna(idUsuarioUCO, idEquipo, fecha, franja)) {
                std::cout << "[OK] Reserva confirmada y guardada en binario. Franja ocupada.\n";
            } else {
                std::cout << "[ERROR] El horario no esta disponible o el equipo esta ocupado.\n";
            }
        }
    }
}

int main() {
    GestorCICT gestor("../data/equipos.dat", "../data/reservas.dat");

    // Simulamos sesión iniciada de un investigador de la UCO (ID = 1)
    int idInvestigadorUCO = 1;
    menuInvestigadorUCO(gestor, idInvestigadorUCO);

    return 0;
}