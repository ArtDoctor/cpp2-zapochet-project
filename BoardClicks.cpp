#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include "Cell.h"
#include "Board.h"

using namespace std;

void Board::handleCellClick(int row, int col, vector<vector<Cell>> &board) {
    if (board[row][col].mineCount == 0) {
        handleZeroCellClick(row, col, board);
    } else{
        // Check if the cell is hidden
        if (board[row][col].getState() == cellState::Hidden) {
            // Reveal the cell
            board[row][col].setState(cellState::Revealed);
        }
    }
}

void Board::handleZeroCellClick(int row, int col, vector<vector<Cell>> &board) {
    // Check if the cell is hidden
    if (board[row][col].getState() == cellState::Hidden) {
        // Reveal the cell
        board[row][col].setState(cellState::Revealed);
        if (board[row][col].mineCount == 0) {
            // Continue revealing neighboring cells
            for (int r = row - 1; r <= row + 1; ++r) {
                for (int c = col - 1; c <= col + 1; ++c) {
                    if (r >= 0 && r < rows && c >= 0 && c < cols) {
                        if (board[r][c].mineCount == 0 && board[r][c].getState() == cellState::Hidden) {
                            handleZeroCellClick(r, c, board);
                        } else if (board[r][c].mineCount > 0 && board[r][c].getState() == cellState::Hidden) {
                            handleCellClick(r, c, board);
                        }
                    }
                }
            }
        }
    }
}

void Board::handleLeftClick(const sf::Vector2i &mousePosition, sf::RenderWindow* window){
    // Convert mouse position to grid coordinates
    int col = mousePosition.x / CELL_SIZE;
    int row = mousePosition.y / CELL_SIZE;

    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        // Check the content of the clicked cell
        if (board[row][col].getState() == cellState::Hidden){
            if (board[row][col].getContent() == mineType::Mine) {
                sf::RenderWindow gameOverWindow(sf::VideoMode(200, 100), "Game Over!");
                sf::Font font;
                if (!font.loadFromFile("DejaVuSans.ttf")) {
                    cerr << "Failed to load font!" << endl;
                    return;
                }
                sf::Text text("Game Over!", font, 24);
                text.setFillColor(sf::Color::Red);
                text.setPosition(50, 30);

                while (gameOverWindow.isOpen()) {
                    sf::Event event;
                    while (gameOverWindow.pollEvent(event)) {
                        if (event.type == sf::Event::Closed) {
                            gameOverWindow.close();
                            window->close();
                        }
                    }

                    gameOverWindow.clear(sf::Color::Black);
                    gameOverWindow.draw(text);
                    gameOverWindow.display();
                }
            } else {
                // Reveal the cell
                handleCellClick(row, col, board);
                updateBoard(window);
                checkIfWin(window);
            }
        }
    }
}

void Board::handleRightClick(const sf::Vector2i &mousePosition){
    // Convert mouse position to grid coordinates
    int col = mousePosition.x / CELL_SIZE;
    int row = mousePosition.y / CELL_SIZE;

    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        // Check the content of the clicked cell
        if (board[row][col].getState() == cellState::Hidden) {
            // Flag the cell
            board[row][col].setState(cellState::Flagged);
        } else if (board[row][col].getState() == cellState::Flagged) {
            // Unflag the cell
            board[row][col].setState(cellState::Hidden);
        }
    }
}
