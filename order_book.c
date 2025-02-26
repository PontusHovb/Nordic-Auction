#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "GUI.h"

// Resize order book array
OrderBook** resize_order_book(OrderBook** order_book, int* size) {
    *size *= 2;
    order_book = (OrderBook**)realloc(order_book, (*size) * sizeof(OrderBook*));
    return order_book;
}

// Compare orders, used for sorting
int compare_orders(const void* a, const void* b) {
    OrderBook* orderA = *(OrderBook**)a;
    OrderBook* orderB = *(OrderBook**)b;
    if (orderA->price < orderB->price) return -1;
    if (orderA->price > orderB->price) return 1;
    return 0;
}

// Function to generate a random number following a normal distribution
double generate_normal(double mean, double stddev) {
    static int has_spare = 0;
    static double spare;
    if (has_spare) {
        has_spare = 0;
        return mean + stddev * spare;
    }

    has_spare = 1;
    static double u, v, s;
    do {
        u = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
        v = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);

    s = sqrt(-2.0 * log(s) / s);
    spare = v * s;
    return mean + stddev * u * s;
}

// Receive order and add to current order book
void input_order(OrderBook*** order_book, int* size, char direction, double price, int quantity) {
    price = round(price); // Round price to the nearest integer
    for (int i = 0; (*order_book)[i] != NULL; i++) {
        if ((*order_book)[i]->price == price) {
            // Add order
            if (direction == 'B') {
                (*order_book)[i]->buy_orders += quantity;
            } else if (direction == 'S') {
                (*order_book)[i]->sell_orders += quantity;
            }
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
}

// Function to remove an order from the market
void remove_order(OrderBook*** order_book, char direction, double price, int quantity) {
    price = round(price); // Round price to the nearest integer
    for (int i = 0; (*order_book)[i] != NULL; i++) {
        if ((*order_book)[i]->price == price) {
            // Remove order
            if (direction == 'B') {
                (*order_book)[i]->buy_orders -= quantity;
                if ((*order_book)[i]->buy_orders < 0) {
                    (*order_book)[i]->buy_orders = 0;
                }
            } else if (direction == 'S') {
                (*order_book)[i]->sell_orders -= quantity;
                if ((*order_book)[i]->sell_orders < 0) {
                    (*order_book)[i]->sell_orders = 0;
                }
            }
            return;
        }
    }
}

// Add a main function
int main() {
    // Initialize order book
    int size = 20;
    OrderBook** order_book = (OrderBook**)malloc(size * sizeof(OrderBook*));
    for (int i = 0; i < size; i++) {
        order_book[i] = NULL;
    }

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

    // Initialize order book with prices from 95 to 105 with 0 buy/sell orders
    for (double price = 95.0; price <= 105.0; price += 1.0) {
        input_order(&order_book, &size, 'B', price, 0);
    }

    // Create 1,000 orders before displaying the plot
    for (int i = 0; i < 1000; i++) {
        char direction = (rand() % 2 == 0) ? 'B' : 'S';
        double mean = (direction == 'B') ? 99.0 : 101.0;
        double price = generate_normal(mean, 2.0);
        if (price < 95.0) price = 95.0;
        if (price > 105.0) price = 105.0;
        int quantity = rand() % 100 + 1; // Random quantity between 1 and 100
        input_order(&order_book, &size, direction, price, quantity);
    }

    // Resize window height to fit columns
    int num_prices = 11;
    int new_height = 50 + num_prices * (20 + 10) + 50; // y_offset + num_prices * (bar_height + bar_spacing) + bottom margin
    SDL_SetWindowSize(win, 800, new_height);

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

        // Simulate the arrival of new orders
        char direction = (rand() % 2 == 0) ? 'B' : 'S';
        char add_remove = (rand() %2 == 0) ? 'A' : 'R';
        double mean = (direction == 'B') ? 99.0 : 101.0;
        double price = generate_normal(mean, 1.0);
        int quantity = rand() % 100 + 1; // Random quantity between 1 and 100
        if (direction == 'B') {
            if (price < 96.0) price = 96.0;
            if (price > 103.0) price = 103.0;
        } else {
            if (price < 98.0) price = 98.0;
            if (price > 104.0) price = 104.0;
        }
        if (add_remove == 'A') {
            input_order(&order_book, &size, direction, price, quantity);
        }
        else {
            remove_order(&order_book, direction, price, quantity);
        }

        plot_order_book(order_book, ren, font);
        SDL_Delay(10); // Delay to simulate time between orders
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
