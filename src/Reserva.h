#ifndef RESERVA_H
#define RESERVA_H

#include <cstring>
#include <string>

struct Reserva {
    int id;
    int idUsuario;
    int idEquipo;
    char fecha[12];    // Formato "YYYY-MM-DD"
    char franja[16];   // Bloques de 2h: ej. "09:00-11:00"

    Reserva() : id(0), idUsuario(0), idEquipo(0) {
        fecha[0] = '\0';
        franja[0] = '\0';
    }

    Reserva(int _id, int _idUsuario, int _idEquipo, const std::string& _fecha, const std::string& _franja)
        : id(_id), idUsuario(_idUsuario), idEquipo(_idEquipo) {
        std::strncpy(fecha, _fecha.c_str(), sizeof(fecha) - 1);
        fecha[sizeof(fecha) - 1] = '\0';

        std::strncpy(franja, _franja.c_str(), sizeof(franja) - 1);
        franja[sizeof(franja) - 1] = '\0';
    }
};

#endif