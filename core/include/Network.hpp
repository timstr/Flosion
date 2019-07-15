#pragma once

#include <Immovable.hpp>

#include <vector>

namespace flo {
    
    class SoundNode;
    class NumberNode;

    class Network : private Immovable {
    public:

        template<typename SoundNodeType, typename... Args>
        SoundNodeType* addSoundNode(Args&&...);

        std::vector<SoundNode*> getAllSoundNodes() noexcept;
        const std::vector<SoundNode*> getAllSoundNodes() const noexcept;

        std::vector<NumberNode*> getAllNumberNodes() noexcept;
        const std::vector<NumberNode*> getAllNumberNodes() const noexcept;

        // TODO: ???

    private:
        std::vector<SoundNode*> m_soundNodes;
        std::vector<NumberNode*> m_numberNodes;
    };

} // namespace flo
