#include <Flosion/Core/Signal.hpp>

#include <cassert>

namespace flo {

    Connection::Connection()
        : m_signal(nullptr) {
            
    }

    Connection::Connection(Connection&& c)
        : m_signal(std::exchange(c.m_signal, nullptr)) {
        if (m_signal){
            m_signal->moveConnection(&c, this);
        }
    }

    Connection& Connection::operator=(Connection&& c){
        if (m_signal){
            m_signal->removeConnection(this);
        }
        m_signal = std::exchange(c.m_signal, nullptr);
        if (m_signal){
            m_signal->moveConnection(&c, this);
        }
        return *this;
    }

    Connection::~Connection(){
        reset();
    }

    void Connection::reset(){
        if (m_signal){
            m_signal->removeConnection(this);
            m_signal = nullptr;
        }
    }

} // namespace flo
