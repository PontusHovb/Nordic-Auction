#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "GUI.h"

// Function to plot order book using SDL2
void plot_order_book(OrderBook** order_book, SDL_Renderer* ren, TTF_Font* font) {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    int width, height;
    SDL_GetWindowSize(SDL_GetWindowFromID(1), &width, &height);

    int bar_height = 20;
    int bar_spacing = 10;
    int y_offset = 50;
    int margin = 100;

    // Calculate the maximum quantity to scale the bars
    int max_quantity = 0;
    for (int i = 0; order_book[i] != NULL; i++) {
        if (order_book[i]->buy_orders > max_quantity) {
            max_quantity = order_book[i]->buy_orders;
        }
        if (order_book[i]->sell_orders > max_quantity) {
            max_quantity = order_book[i]->sell_orders;
        }
    }

    // Plot buy orders
    SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
    for (int i = 0; order_book[i] != NULL; i++) {
        if (order_book[i]->buy_orders > 0) {
            int y = y_offset + i * (bar_height + bar_spacing);
            int bar_width = (int)((order_book[i]->buy_orders / (double)max_quantity) * (((width - 100) / 2) - margin));
            SDL_Rect rect = { width / 2 - bar_width - margin, y, bar_width, bar_height };
            SDL_RenderFillRect(ren, &rect);
        }
    }

    // Plot sell orders
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    for (int i = 0; order_book[i] != NULL; i++) {
        if (order_book[i]->sell_orders > 0) {
            int y = y_offset + i * (bar_height + bar_spacing);
            int bar_width = (int)((order_book[i]->sell_orders / (double)max_quantity) * (((width - 100) / 2) - margin));
            SDL_Rect rect = { width / 2 + margin, y, bar_width, bar_height };
            SDL_RenderFillRect(ren, &rect);
        }
    }

    // Display prices
    SDL_Color textColor = { 255, 255, 255, 255 };
    for (int i = 0; order_book[i] != NULL; i++) {
        int y = y_offset + i * (bar_height + bar_spacing) + bar_height / 2;
        char price_text[20];
        snprintf(price_text, sizeof(price_text), "%.2f", order_book[i]->price);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, price_text, textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, textSurface);
        SDL_Rect dstrect = { width / 2 - 50, y - 15, 100, 30 };
        SDL_RenderCopy(ren, texture, NULL, &dstrect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(ren);
}
