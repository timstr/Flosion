#pragma once

#include "FlosionUICore.h"
#include "sampler.h"

namespace fui {

	struct SamplerObject : ProcessingObject {
		SamplerObject(){
			size = {400, 400};
			addSoundInput(new SoundInput(&sampler.input, this, {-5, 5}));
			addSoundOutput(new SoundOutput(&sampler, this, {size.x - 25, 5}));
			addNumberOutput(new NumberOutput(&sampler.input.frequency, this, {30, -5}));
			addChildWindow(new ui::Text("Sampler", fui::getFont()), {30, 30});
		}

		void onLeftClick(int clicks) override {
			sf::Vector2f mousepos = localMousePos();
			NoteWindow* notewin = new NoteWindow(this, sampler.addNote(freqFromYPos(mousepos.y), 0.1, timeFromXPos(mousepos.x), 0.25 * musical::SFREQ));
			addChildWindow(notewin, mousepos);
			notewin->startDrag();
		}

		private:

		static double freqFromYPos(float pos){
			return 16.3516 * pow(2.0, pos / 100.0);
		}

		static uint32_t timeFromXPos(float pos){
			return pos * musical::SFREQ / 100;
		}

		struct NoteWindow : ui::Window {
			NoteWindow(SamplerObject* _parent, musical::Sampler::Note* _note){
				parent = _parent;
				note = _note;
				size = {25, 10};
			}
			void onLeftClick(int clicks) override {
				if (clicks == 1){
					startDrag();
				} else if (clicks == 2){
					parent->sampler.removeNote(note);
					close();
				}
			}
			void onDrag() override {
				if (pos.x < 0){
					pos.x = 0;
				}
				if (pos.x > parent->size.x - size.x){
					pos.x = parent->size.x - size.x;
				}
				if (pos.y < 0){
					pos.y = 0;
				}
				if (pos.y > parent->size.y - size.y){
					pos.y = parent->size.y - size.y;
				}
				note->start_time = timeFromXPos(pos.x);
				note->frequency = freqFromYPos(parent->size.y - pos.y);
			}
			SamplerObject* parent;
			musical::Sampler::Note* note;
		};

		musical::Sampler sampler;
	};
	fuiRegisterObject(SamplerObject, "sampler");
}