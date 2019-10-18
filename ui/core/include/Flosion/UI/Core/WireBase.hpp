#pragma once

#include "boxUI.h"
#include "objectUI.h"
#include <cassert>

namespace flui {



	template<typename SourceType, typename DestinationType>
	struct WireInputBase;

	template<typename SourceType, typename DestinationType>
	struct WireOutputBase;

	template<typename SourceType, typename DestinationType>
	struct WireHead;

	template<typename SourceType, typename DestinationType>
	struct WireTail;

	template<typename SourceType, typename DestinationType>
	struct WireBase;


	// WireInputBase is what a given WireType can be dropped onto
	// to connect the wire to its destination
	// the input has either zero or one inbound wires
	template<typename SourceType, typename DestinationType>
	struct WireInputBase : ui::InlineElement {

		using InputType = WireInputBase<SourceType, DestinationType>;
		using WireType = WireBase<SourceType, DestinationType>;
		using WireHeadType = WireHead<SourceType, DestinationType>;

		WireInputBase(ui::WeakRef<Object> parentobject, DestinationType& destination) :
			m_parentobject(parentobject),
			m_destination(destination) {

		}

		// override to return concrete WireType object
		virtual ui::Ref<WireType> createWire(ui::Ref<Box> box) = 0;

		DestinationType& destination(){
			return m_destination;
		}

		void updateWire() {
			if (auto wirehead = m_wireheadin.lock()){
				auto obj = m_parentobject.lock();
				assert(obj);
				wirehead->moveTo(pos() + obj->pos());
			}
		}

		ui::Ref<Object> parentObject() {
			return m_parentobject.lock();
		}

		void setWireIn(const ui::Ref<WireHeadType>& wirehead) {
			assert(wirehead);
			auto self = thisAs<InputType>();
			if (auto wh = m_wireheadin.lock()) {
				m_wireheadin.reset();
				wh->disconnectFrom(self);
			}
			m_wireheadin = wirehead;
			wirehead->connectTo(self);
			updateWire();
		}

		void removeWireIn(const ui::Ref<WireHeadType>& wirehead) {
			assert(wirehead);
			auto wh = m_wireheadin.lock();
			assert(wh && wh == wirehead);
			m_wireheadin.reset();
			wirehead->disconnectFrom(thisAs<InputType>());
		}

	private:

		bool onLeftClick(int clicks) override {
			// if there is a wire connected, disconnect and start dragging the wire's head.
			// If there's no wire connected, create a new wire whose head is connected and start
			// dragging its tail.
			auto self = thisAs<InputType>();
			auto parent = m_parentobject.lock();
			if (!parent){
				throw std::runtime_error("The WireInput does not have a parent object");
			}

			if (auto wirehead = m_wireheadin.lock()){
				wirehead->disconnectFrom(self);
				wirehead->setPos(pos() + parent->pos() + localMousePos() - wirehead->size() * 0.5f);
				wirehead->startDrag();
				transferResponseTo(wirehead);
			} else {
				m_wireheadin.reset();
				auto box = parent->getParentBox();
				if (!box){
					throw std::runtime_error("The WireInput's parent object does not have a box");
				}
				auto newwire = createWire(box);
				newwire->head()->connectTo(self);
				updateWire();
				newwire->tail()->setPos(pos() + parent->pos() + localMousePos() - newwire->tail()->size() * 0.5f);
				newwire->tail()->startDrag();
				transferResponseTo(newwire->tail());
			}
			return true;
		}

		bool onDrop(const ui::Ref<Element>& element) override {
			// if the dropped element is WireType's head, connect it
			if (auto wirehead = std::dynamic_pointer_cast<WireHeadType, Element>(element)){
				auto self = thisAs<InputType>();
				if (auto oldwire = m_wireheadin.lock()){
					oldwire->disconnectFrom(self);
				}
				wirehead->connectTo(self);
				return true;
			}
			return false;
		}

		ui::WeakRef<Object> m_parentobject;
		ui::WeakRef<WireHeadType> m_wireheadin;
		DestinationType& m_destination;

		friend struct Object;
	};

	// WireOutputBase is what a given WireType can be dropped onto
	// to connect the wire to its destination
	// the input can have any number of outbound wires
	template<typename SourceType, typename DestinationType>
	struct WireOutputBase : ui::InlineElement {

		using OutputType = WireOutputBase<SourceType, DestinationType>;
		using WireType = WireBase<SourceType, DestinationType>;
		using WireTailType = WireTail<SourceType, DestinationType>;

		WireOutputBase(ui::WeakRef<Object> parentobject, SourceType& source) :
			m_parentobject(parentobject),
			m_source(source) {

		}

		// override to return concrete WireType object
		virtual ui::Ref<WireType> createWire(ui::Ref<Box> box) = 0;

		SourceType& source(){
			return m_source;
		}

		void updateWires(){
			for (const auto& wiretail : m_wiretailsout) {
				if (auto wt = wiretail.lock()) {
					auto obj = m_parentobject.lock();
					assert(obj);
					wt->moveTo(pos() + obj->pos());
				}
			}
		}

