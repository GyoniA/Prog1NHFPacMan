#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "SzellemDontesek.h"
#include "InputOutput.h"
#include "debugmalloc.h"

//A megadott irányba változtatja a megadott koordinátákat
void MozgasIranyba (Irany Ibe, int *X, int *Y, const Mezo **palya) {
    int Lehetosegek[4][2] = {{*X, *Y - 1}, {*X - 1, *Y}, {*X, *Y + 1}, {*X + 1, *Y}}; //iránytól függően koordináták
    if (palya[Lehetosegek[Ibe][1]][Lehetosegek[Ibe][0]] != fal) {
        *X = Lehetosegek[Ibe][0];
        *Y = Lehetosegek[Ibe][1];
    }
}

/* Kirajzolja a megadott szellemet a megadott koordinátákra.
*/
void SzellemKirajzolas(SDL_Renderer *renderer, SDL_Texture *PacmanTexturak, int X, int Y, int Index, int Arany, int FeliratY) {

    Index++; //Mert az elején van a pacman másik állapota.
    //A forrás képből ezekről a koordinátákról, ilyen méretű részletet másol.
    SDL_Rect src = {Index*32+1, 1, 30, 30};
    //A cél képre, ezekre a koordinátákra másol.
    SDL_Rect dest = {Arany*X+1, Arany*Y+FeliratY+1, Arany-2, Arany-2};
    //Képrészlet másolása.
    SDL_RenderCopy(renderer, PacmanTexturak, &src, &dest);
}

/* Kirajzolja PacMan-t a megadott koordinátákra, a megadott állapotban.
*/
void PacManKirajzolas(SDL_Renderer *renderer, SDL_Texture *PacmanTexturak, int X, int Y, bool NyitvaE, Irany Merre, int Arany, int FeliratY) {
    if (NyitvaE) {
        //A forrás képből ezekről a koordinátákról, ilyen méretű részletet másolunk.
        SDL_Rect src = {32+1, 1, 30, 30};

        int TempArany = (Arany%2 == 0) ? Arany : Arany-1; //akkor kell, ha páratlan lenne, mert akkor rosszul forgatja az SDL, ezért 1-el kisebbet rajzolok
        //A cél képre, ezekre a koordinátákra másoljuk.
        SDL_Rect dest = {Arany*X+1, Arany*Y+FeliratY+1, TempArany-2, TempArany-2};

        //Képrészlet másolása forgatással.
        SDL_RenderCopyEx(renderer, PacmanTexturak, &src, &dest, 360+(-90*(Merre-1)), NULL, SDL_FLIP_NONE);
    }
    else {
        //A forrás képből ezekről a koordinátákról, ilyen méretű részletet másolunk.
        SDL_Rect src = {1, 1, 30, 30};
        //A cél képre, ezekre a koordinátákra másoljuk.
        SDL_Rect dest = {Arany*X+1, Arany*Y+FeliratY+1, Arany-2, Arany-2};
        //Képrészlet másolása.
        SDL_RenderCopy(renderer, PacmanTexturak, &src, &dest);
    }
}

/*Eltunteti a megadott karaktert (Pacman[0], Szellemek[1-4])
 *a megadott koordinátákrol es a helyere vagy ures helyet vagy pontot rajzol.
*/
void Eltuntetes (int x, int y, const Mezo **palya, SDL_Renderer *renderer, int Arany, int FeliratY) {
    boxRGBA(renderer, Arany*x+Arany-1, Arany*y+FeliratY+1, Arany*x+1, Arany*y+FeliratY+Arany-1, 0, 0, 0, 255);
    if(palya[y][x] == pont) {
        boxRGBA(renderer, Arany*x+Arany*0.55, Arany*y+FeliratY+Arany*0.45, Arany*x+Arany*0.45, Arany*y+FeliratY+Arany*0.55, 255, 255, 0, 255);
    }
}

