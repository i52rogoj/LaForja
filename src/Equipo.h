#ifndef EQUIPO_H
#define EQUIPO_H

#include <cstring>
#include <string>

struct Equipo {
    int id;
    char nombre[100];
    char campus[50];   // "Rabanales", "Menendez Pidal"
    char estado[30];   // "Disponible", "En mantenimiento", "Averiado"

    Equipo() : id(0) {
        nombre[0] = '\0';
        campus[0] = '\0';
        estado[0] = '\0';
    }

    Equipo(int _id, const std::string& _nombre, const std::string& _campus, const std::string& _estado) : id(_id) {
        std::strncpy(nombre, _nombre.c_str(), sizeof(nombre) - 1);
        nombre[sizeof(nombre) - 1] = '\0';

        std::strncpy(campus, _campus.c_str(), sizeof(campus) - 1);
        campus[sizeof(campus) - 1] = '\0';

        std::strncpy(estado, _estado.c_str(), sizeof(estado) - 1);
        estado[sizeof(estado) - 1] = '\0';
    }
};

#endif