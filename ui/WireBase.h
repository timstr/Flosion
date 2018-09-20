#pragma once

#include "boxUI.h"
#include "objectUI.h"
#include <cassert>

namespace fui {



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

	private:

		bool onLeftClick(int clicks) override {
			// if there is a wire connected, disconnect and start dragging the wire's head.
			// If there's no wire connected, create a new wire whose head is connected and start
			// dragging its tail.
			auto self = thisAs<InputType>();
			if (auto wirehead = m_wireheadin.lock()){
				wirehead->disconnectFrom(self);
				wirehead->startDrag();
			} else {
				m_wireheadin.reset();
				auto parent = m_parentobject.lock();
				if (!parent){
					throw std::runtime_error("The WireInput does not have a parent object");
				}
				auto box = parent->getParentBox();
				if (!box){
					throw std::runtime_error("The WireInput's parent object does not have a box");
				}
				auto newwire = createWire(box);
				newwire->head()->connectTo(self);
				newwire->tail()->startDrag();
			}
		}

		bool onDrop(const ui::Ref<Element>& element) override {
			// if the dropped element is WireType's head, connect it
			if (auto wirehead = std::dynamic_pointer_cast<WireHeadType, Element>(element)){
				auto self = thisAs<InputType>();
				if (auto oldwire = m_wireheadin.lock()){
					oldwire->disconnectFrom(self);
				}
				wirehead->connectTo(self);
			}
		}

		ui::WeakRef<Object> m_parentobject;
		ui::WeakRef<WireHeadType> m_wireheadin;

		DestinationType& m_destination;
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

	private:

		bool onLeftClick(int clicks) override {
			// if there is a wire connected, disconnect and start dragging the wire's tail.
			// If there's no wire connected, create a new wire whose tail is connected and start
			// dragging its head.
			auto self = thisAs<OutputType>();
			
			if (m_wiretailsout.size() > 0){
				auto wiretail = m_wiretailsout.back().lock();
				m_wiretailsout.pop_back();
				wiretail->disconnectFrom(self);
				wiretail->startDrag();
			} else {
				auto parent = m_parentobject.lock();
				if (!parent){
					throw std::runtime_error("The WireInput does not have a parent object");
				}
				auto box = parent->getParentBox();
				if (!box){
					throw std::runtime_error("The WireInput's parent object does not have a box");
				}
				auto newwire = createWire(box);
				newwire->tail()->connectTo(self);
				newwire->head()->startDrag();
			}
		}

		bool onDrop(const ui::Ref<Element>& element) override {
			// if the dropped element is WireType's tail, connect it
			if (auto wiretail = std::dynamic_pointer_cast<WireTailType, Element>(element)){
				auto self = thisAs<OutputType>();
				wiretail->connectTo(self);
				m_wiretailsout.push_back(wiretail);
			}
		}

		ui::WeakRef<Object> m_parentobject;
		std::vector<ui::WeakRef<WireTailType>> m_wiretailsout;

		SourceType& m_source;
	};


	template<typename SourceType, typename DestinationType>
	struct EndPointBase : ui::FreeElement {

		using InputType = WireInputBase<SourceType, DestinationType>;
		using OutputType = WireOutputBase<SourceType, DestinationType>;
		using WireType = WireBase<SourceType, DestinationType>;

		EndPointBase(ui::WeakRef<WireType> parentwire) :
			m_parentwire(parentwire) {
		}

		ui::Ref<WireType> wire() const {
			return m_parentwire.lock();
		}

	protected:

		ui::WeakRef<WireType> m_parentwire;

	private:

		void onDrag() override {
			auto parent = m_parentwire.lock();
			assert(parent);
			parent->updateVertices();
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
			assert(m_target.lock() != input);
			m_target = input;
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
			assert(m_target.lock() == input);
			m_target.reset();
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
			assert(m_target.lock() != output);
			m_target = output;
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
			assert(m_target.lock() == output);
			m_target.reset();
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
			m_vertices[0] = sf::Vertex(m_wiretail->pos(), sf::Color(0x000080FF));
			m_vertices[1] = sf::Vertex(m_wirehead->pos(), sf::Color(0x0000FFFF));
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

}; // namespace fui