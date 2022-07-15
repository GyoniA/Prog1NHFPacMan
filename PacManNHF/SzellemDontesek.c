#include <math.h>
#include "debugmalloc.h"

typedef enum Irany { fel, bal, le, jobb} Irany;
typedef enum Mezo { PacMan, elsoSzellem, masodikSzellem, harmadikSzellem, negyedikSzellem, pont, ures, fal} Mezo;

double Tavolsag (int X, int Y, int CelX, int CelY) {
    return sqrt(pow(X-CelX, 2) + pow(Y-CelY, 2));
}

/*
 *Az első szellem útkeresése, és a többiében is használom.
 *Minimum kiválasztás a lehetséges és valid lépések közül.
 *Az első szellem célja Pac Man jelenlegi helye.
*/
Irany FordulasNelkul (int X, int Y, int CelX, int CelY, Irany HonnanJott, const Mezo **palya, int dbSor) {
    double Tavolsagok[4];
    if (HonnanJott + 2 > 3) { //mert így lesz meg a tényleges előző pozíció
        HonnanJott -= 2;
    } else {
        HonnanJott += 2;
    }
    int Lehetosegek[4][2] = {{X, Y - 1}, {X - 1, Y}, {X, Y + 1}, {X + 1, Y}}; //iránytól függően koordináták

    for (int i = 0; i < 4; ++i) {
        if(i == HonnanJott || Lehetosegek[i][0] > dbSor-1 || Lehetosegek[i][0] < 0 || Lehetosegek[i][1] > dbSor-1 || Lehetosegek[i][1] < 0 || palya[Lehetosegek[i][1]][Lehetosegek[i][0]] == fal) {
            Tavolsagok[i] = dbSor*2; // mert ez nagyobb lesz mint bármilyen távolság mert dbSor x dbSor a pálya, ez azért kell, hogy ne forduljon meg és ne menjen ki a pályáról
        }
        else {
            Tavolsagok[i] = Tavolsag(Lehetosegek[i][0], Lehetosegek[i][1], CelX, CelY);
        }
    }

    int minIndex = 0;
    for (int i = 1; i < 4; ++i) {
        if (Tavolsagok[i] < Tavolsagok[minIndex]) {
            minIndex = i;
        }
    }
    return minIndex;
}

/*
 *A második szellem útkeresése.
 *A második szellem célja a Pac Man előtti negyedik négyzet.
 *(ha az nem pályán kívüli vagy fal, ebben az esetben közelebbi négyzetet néz amíg nem egy elérhető terület, legrosszabb esetben ő is ajátékos helyét keresi)
*/
Irany MasodikSzellem (int X, int Y, int PacmanX, int PacmanY, Irany HonnanJott, Irany PacmanIranya, const Mezo **palya, int dbSor) {
    int Lehetosegek[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}}; //iránytól függően koordináta változások
    PacmanX += 4 * Lehetosegek[PacmanIranya][0];
    PacmanY += 4 * Lehetosegek[PacmanIranya][1];
    while (PacmanX > dbSor-1 || PacmanX < 0 || PacmanY > dbSor-1 || PacmanY < 0 || palya[PacmanY][PacmanX] == fal) {
        PacmanX -= Lehetosegek[PacmanIranya][0];
        PacmanY -= Lehetosegek[PacmanIranya][1];

    }
    return FordulasNelkul(X, Y, PacmanX, PacmanY, HonnanJott, palya, dbSor);
}

/*
 *A harmadik szellem útkeresése.
 *A harmadik szellem célja az első szellemből Pac Man előtti második négyzetbe húzott vektor kétszeresének a helye.
 *(vagy ahhoz a legközelebbi pályán belüli négyzet)
*/
Irany HarmadikSzellem (int X, int Y, int PacmanX, int PacmanY, Irany HonnanJott, Irany PacmanIranya, int ElsoSzellemX, int ElsoSzellemY, const Mezo **palya, int dbSor) {
    int Lehetosegek[4][2] = {{0, -2}, {-2, 0}, {0, 2}, {2, 0}};
    PacmanX = abs(PacmanX + Lehetosegek[PacmanIranya][0] - ElsoSzellemX) * 2; //az első szellemből Pac Man előtti második négyzetbe húzott vektor kétszeresének x koordinátája
    PacmanY = abs(PacmanY + Lehetosegek[PacmanIranya][1] - ElsoSzellemY) * 2; //az első szellemből Pac Man előtti második négyzetbe húzott vektor kétszeresének y koordinátája
    return FordulasNelkul(X, Y, PacmanX, PacmanY, HonnanJott, palya, dbSor);
}

/*
 *A negyedik szellem útkeresése.
 *A negyedik szellem ha 8 négyzethossznál közelebb van Pac Man-hez akkor a pálya bal alsó négyzetét, ha távolabb akkor Pac Man jelenlegi helyét célozza.
*/
Irany NegyedikSzellem (int X, int Y, int PacmanX, int PacmanY, Irany HonnanJott, const Mezo **palya, int dbSor) {
    if (Tavolsag(X, Y, PacmanX, PacmanY) < 8) {
        return FordulasNelkul(X, Y, 0, dbSor-1, HonnanJott, palya, dbSor);
    }
    return FordulasNelkul(X, Y, PacmanX, PacmanY, HonnanJott, palya, dbSor);

}
