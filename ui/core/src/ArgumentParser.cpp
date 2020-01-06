#include <Flosion/UI/Core/ArgumentParser.hpp>

namespace flui {

    bool ArgumentParser::parse(const std::vector<std::string>& args){
        auto it = m_items.begin();
        for (const auto& w : args){
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
