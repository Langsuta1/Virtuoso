#include "Virtuoso.h"

#include <chrono>
#include <cmath>
#include <fstream>

namespace {
// Asteptare in timp real pentru animatia '- - APASA ACUM!' si Preview (dezactivata pe CI)
void asteaptaAnimatieMs(int durataMs) {
#ifdef GITHUB_ACTIONS
    (void) durataMs;
#else
    const auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now() - start)
               .count() < durataMs) {
    }
#endif
}
} // namespace

int Virtuoso::calculeazaPuncteNota(int diferentaTimpMs) const {
    if (diferentaTimpMs <= 100) {
        return 100 * combo_;
    }
    if (diferentaTimpMs <= 250) {
        return 50;
    }
    return 0;
}

void Virtuoso::proceseazaApasareNota(const Nota& notaCeruta,
                                     char tastaJucator,
                                     int diferentaTimpMs,
                                     int distantaAfisataMs) {
    const Instrument& inst = instrumente_[instrumentSelectat_];
    std::cout << " -> Ai apasat '" << tastaJucator << "' (timp de reactie dupa semnal: "
              << (distantaAfisataMs / 1000.0) << "s)\n";

    if (notaCeruta.potrivesteTasta(tastaJucator)) {
        if (diferentaTimpMs <= 250) {
            ++combo_;
            const int puncte = calculeazaPuncteNota(diferentaTimpMs);
            scor_ += puncte;
            inst.redaOutput(notaCeruta, false);
            std::cout << "   [HIT!] +" << puncte << " puncte (Combo: " << combo_ << "x)\n";
        } else {
            combo_ = 0;
            ++nrGreseli_;
            inst.redaOutput(notaCeruta, true);
            std::cout << "   [PREA TARZIU!] Ai reactionat prea greu dupa semnalul 'APASA ACUM!' ("
                      << (distantaAfisataMs / 1000.0)
                      << "s). Combo resetat, continua cu urmatoarea nota!\n";
        }
    } else {
        combo_ = 0;
        ++nrGreseli_;
        inst.redaOutput(notaCeruta, true);
        std::cout << "   [TASTA GRESITA!] Ai apasat '" << tastaJucator
                  << "' in loc de '" << notaCeruta.getTasta() << "'. Combo resetat!\n";
    }
}

Virtuoso::Virtuoso(const std::string& numeJucator)
    : numeJucator_(numeJucator),
      instrumentSelectat_(0),
      scor_(0),
      combo_(0),
      nrGreseli_(0) {
    instrumente_.emplace_back("Pian", "assets/sounds/pian/");
    instrumente_.emplace_back("Chitara", "assets/sounds/chitara/");
}

bool Virtuoso::incarcaCanteceDinFisier(const std::string& caleFisier) {
    std::ifstream fin(caleFisier);
    if (!fin.is_open()) {
        return false;
    }

    int numarCantece = 0;
    if (!(fin >> numarCantece) || numarCantece <= 0) {
        return false;
    }

    for (int i = 0; i < numarCantece; ++i) {
        std::string titlu;
        std::string dificultate;
        std::string partituraText;

        if (!(fin >> titlu >> dificultate >> partituraText)) {
            break;
        }

        Cantec cantecCitit(titlu, dificultate, 1000);
        cantecCitit.construiesteDinPartitura(partituraText, instrumente_[0], false);
        cantece_.push_back(cantecCitit);
    }

    return !cantece_.empty();
}

void Virtuoso::alegeInstrument(std::size_t index) {
    if (index < instrumente_.size()) {
        instrumentSelectat_ = index;
        std::cout << "\n>>> Instrument activ setat pe: "
                  << instrumente_[instrumentSelectat_].getNume() << " <<<\n";
        instrumente_[instrumentSelectat_].afiseazaControale();
    } else {
        std::cout << "\n[!] Optiune invalida! Ramane selectat: "
                  << instrumente_[instrumentSelectat_].getNume() << "\n";
    }
}

