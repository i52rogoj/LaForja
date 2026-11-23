#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include "GestorCICT.h"

void menuInvestigadorUCO(GestorCICT& gestor, int idUsuarioUCO) {
    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n--- PANEL INVESTIGADOR UCO (HU-01) ---\n";
        std::cout << "1. Ver catalogo de equipos disponibles\n";
        std::cout << "2. Reservar franja horaria de equipo\n";
        std::cout << "0. Volver al menu principal\n";
        std::cout << "Opcion: ";
        if (!(std::cin >> opcion)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            opcion = -1;
            continue;
        }

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

void formularioRegistroClienteExterno(GestorCICT& gestor) {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string nombreSolicitante, email, telefono, password;
    std::string nombreEmpresa, cif, datosContables;

    std::cout << "\n--- SOLICITUD DE REGISTRO DE CLIENTE EXTERNO (HU-02) ---\n";
    std::cout << "Complete todos los campos obligatorios (personales y fiscales/contables):\n";

    std::cout << "Nombre completo del solicitante (*): ";
    std::getline(std::cin, nombreSolicitante);

    std::cout << "Correo electronico corporativo (*): ";
    std::getline(std::cin, email);

    std::cout << "Telefono de contacto (*): ";
    std::getline(std::cin, telefono);

    std::cout << "Contrasena deseada (*): ";
    std::getline(std::cin, password);

    std::cout << "Nombre o razon social de la empresa (*): ";
    std::getline(std::cin, nombreEmpresa);

    std::cout << "CIF de la empresa (*): ";
    std::getline(std::cin, cif);

    std::cout << "Datos fiscales/contables - IBAN o cuenta de facturacion (*): ";
    std::getline(std::cin, datosContables);

    ClienteExterno nuevaSolicitud(0, nombreSolicitante, email, telefono, password,
                                  nombreEmpresa, cif, datosContables);

    std::vector<std::string> camposFaltantes;
    ResultadoRegistroExterno resultado =
        gestor.solicitarRegistroClienteExterno(nuevaSolicitud, camposFaltantes);

    switch (resultado) {
        case ResultadoRegistroExterno::EXITO:
            std::cout << "\n[OK] Solicitud de registro enviada correctamente.\n";
            std::cout << "     ID de solicitud: " << nuevaSolicitud.id << "\n";
            std::cout << "     Estado de la cuenta: " << nuevaSolicitud.estado << "\n";
            std::cout << "     Su solicitud esta siendo revisada por el Administrador del CICT.\n";
            break;

        case ResultadoRegistroExterno::CAMPOS_INCOMPLETOS:
            std::cout << "\n[ERROR] Registro denegado: faltan campos obligatorios para tramitar el alta:\n";
            for (const auto& campo : camposFaltantes) {
                std::cout << "  - [OBLIGATORIO] " << campo << "\n";
            }
            break;

        case ResultadoRegistroExterno::CORREO_DUPLICADO:
            std::cout << "\n[AVISO] Ya existe una solicitud o cuenta asociada al correo '"
                      << email << "'. No se permite crear una cuenta duplicada.\n";
            break;

        case ResultadoRegistroExterno::CIF_DUPLICADO:
            std::cout << "\n[AVISO] Ya existe una solicitud o cuenta asociada al CIF '"
                      << cif << "'. No se permite crear una cuenta duplicada.\n";
            break;

        case ResultadoRegistroExterno::ERROR_SISTEMA:
            std::cout << "\n[ERROR] No se pudo guardar la solicitud en el sistema.\n";
            break;
    }
}

int main() {
    GestorCICT gestor("../data/equipos.dat",
                     "../data/reservas.dat",
                     "../data/clientes_externos.dat");

    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n==============================================\n";
        std::cout << "   SG-CICT - Universidad de Cordoba (UCO)     \n";
        std::cout << "==============================================\n";
        std::cout << "1. Acceso Investigador UCO (HU-01)\n";
        std::cout << "2. Solicitar registro como Cliente Externo (HU-02)\n";
        std::cout << "0. Salir\n";
        std::cout << "Seleccione una opcion: ";

        if (!(std::cin >> opcion)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            opcion = -1;
            continue;
        }

        if (opcion == 1) {
            // Simulamos sesión iniciada de un investigador de la UCO (ID = 1)
            int idInvestigadorUCO = 1;
            menuInvestigadorUCO(gestor, idInvestigadorUCO);
        } else if (opcion == 2) {
            formularioRegistroClienteExterno(gestor);
        }
    }

    return 0;
}
