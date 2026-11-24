#include "Instrument.h"

// Audio nativ Windows - dezactivat pe Linux/macOS si pe GitHub Actions
#if defined(_WIN32) && !defined(GITHUB_ACTIONS)
#include <windows.h>
#include <mmsystem.h>
#endif

namespace {
void redaFisierWav(const std::string& caleFisier) {
#if defined(_WIN32) && !defined(GITHUB_ACTIONS)
    // SND_ASYNC reda sunetul pe fundal ca sa nu blocheze consola
    PlaySoundA(caleFisier.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
#else
    (void) caleFisier;
#endif
}
} // namespace

void Instrument::afiseazaGraficaInstrument(const Nota& n) const {
    if (nume_ == "Pian") {
        std::cout << "   [ECRAN PIAN]    -> ";
        for (const auto& t : tasteMapate_) {
            if (t.getTasta() == n.getTasta()) {
                std::cout << "[### " << t.getEticheta() << " ###] ";
            } else {
                std::cout << "[ " << t.getTasta() << " ] ";
            }
        }
        std::cout << "\n";
    } else {
        std::cout << "   [ECRAN CHITARA] -> Coarda (" << n.getEticheta()
                  << "): o====~-~-~-~====o\n";
    }
}

Instrument::Instrument(const std::string& nume, const std::string& folderSunete)
    : nume_(nume), folderSunete_(folderSunete) {
    tasteMapate_.emplace_back('q', "Do",  "do.wav",  500);
    tasteMapate_.emplace_back('w', "Re",  "re.wav",  500);
    tasteMapate_.emplace_back('e', "Mi",  "mi.wav",  500);
    tasteMapate_.emplace_back('r', "Fa",  "fa.wav",  500);
    tasteMapate_.emplace_back('t', "Sol", "sol.wav", 500);
    tasteMapate_.emplace_back('y', "La",  "la.wav",  500);
}

bool Instrument::cautaNotaDupaTasta(char tasta, Nota& rezultat) const {
    for (const auto& n : tasteMapate_) {
        if (n.potrivesteTasta(tasta)) {
            rezultat = n;
            return true;
        }
    }
    return false;
}

void Instrument::redaOutput(const Nota& n, bool gresit) const {
    if (gresit) {
        const std::string caleEroare = folderSunete_ + "gresit.wav";
        std::cout << "   [SUNET FALS]    -> " << caleEroare << "\n";
        std::cout << "   [ECRAN]         -> (X) Nota sau timing gresit!\n";
        redaFisierWav(caleEroare);
        return;
    }

    const std::string caleNota = folderSunete_ + n.getFisierSunet();
    std::cout << "   [SUNET]         -> Redau: " << caleNota << "\n";
    afiseazaGraficaInstrument(n);
    redaFisierWav(caleNota);
}

void Instrument::afiseazaControale() const {
    std::cout << "Taste " << nume_ << " (QWERTY): ";
    for (const auto& n : tasteMapate_) {
        std::cout << n << "  ";
    }
    std::cout << "\n";
}

const std::string& Instrument::getNume() const {
    return nume_;
}

std::ostream& operator<<(std::ostream& os, const Instrument& inst) {
    os << inst.nume_ << " -> Taste: ";
    for (const auto& n : inst.tasteMapate_) {
        os << n << "  ";
    }
    return os;
}
