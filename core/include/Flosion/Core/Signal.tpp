#include <cassert>

namespace flo {

    template<typename... Args>
    Signal<Args...>::Signal(){
        
    }

    template<typename... Args>
    Signal<Args...>::Signal(Signal&& s)
        : m_handlers(std::move(s.m_handlers)) {
        for (auto [c, h] : m_handlers){
            assert(c->m_signal == &s);
            c->m_signal = this;
        }
    }

    template<typename... Args>
    Signal<Args...>& Signal<Args...>::operator=(Signal&& s){
        for (auto [c, h] : m_handlers){
            assert(c->m_signal == this);
            c->m_signal = nullptr;
        }
        m_handlers = std::move(s.m_handlers);
        for (auto [c, h] : m_handlers){
            assert(c->m_signal == &s);
            c->m_signal = this;
        }
        return *this;
    }

    template<typename... Args>
    Signal<Args...>::~Signal(){
        for (auto& [c, h] : m_handlers){
            assert(c->m_signal == this);
            c->m_signal = nullptr;
        }
    }

    template<typename... Args>
    Connection Signal<Args...>::connect(Signal<Args...>::Handler h){
        auto c = Connection{};
        c.m_signal = this;
        m_handlers.try_emplace(&c, std::move(h));
        return c;
    }

    template<typename... Args>
    void Signal<Args...>::broadcast(Args... args){
        auto tempHandlers = m_handlers;
        for (auto& [c, h] : tempHandlers){
            h(args...);
        }
    }

    template<typename... Args>
    void Signal<Args...>::moveConnection(Connection* from, Connection* to){
        assert(from != to);
        auto it = m_handlers.find(from);
        assert(it != m_handlers.end());
        auto h = std::move(it->second);
        m_handlers.erase(it);
        m_handlers.try_emplace(to, std::move(h));
    }

    template<typename... Args>
    void Signal<Args...>::removeConnection(Connection* c){
        auto it = m_handlers.find(c);
        assert(it != m_handlers.end());
        m_handlers.erase(it);
    }

} // namespace flo
