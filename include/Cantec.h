#ifndef CANTEC_H
#define CANTEC_H

#include <iostream>
#include <string>
#include <vector>
#include "Instrument.h"
#include "Nota.h"

class Cantec {
private:
    std::string titlu_;
    std::string dificultate_;
    std::string partituraText_;
    int pauzaStandardMs_; // Durata unui '-' (400ms=Greu, 600ms=Mediu, 1000ms=Usor)
    std::vector<Nota> note_;

    std::string descrieLungime() const;

public:
    // Durata unui '-' la viteza reala 1:1 (folosita in Preview si la raportul de viteza)
    static constexpr int kPauzaRealaMs = 400;

    explicit Cantec(const std::string& titlu = "Melodie Noua",
                    const std::string& dificultate = "Mediu",
                    int pauzaStandardMs = 1000);

    void adaugaNota(const Nota& n);

    // Transforma o partitura text (ex: "q--w-qq-e") in obiecte Nota cu timpii calculati din '-'
    void construiesteDinPartitura(const std::string& partituraText,
                                  const Instrument& inst,
                                  bool afiseazaCreare = false);

    int calculeazaDurataTotalaMs() const;
    void schimbaRitm(double factorTimp);

    // 1 = Greu (1:1), 2 = Mediu (1:1.5), 3 = Usor (1:2.5)
    void seteazaNivelViteza(int nivelViteza);
    std::string getRaportViteza() const;

    void afiseazaSecventaTaste() const;

    const std::string& getTitlu() const;
    const std::string& getDificultate() const;
    const std::string& getPartituraText() const;
    int getPauzaStandardMs() const;
    const std::vector<Nota>& getNote() const;

    friend std::ostream& operator<<(std::ostream& os, const Cantec& c);
};

#endif // CANTEC_H