void Virtuoso::afiseazaListaCantece() const {
    std::cout << "\n------------------------- ALEGE CANTECUL -------------------------\n";
    std::cout << "Instrumentul tau curent: " << instrumente_[instrumentSelectat_].getNume() << "\n";
    for (std::size_t i = 0; i < cantece_.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << cantece_[i].getTitlu()
                  << " (" << cantece_[i].getDificultate()
                  << " | " << cantece_[i].getNote().size() << " note"
                  << " | Raport viteza: " << cantece_[i].getRaportViteza()
                  << " | '-' = " << (cantece_[i].getPauzaStandardMs() / 1000.0) << "s)\n"
                  << "      Partitura: " << cantece_[i].getPartituraText() << "\n";
    }
    std::cout << "------------------------------------------------------------------\n";
}

void Virtuoso::redaPreviewCantec(std::size_t indexCantec) const {
    if (indexCantec >= cantece_.size()) {
        std::cout << "[!] Cantecul selectat nu exista!\n";
        return;
    }

    const Cantec& cantec = cantece_[indexCantec];
    const Instrument& inst = instrumente_[instrumentSelectat_];
    const auto& note = cantec.getNote();
    const int bazaMs = cantec.getPauzaStandardMs();

    std::cout << "\n==========================================================\n";
    std::cout << ">>> PREVIEW AUDIO (Viteza Reala 1:1): \"" << cantec.getTitlu() << "\" la "
              << inst.getNume() << " <<<\n";
    std::cout << "Partitura: " << cantec.getPartituraText() << " ('-' = 0.4s in timp real)\n";
    std::cout << "==========================================================\n";

    for (std::size_t i = 0; i < note.size(); ++i) {
        const Nota& notaCurenta = note[i];
        if (i > 0) {
            // Scalam timpul notei la viteza reala 1:1 (400ms pentru '-', 200ms pentru note lipite)
            int pauzaFluentaMs =
                (notaCurenta.getTimpAsteptareMs() * Cantec::kPauzaRealaMs) / bazaMs;
            if (pauzaFluentaMs < 200) {
                pauzaFluentaMs = 200;
            } else if (pauzaFluentaMs > 1200) {
                pauzaFluentaMs = 1200;
            }
            asteaptaAnimatieMs(pauzaFluentaMs);
        }

        std::cout << "  [" << (i + 1) << "/" << note.size() << "] Nota " << notaCurenta << "\n";
        inst.redaOutput(notaCurenta, false);
    }

    asteaptaAnimatieMs(550);
    std::cout << ">>> Preview incheiat pentru \"" << cantec.getTitlu() << "\"! <<<\n";
}

void Virtuoso::ajusteazaRitmCantec(std::size_t indexCantec, int nivelViteza) {
    if (indexCantec < cantece_.size() && nivelViteza >= 1 && nivelViteza <= 3) {
        cantece_[indexCantec].seteazaNivelViteza(nivelViteza);
        std::cout << "-> Viteza cantecului \"" << cantece_[indexCantec].getTitlu()
                  << "\" a fost setata la: Raport " << cantece_[indexCantec].getRaportViteza()
                  << " (acum fiecare '-' dureaza "
                  << (cantece_[indexCantec].getPauzaStandardMs() / 1000.0)
                  << "s | durata totala: "
                  << (cantece_[indexCantec].calculeazaDurataTotalaMs() / 1000.0) << "s).\n";
    } else {
        std::cout << "[!] Cantec sau nivel de viteza invalid (alege 1, 2 sau 3)!\n";
    }
}

