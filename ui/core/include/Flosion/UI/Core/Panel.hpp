#pragma once

#include <GUI/GUI.hpp>
#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/SoundInput.hpp>

#include <map>

namespace flui {

    class Object;
    class NumberInputPeg;
    class NumberOutputPeg;
    class NumberWire;
    class SoundInputPeg;
    class SoundOutputPeg;
    class SoundWire;
	
	// Panel is what holds all objects
	class Panel : public ui::Boxed<ui::FreeContainer>, public ui::Control {
    public:
		Panel();
        ~Panel();

		void addObject(std::unique_ptr<Object>);
        void removeObject(const Object*);
        
        // Adds a NumberWire to the panel.
        // At most one of src and dst may be null.
        NumberWire* addNumberWire(flo::NumberSource* src, flo::NumberInput* dst);
        void removeNumberWire(NumberWire*);

        // Adds a SoundWire to the panel.
        // At most one of src and dst may be null.
        SoundWire* addSoundWire(flo::SoundSource* src, flo::SoundInput* dst);
        void removeSoundWire(SoundWire*);

		/*void releaseObject(ui::Ref<Object> object);

		void addSoundWire(ui::Ref<SoundWire> wire);

		void removeSoundWire(SoundWire& wire);

		void addNumberWire(ui::Ref<NumberWire> wire);

		void removeNumberWire(NumberWire& wire);*/

	private:

		std::vector<Object*> m_objects;
		std::vector<NumberWire*> m_numberwires;
		std::vector<SoundWire*> m_soundwires;

    private:

        NumberInputPeg* findPegFor(const flo::NumberInput*);
        NumberOutputPeg* findPegFor(const flo::NumberSource*);
        SoundInputPeg* findPegFor(const flo::SoundInput*);
        SoundOutputPeg* findPegFor(const flo::SoundSource*);

        friend class SoundWire;
        friend class NumberWire;
        
        std::map<const flo::NumberInput*, NumberInputPeg*> m_numberInputPegs;
        std::map<const flo::NumberSource*, NumberOutputPeg*> m_numberOutputPegs;
        std::map<const flo::SoundInput*, SoundInputPeg*> m_soundInputPegs;
        std::map<const flo::SoundSource*, SoundOutputPeg*> m_soundOutputPegs;

		// calculate the bounding box of all contents
		// returns { top-left, bottom-right } as positions
		// in local space
		//std::pair<ui::vec2, ui::vec2> getContentBounds();

		// move all objects and wires by delta
		//void shiftContents(vec2 delta);

	private:

		bool onLeftClick(int) override;
	};

    // TODO: collapsible panel

	class MainPanel : public Panel {
    public:
		MainPanel();
	};

} // namespace flui