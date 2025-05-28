#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include "Cell.h"
#include "Board.h"
#include "Button.h"

using namespace std;

typedef Cell::Content mineType;
typedef Cell::State cellState;

int main() {
    int rows = 10;    // Number of rows
    int cols = 10;    // Number of columns
    int mineCount = 13; // Number of mines
    int CELL_SIZE = 30; // Size of each cell
    bool showCheats = false; // Show mine locations
    Colors colors;
    Button button(10, rows * CELL_SIZE + 10, 100, 60, "Cheats");

    Board board(rows, cols, CELL_SIZE);

    // Create the Minesweeper board
    board.placeMines(mineCount);

    // Create an SFML window
    sf::RenderWindow window(sf::VideoMode(cols * CELL_SIZE * 2 + CELL_SIZE + 1, rows * CELL_SIZE + 80), "Minesweeper");

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle mouse click
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                board.handleLeftClick(mousePosition, &window);
                board.updateBoard(&window);

                if (button.isClicked(mousePosition)) {
                    showCheats = !showCheats;
                    board.setCheats(showCheats, &window);
                }
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                // Handle right-click
                board.handleRightClick(sf::Mouse::getPosition(window));
                board.updateBoard(&window);
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // Clear the screen
        window.clear(sf::Color::White);
        button.draw(window);

        // Update the board
        board.updateBoard(&window);
    }
    return 0;
}
