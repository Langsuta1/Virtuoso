#ifndef NOTA_H
#define NOTA_H

#include <iostream>
#include <string>

class Nota {
private:
    char tasta_;
    std::string eticheta_;
    std::string fisierSunet_;
    int timpAsteptareMs_;

    void valideazaTimp();

public:
    // explicit previne conversiile implicite (ex: un char transformat automat in Nota)
    explicit Nota(char tasta = 'q', const std::string& eticheta = "Do", const std::string& fisierSunet = "do.wav", int timpAsteptareMs = 500);

    // Regula celor 3
    Nota(const Nota& other);
    Nota& operator=(const Nota& other);
    ~Nota();

    bool potrivesteTasta(char tastaApasata) const;
    void ajusteazaTimp(double factor);

    char getTasta() const;
    const std::string& getEticheta() const;
    const std::string& getFisierSunet() const;
    int getTimpAsteptareMs() const;

    friend std::ostream& operator<<(std::ostream& os, const Nota& n);
};

#endif // NOTA_H