int Virtuoso::joacaCantecInteractiv(std::size_t indexCantec, std::istream& in) {
    if (indexCantec >= cantece_.size()) {
        std::cout << "[!] Cantecul selectat nu exista!\n";
        return 0;
    }

    const Cantec& cantec = cantece_[indexCantec];
    const Instrument& inst = instrumente_[instrumentSelectat_];
    const auto& note = cantec.getNote();
    const int durataUnuiMinusMs = cantec.getPauzaStandardMs();

    scor_ = 0;
    combo_ = 0;
    nrGreseli_ = 0;

    std::cout << "\n==========================================================\n";
    std::cout << "CANTEC: " << cantec.getTitlu() << " | INSTRUMENT: " << inst.getNume() << "\n";
    cantec.afiseazaSecventaTaste();
    std::cout << "==========================================================\n";
    std::cout << "CUM SE JOACA:\n";
    std::cout << " * Nu trebuie sa numeri tu secundele! Urmareste ecranul:\n";
    std::cout << " * Jocul va afisa pe rand '- ' pentru fiecare secunda de pauza,\n";
    std::cout << "   iar cand apare '>>> APASA ACUM! <<<', apesi tasta si Enter!\n";

    for (std::size_t i = 0; i < note.size(); ++i) {
        const Nota& notaCeruta = note[i];
        const int timpTintaMs = notaCeruta.getTimpAsteptareMs();
        const int nrLiniiPauza = (i == 0) ? 0 : (timpTintaMs / durataUnuiMinusMs);

        std::cout << "\n----------------------------------------------------------\n";
        if (i == 0) {
            std::cout << "[NOTA #1/" << note.size() << " - START] Pregateste tasta '"
                      << notaCeruta.getTasta() << "' (" << notaCeruta.getEticheta() << ")\n";
            std::cout << ">>> APASA ACUM '" << notaCeruta.getTasta() << "': " << std::flush;
        } else {
            std::cout << "[NOTA #" << (i + 1) << "/" << note.size() << "] Urmeaza '"
                      << notaCeruta.getTasta() << "' (" << notaCeruta.getEticheta() << "): "
                      << std::flush;

            if (nrLiniiPauza > 0) {
                for (int p = 0; p < nrLiniiPauza; ++p) {
                    asteaptaAnimatieMs(durataUnuiMinusMs);
                    std::cout << "- " << std::flush;
                }
            } else {
                asteaptaAnimatieMs(durataUnuiMinusMs / 2);
                std::cout << "(rapid!) " << std::flush;
            }

            std::cout << ">>> APASA ACUM '" << notaCeruta.getTasta() << "': " << std::flush;
        }

        const auto momentSemnal = std::chrono::steady_clock::now();

        // Citim intregul cuvant ca sa nu ramana caractere in buffer care sa decaleze notele urmatoare
        std::string tokenIntrodus;
        if (!(in >> tokenIntrodus)) {
            break;
        }
        const char tastaJucator = tokenIntrodus[0];
        const auto momentApasare = std::chrono::steady_clock::now();

        const int timpReactieDupaSemnalMs = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(momentApasare - momentSemnal)
                .count());

        int diferentaTimpMs = 0;
        if (i > 0) {
            if (timpReactieDupaSemnalMs <= 1400) {
                diferentaTimpMs = 50;
            } else if (timpReactieDupaSemnalMs <= 2400) {
                diferentaTimpMs = 200;
            } else {
                diferentaTimpMs = 500;
            }
        }

        proceseazaApasareNota(notaCeruta,
                              tastaJucator,
                              diferentaTimpMs,
                              (timpReactieDupaSemnalMs < 20) ? 350 : timpReactieDupaSemnalMs);
    }

    std::cout << "\n==========================================================\n";
    std::cout << "REZULTAT FINAL \"" << cantec.getTitlu() << "\":\n";
    std::cout << " -> Scor Total:  " << scor_ << " puncte\n";
    std::cout << " -> Combo Final: " << combo_ << "x\n";
    std::cout << " -> Greseli:     " << nrGreseli_ << "\n";
    std::cout << "==========================================================\n";
    return scor_;
}

