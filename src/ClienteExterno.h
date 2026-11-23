#ifndef CLIENTE_EXTERNO_H
#define CLIENTE_EXTERNO_H

#include <cstring>
#include <string>

struct ClienteExterno {
    int id;
    char nombreSolicitante[100];
    char email[100];
    char telefono[20];
    char password[64];
    char nombreEmpresa[100];
    char cif[20];
    char datosContables[100]; // IBAN / Cuenta de facturacion / Unidad contable externa
    char estado[30];          // "Pendiente de aprobacion", "Activa", "Rechazada"

    ClienteExterno() : id(0) {
        nombreSolicitante[0] = '\0';
        email[0] = '\0';
        telefono[0] = '\0';
        password[0] = '\0';
        nombreEmpresa[0] = '\0';
        cif[0] = '\0';
        datosContables[0] = '\0';
        std::strncpy(estado, "Pendiente de aprobacion", sizeof(estado) - 1);
        estado[sizeof(estado) - 1] = '\0';
    }

    ClienteExterno(int _id,
                   const std::string& _nombreSolicitante,
                   const std::string& _email,
                   const std::string& _telefono,
                   const std::string& _password,
                   const std::string& _nombreEmpresa,
                   const std::string& _cif,
                   const std::string& _datosContables,
                   const std::string& _estado = "Pendiente de aprobacion")
        : id(_id) {
        std::strncpy(nombreSolicitante, _nombreSolicitante.c_str(), sizeof(nombreSolicitante) - 1);
        nombreSolicitante[sizeof(nombreSolicitante) - 1] = '\0';

        std::strncpy(email, _email.c_str(), sizeof(email) - 1);
        email[sizeof(email) - 1] = '\0';

        std::strncpy(telefono, _telefono.c_str(), sizeof(telefono) - 1);
        telefono[sizeof(telefono) - 1] = '\0';

        std::strncpy(password, _password.c_str(), sizeof(password) - 1);
        password[sizeof(password) - 1] = '\0';

        std::strncpy(nombreEmpresa, _nombreEmpresa.c_str(), sizeof(nombreEmpresa) - 1);
        nombreEmpresa[sizeof(nombreEmpresa) - 1] = '\0';

        std::strncpy(cif, _cif.c_str(), sizeof(cif) - 1);
        cif[sizeof(cif) - 1] = '\0';

        std::strncpy(datosContables, _datosContables.c_str(), sizeof(datosContables) - 1);
        datosContables[sizeof(datosContables) - 1] = '\0';

        std::strncpy(estado, _estado.c_str(), sizeof(estado) - 1);
        estado[sizeof(estado) - 1] = '\0';
    }
};

#endif
