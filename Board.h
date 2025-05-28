#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Cell.h"

using namespace std;

typedef Cell::Content mineType;
typedef Cell::State cellState;

struct Colors {
    sf::Color emptyColor = sf::Color(100, 100, 100); // Dark gray empty cells
    sf::Color mineColor = sf::Color(255, 50, 50);   // Red for mines
    sf::Color safeColor = sf::Color(200, 200, 200); // Light gray safe cells
    sf::Color flagColor = sf::Color(50, 255, 50);    // Green for flagged cells
};

class Board{
    private:
        vector<vector<Cell>> board;
        int rows;
        int cols;
        Colors colors;
        int CELL_SIZE = 30;
        bool showCheats = false;
    public:
        Board(int rows, int cols);
        Board(int rows, int cols, int cellSize);
        void updateBoard(sf::RenderWindow* window);
        void placeMines(int mineCount);
        void handleLeftClick(const sf::Vector2i &mousePosition, sf::RenderWindow* window);
        void handleRightClick(const sf::Vector2i &mousePosition);
        void setCheats(bool cheats, sf::RenderWindow* window);
    private:
        void handleZeroCellClick(int row, int col, vector<vector<Cell>> &board);
        void handleCellClick(int row, int col, vector<vector<Cell>> &board);
        void checkIfWin(sf::RenderWindow* window);
};

#endif // BOARD_H
