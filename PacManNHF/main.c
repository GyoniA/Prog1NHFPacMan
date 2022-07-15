#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "debugmalloc.h"
#include "SzellemDontesek.h"
#include "InputOutput.h"
#include "SDLSzovegBeolvasas.h"
#include "JatekLoop.h"

void sdl_init(char const *felirat, int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer, TTF_Font **pfont) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("consola.ttf", 32);
    if (font == NULL) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    *pwindow = window;
    *prenderer = renderer;
    *pfont = font;
}

int main(int argc, char *argv[]) {
    SDL_Renderer *renderer;
    SDL_Window *window;
    TTF_Font *font;

    const int FeliratY = 16;       //a felső sáv magassága (szélessége egyenlő a pálya szélességével) (ide kerül kiírásra a játékos jelenlegi pontszáma)
    const int PalyaX = 1024;       //a pálya szélessége a kijelzőn
    const int PalyaY = 1024;       //a pálya magassága a kijelzőn

    sdl_init("PacMan", PalyaX, PalyaY+FeliratY, &window, &renderer, &font);

    int Score = 0;        //A játékos pontszáma.
    bool NyertE = false;  //Számon tartja, hogy miért lett vége a játéknak.
    PacManLoop(font, renderer, &Score, &NyertE, FeliratY, PalyaX, PalyaY);

    if (NyertE) {
        KepernyoreIras(font, "YOU WON", (SDL_Color){255, 255, 0}, renderer, PalyaX/2-300, (PalyaY+FeliratY)/2-415, 76);
    } else {
        KepernyoreIras(font, "YOU LOST", (SDL_Color){255, 0, 0}, renderer, PalyaX/2-300, (PalyaY+FeliratY)/2-415, 76);
    }

    KepernyoreIras(font, "ENTER YOUR NAME:", (SDL_Color){255, 255, 255}, renderer, PalyaX/2-400, (PalyaY+FeliratY)/2-158, 16);

    char pontok[50] = "";
    sprintf(pontok, "SCORE: %d", Score);
    KepernyoreIras(font, pontok, (SDL_Color){255, 255, 255}, renderer, PalyaX/2-400, (PalyaY+FeliratY)/2-126, 16);

    char JatekosNeve[50];
    SDL_Rect Teglalap = {PalyaX/2-172, (PalyaY+FeliratY)/2-162, 400, 24};
    input_text(JatekosNeve, 50, Teglalap, (SDL_Color){0, 0, 0},  (SDL_Color){255, 255, 255}, font, renderer); //Beolvassa a játékos nevét.

    Eredmeny *Elso = EredmenyekBeolvas();
    EredmenyBeszuras(Score, JatekosNeve, Elso); //Beszúrja a mostani játékos pontszámát és nevét a lista megfelelő helyére.
    EredmenyKiiras(Elso, PalyaX, PalyaY, FeliratY, renderer, font);
    SDL_RenderPresent(renderer);
    SDL_Event ev;
    while (SDL_WaitEvent(&ev) && ev.type != SDL_QUIT) {
    }

    //A lefoglalt memóriaterületek felszabadítása.
    EredmenyekFelszabaditasa(Elso);
    sdl_close(&window, &renderer, &font);

    return 0;
}
