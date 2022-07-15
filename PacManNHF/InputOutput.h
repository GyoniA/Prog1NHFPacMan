#ifndef INPUTOUTPUT_H_INCLUDED
#define INPUTOUTPUT_H_INCLUDED

typedef struct Eredmeny {
    int Pontszam;
    char *Nev;
    struct Eredmeny *elozo, *kovetkezo;
} Eredmeny;

void KepernyoreIras(TTF_Font* font, const char* text, SDL_Color Szin, SDL_Renderer* renderer, int X, int Y, int meret);

Eredmeny *EredmenyekBeolvas ();

Eredmeny *EredmenyBeszuras (int Pontszam, char *Nev, Eredmeny *Elso);

void EredmenyekFelszabaditasa (Eredmeny *Elso);

void EredmenyKiiras(Eredmeny *Elso, int PalyaX, int PalyaY, int FeliratY, SDL_Renderer* renderer, TTF_Font* font);

Mezo **Beolvasas (int KarKoord[][2], int *DBPont, int PalyaX, int FeliratY, int dbSor, SDL_Renderer* renderer, int Arany);
#endif
