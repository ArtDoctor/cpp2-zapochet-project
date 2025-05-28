#ifndef CELL_H
#define CELL_H

#include <SFML/Graphics.hpp>

class Cell {
public:
    enum class State {
        Hidden,
        Revealed,
        Flagged
    };

    enum class Content {
        Empty,
        Mine
    };

    int mineCount = -1;
    Cell();

    void setState(State state);
    State getState() const;

    void setContent(Content content);
    Content getContent() const;
private:
    State m_state;
    Content m_content;
};

#endif // CELL_H