		ui::Ref<Object> parentObject() {
			return m_parentobject.lock();
		}

		void addWireOut(const ui::Ref<WireTailType>& wiretail) {
			assert(wiretail);
			removeWireOut(wiretail);
			m_wiretailsout.push_back(wiretail);
			wiretail->connectTo(thisAs<OutputType>());
		}

		void removeWireOut(const ui::Ref<WireTailType>& wiretail) {
			assert(wiretail);
			auto self = thisAs<OutputType>();
			for (auto it = m_wiretailsout.begin(); it != m_wiretailsout.end();) {
				if (auto wt = it->lock(); wt && wt == wiretail) {
					it = m_wiretailsout.erase(it);
					wt->disconnectFrom(self);
				} else {
					++it;
				}
			}
		}

	private:

		bool onLeftClick(int clicks) override {
			// if there is a wire connected, disconnect and start dragging the wire's tail.
			// If there's no wire connected, create a new wire whose tail is connected and start
			// dragging its head.
			auto self = thisAs<OutputType>();
			auto parent = m_parentobject.lock();
			if (!parent){
				throw std::runtime_error("The WireInput does not have a parent object");
			}
			
			if (m_wiretailsout.size() > 0){
				auto wiretail = m_wiretailsout.back().lock();
				assert(wiretail);
				m_wiretailsout.pop_back();
				wiretail->disconnectFrom(self);
				wiretail->setPos(pos() + parent->pos() + localMousePos() - wiretail->size() * 0.5f);
				wiretail->startDrag();
				transferResponseTo(wiretail);
			} else {
				auto box = parent->getParentBox();
				if (!box){
					throw std::runtime_error("The WireInput's parent object does not have a box");
				}
				auto newwire = createWire(box);
				newwire->tail()->connectTo(self);
				updateWires();
				newwire->head()->setPos(pos() + parent->pos() + localMousePos() - newwire->head()->size() * 0.5f);
				newwire->head()->startDrag();
				transferResponseTo(newwire->head());
			}
			return true;
		}

		bool onDrop(const ui::Ref<Element>& element) override {
			// if the dropped element is WireType's tail, connect it
			if (auto wiretail = std::dynamic_pointer_cast<WireTailType, Element>(element)){
				auto self = thisAs<OutputType>();
				wiretail->connectTo(self);
				m_wiretailsout.push_back(wiretail);
				return true;
			}
			return false;
		}

		ui::WeakRef<Object> m_parentobject;
		std::vector<ui::WeakRef<WireTailType>> m_wiretailsout;

		SourceType& m_source;

		friend struct Object;
	};


	template<typename SourceType, typename DestinationType>
	struct EndPointBase : ui::FreeElement {

		using InputType = WireInputBase<SourceType, DestinationType>;
		using OutputType = WireOutputBase<SourceType, DestinationType>;
		using WireType = WireBase<SourceType, DestinationType>;

		EndPointBase(ui::WeakRef<WireType> parentwire) :
			m_parentwire(parentwire) {

			// TODO: make customizable
			setSize({20, 20}, true);
			setBorderRadius(10);
			setBackgroundColor(sf::Color(0x404040FF));
			setBorderThickness(1);
			setBorderColor(sf::Color(0x808080FF));
		}

		void moveTo(vec2 pos){
			if (abs(pos.x - left()) + abs(pos.y - top()) > 0.0001){
				auto w = wire();
				setPos(pos - w->pos());
				w->updateVertices();
			}
		}

		ui::Ref<WireType> wire() const {
			return m_parentwire.lock();
		}

	protected:

		ui::WeakRef<WireType> m_parentwire;

	private:

		void onDrag() override {
			auto w = wire();
			assert(w);
			w->updateVertices();
		}
	};

	// WireHead is the ui Element for the destination end of a wire, which can be
	// plugged into a provided InputType element
	template<typename SourceType, typename DestinationType>
	struct WireHead : EndPointBase<SourceType, DestinationType> {

		WireHead(ui::WeakRef<WireType> parent_wire) :
			EndPointBase<SourceType, DestinationType>(parent_wire) {

		}

		void connectTo(const ui::Ref<InputType>& input) {
			// if wire's tail is connected, call onConnect
			assert(input);
			if (m_target.lock() == input) {
				return;
			}
			m_target = input;
			input->setWireIn(thisAs<WireHead>());
			auto parentwire = wire();
			assert(parentwire);
			auto output = parentwire->tail()->target();
			if (output){
				parentwire->onConnect(output->source(), input->destination());
			}
		}

		void disconnectFrom(const ui::Ref<InputType>& input) {
			// if wire's tail is connected, call onDisconnect
			assert(input);
			if (m_target.lock() != input) {
				return;
			}
			m_target.reset();
			input->removeWireIn(thisAs<WireHead>());
			auto parentwire = wire();
			assert(parentwire);
			auto output = parentwire->tail()->target();
			if (output){
				parentwire->onDisconnect(output->source(), input->destination());
			}
		}

		bool onLeftClick(int clicks) override {
			if (auto t = m_target.lock()){
				disconnectFrom(t);
			}
			startDrag();
			return true;
		}

