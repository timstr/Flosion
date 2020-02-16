#include <Flosion/Objects/Splicer.hpp>

namespace flo {

    void SplicerState::reset() noexcept {
        currentRoute = RouteType::Main;
    }

    Splicer::Splicer()
        : m_routingNode(*this)
        , m_innerInput(this)
        , m_mainInput(&m_routingNode)
        , m_sideInput(&m_routingNode)
        , m_installedSoundSource(nullptr)
        , m_installedInput(nullptr) {

    }

    SingleSoundInput* Splicer::getMainInput() noexcept {
        return &m_mainInput;
    }

    SingleSoundInput* Splicer::getSideInput() noexcept {
        return &m_sideInput;
    }

    bool Splicer::safeToInstall(const SoundSource* ss) const noexcept {
        if (!ss){
            return false;
        }
        if (ss->isDivergent() || ss->isOutOfSync()){
            return false;
        }
        const auto ddcs = ss->getDirectDependencies();
        if (ddcs.size() != 1){
            return false;
        }
        const auto ddc = ddcs[0];
        if (ddc->isDivergent() || ddc->isOutOfSync()){
            return false;
        }
        return true;
    }

    void Splicer::install(SoundSource* ss){
        if (ss && !safeToInstall(ss)){
            throw std::runtime_error("Wrong. Try again.");
        }

        // TODO: somehow prevent the object from being used.

        // uninstall previous SoundSource
        if (m_installedSoundSource){
            assert(m_installedInput);
            // TODO: hmmmm
            m_installedSoundSource = nullptr;
            m_installedInput = nullptr;
        }
        m_installedSoundSource = ss;
        if (m_installedSoundSource){
            const auto ddcs = ss->getDirectDependencies();
            assert(ddcs.size() == 1);
            m_installedInput = ddcs[0]->toSoundInput();
            assert(m_installedInput);
        }
    }

    void Splicer::renderNextChunk(SoundChunk& chunk, SplicerState* state){
        if (!m_installedSoundSource){
            chunk.silence();
            return;
        }

        // Side route: inner soundsource is called upon,
        // call is passed to side input
        state->currentRoute = SplicerState::RouteType::Side;
        m_innerInput.getNextChunkFor(chunk, this, state);

        // Main route: inner soundsource is bypassed,
        // call is passed to main input
        state->currentRoute = SplicerState::RouteType::Main;
        
        assert(m_installedSoundSource);
        assert(hasDirectDependency(&m_innerInput));
        assert(m_innerInput.hasDirectDependency(m_installedSoundSource));
        assert(m_installedSoundSource->hasDirectDependency(m_installedInput));
        assert(m_installedInput->hasDirectDependency(&m_routingNode));

        const auto s1 = m_innerInput.getState(m_installedSoundSource, state);
        const auto s2 = m_installedSoundSource->getState(this, s1, 0);
        const auto s3 = m_installedInput->getState(m_installedSoundSource, s2, 0);
        static_cast<SoundSource&>(m_routingNode).getNextChunkFor(chunk, m_installedInput, s3);
    }

    Splicer::RoutingNode::RoutingNode(Splicer& parentSplicer)
        : m_parentSplicer(parentSplicer) {

    }

    void Splicer::RoutingNode::renderNextChunk(SoundChunk& chunk, EmptySoundState* state){
        const SoundState* s = state;
        while (s){
            if (s->getOwner() == &m_parentSplicer){
                auto ss = m_parentSplicer.getDerivedState<SplicerState>(state);
                if (ss->currentRoute == SplicerState::RouteType::Main){
                    m_parentSplicer.getMainInput()->getNextChunkFor(chunk, this, state);
                } else {
                    m_parentSplicer.getSideInput()->getNextChunkFor(chunk, this, state);
                }
                return;
            }
            s = s->getDependentState();
        }
        assert(false);
        throw std::runtime_error("Failed to find state");
    }

} // namespace flo
