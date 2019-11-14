#include <Flosion/Objects/Router.hpp>

#include <cassert>

namespace flo {

    Router::Router() : m_input(this) {
        
    }

    NumberSource* Router::addNumberSource(){
        const auto n = numNumberSources();
        m_input.addNumberSource(n);
        for (auto& o : m_outputs){
            o->addNumberInput(n);
        }
        return getNumberSource(n);
    }
    void Router::removeNumberSource(const NumberSource* ns){
        size_t i = 0;
        for (; i < m_input.numNumberSources(); ++i){
            if (m_input.getNumberSource(i) == ns){
                break;
            }
        }
        if (i == m_input.numNumberSources()){
            throw std::runtime_error("That NumberSource is not from this router");
        }
        for (auto& o : m_outputs){
            o->removeNumberInput(i);
        }
        m_input.removeNumberSource(i);
    }
    size_t Router::numNumberSources() const {
        return m_input.numNumberSources();
    }

    SoundSource* Router::addSoundSource(){
        auto ss = std::make_unique<Output>(this);
        ss->addDependency(&m_input);
        for (size_t i = 0, iEnd = m_input.numNumberSources(); i != iEnd; ++i){
            ss->addNumberInput(i);
        }
        m_outputs.push_back(std::move(ss));
        return m_outputs.back().get();
    }
    void Router::removeSoundSource(const SoundSource* ss){
        size_t o = 0;
        for (; o < m_outputs.size(); ++o){
            if (m_outputs[o].get() == ss){
                break;
            }
        }
        if (o == m_outputs.size()){
            throw std::runtime_error("That SoundSource is not from this router");
        }
        auto it = m_outputs.begin() + o;
        const auto& myss = *it;
        myss->removeDependency(&m_input);
        m_outputs.erase(it);
    }
    size_t Router::numSoundSources() const {
        return m_outputs.size();
    }

    NumberSource* Router::getNumberSource(size_t n){
        return m_input.getNumberSource(n);
    }
    const NumberSource* Router::getNumberSource(size_t n) const {
        return m_input.getNumberSource(n);
    }
    
    NumberInput* Router::getNumberInput(size_t n, size_t o){
        assert(o < numSoundSources());
        return m_outputs[o]->getNumberInput(n);
    }
    const NumberInput* Router::getNumberInput(size_t n, size_t o) const {
        assert(o < numSoundSources());
        return m_outputs[o]->getNumberInput(n);
    }

    NumberInput* Router::getNumberInput(const NumberSource* ns, const SoundSource* ss){
        return const_cast<NumberInput*>(const_cast<const Router*>(this)->getNumberInput(ns, ss));
    }
    const NumberInput* Router::getNumberInput(const NumberSource* ns, const SoundSource* ss) const {
        size_t i = 0;
        for (; i < m_input.numNumberSources(); ++i){
            if (m_input.getNumberSource(i) == ns){
                break;
            }
        }
        if (i == m_input.numNumberSources()){
            throw std::runtime_error("That NumberSource is not from this router");
        }
        size_t o = 0;
        for (; o < m_outputs.size(); ++o){
            if (m_outputs[o].get() == ss){
                break;
            }
        }
        if (o == m_outputs.size()){
            throw std::runtime_error("That SoundSource is not from this router");
        }
        return getNumberInput(i, o);
    }

    SoundSource* Router::getSoundSource(size_t o){
        assert(o < Router::numSoundSources());
        return m_outputs[o].get();
    }
    const SoundSource* Router::getSoundSource(size_t o) const {
        assert(o < numSoundSources());
        return m_outputs[o].get();
    }

    SingleSoundInput* Router::getSoundInput(){
        return &m_input;
    }
    const SingleSoundInput* Router::getSoundInput() const {
        return &m_input;
    }



    Router::Output::Output(Router* parent) : m_parent(parent) {
    
    }

    void Router::Output::addNumberInput(size_t where){
        assert(where <= m_numberInputs.size());
        auto ni = std::make_unique<SoundNumberInput>(this);
        m_numberInputs.insert(m_numberInputs.begin() + where, std::move(ni));
    }
    void Router::Output::removeNumberInput(size_t where){
        assert(where < m_numberInputs.size());
        auto it = m_numberInputs.begin() + where;
        m_numberInputs.erase(it);
    }
    NumberInput* Router::Output::getNumberInput(size_t which){
        assert(which < m_numberInputs.size());
        return m_numberInputs[which].get();
    }
    const NumberInput* Router::Output::getNumberInput(size_t which) const {
        assert(which < m_numberInputs.size());
        return m_numberInputs[which].get();
    }

    void Router::Output::renderNextChunk(SoundChunk& chunk, EmptySoundState* state){
        m_parent->m_input.getNextChunkFor(chunk, this, state);
    }



    Router::Input::Input(Router* router)
        : SingleSoundInput(nullptr)
        , m_router(router) {
        
    }

    void Router::Input::addNumberSource(size_t where){
        assert(where <= m_numberSources.size());
        auto ns = std::make_unique<InputNumberSource>(this);
        m_numberSources.insert(m_numberSources.begin() + where, std::move(ns));
        for (size_t i = 0, iEnd = m_numberSources.size(); i != iEnd; ++i){
            m_numberSources[i]->m_numberSourceIdx = i;
        }
    }
    void Router::Input::removeNumberSource(size_t where){
        assert(where < m_numberSources.size());
        m_numberSources.erase(m_numberSources.begin() + where);
        for (size_t i = 0, iEnd = m_numberSources.size(); i != iEnd; ++i){
            m_numberSources[i]->m_numberSourceIdx = i;
        }
    }
    size_t Router::Input::numNumberSources() const {
        return m_numberSources.size();
    }

    NumberSource* Router::Input::getNumberSource(size_t which){
        assert(which < m_numberSources.size());
        return m_numberSources[which].get();
    }
    const NumberSource* Router::Input::getNumberSource(size_t which) const {
        assert(which < m_numberSources.size());
        return m_numberSources[which].get();
    }

    Router* Router::Input::getRouter(){
        return m_router;
    }
    const Router* Router::Input::getRouter() const {
        return m_router;
    }



    Router::Input::InputNumberSource::InputNumberSource(Input* parent)
        : SoundNumberSource(parent)
        , m_parent(parent)
        , m_numberSourceIdx(-1) {

    }

    double Router::Input::InputNumberSource::evaluate(const StateType* state, const SoundState* context) const noexcept {
        auto ds = state->getDependentState();
        assert(ds);
        auto router = m_parent->getRouter();
        const auto& outputs = router->m_outputs;
        const auto it = std::find_if(
            outputs.begin(),
            outputs.end(),
            [&](const std::unique_ptr<Output>& op){
                return ds->getOwner() == op.get();
            }
        );
        assert(it != outputs.end());
        assert(m_numberSourceIdx != -1);
        return (*it)->getNumberInput(m_numberSourceIdx)->getValue(context);
    }

} // namespace flo
