#pragma once

#include <exception>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <typeinfo>
#include <typeindex>
#include <vector>

// TODO: use std::span?

// TODO: worry about endianness
// see https://en.cppreference.com/w/cpp/types/endian

#ifndef _WIN64
    #error "This code is not portable. Sorry. Ask Tim to fix it."
#endif

namespace flui {

    class Object;
    class NumberInputPeg;
    class NumberOutputPeg;
    class NumberWire;
    class SoundInputPeg;
    class SoundOutputPeg;
    class SoundWire;
    class Panel;

    class Serializer;
    class Deserializer;
    class Serializable;

    class SerializationException : public std::exception {
    public:
        const char* what() const noexcept override;
    };



    class Serializer {
    public:
        Serializer() = default;
        ~Serializer() = default;

        Serializer(Serializer&&) = delete;
        Serializer(const Serializer&) = delete;
        Serializer& operator=(Serializer&&) = delete;
        Serializer& operator=(const Serializer&) = delete;

        // NOTE: the "current" heap object is simply the last entry in m_objects

        // Serializes all the objects in a panel, or if whichObjects is not empty,
        // serializes only those objects that are selected. In that case, all selected
        // objects must directly belong to the panel. Any wires attached to objects in
        // the panel that were not selected will not be serialized.
        void serializeFrom(const Panel*, const std::vector<const Object*>& whichObjects = {});

        // Creates a vector containing the serialized data. This may be saved to a string,
        // written to a file, sent over a network, etc. This data can be used to construct
        // a Deserializer which can be used to recover the original objects and wires.
        std::vector<std::byte> dump() const;

        // When an Object is being serialized, it is responsible for adding all
        // of its pegs via these methods
        void addPeg(const SoundInputPeg*);
        void addPeg(const SoundOutputPeg*);
        void addPeg(const NumberInputPeg*);
        void addPeg(const NumberOutputPeg*);

    public:

        // Chainable inserters for single values 
        Serializer& b(bool);

        Serializer& u8(std::uint8_t);
        Serializer& u16(std::uint16_t);
        Serializer& u32(std::uint32_t);
        Serializer& u64(std::uint64_t);

        Serializer& i8(std::int8_t);
        Serializer& i16(std::int16_t);
        Serializer& i32(std::int32_t);
        Serializer& i64(std::int64_t);

        Serializer& f32(float);
        Serializer& f64(double);

        Serializer& str(const std::string_view&);

        // Chainable inserters for spans

        Serializer& u8_span(const std::uint8_t* src, std::uint64_t len);
        Serializer& u16_span(const std::uint16_t* src, std::uint64_t len);
        Serializer& u32_span(const std::uint32_t* src, std::uint64_t len);
        Serializer& u64_span(const std::uint64_t* src, std::uint64_t len);

        Serializer& i8_span(const std::int8_t* src, std::uint64_t len);
        Serializer& i16_span(const std::int16_t* src, std::uint64_t len);
        Serializer& i32_span(const std::int32_t* src, std::uint64_t len);
        Serializer& i64_span(const std::int64_t* src, std::uint64_t len);

        Serializer& f32_span(const float* src, std::uint64_t len);
        Serializer& f64_span(const double* src, std::uint64_t len);

        Serializer& str_span(const std::string* src, std::uint64_t len);
        

        // Chainable inserters for vectors (equivalent to using spans)

        Serializer& u8_vec(const std::vector<std::uint8_t>&);
        Serializer& u16_vec(const std::vector<std::uint16_t>&);
        Serializer& u32_vec(const std::vector<std::uint32_t>&);
        Serializer& u64_vec(const std::vector<std::uint64_t>&);

        Serializer& i8_vec(const std::vector<std::int8_t>&);
        Serializer& i16_vec(const std::vector<std::int16_t>&);
        Serializer& i32_vec(const std::vector<std::int32_t>&);
        Serializer& i64_vec(const std::vector<std::int64_t>&);

        Serializer& f32_vec(const std::vector<float>&);
        Serializer& f64_vec(const std::vector<double>&);

        Serializer& str_vec(const std::vector<std::string>&);

    private:

        // TODO: this name no longer makes sense
        using heap_id = std::uint64_t;

        void addObject(const Object*);

