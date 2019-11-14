#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Router.hpp>

namespace flui {

    class Router : public Object {
    public:
        Router();

    private:
        void addSoundSource();

        void addNumberInput();

        class OutputBlock : public ui::Boxed<ui::FreeContainer> {
        public:
            OutputBlock(Router* parentRouter, const flo::SoundSource*);

            void addItem(std::unique_ptr<ui::Element>);

        private:
            Router* const m_parentRouter;
            const flo::SoundSource* const m_soundSource;
            ui::VerticalList& m_list;
        };

        flo::Router m_router;
        std::vector<OutputBlock*> m_outputBlocks;
    };

} // namespace flui
