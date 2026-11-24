#ifndef VIRTUOSO_H
#define VIRTUOSO_H

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include "Cantec.h"
#include "Instrument.h"

class Virtuoso {
private:
    std::vector<Instrument> instrumente_;
    std::vector<Cantec> cantece_;
    std::size_t instrumentSelectat_;
    int scor_;
    int combo_;
    int nrGreseli_;

    int calculeazaPuncteNota(int diferentaTimpMs) const;
    void proceseazaApasareNota(const Nota& notaCeruta, char tastaJucator, int diferentaTimpMs, int distantaAfisataMs);

public:
    explicit Virtuoso(std::size_t instrumentInitial = 0);

    bool incarcaCanteceDinFisier(const std::string& caleFisier);
    void alegeInstrument(std::size_t index);
    void afiseazaListaCantece() const;
    void redaPreviewCantec(std::size_t indexCantec) const;

    // 1 = Greu (1:1), 2 = Mediu (1:1.5), 3 = Usor (1:2.5)
    void ajusteazaRitmCantec(std::size_t indexCantec, int nivelViteza);

    int joacaCantecInteractiv(std::size_t indexCantec, std::istream& in = std::cin);

    // Varianta fara std::cin, folosita de testele automate (CTest)
    int joacaCantec(std::size_t indexCantec, const std::vector<char>& tasteApasate, const std::vector<int>& timpiMs);

    void creeazaCantec(const std::string& titluNou, const std::string& secventaTaste, int ritmMs = 1000);

    std::size_t getNumarCantece() const;
    int getScor() const;
    int getCombo() const;
    int getNrGreseli() const;

    friend std::ostream& operator<<(std::ostream& os, const Virtuoso& v);
};

#endif // VIRTUOSO_H
