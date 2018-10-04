#pragma once
#include "ObjectFactory.h"
#include "audio.h"
#include "filebrowser.h"

namespace fui {

	struct AudioObject : Object {
		AudioObject(){
			write("Audio Clip", getFont());
			add<SoundOutput>(thisAs<Object>(), audio, "Sound Output");

			add<ui::CallbackButton>("load", getFont(), [this](){
				std::string filename = openFileDialog(L"audio files\0*.ogg;*.wav;*.flac;*.aiff\0\0");
				audio.loadFromFile(filename);
				if (filename != ""){
					size_t pos = filename.find_last_of('\\'); 
					std::string shortname = filename.substr(pos + 1);
					caption->clear();
					caption->write(shortname, getFont());
				}
			});

			caption = add<ui::BlockElement>();
		}

		private:

		flo::Audio audio;
		ui::Ref<ui::Element> caption;
	};
	RegisterFactoryObject(AudioObject, "sound", "audioclip");
}
