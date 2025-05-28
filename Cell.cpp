#include "Cell.h"

Cell::Cell() : m_state(State::Hidden), m_content(Content::Empty) {}

void Cell::setState(State state) {
    m_state = state;
}

Cell::State Cell::getState() const {
    return m_state;
}

void Cell::setContent(Content content) {
    m_content = content;
}

Cell::Content Cell::getContent() const {
    return m_content;
}