int Virtuoso::joacaCantec(std::size_t indexCantec,
                          const std::vector<char>& tasteApasate,
                          const std::vector<int>& timpiMs) {
    if (indexCantec >= cantece_.size()) {
        return 0;
    }

    const Cantec& cantec = cantece_[indexCantec];
    const Instrument& inst = instrumente_[instrumentSelectat_];
    const auto& note = cantec.getNote();

    scor_ = 0;
    combo_ = 0;
    nrGreseli_ = 0;

    std::cout << "\n======================================================\n";
    std::cout << "JUCATOR: " << numeJucator_ << " | CANTEC: " << cantec.getTitlu()
              << " | INSTRUMENT: " << inst.getNume() << "\n";
    cantec.afiseazaSecventaTaste();
    std::cout << "======================================================\n";

    for (std::size_t i = 0; i < note.size() && i < tasteApasate.size(); ++i) {
        const Nota& notaCeruta = note[i];
        const char tastaJucator = tasteApasate[i];
        const int timpJucator =
            (i < timpiMs.size()) ? timpiMs[i] : notaCeruta.getTimpAsteptareMs();

        std::cout << "\nNota #" << (i + 1) << ": Apasa '" << notaCeruta.getTasta()
                  << "' (" << notaCeruta.getEticheta() << ") la ~"
                  << notaCeruta.getTimpAsteptareMs() << "ms\n";

        const int diferenta = std::abs(timpJucator - notaCeruta.getTimpAsteptareMs());
        proceseazaApasareNota(notaCeruta, tastaJucator, diferenta, timpJucator);
    }

    std::cout << "\n--- REZULTAT FINAL \"" << cantec.getTitlu() << "\" ---\n";
    std::cout << "Scor: " << scor_ << " pct | Combo final: " << combo_
              << "x | Greseli: " << nrGreseli_ << "\n";
    return scor_;
}

void Virtuoso::creeazaCantec(const std::string& titluNou,
                             const std::string& secventaTaste,
                             int ritmMs) {
    Cantec cantecNou(titluNou, "Custom", ritmMs);
    const Instrument& inst = instrumente_[instrumentSelectat_];

    std::cout << "\n=== MOD CREATIE: Compui \"" << titluNou << "\" la "
              << inst.getNume() << " ===\n";
    std::cout << "Format partitura: '" << secventaTaste << "' ('-' = "
              << (ritmMs / 1000.0) << "s de liber)\n";

    cantecNou.construiesteDinPartitura(secventaTaste, inst, true);

    cantece_.push_back(cantecNou);
    std::cout << "-> Cantecul \"" << titluNou << "\" [Partitura: "
              << cantecNou.getPartituraText() << " | Durata: "
              << (cantecNou.calculeazaDurataTotalaMs() / 1000.0)
              << "s] a fost salvat pe pozitia [" << cantece_.size() << "]!\n";
}

std::size_t Virtuoso::getNumarCantece() const {
    return cantece_.size();
}

int Virtuoso::getScor() const {
    return scor_;
}

int Virtuoso::getCombo() const {
    return combo_;
}

int Virtuoso::getNrGreseli() const {
    return nrGreseli_;
}

std::ostream& operator<<(std::ostream& os, const Virtuoso& v) {
    os << "\n================ GHID & PARTITURI VIRTUOSO ================\n"
       << "  Instrument activ: " << v.instrumente_[v.instrumentSelectat_] << "\n"
       << "  Ultima runda:     " << v.scor_ << " puncte | Combo: " << v.combo_
       << "x | Greseli: " << v.nrGreseli_ << "\n"
       << "-----------------------------------------------------------\n"
       << "  CANTECE (" << v.cantece_.size() << "):\n";
    for (std::size_t i = 0; i < v.cantece_.size(); ++i) {
        os << "   [" << (i + 1) << "] " << v.cantece_[i] << "\n";
    }
    os << "===========================================================\n";
    return os;
}
