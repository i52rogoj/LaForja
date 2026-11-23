#ifndef GESTOR_CICT_H
#define GESTOR_CICT_H

#include <string>
#include <vector>
#include "Equipo.h"
#include "Reserva.h"
#include "ClienteExterno.h"

enum class ResultadoRegistroExterno {
    EXITO,
    CAMPOS_INCOMPLETOS,
    CORREO_DUPLICADO,
    CIF_DUPLICADO,
    ERROR_SISTEMA
};

class GestorCICT {
private:
    std::string rutaEquipos_;
    std::string rutaReservas_;
    std::string rutaClientesExternos_;

    void inicializarDatosSiNoExisten();
    int obtenerSiguienteIdReserva();
    int obtenerSiguienteIdClienteExterno();
    void notificarAdministradorNuevaSolicitud(const ClienteExterno& cliente) const;

public:
    GestorCICT(const std::string& rutaEquipos = "../data/equipos.dat",
               const std::string& rutaReservas = "../data/reservas.dat",
               const std::string& rutaClientesExternos = "../data/clientes_externos.dat");

    // Métodos HU-01: Reserva de equipamiento por investigador interno
    std::vector<Equipo> obtenerEquiposDisponibles();
    bool verificarDisponibilidadFranja(int idEquipo, const std::string& fecha, const std::string& franja);
    bool crearReservaInterna(int idUsuario, int idEquipo, const std::string& fecha, const std::string& franja);

    // Métodos HU-02: Solicitud de registro de cliente externo
    std::vector<std::string> validarCamposObligatoriosExterno(const ClienteExterno& cliente) const;
    bool existeCorreoExterno(const std::string& email) const;
    bool existeCifExterno(const std::string& cif) const;
    ResultadoRegistroExterno solicitarRegistroClienteExterno(ClienteExterno& cliente,
                                                             std::vector<std::string>& camposFaltantes);
    std::vector<ClienteExterno> obtenerClientesExternos() const;
};

#endif
