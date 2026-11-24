#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <iostream>
#include <string>
#include <vector>
#include "Nota.h"

class Instrument {
private:
    std::string nume_;
    std::string folderSunete_;
    std::vector<Nota> tasteMapate_;

    void afiseazaGraficaInstrument(const Nota& n) const;

public:
    Instrument(const std::string& nume, const std::string& folderSunete);

    // Parametrul 'rezultat' e transmis prin referinta (&) ca sa putem intoarce nota gasita
    bool cautaNotaDupaTasta(char tasta, Nota& rezultat) const;

    void redaOutput(const Nota& n, bool gresit = false) const;
    void afiseazaControale() const;

    const std::string& getNume() const;

    friend std::ostream& operator<<(std::ostream& os, const Instrument& inst);
};

#endif // INSTRUMENT_H
