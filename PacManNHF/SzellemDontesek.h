#ifndef SZELLEMDONTESEK_H_INCLUDED
#define SZELLEMDONTESEK_H_INCLUDED

typedef enum Irany { fel, bal, le, jobb} Irany;
typedef enum Mezo { PacMan, elsoSzellem, masodikSzellem, harmadikSzellem, negyedikSzellem, pont, ures, fal} Mezo;

double Tavolsag (int X, int Y, int CelX, int CelY);

/*
 *Az első szellem útkeresése, és a többiében is használom.
 *Minimum kiválasztás a lehetséges és valid lépések közül.
 *Az első szellem célja Pac Man jelenlegi helye.
*/
Irany FordulasNelkul (int X, int Y, int CelX, int CelY, Irany HonnanJott, const Mezo **palya, int dbSor);


/*
 *A második szellem útkeresése.
 *A második szellem célja a Pac Man előtti negyedik négyzet.
 *(ha az nem pályán kívüli vagy fal, ebben az esetben közelebbi négyzetet néz amíg nem egy elérhető terület, legrosszabb esetben ő is ajátékos helyét keresi)
*/
Irany MasodikSzellem (int X, int Y, int PacmanX, int PacmanY, Irany HonnanJott, Irany PacmanIranya, const Mezo **palya, int dbSor);

/*
 *A harmadik szellem útkeresése.
 *A harmadik szellem célja az első szellemből Pac Man előtti második négyzetbe húzott vektor kétszeresének a helye.
 *(vagy ahhoz a legközelebbi pályán belüli négyzet)
*/
Irany HarmadikSzellem (int X, int Y, int PacmanX, int PacmanY, Irany HonnanJott, Irany PacmanIranya, int ElsoSzellemX, int ElsoSzellemY, const Mezo **palya, int dbSor);

/*
 *A negyedik szellem útkeresése.
 *A negyedik szellem ha 8 négyzethossznál közelebb van Pac Man-hez akkor a pálya bal alsó négyzetét, ha távolabb akkor Pac Man jelenlegi helyét célozza.
*/
Irany NegyedikSzellem (int X, int Y, int PacmanX, int PacmanY, Irany HonnanJott, const Mezo **palya, int dbSor);

#endif
