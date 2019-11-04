#include <Flosion/UI/Core/ArgumentParser.hpp>

namespace flui {

    bool ArgumentParser::parse(const std::string& s){
        auto it = m_items.begin();
        std::stringstream ss;
        ss.str(s);
        std::string w;
        while (ss >> w){
            while (true){
                if (it == m_items.end()){
                    return false;
                }
                auto c = (*it)->parse(w);
                if (c < 0){
                    return false;
                }
                ++it;
                if (c > 0){
                    break;
                }
            }
        }
        return true;
    }

} // namespace flui
