#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Router.hpp>

namespace flui {

    // TODO: Router is NOT a SoundObject. It does not make
    // sense to add borrowers to the side of a Router.
    // Rather, each output is a SoundObject.
    class Router : public Object {
    public:
        Router();

    private:
        void updateLayout(FlowDirection) override;

        void addSoundSource();

        void addNumberInput();

        class OutputBlock : public SoundObject {
        public:
            OutputBlock(Router* parentRouter, flo::SoundSource*);

            void addItem(std::unique_ptr<ui::Element>);

        private:
            Router* const m_parentRouter;
            flo::SoundSource* const m_soundSource;

            // TODO: this will also be interesting
            void serialize(Serializer&) const override;
            void deserialize(Deserializer&) override;
        };

        flo::Router m_router;
        std::vector<OutputBlock*> m_outputBlocks;

        // TODO: this will be interesting
        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
