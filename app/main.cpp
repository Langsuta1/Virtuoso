#include <cstddef>
#include <iostream>
#include <string>

#include "Cantec.h"
#include "Instrument.h"
#include "Nota.h"
#include "Virtuoso.h"

int main() {
    // Testam Regula celor 3 pe clasa Nota (constructor de copiere, operator=, destructor)
    const Nota notaOriginala('q', "Do", "do.wav", 1000);
    const Nota notaCopiata(notaOriginala);
    Nota notaAtribuita('t', "Sol", "sol.wav", 500);
    if (notaAtribuita.getTasta() != notaCopiata.getTasta()) {
        notaAtribuita = notaCopiata;
    }

    Virtuoso joc(0);
    if (!joc.incarcaCanteceDinFisier("assets/cantece.txt")) {
        joc.creeazaCantec("Melodie_Demo", "q-w-e-r-t-y", 1000);
    }

    std::cout << "==========================================================\n";
    std::cout << "               BINE AI VENIT IN VIRTUOSO!                 \n";
    std::cout << "  Taste (QWERTY): q=Do, w=Re, e=Mi, r=Fa, t=Sol, y=La     \n";
    std::cout << "  Format Partitura: '-' = 1 secunda de liber intre note   \n";
    std::cout << "                    Note lipite (ex: qq) = fara pauza     \n";
    std::cout << "  Nota de baza:   " << notaAtribuita << "\n";
    std::cout << "==========================================================\n";

    bool rulare = true;
    while (rulare) {
        std::cout << "\n================ MENIU PRINCIPAL ===============\n";
        std::cout << "  1. Schimba instrumentul (Pian / Chitara)\n";
        std::cout << "  2. Canta un cantec (dupa partitura cu '-')\n";
        std::cout << "  3. Asculta Preview cantec (in ritm real!)\n";
        std::cout << "  4. Compune propriul cantec (ex: q--w--qq-e)\n";
        std::cout << "  5. Modifica viteza (1:1 Greu / 1:1.5 Mediu / 1:2.5 Usor)\n";
        std::cout << "  6. Ghid Taste & Lista Partituri\n";
        std::cout << "  0. Iesire din joc\n";
        std::cout << "================================================\n";
        std::cout << "Optiunea ta (0-6): " << std::flush;

        int optiune = 0;
        if (!(std::cin >> optiune)) {
            break;
        }

        if (optiune == 1) {
            std::size_t nrInst = 0;
            std::cout << "\nAlege instrumentul:\n  [1] Pian\n  [2] Chitara\nOptiune (1 sau 2): " << std::flush;
            if (std::cin >> nrInst) {
                if (nrInst >= 1) {
                    joc.alegeInstrument(nrInst - 1);
                } else {
                    std::cout << "\n[!] Optiune invalida! Alege 1 (Pian) sau 2 (Chitara).\n";
                }
            }
        } else if (optiune == 2) {
            joc.afiseazaListaCantece();
            std::size_t nrCantec = 0;
            std::cout << "Alege numarul cantecului pe care vrei sa il canti (1.." << joc.getNumarCantece() << "): " << std::flush;
            if (std::cin >> nrCantec) {
                if (nrCantec >= 1) {
                    joc.joacaCantecInteractiv(nrCantec - 1, std::cin);
                    std::cout << "Rezumat runda -> Scor: " << joc.getScor() << " | Combo: " << joc.getCombo() << " | Greseli: " << joc.getNrGreseli() << "\n";
                } else {
                    std::cout << "[!] Numar de cantec invalid!\n";
                }
            }
        } else if (optiune == 3) {
            joc.afiseazaListaCantece();
            std::size_t nrCantec = 0;
            std::cout << "Alege numarul cantecului pentru Preview (1.." << joc.getNumarCantece() << "): " << std::flush;
            if (std::cin >> nrCantec) {
                if (nrCantec >= 1) {
                    joc.redaPreviewCantec(nrCantec - 1);
                } else {
                    std::cout << "[!] Numar de cantec invalid!\n";
                }
            }
        } else if (optiune == 4) {
            std::string titluNou;
            std::string partituraNoua;
            std::cout << "\nIntrodu titlul cantecului (fara spatii, ex: Melodia_Mea): " << std::flush;
            std::cin >> titluNou;
            std::cout << "Introdu partitura folosind q,w,e,r,t,y si '-' pentru 1s liber (ex: q--w--w--qq-q-we-e): " << std::flush;
            std::cin >> partituraNoua;
            joc.creeazaCantec(titluNou, partituraNoua, 1000);
        } else if (optiune == 5) {
            joc.afiseazaListaCantece();
            std::size_t nrCantec = 0;
            int nivelViteza = 3;
            std::cout << "Alege numarul cantecului (1.." << joc.getNumarCantece() << "): " << std::flush;
            std::cin >> nrCantec;
            std::cout << "\nAlege nivelul de viteza:\n";
            std::cout << "  [1] Greu  - Raport 1:1   cu cel real ('-' = 0.4s, identic cu Preview-ul!)\n";
            std::cout << "  [2] Mediu - Raport 1:1.5 fata de real ('-' = 0.6s, putin mai incet)\n";
            std::cout << "  [3] Usor  - Raport 1:2.5 fata de real ('-' = 1.0s, ritm de incepatori)\n";
            std::cout << "Optiunea ta (1-3): " << std::flush;
            if (std::cin >> nivelViteza) {
                if (nrCantec >= 1) {
                    joc.ajusteazaRitmCantec(nrCantec - 1, nivelViteza);
                } else {
                    std::cout << "[!] Numar de cantec invalid!\n";
                }
            }
        } else if (optiune == 6) {
            std::cout << joc;
        } else if (optiune == 0) {
            std::cout << "\nLa revedere! Multumim ca ai jucat Virtuoso!\n";
            rulare = false;
        } else {
            std::cout << "\n[!] Optiune invalida! Alege o cifra intre 0 si 6.\n";
        }
    }

    return 0;
}
