// Dezactivam NDEBUG ca assert() sa ramana activ si in modul Release
#undef NDEBUG
#include <cassert>
#include <sstream>
#include <vector>

#include "Cantec.h"
#include "Instrument.h"
#include "Nota.h"
#include "Virtuoso.h"

int main() {
    const Nota n1('q', "Do", "do.wav", 50); // sub 100ms -> validat automat la 100ms
    assert(n1.getTimpAsteptareMs() == 100);
    assert(n1.getTasta() == 'q');
    assert(n1.getEticheta() == "Do");
    assert(n1.getFisierSunet() == "do.wav");
    assert(n1.potrivesteTasta('q'));
    assert(!n1.potrivesteTasta('w'));

    Nota n2(n1);
    n2.ajusteazaTimp(10.0);
    assert(n2.getTimpAsteptareMs() == 1000);

    Nota n3('t', "Sol", "sol.wav", 300);
    assert(n3.getTasta() == 't');
    n3 = n2;
    assert(n3.getTimpAsteptareMs() == 1000);

    Instrument pian("Pian", "assets/sounds/pian/");
    Cantec cantec("TestSong", "Usor", 1000);
    cantec.adaugaNota(n2);
    cantec.adaugaNota(Nota('w', "Re", "re.wav", 1000));
    assert(cantec.getTitlu() == "TestSong");
    assert(cantec.getDificultate() == "Usor");
    assert(cantec.getNote().size() == 2);
    assert(cantec.calculeazaDurataTotalaMs() == 2000);

    cantec.construiesteDinPartitura("q--w-qq", pian, false);
    assert(cantec.getNote().size() == 4);
    assert(cantec.getPartituraText() == "q--w-qq");
    assert(cantec.getPauzaStandardMs() == 1000);
    assert(cantec.calculeazaDurataTotalaMs() == 4500);

    cantec.seteazaNivelViteza(1);
    assert(cantec.getPauzaStandardMs() == 400);
    assert(cantec.calculeazaDurataTotalaMs() == 1800);
    assert(cantec.getRaportViteza() == "1:1 [Greu - Viteza Reala]");

    cantec.seteazaNivelViteza(2);
    assert(cantec.getPauzaStandardMs() == 600);
    assert(cantec.getRaportViteza() == "1:1.5 [Mediu]");

    assert(pian.getNume() == "Pian");
    Nota gasita;
    assert(pian.cautaNotaDupaTasta('e', gasita));
    assert(gasita.getEticheta() == "Mi");
    assert(!pian.cautaNotaDupaTasta('z', gasita));

    Virtuoso joc("Tester");
    joc.alegeInstrument(0);
    joc.creeazaCantec("Custom_Test", "q-w--e", 1000);
    assert(joc.getNumarCantece() == 1);
    joc.redaPreviewCantec(0);

    const int scorObtinut = joc.joacaCantec(0, {'q', 'w', 'e'}, {1000, 1000, 2000});
    assert(scorObtinut == 600);
    assert(joc.getScor() == 600);
    assert(joc.getCombo() == 3);
    assert(joc.getNrGreseli() == 0);

    joc.joacaCantec(0, {'q', 'x', 'e'}, {1000, 1000, 2000});
    assert(joc.getNrGreseli() == 1);

    std::ostringstream oss;
    oss << joc;
    assert(!oss.str().empty());

    return 0;
}
