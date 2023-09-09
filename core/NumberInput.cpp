#include "NumberInput.h"

#include "NumberInput.h"
#include "NumberSource.h"

namespace flo {
	NumberInput::NumberInput(NumberSource* _owner) noexcept :
		owner_numbersource(_owner),
		owner_stateful(nullptr),
		source(nullptr) {

		assert(owner_numbersource);
		auto& inputs = owner_numbersource->dependants;
		inputs.push_back(this);
	}
	NumberInput::NumberInput(Stateful* _owner) noexcept :
		owner_numbersource(nullptr),
		owner_stateful(_owner),
		source(nullptr) {

	}
	NumberInput::NumberInput(nullptr_t) noexcept :
		owner_numbersource(nullptr),
		owner_stateful(nullptr),
		source(nullptr) {
	
	}
	NumberInput::~NumberInput() noexcept {
		if (source){
			auto& d = source->dependants;
			d.erase(std::remove(d.begin(), d.end(), this), d.end());
		}

		if (owner_numbersource){
			auto& inputs = owner_numbersource->dependants;
			inputs.erase(std::remove(inputs.begin(), inputs.end(), this), inputs.end());
		}
	}
	bool NumberInput::isSafeSource(const NumberSource* src) const noexcept {
		if (src == nullptr){
			return true;
		}

		std::vector<const Stateful*> stateful_sources;
		src->findAllStatefulSources(stateful_sources);
		if (stateful_sources.size() == 0){
			return true;
		} else {
			std::vector<const Stateful*> stateful_dests;
			findAllStatefulDests(stateful_dests);
			if (stateful_dests.size() == 0){
				return !findStatelessDest();
			} else {
				for (const Stateful* src : stateful_sources){
					for (const Stateful* dst: stateful_dests){
						if (!src->hasDependency(dst)){
							return false;
						}
					}
				}
				return true;
			}
		}
	}
	void NumberInput::setSource(NumberSource* _source){
		if (!isSafeSource(_source)){
			throw std::runtime_error("The numbersource is not safe to connect; it violates the integrity of stateful information");
		}
		if (source){
			auto& d = source->dependants;
			d.erase(std::remove(d.begin(), d.end(), this), d.end());
		}
		source = _source;
		if (source){
			source->dependants.push_back(this);
		}
	}
	NumberSource* NumberInput::getSource() const {
		return source;
	}
	void NumberInput::findAllStatefulSources(std::vector<const Stateful*>& sources) const noexcept {
		if (source){
			source->findAllStatefulSources(sources);
		}
	}
	void NumberInput::findAllStatefulDests(std::vector<const Stateful*>& dests) const noexcept {
		if (owner_stateful){
			dests.push_back(owner_stateful);
		}
		if (owner_numbersource){
			owner_numbersource->findAllStatefulDests(dests);
		}
	}
	bool NumberInput::findStatelessDest() const noexcept {
		if (owner_numbersource){
			return owner_numbersource->findStatelessDest();
		} else if (owner_stateful){
			return false;
		} else {
			return true;
		}
	}

	// state shall be the current contextual state chain
	// any allowable values shall come from global constants which are not stateful
	// or from sound processing objects, which shall be found as parents of some state
	// in the state chain (accessed through the line of parents in the given state)
	// this state is to be passed on as-is to preserve its full context
	float NumberInput::getValue(const State* state, float default_value) const noexcept {
		if (source){
			return source->evaluate(state);
		} else {
			return default_value;
		}
	}

}