        bool tryAddWire(const SoundWire*);
        bool tryAddWire(const NumberWire*);

        std::optional<heap_id> findID(const SoundInputPeg*) const;
        std::optional<heap_id> findID(const SoundOutputPeg*) const;
        std::optional<heap_id> findID(const NumberInputPeg*) const;
        std::optional<heap_id> findID(const NumberOutputPeg*) const;

    private:
        std::vector<std::byte>& getCurrentObject();

        std::vector<std::vector<std::byte>> m_objects;

        // List of known sound/number input/output pegs.
        // Ids of these pegs are their indices in these vectors.
        // ids for different types of pegs are unrelated to eachother
        std::vector<const SoundInputPeg*> m_soundInputPegs;
        std::vector<const SoundOutputPeg*> m_soundOutputPegs;
        std::vector<const NumberInputPeg*> m_numberInputPegs;
        std::vector<const NumberOutputPeg*> m_numberOutputPegs;

        // lists of wire connections represented as (input peg id, output peg id) pairs
        std::vector<std::pair<heap_id, heap_id>> m_soundWires;
        std::vector<std::pair<heap_id, heap_id>> m_numberWires;
    };


    class Deserializer {
    public:
        // Creates a Deserializer from a vector binary data.
        // This data needs to have been created by a Serializer.

        Deserializer(const std::vector<std::byte>& buffer);
        Deserializer() = delete;
        ~Deserializer() = default;

        Deserializer(Deserializer&&) = delete;
        Deserializer(const Deserializer&) = delete;
        Deserializer& operator=(Deserializer&&) = delete;
        Deserializer& operator=(const Deserializer&) = delete;

        // Deserializes the stored objects into the given panel.
        // Returns a vector of the objects that were just created.
        // This could be used i.e. for selecting those objects right after pasting
        std::vector<Object*> deserializeTo(Panel*);

        // When an Object is being deserialized, it is responsible for adding all
        // of its pegs via these methods
        void addPeg(SoundInputPeg*);
        void addPeg(SoundOutputPeg*);
        void addPeg(NumberInputPeg*);
        void addPeg(NumberOutputPeg*);

    public:

        // Chainable extractors (pass by reference)
        Deserializer& b(bool&);

        Deserializer& u8(std::uint8_t&);
        Deserializer& u16(std::uint16_t&);
        Deserializer& u32(std::uint32_t&);
        Deserializer& u64(std::uint64_t&);

        Deserializer& i8(std::int8_t&);
        Deserializer& i16(std::int16_t&);
        Deserializer& i32(std::int32_t&);
        Deserializer& i64(std::int64_t&);

        Deserializer& f32(float&);
        Deserializer& f64(double&);

        Deserializer& str(std::string&);

        // Non-chainable extractors (simple return values)
        bool b();

        std::uint8_t u8();
        std::uint16_t u16();
        std::uint32_t u32();
        std::uint64_t u64();

        std::int8_t i8();
        std::int16_t i16();
        std::int32_t i32();
        std::int64_t i64();

        float f32();
        double f64();

        std::string str();

        // Spans

        std::uint64_t peekSpanLength();
        
        Deserializer& b_span(bool* dst, std::uint64_t len);

        Deserializer& u8_span(std::uint8_t* dst, std::uint64_t len);
        Deserializer& u16_span(std::uint16_t* dst, std::uint64_t len);
        Deserializer& u32_span(std::uint32_t* dst, std::uint64_t len);
        Deserializer& u64_span(std::uint64_t* dst, std::uint64_t len);

        Deserializer& i8_span(std::int8_t* dst, std::uint64_t len);
        Deserializer& i16_span(std::int16_t* dst, std::uint64_t len);
        Deserializer& i32_span(std::int32_t* dst, std::uint64_t len);
        Deserializer& i64_span(std::int64_t* dst, std::uint64_t len);

        Deserializer& f32_span(float* dst, std::uint64_t len);
        Deserializer& f64_span(double* dst, std::uint64_t len);

        Deserializer& str_span(std::string* dst, std::uint64_t len);

        // Vectors
        std::vector<bool> b_vec();

        std::vector<std::uint8_t> u8_vec();
        std::vector<std::uint16_t> u16_vec();
        std::vector<std::uint32_t> u32_vec();
        std::vector<std::uint64_t> u64_vec();

