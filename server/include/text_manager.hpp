#ifndef TEXT_MANAGER_HPP
#define TEXT_MANAGER_HPP

#include <string>

class TextManager {
    std::vector<std::string> texts = {
        "адлыволдао выдлаоывлд адлыво",
        "12312 321 2 33",
        "lkfdjsjl ksdlkfjdsl sldfkfjsd"
    };
public:
    std::string rand_text();
};

#endif
