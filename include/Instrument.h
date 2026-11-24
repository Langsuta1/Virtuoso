#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <iostream>
#include <string>
#include <vector>
#include "Nota.h"

// Clasa Instrument reprezinta Pianul sau Chitara.
// OOP Compunere: contine un vector de obiecte Nota (tasteMapate_) care formeaza claviatura QWERTY.
// Rolul ei este sa transforme Input-ul (tasta apasata) in Output (sunet .wav + desen pe ecran).
class Instrument {
private:
    std::string nume_;               // "Pian" sau "Chitara"
    std::string folderSunete_;       // Calea catre fisierele .wav ale instrumentului
    std::vector<Nota> tasteMapate_;  // Cele 6 taste QWERTY (q=Do, w=Re, e=Mi, r=Fa, t=Sol, y=La)

    // Functie privata care deseneaza pe ecran clapele de Pian sau coarda de Chitara.
    void afiseazaGraficaInstrument(const Nota& n) const;

public:
    // Constructor care seteaza numele, folderul cu sunete si incarca automat tastele QWERTY.
    Instrument(const std::string& nume, const std::string& folderSunete);

    // Cauta daca o litera de pe tastatura exista pe instrument.
    // Daca o gaseste, o pune in parametrul 'rezultat' (transmis prin referinta '&') si returneaza true.
    bool cautaNotaDupaTasta(char tasta, Nota& rezultat) const;

    // Reda fisierul .wav din boxe si afiseaza animatia instrumentului pe ecran.
    void redaOutput(const Nota& n, bool gresit = false) const;

    // Afiseaza ce tasta corespunde fiecarei note pe acest instrument.
    void afiseazaControale() const;

    const std::string& getNume() const;

    // Operator<< pentru afisarea instrumentului si a tastelor sale.
    friend std::ostream& operator<<(std::ostream& os, const Instrument& inst);
};

#endif // INSTRUMENT_H
