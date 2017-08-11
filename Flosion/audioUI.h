#pragma once
#include "FlosionUI.h"
#include "audio.h"
#include "filebrowser.h"

namespace fui {

	struct AudioObject : ProcessingObject {
		AudioObject(){
			size = {150, 150};
			addChildWindow(new ui::Text("Audio Clip", getFont()));
			addSoundOutput(new SoundOutput(&audio, this, {size.x - 20, 20}));
			addChildWindow(new LoadButton(this), {25, 20});
			addChildWindow(caption = new ui::Text("-", fui::getFont()), {5, 80});
		}

		private:
		struct LoadButton : ui::Window {
			LoadButton(AudioObject* _parent){
				parent = _parent;
				size = {50, 50};
				addChildWindow(new ui::Text("Load", getFont()));
			}

			void onLeftClick(int clicks) override {
				std::string filename = openFileDialog(L"audio files\0*.ogg;*.wav;*.flac;*.aiff\0\0");
				parent->audio.loadFromFile(filename);
				if (filename != ""){
					size_t pos = filename.find_last_of('\\'); 
					std::string shortname = filename.substr(pos + 1);
					parent->caption->setText(shortname);
				}
			}

			private:
			AudioObject* parent;
		};

		musical::Audio audio;
		ui::Text* caption;
	};
	fuiRegisterObject(AudioObject, "audio", "sound", "audioclip");
}