        std::vector<std::int8_t> i8_vec();
        std::vector<std::int16_t> i16_vec();
        std::vector<std::int32_t> i32_vec();
        std::vector<std::int64_t> i64_vec();

        std::vector<float> f32_vec();
        std::vector<double> f64_vec();

        std::vector<std::string> str_vec();

        // TODO

    public:

        static const std::string& getObjectIdentifier(const Object*);

        static std::unique_ptr<Object> makeObject(const std::string& identifier);

        template<typename ObjectType>
        class Registrator {
        public:
            // Adds the class, its identifier, and a factory function to the static maps
            Registrator(const std::string& identifier);

            // Removes the class, its identifier, and the factory function from the static maps
            ~Registrator();
        };

    private:

        // TODO: this name no longer makes sense
        // TODO: rename to peg id
        using heap_id = std::uint64_t;

        Object* makeCurrentObject(Panel*);

        SoundInputPeg* findSoundInputPeg(heap_id);
        SoundOutputPeg* findSoundOutputPeg(heap_id);
        NumberInputPeg* findNumberInputPeg(heap_id);
        NumberOutputPeg* findNumberOutputPeg(heap_id);

    private:
        using IterableObjects = std::vector<std::pair<std::vector<std::byte>, std::vector<std::byte>::const_iterator>>;

        IterableObjects m_objects;
        IterableObjects::iterator m_currentObject;

        const std::vector<std::byte>& currentObject();
        std::vector<std::byte>::const_iterator& currentIterator();

        // List of known sound/number input/output pegs.
        // Ids of these pegs are their indices in these vectors.
        // ids for different types of pegs are unrelated to eachother
        std::map<heap_id, SoundInputPeg*> m_soundInputPegs;
        std::map<heap_id, SoundOutputPeg*> m_soundOutputPegs;
        std::map<heap_id, NumberInputPeg*> m_numberInputPegs;
        std::map<heap_id, NumberOutputPeg*> m_numberOutputPegs;

        // lists of wire connections represented as (input peg id, output peg id) pairs
        std::vector<std::pair<heap_id, heap_id>> m_soundWires;
        std::vector<std::pair<heap_id, heap_id>> m_numberWires;

        using ObjectCreator = std::function<std::unique_ptr<Object>()>;

        // Used for converting from non-portable typeid() to portable
        // custom unique identifier
        static std::map<std::type_index, std::string>& getTypeIDMap();

        // Used for creating objects from portable unique identifiers
        static std::map<std::string, ObjectCreator>& getFactoryMap();

        static void addSerializable(std::type_index theClass, const std::string& identifier, ObjectCreator creator);

        static void removeSerializable(std::type_index theClass);
    };

    // Interface for polymorphic types
    class Serializable {
    public:
        Serializable() = default;
        virtual ~Serializable() = default;

        Serializable(Serializable&&) = delete;
        Serializable(const Serializable&) = delete;
        Serializable& operator=(Serializable&&) = delete;
        Serializable& operator=(const Serializable&) = delete;

        virtual void serialize(Serializer&) const = 0;
        virtual void deserialize(Deserializer&) = 0;
    };

} // namespace flui

#define REGISTER_SERIALIZABLE(ClassName, UniqueIdentifier) \
    namespace FlosionUISerializationImpl { \
        ::flui::Deserializer::Registrator<ClassName> s_registratorFor_##ClassName { UniqueIdentifier }; \
    }

// TODO: put the following in a .tpp file

namespace flui {

    template<typename ObjectType>
    inline Deserializer::Registrator<ObjectType>::Registrator(const std::string& identifier) {
        
        static_assert(std::is_base_of<Object, ObjectType>::value, "The provided object type must derive from Object");
        static_assert(std::is_default_constructible_v<ObjectType>, "The provided object type must be default constructible");

        auto tidx = std::type_index(typeid(ObjectType));
        auto creator = [](){ return std::make_unique<ObjectType>(); };
        
        Deserializer::addSerializable(tidx, identifier, creator);
    }

    template<typename ObjectType>
    inline Deserializer::Registrator<ObjectType>::~Registrator(){
        auto tidx = std::type_index(typeid(ObjectType));
        Deserializer::removeSerializable(tidx);
    }

} // namespace flui
