#pragma once

#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <vector>

// TODO: use std::span?

// TODO: worry about platform-dependent sizes of integers
// TODO: how can dangers around implicit conversions between integers
// be avoided? i.e. int could convert to std::int32_t or to std::int64_t

// TODO: worry about endianness
// see https://en.cppreference.com/w/cpp/types/endian

#ifndef _WIN64
    #error "This code is not portable. Sorry. Ask Tim to fix it."
#endif

namespace flui {

    namespace detail {

        using Blob = std::vector<unsigned char>;

        void appendRawBytes(Blob& v, const unsigned char* src, std::uint64_t bytes);

        void readRawBytesAndAdvance(const Blob& v, Blob::const_iterator& it, unsigned char* dst, std::uint64_t bytes);

        // TODO: do this portably
        // Assuming 64-bit Windows for now, so no different integer sizes or endianness
        template<typename T>
        void appendScalar(Blob& v, const T& s);

        // TODO: do this portably
        // Assuming 64-bit Windows for now, so no different integer sizes or endianness
        template<typename T>
        void readScalarAndAdvance(const Blob& v, Blob::const_iterator& it, T& s);

    } // namespace detail

    // TODO:
    // - binary mode
    //     - use big endian encoding in format
    //     - careful with text
    //     - input/output shall be vector<unsigned char>
    // - basic overloads or template specialization for primitives and common std library types
    // - abstract base class for polymorphic types
    //     - Doesn't need integration with factory
    // - heap structure, with unique ID per heap object
    //   - one object is considered root, or entry
    // - onComplete signal, for connecting wires
    //     - When deserializing, instantiate objects, then connect sound wires, then connect number wires,
    //       and all should be fine

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


    // Types are not serializable unless specialized
    template<typename T, typename Enable = void>
    struct is_serializable : std::false_type {};

    // Helper template variable
    template<typename T>
    constexpr bool is_serializable_v = is_serializable<T>::value;

    // Arithmetic types (bool, integers, char, floating point) are serializable
    template<typename T>
    struct is_serializable<
        T,
        typename std::enable_if_t<std::is_arithmetic_v<T>>
    > : std::true_type {};

    // Strings are serializable
    template<>
    struct is_serializable<std::string, void> : std::true_type {};

    // Vectors of serializable types are serializable
    template<typename T>
    struct is_serializable<
        std::vector<T>,
        typename std::enable_if_t<is_serializable_v<T>>
    > : std::true_type {};

    // TODO: more types?

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
        std::vector<unsigned char> dump() const;

        // When an Object is being serialized, it is responsible for adding all
        // of its pegs via these methods
        void addPeg(const SoundInputPeg*);
        void addPeg(const SoundOutputPeg*);
        void addPeg(const NumberInputPeg*);
        void addPeg(const NumberOutputPeg*);

    public:

        // Writes a sequence of bytes to the current object
        void writeBytes(const unsigned char* src, std::uint64_t bytes);

        // Writes a contiguous sequence of serializable values to the current object
        // NOTE: it is safe to later read this data using >> and vector<T>
        template<typename T, typename std::enable_if_t<is_serializable_v<T>>* = nullptr>
        void writeSpan(const T* data, std::uint64_t count);

        // Writes a simple arithmetic type
        template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
        Serializer& operator<<(const T&);

        // Writes a string
        Serializer& operator<<(const std::string&);

        // Writes a vector of serializable values
        // NOTE: it is safe to later read this data using readSpan
        template<typename T, typename std::enable_if_t<is_serializable_v<T>>* = nullptr>
        Serializer& operator<<(const std::vector<T>&);

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
        std::vector<detail::Blob> m_objects;

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

        Deserializer(const std::vector<unsigned char>& buffer);
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

        // Reads a sequence of bytes from the current object
        void readBytes(unsigned char* dst, std::uint64_t bytes);

        // Reads a contiguous sequence of serializable values from the current object
        // Throws an exception if the count is mismatched.
        // NOTE: it is safe to read a span that was written using << and vector<T>
        template<typename T, typename std::enable_if_t<is_serializable_v<T>>* = nullptr>
        void readSpan(T* data, std::uint64_t count);

        // Assuming there is a span to be read, peeks ahead to get the size of
        // that span. Only intended to be called right before readSpan()
        std::uint64_t peekSpanCount();

        template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
        Deserializer& operator>>(T&);

        Deserializer& operator>>(std::string&);

        template<typename T, typename std::enable_if_t<is_serializable_v<T>>* = nullptr>
        Deserializer& operator>>(std::vector<T>&);

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
        using heap_id = std::uint64_t;

        std::unique_ptr<Object> makeCurrentObject();

        SoundInputPeg* findSoundInputPeg(heap_id);
        SoundOutputPeg* findSoundOutputPeg(heap_id);
        NumberInputPeg* findNumberInputPeg(heap_id);
        NumberOutputPeg* findNumberOutputPeg(heap_id);

    private:
        using IterableObjects = std::vector<std::pair<detail::Blob, detail::Blob::const_iterator>>;

        IterableObjects m_objects;
        IterableObjects::iterator m_currentObject;

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

#include <cassert>

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

    template<typename T, typename std::enable_if_t<is_serializable_v<T>>*>
    inline void Serializer::writeSpan(const T* data, std::uint64_t count){
        assert(data);
        *this << count;
        for (std::uint64_t i = 0; i < count; ++i) {
            *this << data[i];
        }
    }

    template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>*>
    inline Serializer& Serializer::operator<<(const T& t){
        assert(m_objects.size() > 0);
        detail::appendScalar(m_objects.back(), t);
        return *this;
    }

    template<typename T, typename std::enable_if_t<is_serializable_v<T>>*>
    inline Serializer& Serializer::operator<<(const std::vector<T>& v){
        writeSpan(v.data(), v.size());
        return *this;
    }

    template<typename T, typename std::enable_if_t<is_serializable_v<T>>*>
    inline void Deserializer::readSpan(T* data, std::uint64_t count){
        assert(data);
        auto actualCount = std::uint64_t{};
        *this >> actualCount;
        if (actualCount != count) {
            throw SerializationException{};
        }
        for (std::uint64_t i = 0; i < count; ++i) {
            *this >> data[i];
        }
    }

    template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>*>
    inline Deserializer& Deserializer::operator>>(T& t){
        assert(m_currentObject != end(m_objects));
        const auto& v = m_currentObject->first;
        auto& it = m_currentObject->second;
        assert(it != end(v));
        detail::readScalarAndAdvance(v, it, t);
        return *this;
    }

    template<typename T, typename std::enable_if_t<is_serializable_v<T>>*>
    inline Deserializer& Deserializer::operator>>(std::vector<T>& v){
        const auto size = peekSpanCount();
        v.clear();
        v.resize(size);
        readSpan(v.data(), size);
        return *this;
    }

    template<typename T>
    inline void detail::appendScalar(Blob& v, const T& s) {
        const auto src = reinterpret_cast<const unsigned char*>(&s);
        const auto bytes = sizeof(T);
        appendRawBytes(v, src, bytes);
    }

    template<typename T>
    inline void detail::readScalarAndAdvance(const Blob& v, Blob::const_iterator& it, T& s) {
        const auto dst = reinterpret_cast<unsigned char*>(&s);
        const auto bytes = sizeof(T);
        readRawBytesAndAdvance(v, it, dst, bytes);
    }

} // namespace flui