		void onLeftRelease() override {
			stopDrag();
			drop(localMousePos());
		}

		ui::Ref<InputType> target() const {
			return m_target.lock();
		}

	private:

		ui::WeakRef<InputType> m_target;
	};

	// WireTail is the ui Element for the source end of a wire, which can be
	// plugged into a provided OutputType element
	template<typename SourceType, typename DestinationType>
	struct WireTail : EndPointBase<SourceType, DestinationType> {
		WireTail(ui::WeakRef<WireType> parent_wire) :
			EndPointBase<SourceType, DestinationType>(parent_wire) {

		}

		void connectTo(const ui::Ref<OutputType>& output) {
			// call onConnect if wire's head is connected
			// TODO: when multiple heads are allowed, call onConnect for any of wire's connected heads
			assert(output);
			if (m_target.lock() == output) {
				return;
			}
			m_target = output;
			output->addWireOut(thisAs<WireTail>());
			auto parentwire = wire();
			assert(parentwire);
			auto input = parentwire->head()->target();
			if (input){
				parentwire->onConnect(output->source(), input->destination());
			}
		}

		void disconnectFrom(const ui::Ref<OutputType>& output) {
			// call onDisconnect if wire's head is connected
			// TODO: when multiple heads are allowed, call onDisconnect for any of wire's connected heads
			assert(output);
			if (m_target.lock() != output) {
				return;
			}
			m_target.reset();
			output->removeWireOut(thisAs<WireTail>());
			auto parentwire = wire();
			assert(parentwire);
			auto input = parentwire->head()->target();
			if (input){
				parentwire->onDisconnect(output->source(), input->destination());
			}
		}

		bool onLeftClick(int clicks) override {
			if (auto t = m_target.lock()){
				disconnectFrom(t);
			}
			startDrag();
			return true;
		}

		void onLeftRelease() override {
			stopDrag();
			drop(localMousePos());
		}

		ui::Ref<OutputType> target() const {
			return m_target.lock();
		}

	private:

		ui::WeakRef<OutputType> m_target;
	};

	// WireBase is a draggable, directional, pluggable wire.
	// InputElement and OutputElement are the ui::Element subtypes that
	// the wire connects.
	template<typename SourceType, typename DestinationType>
	struct WireBase : ui::FreeElement {

		using WireType = WireBase<SourceType, DestinationType>;
		using WireHeadType = WireHead<SourceType, DestinationType>;
		using WireTailType = WireTail<SourceType, DestinationType>;
		using InputType = WireInputBase<SourceType, DestinationType>;
		using OutputType = WireOutputBase<SourceType, DestinationType>;

		WireBase(ui::WeakRef<Box> parentbox) :
			m_parentbox(parentbox) {

			auto self = thisAs<WireType>();
			m_wirehead = add<WireHeadType>(self);
			m_wiretail = add<WireTailType>(self);

			m_parentbox.lock()->adopt(self);
		}

		void render(sf::RenderWindow& rw) override {
			rw.draw(m_vertices.data(), m_vertices.size(), sf::LineStrip);
		}

		
		bool hit(vec2 pos) const override {
			// TODO: keep position at zero at all times and do the math here
			// to make wire interactive
			return false;
		}

		// called when the previously disconnected wire is connected to both an input and an output
		virtual void onConnect(SourceType& source, DestinationType& destination) = 0;

		// called when the previously connected wire is disconnected from its input or its output
		virtual void onDisconnect(SourceType& source, DestinationType& destination) = 0;
		
		ui::Ref<WireHeadType> head() const {
			return m_wirehead;
		}

		ui::Ref<WireTailType> tail() const {
			return m_wiretail;
		}

		void updateVertices(){
			// TODO: curves, splines, branches and stuff
			m_vertices.resize(2);
			m_vertices[0] = sf::Vertex(m_wiretail->pos() + m_wiretail->size() * 0.5f, sf::Color(0x000080FF));
			m_vertices[1] = sf::Vertex(m_wirehead->pos() + m_wirehead->size() * 0.5f, sf::Color(0x0000FFFF));
		}

		ui::Ref<Box> parentBox() {
			return m_parentbox.lock();
		}

	private:

		// TODO: add a node struct which pins the wire in place
		// when it is clicked on
		// a node should be draggable and removeable
		
		// TODO: allow branched wires that connect one output to many potential inputs
		// should be feasible with current onConnect/onDisconnect api


		ui::WeakRef<Box> m_parentbox;
		ui::WeakRef<InputType> m_input;
		ui::WeakRef<OutputType> m_output;

		// TODO: make vector<ui::Ref<WireTail>> when multiple heads are allowed
		ui::Ref<WireHeadType> m_wirehead;
		
		ui::Ref<WireTailType> m_wiretail;

		// TODO: make vector<vector<sf::Vertex>> when wire nodes are
		// added, one vector<sf::Vertex> between every neighbouring
		// pair of nodes/endpoints
		std::vector<sf::Vertex> m_vertices;
	};

}; // namespace flui
