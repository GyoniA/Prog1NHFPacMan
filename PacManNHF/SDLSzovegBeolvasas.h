#ifndef SDLSZOVEGBEOLVASAS_H_INCLUDED
#define SDLSZOVEGBEOLVASAS_H_INCLUDED
//forrás: https://infoc.eet.bme.hu/sdl/#7
void sdl_close(SDL_Window **pwindow, SDL_Renderer **prenderer, TTF_Font **pfont);

bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer);

#endif
