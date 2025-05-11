#include "text_manager.hpp"
#include <cstdlib>
#include <ctime>

std::string TextManager::rand_text() {
    std::srand(std::time(nullptr));
    return texts[std::rand() % texts.size()];
}
