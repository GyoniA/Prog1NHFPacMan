#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include "debugmalloc.h"

typedef enum Mezo { PacMan, elsoSzellem, masodikSzellem, harmadikSzellem, negyedikSzellem, pont, ures, fal} Mezo;

/*Egy duplán láncolt lista, ami eltárolja a játékosok nevét és pontszámát.
 *A bemeneti fájl pont szerint csökkenő sorrendben van és úgy is írja ki a program a végén.
*/
typedef struct Eredmeny {
    int Pontszam;
    char *Nev;
    struct Eredmeny *elozo;
    struct Eredmeny *kovetkezo;
} Eredmeny;

//Kiírja a képernyőre a megadott szöveget a megadott fonttal, színnel, koordinátákkal és mérettel.
void KepernyoreIras(TTF_Font* font, const char* text, SDL_Color Szin, SDL_Renderer* renderer, int X, int Y, int meret) {
    SDL_Surface *szoveg = TTF_RenderUTF8_Blended(font, text, Szin);
    int szeles = (meret-2)*strlen(text);
    int magas = meret;
    SDL_Texture *szoveg_t = SDL_CreateTextureFromSurface(renderer, szoveg);
    SDL_Rect cel = { X, Y, szeles, magas};
    SDL_RenderCopy(renderer, szoveg_t, NULL, &cel);
    SDL_FreeSurface(szoveg);
    SDL_DestroyTexture(szoveg_t);
}

/*Beszúr egy új elemet a megadott kezdőelemű listába, a megadott névvel és pontszámmal,
 *úgy hogy az új elem pontszám szerint jó helyen legyen.
*/
Eredmeny *EredmenyBeszuras (int Pontszam, char *Nev, Eredmeny *Elso) {
    Eredmeny *Ideiglenes;
    Ideiglenes = (Eredmeny*) malloc(sizeof(Eredmeny));

    Ideiglenes->Pontszam = Pontszam;
    Ideiglenes->Nev = (char*)malloc(sizeof(char)*(strlen(Nev)+1));
    strcpy(Ideiglenes->Nev, Nev);

    Ideiglenes->elozo = NULL;
    Ideiglenes->kovetkezo = NULL;
    if (Elso == NULL) {
        return Ideiglenes;
    }
    else {
        Eredmeny *i = Elso;
        while (i->kovetkezo != NULL && i->Pontszam > Pontszam) {
            i = i->kovetkezo;
        }
        if (i->Pontszam > Pontszam) {
            i->kovetkezo = Ideiglenes;
            Ideiglenes->elozo = i;
        }
        else {
            Ideiglenes->elozo = i->elozo;
            if (i->elozo != NULL) {
                i->elozo->kovetkezo = Ideiglenes;
            }
            Ideiglenes->kovetkezo = i;
            i->elozo = Ideiglenes;
        }
    }
   return Elso;
}

//Felszabadítja a megadott kezdőelemű listát, az összes elemét és az összes elemének a dinamikusan foglalt nevét.
void EredmenyekFelszabaditasa (Eredmeny *Elso) {
    Eredmeny *i = Elso;
    while (i != NULL) {
        Eredmeny *kovetkezo = i->kovetkezo;
        free(i->Nev);
        free(i);
        i = kovetkezo;
    }
}

/*Fájlból beolvassa a legjobbb eredményeket, ezeket belerakja egy duplán láncolt listába
 *(az EredmenyBeszuras függvénnyel) és visszatér az első elemre mutató pointerrel.
*/
Eredmeny *EredmenyekBeolvas () {
    FILE* fp;
    fp = fopen("LegjobbEredmenyek.txt", "r");

    Eredmeny *Elso = NULL;
    int Pontszam;
    char Nev[50];
    bool VegeE = false;
    while (!VegeE) {
        fscanf(fp, "%s %d", Nev, &Pontszam);
        if (Pontszam != -1) {
            Elso = EredmenyBeszuras(Pontszam, Nev, Elso);
        }
        else {
            VegeE = true;
        }
    }
    fclose(fp);
    return Elso;
}

