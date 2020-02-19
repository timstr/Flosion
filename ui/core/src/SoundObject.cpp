#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/UI/Core/Panel.hpp>

namespace flui {

    SoundObject::SoundObject(flo::SoundSource* ss)
        : m_borrowerList(
            &putCell<ui::FreeContainer>(1, 2)
            .add<ui::Boxed<ui::VerticalList>>(ui::FreeContainer::Center, ui::FreeContainer::Center)
        )
        , m_soundSource(ss) {

        std::visit(
            [](auto& l){
                l->setPadding(5.0f);
                l->setBackgroundColor(0x913d0080);
            },
            m_borrowerList
        );
    }

    SoundObject::~SoundObject(){
        // NOTE: m_soundSource is expected to be a dangling pointer at this point
        assert(m_borrowers.size() == 0);
    }

    void SoundObject::attachBorrower(BorrowingNumberObject* bno){
        assert(bno->getParentPanel() == getParentPanel());

        auto b = bno->getBorrower();
        assert(b->getStateLender() == nullptr || b->getStateLender() == m_soundSource);
        if (b->getStateLender() == nullptr){
            b->borrowFrom(m_soundSource);
            // NOTE: the above line is expected to cause this UI function
            // to be triggered again (through signals)
            assert(std::count(m_borrowers.begin(), m_borrowers.end(), bno) == 1);
        } else {
            m_borrowers.push_back(bno);
            bno->m_lender = this;

            std::visit(
                [&](auto& l){
                    l->push_back(ui::makeOrphan(bno));
                },
                m_borrowerList
            );
        }
    }

    void SoundObject::detachBorrower(BorrowingNumberObject* bno){
        assert(bno->getBorrower()->getStateLender() == nullptr);

        auto it = std::find(m_borrowers.begin(), m_borrowers.end(), bno);
        assert(it != m_borrowers.end());
        m_borrowers.erase(it);
        bno->m_lender = nullptr;

        if (bno->dragging()){
            auto rp = bno->rootPos();
            auto p = getParentPanel();
            p->adopt(ui::makeOrphan(bno));
            bno->setPos(rp - p->rootPos());
            // NOTE: dragging stops when the object is removed from the window
            bno->startDrag();
        } else {
            // Hmmmmmmm...
            // bno->close();
        }
    }

    flo::SoundSource* SoundObject::getSoundSource() noexcept {
        return m_soundSource;
    }

    const flo::SoundSource* SoundObject::getSoundSource() const noexcept {
        return m_soundSource;
    }

    const SoundObject* SoundObject::toSoundObject() const noexcept {
        return this;
    }

    bool SoundObject::onDrop(ui::Draggable* d){
        if (auto bo = dynamic_cast<BorrowingNumberObject*>(d)){
            assert(bo->getBorrower()->getStateLender() == nullptr);
            bo->getBorrower()->borrowFrom(m_soundSource);
            assert(bo->getBorrower()->getStateLender() == m_soundSource);
            return true;
        }
        return Object::onDrop(d);
    }

    void SoundObject::updateLayout(FlowDirection fd){
        auto sideElements = std::visit(
            [](auto& l){
                std::vector<std::unique_ptr<ui::Element>> ret;
                while (!l->empty()){
                    ret.push_back(l->getChild(0)->orphan());
                }
                return ret;
            },
            m_borrowerList
        );

        Object::updateLayout(fd);

        auto makeVerticalList = [](std::vector<std::unique_ptr<Element>>& elems){
            auto l = std::make_unique<ui::Boxed<ui::VerticalList>>();
            for (auto& e : elems){
                l->push_back(std::move(e));
            }
            return l;
        };

        auto makeHorizontalList = [](std::vector<std::unique_ptr<Element>>& elems){
            auto l = std::make_unique<ui::Boxed<ui::HorizontalList>>();
            for (auto& e : elems){
                l->push_back(std::move(e));
            }
            return l;
        };

        auto makeCentered = [](auto& l){
            auto fc = std::make_unique<ui::FreeContainer>();
            fc->adopt(ui::FreeContainer::Center, ui::FreeContainer::Center, std::move(l));
            return fc;
        };
        
        switch (fd){
        case FlowDirection::Left:
        case FlowDirection::Right:
            {
                auto l = makeVerticalList(sideElements);
                m_borrowerList = l.get();
                putCell(1, 2, makeCentered(l));
                break;
            }
        case FlowDirection::Up:
        case FlowDirection::Down:
            {
                auto l = makeHorizontalList(sideElements);
                m_borrowerList = l.get();
                putCell(0, 1, makeCentered(l));
                break;
            }
        }

        for (auto& b : m_borrowers){
            b->trySetFlowDirection(fd);
        }
    }

    void SoundObject::initialize(){
        m_onBorrowerAddedConn = m_soundSource->onBorrowerAdded.connect(
            [&](const flo::BorrowingNumberSource* bns){
                auto b = getParentPanel()->findBorrowerFor(bns);
                assert(b);
                attachBorrower(b);
            }
        );

        m_onBorrowerRemovedConn = m_soundSource->onBorrowerRemoved.connect(
            [&](const flo::BorrowingNumberSource* bns){
                for (const auto& b : m_borrowers){
                    if (b->getBorrower() == bns){
                        detachBorrower(b);
                        return;
                    }
                }
                assert(false);
                throw std::runtime_error("Aaagh");
            }
        );
    }

} // namespace flui
