#include "Cantec.h"

#include <chrono>
#include <sstream>

namespace {
// Scurta pauza intre note cand compui un cantec, ca sunetele sa nu se suprapuna
void asteaptaRedareCreareMs(int durataMs) {
#ifdef GITHUB_ACTIONS
    (void) durataMs; // Dezactivat pe CI pentru viteza
#else
    const auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now() - start)
               .count() < durataMs) {
    }
#endif
}
} // namespace

std::string Cantec::descrieLungime() const {
    if (note_.size() <= 11) {
        return "Scurt";
    }
    if (note_.size() <= 15) {
        return "Mediu";
    }
    return "Lung";
}

Cantec::Cantec(const std::string& titlu,
               const std::string& dificultate,
               int pauzaStandardMs)
    : titlu_(titlu),
      dificultate_(dificultate),
      partituraText_(""),
      pauzaStandardMs_((pauzaStandardMs >= 200) ? pauzaStandardMs : 1000) {}

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

void Cantec::construiesteDinPartitura(const std::string& partituraText,
                                      const Instrument& inst,
                                      bool afiseazaCreare) {
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
                // Notele lipite (0 liniute '-') sunt rapide (jumatate din pauza standard)
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
                    std::cout << "   [PAUZA]         -> " << liniiPauzaCurenta << "x '-' ("
                              << (timpAsteptareNotaMs / 1000.0) << " secunde de liber)\n";
                }
                inst.redaOutput(notaCuPauza, false);
                asteaptaRedareCreareMs(450);
            }
            liniiPauzaCurenta = 0;
        } else if (afiseazaCreare) {
            std::cout << "   [!] Caracterul '" << c
                      << "' nu este nici nota (q,w,e,r,t,y), nici pauza ('-') - ignorat.\n";
        }
    }
}

int Cantec::calculeazaDurataTotalaMs() const {
    int total = 0;
    for (const auto& n : note_) {
        total += n.getTimpAsteptareMs();
    }
    return total;
}

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

void Cantec::afiseazaSecventaTaste() const {
    std::cout << "Partitura:  " << partituraText_ << "   (Viteza: " << getRaportViteza()
              << " | '-' = " << (pauzaStandardMs_ / 1000.0) << "s)\n";
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

std::ostream& operator<<(std::ostream& os, const Cantec& c) {
    os << c.titlu_ << " | Partitura: " << c.partituraText_
       << " | Lungime: " << c.descrieLungime() << " (" << c.note_.size() << " note)";
    return os;
}