//Kiírja a képernyőre a legjobb 10 eredményt és kiírja fájlba az összes eredményt. ( fájlba lezáró -1 pontú elemmel)
void EredmenyKiiras(Eredmeny *Elso, int PalyaX, int PalyaY, int FeliratY, SDL_Renderer* renderer, TTF_Font* font) {
    FILE* fp;
    fp = fopen("LegjobbEredmenyek.txt", "w");
    Eredmeny *i = Elso;
    int X = PalyaX/2;
    int Y = (PalyaY+FeliratY)/2-126;
    KepernyoreIras(font, "BEST GAMERS", (SDL_Color){255, 255, 0}, renderer, X, Y, 16);
    KepernyoreIras(font, "POINTS", (SDL_Color){255, 255, 0}, renderer, X+200, Y, 16);
    Y += 40;
    int Helyezes = 1;
    while (i != NULL) {
        fprintf(fp, "%s %d\n", i->Nev, i->Pontszam);
        if (Helyezes < 11) {
            KepernyoreIras(font, i->Nev, (SDL_Color){255, 255, 255}, renderer, X, Y, 16);
            char temp[12];
            sprintf(temp,"%d", i->Pontszam);
            KepernyoreIras(font, temp, (SDL_Color){255, 255, 255}, renderer, X+200, Y, 16);
            Y += 20;
        }

        Helyezes++;
        i = i->kovetkezo;
    }
    fprintf(fp, "%s %d\n", "asd", -1);

    fclose(fp);
}

/*Beolvassa fájlból a pályát, kirajzolja a falakat és a pontokat,
 *eltárolja a pályán lévő pontok darabszámát a DBPont helyre, és a visszatérési értéke a pálya felépítése.
*/
Mezo **Beolvasas (int KarKoord[][2], int *DBPont, int PalyaX, int FeliratY, int *dbSorTemp, SDL_Renderer* renderer, int *AranyTemp) {
    FILE* fp;
    fp = fopen("Palya.txt", "r");
    //fp = fopen("Palya2.txt", "r");
    //fp = fopen("Palya3.txt", "r");
    int dbSor;
    fscanf(fp, "%d", &dbSor); //a palya sorainak a számának beolvasása, ami megegyezik az oszlopainak számával is
    fgetc(fp);
    int Arany = PalyaX / dbSor;
    *dbSorTemp = dbSor;
    *AranyTemp = Arany;
    Mezo **palya; //a program vissztérési értéke lesz, ez tárolja el a pályát
    char **bemenet; //ideiglenes változó, mielőtt átváltja a program Mezo formátumra
    palya = (Mezo**)malloc(sizeof(Mezo*)*dbSor);
    bemenet = (char**) malloc(sizeof(char*)*dbSor);
    for (int i = 0; i < dbSor; ++i) {
        palya[i] = (Mezo*)malloc(sizeof(Mezo)*dbSor);
        bemenet[i] = (char*) malloc(sizeof(char)*dbSor+1);
    }

    if (fp != NULL)
    {
        for (int i = 0; i < dbSor; ++i) {
            fgets(bemenet[i], dbSor+1, (FILE*)fp);
            fgetc(fp);
        }
    }
    else
    {
        perror("Nem sikerult megnyitni a fajlt");
    }

    fclose(fp);

    for (int i = 0; i < dbSor; ++i) {
        for (int j = 0; j < dbSor; ++j) {
            switch (bemenet[i][j]) {
                case '#':
                    palya[i][j] = fal;
                    boxRGBA (renderer, Arany*j+Arany, Arany*i+FeliratY, Arany*j, Arany*i+FeliratY+Arany, 0, 0, 255, 255); //falak kirajzolása
                    break;
                case '0':
                    palya[i][j] = pont;
                    boxRGBA (renderer, Arany*j+Arany*0.55, Arany*i+FeliratY+Arany*0.45, Arany*j+Arany*0.45, Arany*i+FeliratY+Arany*0.55, 255, 255, 0, 255); //pontok kirajzolása
                    *DBPont += 1;
                    break;
                case 'P':
                    palya[i][j] = ures;
                    KarKoord[0][1] = i;
                    KarKoord[0][0] = j;
                    break;
                case ' ':
                    palya[i][j] = ures;
                    break;
                default:
                    KarKoord[bemenet[i][j] - '0'][1] = i;
                    KarKoord[bemenet[i][j] - '0'][0] = j;
                    palya[i][j] = ' ';
                    break;
            }
        }
    }
    for (int i = 0; i < dbSor; ++i) {
        free(bemenet[i]);
    }
    free(bemenet);
    return palya;
}
