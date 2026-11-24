#include "Nota.h"

void Nota::valideazaTimp() {
    if (timpAsteptareMs_ < 100) {
        timpAsteptareMs_ = 100;
    }
}

Nota::Nota(char tasta,
           const std::string& eticheta,
           const std::string& fisierSunet,
           int timpAsteptareMs)
    : tasta_(tasta),
      eticheta_(eticheta),
      fisierSunet_(fisierSunet),
      timpAsteptareMs_(timpAsteptareMs) {
    valideazaTimp();
}

Nota::Nota(const Nota& other)
    : tasta_(other.tasta_),
      eticheta_(other.eticheta_),
      fisierSunet_(other.fisierSunet_),
      timpAsteptareMs_(other.timpAsteptareMs_) {}

Nota& Nota::operator=(const Nota& other) {
    if (this != &other) { // Protectie la auto-atribuire (n1 = n1)
        tasta_ = other.tasta_;
        eticheta_ = other.eticheta_;
        fisierSunet_ = other.fisierSunet_;
        timpAsteptareMs_ = other.timpAsteptareMs_;
    }
    return *this;
}

Nota::~Nota() = default;

bool Nota::potrivesteTasta(char tastaApasata) const {
    return tasta_ == tastaApasata;
}

void Nota::ajusteazaTimp(double factor) {
    if (factor > 0.0) {
        timpAsteptareMs_ = static_cast<int>(timpAsteptareMs_ * factor);
        valideazaTimp();
    }
}

char Nota::getTasta() const {
    return tasta_;
}

const std::string& Nota::getEticheta() const {
    return eticheta_;
}

const std::string& Nota::getFisierSunet() const {
    return fisierSunet_;
}

int Nota::getTimpAsteptareMs() const {
    return timpAsteptareMs_;
}

std::ostream& operator<<(std::ostream& os, const Nota& n) {
    os << "'" << n.tasta_ << "' (" << n.eticheta_ << ")";
    return os;
}
