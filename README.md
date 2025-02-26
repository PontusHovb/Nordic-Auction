# Nordic Auction
A Nordic auction is the auction model applied for opening and closing calls for cash equities traded on Nasdaq Nordic. For these auctions, orders are not auto-matched, instead they are added to the order book during the order management and then executed at the equilibrium price (EP) at the time of the uncross.

1. **Order Collection**: 
    - Participants submit their buy and sell orders to the auction system. Each order specifies the quantity and price at which the participant is willing to buy or sell the security.
    - Orders can be submitted at any time before the auction starts.
    - The auction system maintains an order book that lists all the buy and sell orders.
<p align="center">
    <img src="https://github.com/PontusHovb/Nordic-Auction/blob/main/Plots/Simulated-Order-Book.gif" width="600"/>
</p>
<p align="center"><i>Auction Order Book</i></p>

2. **Price Determination**:
   - The auction system determines the auction price by finding the price at which the paired quantity is maximized
   - This price is known as the equilibrium price or clearing price.

3. **Order Matching**:
   - Orders are matched based on the equilibrium price.
   - Buy orders with prices equal to or higher than the equilibrium price are matched with sell orders with prices equal to or lower than the equilibrium price.
   - If there are more buy orders than sell orders at the equilibrium price, the sell orders are matched first, and the remaining buy orders are partially filled or not filled at all.
   - Similarly, if there are more sell orders than buy orders at the equilibrium price, the buy orders are matched first, and the remaining sell orders are partially filled or not filled at all.

## Simulation
This project includes a simulation of a Nordic auction using SDL2 for graphical representation. The simulation generates buy and sell orders according to a normal distribution and updates the order book in real-time.

To run the simulation, compile the code using the provided Makefile and execute the generated binary.

```sh
make
./order_book
```

## Dependencies
- SDL2
- SDL2_ttf

Ensure that these libraries are installed on your system before compiling the code.

## Source:
https://www.nasdaq.com/docs/2269-Q19%20INET%20Auction%20FS_A4_0118.pdf