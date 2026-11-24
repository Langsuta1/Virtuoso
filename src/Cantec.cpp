#include "Cantec.h"

#include <chrono>
#include <sstream>

namespace {
// Functie locala ascunsa (namespace anonim) care face o scurta pauza intre sunete
// cand iti compui propriul cantec, ca sa auzi notele pe rand in difuzoare.
void asteaptaRedareCreareMs(int durataMs) {
#ifdef GITHUB_ACTIONS
    (void) durataMs; // Pe GitHub Actions nu asteptam, ca testele automate sa fie rapide.
#else
    const auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now() - start)
               .count() < durataMs) {
    }
#endif
}
} // namespace

// Clasifica lungimea cantecului in functie de cate note are in vector.
std::string Cantec::descrieLungime() const {
    if (note_.size() <= 11) {
        return "Scurt";
    }
    if (note_.size() <= 15) {
        return "Mediu";
    }
    return "Lung";
}

// Constructorul clasei Cantec.
Cantec::Cantec(const std::string& titlu, const std::string& dificultate, int pauzaStandardMs)
    : titlu_(titlu), dificultate_(dificultate), partituraText_(""), pauzaStandardMs_((pauzaStandardMs >= 200) ? pauzaStandardMs : 1000) {}

// Adauga o nota in vectorul note_ si actualizeaza si textul partiturii.
void Cantec::adaugaNota(const Nota& n) {
    if (!note_.empty()) {
        const int nrPauze = n.getTimpAsteptareMs() / pauzaStandardMs_;
        for (int i = 0; i < nrPauze; ++i) {
            partituraText_ += '-';
        }
    }
    partituraText_ += n.getTasta();
    note_.push_back(n);
}

// Parser de partitura: ia fiecare caracter din sirul partituraText (ex: "q--w-qq-e").
// Daca vede '-', numara cate secunde de pauza se aduna.
// Daca vede o litera (q,w,e,r,t,y), cauta nota pe instrument si ii seteaza timpul calculat din '-'.
void Cantec::construiesteDinPartitura(const std::string& partituraText, const Instrument& inst, bool afiseazaCreare) {
    note_.clear();
    partituraText_.clear();

    int liniiPauzaCurenta = 0;
    for (const char c : partituraText) {
        if (c == '-') {
            ++liniiPauzaCurenta;
            partituraText_ += '-';
            continue;
        }

        Nota gasita;
        if (inst.cautaNotaDupaTasta(c, gasita)) {
            int timpAsteptareNotaMs = pauzaStandardMs_;
            if (!note_.empty()) {
                // Daca nu exista '-' intre doua note (ex: "qq"), pauza e jumatate (nota rapida).
                // Daca exista unul sau mai multe '-', timpul este nr_linii * durata_unui_minus.
                timpAsteptareNotaMs = (liniiPauzaCurenta == 0)
                                          ? (pauzaStandardMs_ / 2)
                                          : (liniiPauzaCurenta * pauzaStandardMs_);
            }

            Nota notaCuPauza(
                gasita.getTasta(), gasita.getEticheta(), gasita.getFisierSunet(), timpAsteptareNotaMs);
            note_.push_back(notaCuPauza);
            partituraText_ += c;

            if (afiseazaCreare) {
                if (note_.size() > 1) {
                    std::cout << "   [PAUZA]         -> " << liniiPauzaCurenta << "x '-' (" << (timpAsteptareNotaMs / 1000.0) << " secunde de liber)\n";
                }
                inst.redaOutput(notaCuPauza, false);
                asteaptaRedareCreareMs(450);
            }
            liniiPauzaCurenta = 0;
        } else if (afiseazaCreare) {
            std::cout << "   [!] Caracterul '" << c << "' nu este nici nota (q,w,e,r,t,y), nici pauza ('-') - ignorat.\n";
        }
    }
}

// Parcurge toate notele din cantec si aduna timpii lor in milisecunde.
int Cantec::calculeazaDurataTotalaMs() const {
    int total = 0;
    for (const auto& n : note_) {
        total += n.getTimpAsteptareMs();
    }
    return total;
}

// Modifica durata unui '-' si timpul fiecarei note din vector cu factorTimp.
void Cantec::schimbaRitm(double factorTimp) {
    if (factorTimp <= 0.0) {
        return;
    }
    pauzaStandardMs_ = static_cast<int>(pauzaStandardMs_ * factorTimp);
    if (pauzaStandardMs_ < 200) {
        pauzaStandardMs_ = 200;
    }
    for (auto& n : note_) {
        n.ajusteazaTimp(factorTimp);
    }
}

// Seteaza unul din cele 3 niveluri de viteza fata de melodia reala:
// 1 -> 400ms (1:1 Greu), 2 -> 600ms (1:1.5 Mediu), 3 -> 1000ms (1:2.5 Usor).
void Cantec::seteazaNivelViteza(int nivelViteza) {
    int tintaMs = 1000;
    if (nivelViteza == 1) {
        tintaMs = kPauzaRealaMs;
    } else if (nivelViteza == 2) {
        tintaMs = 600;
    }

    const double factor = static_cast<double>(tintaMs) / pauzaStandardMs_;
    schimbaRitm(factor);
    pauzaStandardMs_ = tintaMs;
}

// Calculeaza si returneaza raportul de viteza (ex: 1:1, 1:1.5, 1:2.5) fata de timpul real (400ms).
std::string Cantec::getRaportViteza() const {
    if (pauzaStandardMs_ == kPauzaRealaMs) {
        return "1:1 [Greu - Viteza Reala]";
    }
    if (pauzaStandardMs_ == 600) {
        return "1:1.5 [Mediu]";
    }
    if (pauzaStandardMs_ == 1000) {
        return "1:2.5 [Usor - Incepatori]";
    }
    std::ostringstream oss;
    oss << "1:" << (static_cast<double>(pauzaStandardMs_) / kPauzaRealaMs);
    return oss.str();
}

// Afiseaza pe ecran codul partiturii (cu '-') si lista de note.
void Cantec::afiseazaSecventaTaste() const {
    std::cout << "Partitura:  " << partituraText_ << "   (Viteza: " << getRaportViteza() << " | '-' = " << (pauzaStandardMs_ / 1000.0) << "s)\n";
    std::cout << "Note:       ";
    for (const auto& n : note_) {
        std::cout << n << "  ";
    }
    std::cout << "\n";
}

const std::string& Cantec::getTitlu() const {
    return titlu_;
}

const std::string& Cantec::getDificultate() const {
    return dificultate_;
}

const std::string& Cantec::getPartituraText() const {
    return partituraText_;
}

int Cantec::getPauzaStandardMs() const {
    return pauzaStandardMs_;
}

const std::vector<Nota>& Cantec::getNote() const {
    return note_;
}

// OOP Compunere operator<<: afiseaza simplu doar numele, codul partiturii si lungimea cantecului.
std::ostream& operator<<(std::ostream& os, const Cantec& c) {
    os << c.titlu_ << " | Partitura: " << c.partituraText_
       << " | Lungime: " << c.descrieLungime() << " (" << c.note_.size() << " note)";
    return os;
}
