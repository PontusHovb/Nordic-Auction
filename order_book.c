#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Order book
typedef struct {
    double price;
    int sell_orders;
    int buy_orders;
} OrderBook;

// Function to resize the order book array
OrderBook** resize_order_book(OrderBook** order_book, int* size) {
    *size *= 2;
    order_book = (OrderBook**)realloc(order_book, (*size) * sizeof(OrderBook*));
    return order_book;
}

// Function to compare two orders by price
int compare_orders(const void* a, const void* b) {
    OrderBook* orderA = *(OrderBook**)a;
    OrderBook* orderB = *(OrderBook**)b;
    if (orderA->price < orderB->price) return -1;
    if (orderA->price > orderB->price) return 1;
    return 0;
}

// Function to plot order book using SDL2
void plot_order_book(OrderBook** order_book, SDL_Renderer* ren, TTF_Font* font) {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    int width, height;
    SDL_GetWindowSize(SDL_GetWindowFromID(1), &width, &height);

    int bar_height = 20;
    int bar_spacing = 10;
    int y_offset = 50;

    // Plot buy orders
    SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
    for (int i = 0; order_book[i] != NULL; i++) {
        if (order_book[i]->buy_orders > 0) {
            int y = y_offset + i * (bar_height + bar_spacing);
            int bar_width = (int)((order_book[i]->buy_orders / 100.0) * (width / 2));
            SDL_Rect rect = { width / 2 - bar_width, y, bar_width, bar_height };
            SDL_RenderFillRect(ren, &rect);
        }
    }

    // Plot sell orders
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    for (int i = 0; order_book[i] != NULL; i++) {
        if (order_book[i]->sell_orders > 0) {
            int y = y_offset + i * (bar_height + bar_spacing);
            int bar_width = (int)((order_book[i]->sell_orders / 100.0) * (width / 2));
            SDL_Rect rect = { width / 2, y, bar_width, bar_height };
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

// Receive order and add to current order book
void input_order(OrderBook*** order_book, int* size, char direction, double price, int quantity, SDL_Renderer* ren, TTF_Font* font) {
    for (int i = 0; (*order_book)[i] != NULL; i++) {
        if ((*order_book)[i]->price == price) {
            // Add order
            if (direction == 'B') {
                (*order_book)[i]->buy_orders += quantity;
            } else if (direction == 'S') {
                (*order_book)[i]->sell_orders += quantity;
            }
            plot_order_book(*order_book, ren, font);
            return;
        }
    }
    // Create new price if not in order book
    int i;
    for (i = 0; (*order_book)[i] != NULL; i++);
    if (i >= *size - 1) {
        *order_book = resize_order_book(*order_book, size);
    }
    (*order_book)[i] = (OrderBook*)malloc(sizeof(OrderBook));
    (*order_book)[i]->price = price;
    if (direction == 'B') {
        (*order_book)[i]->buy_orders = quantity;
        (*order_book)[i]->sell_orders = 0;
    } else if (direction == 'S') {
        (*order_book)[i]->sell_orders = quantity;
        (*order_book)[i]->buy_orders = 0;
    }
    (*order_book)[i + 1] = NULL;
    plot_order_book(*order_book, ren, font);
}

// Add a main function
int main() {
    // Initialize order book
    int size = 10;
    OrderBook** order_book = (OrderBook**)malloc(size * sizeof(OrderBook*));
    order_book[0] = NULL;

    // Seed random number generator
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Order Book", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        SDL_DestroyWindow(win);
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Update the font path to a valid TTF font file on your system
    TTF_Font* font = TTF_OpenFont("/System/Library/Fonts/SFNS.ttf", 16);
    if (font == NULL) {
        fprintf(stderr, "TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Randomly add buy orders in the price interval 98-103
    for (double price = 98.0; price <= 103.0; price += 1) {
        int quantity = rand() % 100 + 1; // Random quantity between 1 and 100
        input_order(&order_book, &size, 'B', price, quantity, ren, font);
    }

    // Randomly add sell orders in the price interval 102-107
    for (double price = 102.0; price <= 107.0; price += 1) {
        int quantity = rand() % 100 + 1; // Random quantity between 1 and 100
        input_order(&order_book, &size, 'S', price, quantity, ren, font);
    }

    // Sort order book by price
    int num_orders = 0;
    while (order_book[num_orders] != NULL) num_orders++;
    qsort(order_book, num_orders, sizeof(OrderBook*), compare_orders);

    // Plot order book
    plot_order_book(order_book, ren, font);

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();

    // Free allocated memory
    for (int i = 0; order_book[i] != NULL; i++) {
        free(order_book[i]);
    }
    free(order_book);

    return 0;
}
