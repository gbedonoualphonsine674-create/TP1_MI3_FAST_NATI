#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
using namespace std;

class Etudiant;

// ---------------------------------------------------------------- Livre
class Livre {
private:
    int id;
    string titre;
    string auteur;
    int annee;
    bool disponible;
    Etudiant* emprunteur;   // 0..1 : étudiant qui détient le livre

public:
    Livre(int id, const string& titre, const string& auteur, int annee)
        : id(id), titre(titre), auteur(auteur), annee(annee),
          disponible(true), emprunteur(nullptr) {}

    int getId() const { return id; }
    string getTitre() const { return titre; }
    string getAuteur() const { return auteur; }
    int getAnnee() const { return annee; }
    bool estDisponible() const { return disponible; }
    Etudiant* getEmprunteur() const { return emprunteur; }

    void marquerEmprunte(Etudiant* e) { disponible = false; emprunteur = e; }
    void marquerRetourne() { disponible = true; emprunteur = nullptr; }

    void afficher() const;   // défini après Etudiant
};

// ------------------------------------------------------------- Etudiant
class Etudiant {
private:
    string matricule;
    string nom;
    string prenom;
    string email;
    vector<Livre*> livresEmpruntes;   // 0..*

public:
    Etudiant(const string& matricule, const string& nom,
             const string& prenom, const string& email)
        : matricule(matricule), nom(nom), prenom(prenom), email(email) {}

    string getMatricule() const { return matricule; }
    string getNom() const { return nom; }
    string getPrenom() const { return prenom; }
    string getEmail() const { return email; }
    const vector<Livre*>& getLivres() const { return livresEmpruntes; }

    void ajouterLivre(Livre* l) { livresEmpruntes.push_back(l); }
    void retirerLivre(Livre* l) {
        livresEmpruntes.erase(
            remove(livresEmpruntes.begin(), livresEmpruntes.end(), l),
            livresEmpruntes.end());
    }

    void afficher() const {
        cout << "[" << matricule << "] " << prenom << " " << nom
             << " <" << email << "> - " << livresEmpruntes.size()
             << " livre(s) emprunte(s)\n";
    }
};

void Livre::afficher() const {
    cout << "[" << id << "] " << titre << " - " << auteur << " (" << annee << ") - ";
    if (disponible) cout << "disponible\n";
    else cout << "emprunte par " << emprunteur->getMatricule() << "\n";
}

// ----------------------------------------------------------- Bibliotheque
class Bibliotheque {
private:
    vector<unique_ptr<Livre>> livres;
    vector<unique_ptr<Etudiant>> etudiants;

public:
    void ajouterLivre(int id, const string& t, const string& a, int an) {
        livres.push_back(make_unique<Livre>(id, t, a, an));
    }
    void ajouterEtudiant(const string& m, const string& n,
                         const string& p, const string& e) {
        etudiants.push_back(make_unique<Etudiant>(m, n, p, e));
    }

    Livre* trouverLivre(int id) const {
        for (auto& l : livres) if (l->getId() == id) return l.get();
        return nullptr;
    }
    Etudiant* trouverEtudiant(const string& matricule) const {
        for (auto& e : etudiants) if (e->getMatricule() == matricule) return e.get();
        return nullptr;
    }

    bool emprunter(int idLivre, const string& matricule) {
        Livre* l = trouverLivre(idLivre);
        Etudiant* e = trouverEtudiant(matricule);
        if (!l || !e) { cout << "Livre ou etudiant introuvable.\n"; return false; }
        if (!l->estDisponible()) { cout << "Ce livre est deja emprunte.\n"; return false; }
        l->marquerEmprunte(e);
        e->ajouterLivre(l);
        cout << "Emprunt effectue : \"" << l->getTitre() << "\" -> "
             << e->getPrenom() << " " << e->getNom() << "\n";
        return true;
    }

    bool retourner(int idLivre) {
        Livre* l = trouverLivre(idLivre);
        if (!l) { cout << "Livre introuvable.\n"; return false; }
        if (l->estDisponible()) { cout << "Ce livre n'est pas emprunte.\n"; return false; }
        l->getEmprunteur()->retirerLivre(l);
        l->marquerRetourne();
        cout << "Retour enregistre : \"" << l->getTitre() << "\"\n";
        return true;
    }

    void afficherLivres() const { for (auto& l : livres) l->afficher(); }
    void afficherEtudiants() const { for (auto& e : etudiants) e->afficher(); }
};

// ------------------------------------------------------------------ main
int main() {
    Bibliotheque bib;
    bib.ajouterLivre(1, "Le Petit Prince", "Saint-Exupery", 1943);
    bib.ajouterLivre(2, "L'Etranger", "Albert Camus", 1942);
    bib.ajouterLivre(3, "Les Miserables", "Victor Hugo", 1862);
    bib.ajouterEtudiant("E001", "Benali", "Sara", "sara.benali@univ.dz");
    bib.ajouterEtudiant("E002", "Khelifi", "Yacine", "yacine.khelifi@univ.dz");

    int choix;
    do {
        cout << "\n=== BIBLIOTHEQUE ===\n"
             << "1. Afficher les livres\n"
             << "2. Afficher les etudiants\n"
             << "3. Emprunter un livre\n"
             << "4. Retourner un livre\n"
             << "0. Quitter\n"
             << "Choix : ";
        if (!(cin >> choix)) break;

        switch (choix) {
        case 1: bib.afficherLivres(); break;
        case 2: bib.afficherEtudiants(); break;
        case 3: {
            int id; string mat;
            cout << "ID du livre : "; cin >> id;
            cout << "Matricule : "; cin >> mat;
            bib.emprunter(id, mat);
            break;
        }
        case 4: {
            int id;
            cout << "ID du livre : "; cin >> id;
            bib.retourner(id);
            break;
        }
        case 0: cout << "Au revoir.\n"; break;
        default: cout << "Choix invalide.\n";
        }
    } while (choix != 0);

    return 0;
}