Uint32 Idozites(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

//A játék menetét irányító ciklus függvénye, a végén van memória felszabadítás is.
void PacManLoop (TTF_Font *font, SDL_Renderer *renderer, int *Score, bool *NyertE, const int FeliratY, const int PalyaX, const int PalyaY) {
    int dbSor;      //a sorok és oszlopok száma (négyzetes pálya lehet csak)
    int Arany;      //a bemenet és pálya aránya (egész számban az sdl miatt)

    Irany KarIrany[5]; // Pacman[0] és a szellemek[1-4] jelenlegi iránya.
    for (int i = 0; i < 5; ++i) {
        KarIrany[i] = 0;
    }

    int KarKoord[5][2]; // Pacman[0] és a szellemek[1-4] jelenlegi x és y koordinátája.
    Mezo **palya; // a pálya felépítését tároló változó

    int dbPont;
    palya = Beolvasas(KarKoord, &dbPont, PalyaX, FeliratY, &dbSor, renderer, &Arany); // Hány darab pont van a pályán összesen és a pálya beolvasása.

    SDL_TimerID Timer = SDL_AddTimer(16.6666, Idozites, NULL);
    SDL_Texture *PacmanTexturak = IMG_LoadTexture(renderer, "PacmanTexturak.png");
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
    Mix_Chunk *PacManHang = Mix_LoadWAV("PacManHang.wav");
    int TimePoint = 100000;
    int dbCiklus = 0;
    bool NyitvaE = false; //PacMan szája nyitva van-e
    bool VegeE = false;
	while (!VegeE) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        //input kezelés
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    //Megnézi, hogy melyik gomb lett lenyomva, aztán ellenőrzi, hogy abba az irányba mehet-e PacMan. (A pálya szélén fal van)
                    case SDLK_w: (KarIrany[0] == 2 || palya[KarKoord[0][1] - 1][KarKoord[0][0]] == fal) ? (KarIrany[0] = KarIrany[0]) : (KarIrany[0] = 0); break;
                    case SDLK_a: (KarIrany[0] == 3 || palya[KarKoord[0][1]][KarKoord[0][0] - 1] == fal) ? (KarIrany[0] = KarIrany[0]) : (KarIrany[0] = 1); break;
                    case SDLK_s: (KarIrany[0] == 0 || palya[KarKoord[0][1] + 1][KarKoord[0][0]] == fal) ? (KarIrany[0] = KarIrany[0]) : (KarIrany[0] = 2); break;
                    case SDLK_d: (KarIrany[0] == 1 || palya[KarKoord[0][1]][KarKoord[0][0] + 1] == fal) ? (KarIrany[0] = KarIrany[0]) : (KarIrany[0] = 3); break;
                }
                break;
        case SDL_USEREVENT:
            if (dbCiklus % 10 == 0)
            {
                Eltuntetes(KarKoord[0][0], KarKoord[0][1], palya, renderer, Arany, FeliratY);
                //PacMan mozgása
                MozgasIranyba(KarIrany[0], &KarKoord[0][0], &KarKoord[0][1], palya);
                for (int i = 1; i < 5; ++i) //Elenőrzi, hogy PacMan egy mezőben van-e egy szellemmel.
                {
                    if (KarKoord[0][0] == KarKoord[i][0] && KarKoord[0][1] == KarKoord[i][1]) {
                        VegeE = true;
                    }
                }
                if (palya[KarKoord[0][1]] [KarKoord[0][0]] == pont) //Elenőrzi, hogy PacMan egy mezőben van-e egy ponttal.
                {
                    palya[KarKoord[0][1]] [KarKoord[0][0]] = ures;
                    *Score += 100;
                    dbPont--;
                    if (dbPont < 1) //Ha nincs több pont akkor vége a játéknak és PacMan nyert.
                    {
                        VegeE = true;
                        *NyertE = true;
                        *Score += 10000;
                        if (TimePoint > 0) {
                            *Score += TimePoint;
                        }
                    }
                }
                PacManKirajzolas(renderer, PacmanTexturak, KarKoord[0][0], KarKoord[0][1], NyitvaE, KarIrany[0], Arany, FeliratY);
                NyitvaE = !NyitvaE; //Változik, hogy PacMan szája nyitva vagy csukva van.
            }
            if (dbCiklus % 20 == 0)
            {
                dbCiklus = 0;
                TimePoint -= 100;
                Mix_PlayChannel( -1, PacManHang, 0 );
                for (int i = 1; i < 5; ++i) {
                    Eltuntetes(KarKoord[i][0], KarKoord[i][1], palya, renderer, Arany, FeliratY);
                }
                //szellemek mozgása
                KarIrany[1] = FordulasNelkul(KarKoord[1][0], KarKoord[1][1], KarKoord[0][0],
                                             KarKoord[0][1], KarIrany[1], palya, dbSor);
                MozgasIranyba(KarIrany[1], &KarKoord[1][0], &KarKoord[1][1], palya);
                KarIrany[2] = MasodikSzellem(KarKoord[2][0], KarKoord[2][1], KarKoord[0][0],
                                             KarKoord[0][1], KarIrany[2], KarIrany[0],
                                             palya, dbSor);
                MozgasIranyba(KarIrany[2], &KarKoord[2][0], &KarKoord[2][1], palya);
                KarIrany[3] = HarmadikSzellem(KarKoord[3][0], KarKoord[3][1], KarKoord[0][0],
                                              KarKoord[0][1], KarIrany[3], KarIrany[0],
                                              KarKoord[1][0], KarKoord[1][1], palya, dbSor);
                MozgasIranyba(KarIrany[3], &KarKoord[3][0], &KarKoord[3][1], palya);
                KarIrany[4] = NegyedikSzellem(KarKoord[4][0], KarKoord[4][1], KarKoord[0][0],
                                              KarKoord[0][1], KarIrany[4], palya, dbSor);
                MozgasIranyba(KarIrany[4], &KarKoord[4][0], &KarKoord[4][1], palya);

                for (int i = 1; i < 5; ++i) //Elenőrzi, hogy PacMan egy mezőben van-e egy szellemmel.
                {
                    if (KarKoord[0][0] == KarKoord[i][0] && KarKoord[0][1] == KarKoord[i][1]) {
                        VegeE = true;
                    }
                }
                for (int i = 1; i < 5; ++i)
                {
                    SzellemKirajzolas(renderer, PacmanTexturak, KarKoord[i][0], KarKoord[i][1], i, Arany, FeliratY);
                }
            }
            dbCiklus++;

            char pontok[50] = "";
            sprintf(pontok, "SCORE: %d", *Score);
            boxRGBA (renderer, PalyaX, 0, 0, FeliratY, 0, 0, 0, 255);
            KepernyoreIras(font, pontok, (SDL_Color){255, 255, 255}, renderer, 0, 0, 16); //Kiírja a képernyő bal felső sarkába a jelenlegi pontszámot.

            SDL_RenderPresent(renderer);
            break;
            case SDL_QUIT:
                VegeE = true;
                break;
        }
    }

    //A lefoglalt memóriaterületek felszabadítása.
    SDL_RenderClear(renderer);
    SDL_DestroyTexture(PacmanTexturak);
    Mix_FreeChunk(PacManHang);
    SDL_RemoveTimer(Timer);
    for (int i = 0; i < dbSor; ++i) {
        free(palya[i]);
    }
    free(palya);
}
