#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Cell.h"
#include "Board.h"

using namespace std;

// Constructor
Board::Board(int rows, int cols) : rows(rows), cols(cols) {
    // Initialize the board
    for (int i = 0; i < rows; ++i) {
        vector<Cell> row;
        for (int j = 0; j < cols; ++j) {
            row.push_back(Cell());
        }
        board.push_back(row);
    }
}

// Constructor with cell size
Board::Board(int rows, int cols, int cellSize) : rows(rows), cols(cols), CELL_SIZE(cellSize) {
    // Initialize the board
    for (int i = 0; i < rows; ++i) {
        vector<Cell> row;
        for (int j = 0; j < cols; ++j) {
            row.push_back(Cell());
        }
        board.push_back(row);
    }
}

// Function to draw the board and update the window
void Board::updateBoard(
    sf::RenderWindow* window
) {
    if (showCheats) {
        // Draw the board
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                cell.setPosition(j * CELL_SIZE + cols * CELL_SIZE + CELL_SIZE + 1, i * CELL_SIZE + 1); // Slight padding

                if (board[i][j].getContent() == mineType::Mine) {
                    cell.setFillColor(colors.mineColor); // Mine
                } else {
                    cell.setFillColor(colors.emptyColor); // Empty
                }
                window->draw(cell);

                // Draw the mine count
                if (board[i][j].getContent() == mineType::Empty && board[i][j].mineCount > 0) {
                    sf::Font font;
                    if (!font.loadFromFile("DejaVuSans.ttf")) {
                        cerr << "Failed to load font!" << endl;
                        return;
                    }
                    sf::Text text(to_string(board[i][j].mineCount), font, 16);
                    text.setFillColor(sf::Color::White);
                    text.setPosition(j * CELL_SIZE + cols * CELL_SIZE + CELL_SIZE + 1 + 10, i * CELL_SIZE + 1 + 5);
                    window->draw(text);
                }

            }
        }
    }
    // Draw the user board
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(j * CELL_SIZE + 1, i * CELL_SIZE + 1); // Slight padding

            if (board[i][j].getState() == cellState::Flagged) {
                cell.setFillColor(colors.flagColor); // Flagged
            } else if (board[i][j].getState() == cellState::Revealed) {
                if (board[i][j].getContent() == mineType::Mine) {
                    cell.setFillColor(colors.mineColor); // Mine
                } else {
                    cell.setFillColor(colors.safeColor); // Safe
                }
            } else {
                cell.setFillColor(colors.emptyColor); // Not revealed
            }

            window->draw(cell);

            // Draw the mine count
            if (board[i][j].getState() == cellState::Revealed && board[i][j].mineCount > 0) {
                sf::Font font;
                if (!font.loadFromFile("DejaVuSans.ttf")) {
                    cerr << "Failed to load font!" << endl;
                    return;
                }
                sf::Text text(to_string(board[i][j].mineCount), font, 16);
                text.setFillColor(sf::Color::Black);
                text.setPosition(j * CELL_SIZE + 10, i * CELL_SIZE + 5);
                window->draw(text);
            }
        }
    }
    // Display the board
    window->display();
}


// Function to place mines randomly on the board
void Board::placeMines(int mineCount) {
    srand(time(0));
    while (mineCount > 0) {
        int r = rand() % rows;
        int c = rand() % cols;
        if (board[r][c].getContent() == mineType::Empty) {
            board[r][c].setContent(mineType::Mine);
            --mineCount;
        }
    }

    // Update the mine count for each cell
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (board[i][j].getContent() == mineType::Empty) {
                int count = 0;
                for (int r = i - 1; r <= i + 1; ++r) {
                    for (int c = j - 1; c <= j + 1; ++c) {
                        if (r >= 0 && r < rows && c >= 0 && c < cols && board[r][c].getContent() == mineType::Mine) {
                            ++count;
                        }
                    }
                }
                board[i][j].mineCount = count;
            }
        }
    }
}

void Board::checkIfWin(sf::RenderWindow* window){
    int hiddenCount = 0;
    int mineCount = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (board[i][j].getState() == cellState::Hidden || board[i][j].getState() == cellState::Flagged) {
                ++hiddenCount;
            }
            if (board[i][j].getContent() == mineType::Mine) {
                ++mineCount;
            }
        }
    }
    if (hiddenCount - mineCount == 0) {
        sf::RenderWindow winWindow(sf::VideoMode(200, 100), "You Win!");
        while (winWindow.isOpen()) {
            sf::Event event;
            while (winWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    winWindow.close();
                    window->close();
                }
            }
            sf::Font font;
            if (!font.loadFromFile("DejaVuSans.ttf")) {
                cerr << "Failed to load font!" << endl;
                return;
            }
            sf::Text text("You Win!", font, 24);
            text.setFillColor(sf::Color::Green);
            text.setPosition(10, 10);
            winWindow.draw(text);
            winWindow.display();
        }
    }
}

void Board::setCheats(bool cheats, sf::RenderWindow* window){
    showCheats = cheats;
    updateBoard(window);
}