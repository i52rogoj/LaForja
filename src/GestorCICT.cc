#include "GestorCICT.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace {
std::string trim(const std::string& texto) {
    size_t inicio = 0;
    while (inicio < texto.size() && std::isspace(static_cast<unsigned char>(texto[inicio]))) {
        ++inicio;
    }
    size_t fin = texto.size();
    while (fin > inicio && std::isspace(static_cast<unsigned char>(texto[fin - 1]))) {
        --fin;
    }
    return texto.substr(inicio, fin - inicio);
}

std::string aMinusculas(const std::string& texto) {
    std::string res = trim(texto);
    std::transform(res.begin(), res.end(), res.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return res;
}

void asegurarDirectorioPadre(const std::string& rutaArchivo) {
    std::filesystem::path p(rutaArchivo);
    if (p.has_parent_path() && !p.parent_path().empty()) {
        std::error_code ec;
        std::filesystem::create_directories(p.parent_path(), ec);
    }
}
} // namespace

GestorCICT::GestorCICT(const std::string& rutaEquipos,
                       const std::string& rutaReservas,
                       const std::string& rutaClientesExternos)
    : rutaEquipos_(rutaEquipos),
      rutaReservas_(rutaReservas),
      rutaClientesExternos_(rutaClientesExternos) {
    asegurarDirectorioPadre(rutaEquipos_);
    asegurarDirectorioPadre(rutaReservas_);
    asegurarDirectorioPadre(rutaClientesExternos_);
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

int GestorCICT::obtenerSiguienteIdClienteExterno() {
    std::ifstream in(rutaClientesExternos_, std::ios::binary | std::ios::ate);
    if (!in) return 1;
    std::streamsize tamano = in.tellg();
    return static_cast<int>(tamano / sizeof(ClienteExterno)) + 1;
}

// ============================================================================
// HU-01: Reserva de equipamiento por investigador interno
// ============================================================================

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

// ============================================================================
// HU-02: Solicitud de registro de cliente externo
// ============================================================================

// Escenario 2 (Unhappy flow): Comprueba que todos los datos personales y fiscales/contables estén rellenos
std::vector<std::string> GestorCICT::validarCamposObligatoriosExterno(const ClienteExterno& cliente) const {
    std::vector<std::string> faltantes;

    if (trim(cliente.nombreSolicitante).empty()) {
        faltantes.push_back("Nombre completo del solicitante");
    }
    if (trim(cliente.email).empty()) {
        faltantes.push_back("Correo electronico corporativo");
    }
    if (trim(cliente.telefono).empty()) {
        faltantes.push_back("Telefono de contacto");
    }
    if (trim(cliente.password).empty()) {
        faltantes.push_back("Contrasena de acceso");
    }
    if (trim(cliente.nombreEmpresa).empty()) {
        faltantes.push_back("Nombre o razon social de la empresa");
    }
    if (trim(cliente.cif).empty()) {
        faltantes.push_back("CIF de la entidad corporativa");
    }
    if (trim(cliente.datosContables).empty()) {
        faltantes.push_back("Informacion fiscal/contable (IBAN o cuenta de facturacion)");
    }

    return faltantes;
}

// Escenario 3 (Unhappy flow): Comprueba si ya existe una cuenta o solicitud con ese correo
bool GestorCICT::existeCorreoExterno(const std::string& email) const {
    std::ifstream in(rutaClientesExternos_, std::ios::binary);
    if (!in) return false;

    std::string emailBuscado = aMinusculas(email);
    ClienteExterno reg;
    while (in.read(reinterpret_cast<char*>(&reg), sizeof(ClienteExterno))) {
        if (aMinusculas(reg.email) == emailBuscado) {
            in.close();
            return true;
        }
    }
    in.close();
    return false;
}

// Escenario 3 (Unhappy flow): Comprueba si ya existe una cuenta o solicitud con ese CIF
bool GestorCICT::existeCifExterno(const std::string& cif) const {
    std::ifstream in(rutaClientesExternos_, std::ios::binary);
    if (!in) return false;

    std::string cifBuscado = aMinusculas(cif);
    ClienteExterno reg;
    while (in.read(reinterpret_cast<char*>(&reg), sizeof(ClienteExterno))) {
        if (aMinusculas(reg.cif) == cifBuscado) {
            in.close();
            return true;
        }
    }
    in.close();
    return false;
}

// Notificación automática simulada al Administrador del CICT
void GestorCICT::notificarAdministradorNuevaSolicitud(const ClienteExterno& cliente) const {
    std::cout << "[NOTIFICACION AUTOMATICA -> ADMINISTRADOR CICT] "
              << "Nueva solicitud de alta externa (ID #" << cliente.id << ") "
              << "de la empresa '" << cliente.nombreEmpresa << "' (CIF: " << cliente.cif
              << ", contacto: " << cliente.email << ") pendiente de validacion.\n";
}

// Implementación principal de HU-02 (Escenarios 1, 2 y 3)
ResultadoRegistroExterno GestorCICT::solicitarRegistroClienteExterno(
    ClienteExterno& cliente,
    std::vector<std::string>& camposFaltantes) {
    // 1. Escenario 2 (Unhappy flow): Denegar si falta algún dato obligatorio o contable
    camposFaltantes = validarCamposObligatoriosExterno(cliente);
    if (!camposFaltantes.empty()) {
        return ResultadoRegistroExterno::CAMPOS_INCOMPLETOS;
    }

    // 2. Escenario 3 (Unhappy flow): Bloquear creación duplicada si el correo o CIF ya existen
    if (existeCorreoExterno(cliente.email)) {
        return ResultadoRegistroExterno::CORREO_DUPLICADO;
    }
    if (existeCifExterno(cliente.cif)) {
        return ResultadoRegistroExterno::CIF_DUPLICADO;
    }

    // 3. Escenario 1 (Happy flow): Crear cuenta en estado "Pendiente de aprobacion" y guardar en binario
    cliente.id = obtenerSiguienteIdClienteExterno();
    std::strncpy(cliente.estado, "Pendiente de aprobacion", sizeof(cliente.estado) - 1);
    cliente.estado[sizeof(cliente.estado) - 1] = '\0';

    std::ofstream out(rutaClientesExternos_, std::ios::binary | std::ios::app);
    if (!out) {
        return ResultadoRegistroExterno::ERROR_SISTEMA;
    }

    out.write(reinterpret_cast<const char*>(&cliente), sizeof(ClienteExterno));
    out.close();

    // Enviar notificación automática al Administrador del CICT
    notificarAdministradorNuevaSolicitud(cliente);

    return ResultadoRegistroExterno::EXITO;
}

std::vector<ClienteExterno> GestorCICT::obtenerClientesExternos() const {
    std::vector<ClienteExterno> lista;
    std::ifstream in(rutaClientesExternos_, std::ios::binary);
    if (!in) return lista;

    ClienteExterno reg;
    while (in.read(reinterpret_cast<char*>(&reg), sizeof(ClienteExterno))) {
        lista.push_back(reg);
    }
    in.close();
    return lista;
}
