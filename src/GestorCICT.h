#ifndef GESTOR_CICT_H
#define GESTOR_CICT_H

#include <string>
#include <vector>
#include "Equipo.h"
#include "Reserva.h"

class GestorCICT {
private:
    std::string rutaEquipos_;
    std::string rutaReservas_;

    void inicializarDatosSiNoExisten();
    int obtenerSiguienteIdReserva();

public:
    GestorCICT(const std::string& rutaEquipos = "../data/equipos.dat",
               const std::string& rutaReservas = "../data/reservas.dat");

    // Métodos HU-01
    std::vector<Equipo> obtenerEquiposDisponibles();
    bool verificarDisponibilidadFranja(int idEquipo, const std::string& fecha, const std::string& franja);
    bool crearReservaInterna(int idUsuario, int idEquipo, const std::string& fecha, const std::string& franja);
};

#endif