#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <algorithm> // Pentru remove_if

using namespace std;

// Clasa abstractă de bază pentru entitățile ecosistemului
class EntitateEcosistem {
protected:
    string nume;
    int energie;
    pair<int, int> pozitie;
    float rataSupravietuire;

public:
    EntitateEcosistem(string nume, int energie, pair<int, int> pozitie, float rataSupravietuire)
        : nume(nume), energie(energie), pozitie(pozitie), rataSupravietuire(rataSupravietuire) {}

    virtual ~EntitateEcosistem() = default;

    virtual void actioneaza() = 0;

    pair<int, int> getPozitie() const { return pozitie; }

    void afiseaza() const {
        cout << nume << " la pozitia (" << pozitie.first << ", " << pozitie.second
             << ") cu energie: " << energie << endl;
    }

    bool esteInViata() const { return energie > 0; }
};

// Clasa pentru Plante
class Planta : public EntitateEcosistem {
public:
    Planta(string nume, pair<int, int> pozitie)
        : EntitateEcosistem(nume, 10, pozitie, 0.5f) {}

    void actioneaza() override {
        energie += 5; // Creșterea plantei
        cout << nume << " a crescut, energie: " << energie << endl;
    }

    void reproduce(vector<shared_ptr<EntitateEcosistem>>& ecosistem) {
        if (energie > 20) {
            energie -= 10;
            ecosistem.push_back(make_shared<Planta>("Noua planta", make_pair(rand() % 10, rand() % 10)));
            cout << nume << " s-a reprodus!" << endl;
        }
    }
};

// Clasa abstractă pentru Animale
class Animal : public EntitateEcosistem {
protected:
    int viteza;
    string tipHrana;

public:
    Animal(string nume, int energie, pair<int, int> pozitie, float rataSupravietuire, int viteza, string tipHrana)
        : EntitateEcosistem(nume, energie, pozitie, rataSupravietuire), viteza(viteza), tipHrana(tipHrana) {}

    virtual void mananca(vector<shared_ptr<EntitateEcosistem>>& ecosistem) = 0;

    virtual void deplaseaza() {
        pozitie.first = (pozitie.first + rand() % viteza - viteza / 2 + 10) % 10;
        pozitie.second = (pozitie.second + rand() % viteza - viteza / 2 + 10) % 10;
        cout << nume << " s-a deplasat la (" << pozitie.first << ", " << pozitie.second << ")" << endl;
    }
};

// Subclase pentru diferite tipuri de animale
class Erbivor : public Animal {
public:
    Erbivor(string nume, pair<int, int> pozitie)
        : Animal(nume, 20, pozitie, 0.6f, 2, "plante") {}

    void mananca(vector<shared_ptr<EntitateEcosistem>>& ecosistem) override {
        for (auto& entitate : ecosistem) {
            if (auto planta = dynamic_pointer_cast<Planta>(entitate)) {
                if (planta->getPozitie() == pozitie) {
                    energie += 10;
                    planta->~Planta(); // Plantele sunt consumate
                    cout << nume << " a mâncat o plantă!" << endl;
                    return;
                }
            }
        }
    }
};

class Carnivor : public Animal {
public:
    Carnivor(string nume, pair<int, int> pozitie)
        : Animal(nume, 30, pozitie, 0.8f, 3, "animale") {}

    void mananca(vector<shared_ptr<EntitateEcosistem>>& ecosistem) override {
        for (auto& entitate : ecosistem) {
            if (auto animal = dynamic_pointer_cast<Animal>(entitate)) {
                if (animal->getPozitie() == pozitie && animal.get() != this) {
                    energie += 20;
                    animal->~Animal(); // Animalele sunt consumate
                    cout << nume << " a vânat un animal!" << endl;
                    return;
                }
            }
        }
    }
};

// Clasa Ecosistem
class Ecosistem {
private:
    vector<shared_ptr<EntitateEcosistem>> entitati;
    int dimensiuneHarta;

public:
    Ecosistem(int dimHarta) : dimensiuneHarta(dimHarta) {
        srand(time(0));
    }

    void adaugaEntitate(shared_ptr<EntitateEcosistem> entitate) {
        entitati.push_back(entitate);
    }

    void simuleaza() {
        for (auto& entitate : entitati) {
            if (entitate->esteInViata()) {
                entitate->actioneaza();

                if (auto animal = dynamic_pointer_cast<Animal>(entitate)) {
                    animal->deplaseaza();
                    animal->mananca(entitati);
                }
            }
        }
        // Eliminare entități moarte
        entitati.erase(remove_if(entitati.begin(), entitati.end(),
                                 [](shared_ptr<EntitateEcosistem> entitate) { return !entitate->esteInViata(); }),
                       entitati.end());
    }

    void afiseaza() const {
        for (const auto& entitate : entitati) {
            entitate->afiseaza();
        }
    }
};

// Funcția principală
int main() {
    Ecosistem ecosistem(10);

    ecosistem.adaugaEntitate(make_shared<Planta>("Planta1", make_pair(2, 3)));
    ecosistem.adaugaEntitate(make_shared<Erbivor>("Iepure", make_pair(2, 3)));
    ecosistem.adaugaEntitate(make_shared<Carnivor>("Lup", make_pair(5, 5)));

    for (int i = 0; i < 5; ++i) {
        cout << "Pasul " << i + 1 << endl;
        ecosistem.simuleaza();
        ecosistem.afiseaza();
        cout << "==========================" << endl;
    }

    return 0;
}
