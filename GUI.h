#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    double price;
    int sell_orders;
    int buy_orders;
} OrderBook;

void plot_order_book(OrderBook** order_book, SDL_Renderer* ren, TTF_Font* font);

#endif // GUI_H
