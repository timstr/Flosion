#include <Flosion/UI/Core/Serialization.hpp>

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/Panel.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>

#include <algorithm>
#include <cassert>
#include <numeric>
#include <type_traits>

namespace flui {

    namespace detail {

        enum class Flag : std::uint8_t {
            Byte = 0x01,
            Bool = 0x02,
            Uint8 = 0x03,
            Uint16 = 0x04,
            Uint32 = 0x05,
            Uint64 = 0x06,
            Int8 = 0x07,
            Int16 = 0x08,
            Int32 = 0x09,
            Int64 = 0x0A,
            Float32 = 0x0B,
            Float64 = 0x0C,

            ArrayOf = 0x20,
            ArrayLength = 0x21,

            PegID = 0x30,
            ObjectTypeID = 0x31,

            SectionObjects = 0x40,
            SectionSoundWires = 0x41,
            SectionNumberWires = 0x42

            // TODO: strings?
        };

        template<Flag TF>
        struct flag_size {};

        template<Flag TF>
        struct flag_type {};

#define SPECIALIZE_TAG(FLAGNAME, SIZE, TYPE) \
template<> struct flag_size<Flag::FLAGNAME> { \
    static_assert(sizeof(TYPE) == SIZE); \
    static constexpr std::size_t value = SIZE; \
}; \
template<> struct flag_type<Flag::FLAGNAME> { \
    typedef TYPE type; \
};

        SPECIALIZE_TAG(Byte, 1, std::byte);
        SPECIALIZE_TAG(Bool, 1, bool);
        SPECIALIZE_TAG(Uint8, 1, std::uint8_t);
        SPECIALIZE_TAG(Uint16, 2, std::uint16_t);
        SPECIALIZE_TAG(Uint32, 4, std::uint32_t);
        SPECIALIZE_TAG(Uint64, 8, std::uint64_t);
        SPECIALIZE_TAG(Int8, 1, std::int8_t);
        SPECIALIZE_TAG(Int16, 2, std::int16_t);
        SPECIALIZE_TAG(Int32, 4, std::int32_t);
        SPECIALIZE_TAG(Int64, 8, std::int64_t);
        SPECIALIZE_TAG(Float32, 4, float);
        SPECIALIZE_TAG(Float64, 8, double);

#undef SPECIALIZE_TAG

        template<Flag F>
        constexpr std::size_t flag_size_v = flag_size<F>::value;

        template<Flag F>
        using flag_type_t = typename flag_type<F>::type;

        void writeFlag(std::vector<std::byte>& v, Flag f) {
            static_assert(sizeof(Flag) == 1);
            const auto flag_b = *reinterpret_cast<const std::byte*>(&f);
            v.push_back(flag_b);
        }

        Flag readFlag(const std::vector<std::byte>& v, std::vector<std::byte>::const_iterator& it) {
            static_assert(sizeof(Flag) == 1);
            if (it == end(v)) {
                throw SerializationException{};
            }
            const std::byte* b_ptr = &*it;
            const auto flag = *reinterpret_cast<const Flag*>(b_ptr);
            ++it;
            return flag;
        }


        template<Flag TF, typename T>
        void writeScalarUnchecked(std::vector<std::byte>& v, const T& x) {
            static_assert(std::is_same_v<flag_type_t<TF>, T>);

            const auto x_bptr = reinterpret_cast<const std::byte*>(&x);

            // TODO: handle endianness right here
            const auto numBytes = flag_size_v<TF>;
            if constexpr (numBytes == 1) {
                v.push_back(x_bptr[0]);
            } else if constexpr (numBytes == 2) {
                v.push_back(x_bptr[0]);
                v.push_back(x_bptr[1]);
            } else if constexpr (numBytes == 4) {
                v.push_back(x_bptr[0]);
                v.push_back(x_bptr[1]);
                v.push_back(x_bptr[2]);
                v.push_back(x_bptr[3]);
            } else if constexpr (numBytes == 8) {
                v.push_back(x_bptr[0]);
                v.push_back(x_bptr[1]);
                v.push_back(x_bptr[2]);
                v.push_back(x_bptr[3]);
                v.push_back(x_bptr[4]);
                v.push_back(x_bptr[5]);
                v.push_back(x_bptr[6]);
                v.push_back(x_bptr[7]);
            } else {
                static_assert(std::false_type<T>::value);
            }
        }

        template<Flag TF, typename T>
        void writeScalar(std::vector<std::byte>& v, const T& x) {
            static_assert(std::is_same_v<flag_type_t<TF>, T>);

            writeFlag(v, TF);
            writeScalarUnchecked<TF>(v, x);
        }

        template<Flag F>
        flag_type_t<F> readScalarUnchecked(const std::vector<std::byte>& v, std::vector<std::byte>::const_iterator& it) {
            flag_type_t<F> x;
            const auto x_bptr = reinterpret_cast<std::byte*>(&x);
            const std::byte* b_ptr = &*it;

            // TODO: handle endianness right here
            const auto numBytes = flag_size_v<TF>;
            if (distance(it, end(v)) < numBytes) {
                throw SerializationException{};
            }
            if constexpr (numBytes == 1) {
                x_bptr[0] = b_ptr[0];
            } else if constexpr (numBytes == 2) {
                x_bptr[0] = b_ptr[0];
                x_bptr[1] = b_ptr[1];
            } else if constexpr (numBytes == 4) {
                x_bptr[0] = b_ptr[0];
                x_bptr[1] = b_ptr[1];
                x_bptr[2] = b_ptr[2];
                x_bptr[3] = b_ptr[3];
            } else if constexpr (numBytes == 8) {
                x_bptr[0] = b_ptr[0];
                x_bptr[1] = b_ptr[1];
                x_bptr[2] = b_ptr[2];
                x_bptr[3] = b_ptr[3];
                x_bptr[4] = b_ptr[4];
                x_bptr[5] = b_ptr[5];
                x_bptr[6] = b_ptr[6];
                x_bptr[7] = b_ptr[7];
            } else {
                static_assert(std::false_type<T>::value);
            }

            std::advance(it, numBytes);
            return x;
        }

        template<Flag TF>
        flag_type_t<TF> readScalar(const std::vector<std::byte>& v, std::vector<std::byte>::const_iterator& it) {

            auto f = readFlag(v, it);
            if (f != TF) {
                throw SerializationException{};
            }

            return readScalarUnchecked<TF>(v, it);
        }

        template<Flag TF>
        void writeSpan(std::vector<std::byte>& v, const flag_type_t<TF>* src, std::uint64_t len) {
            writeFlag(v, Flag::ArrayOf);
            writeFlag(v, TF);
            writeFlag(v, Flag::ArrayLength);
            writeScalarUnchecked<Flag::Uint64>(v, len);
            for (std::uint64_t i = 0; i < len; ++i) {
                writeScalarUnchecked<TF>(v, src[i]);
            }
        }

        std::uint64_t peekSpanLength(const std::vector<std::byte>& v, std::vector<std::byte>::const_iterator& it) {
            const auto prevIt = it;
            if (readFlag(v, it) != Flag::ArrayOf) {
                throw SerializationException{};
            }
            readFlag(v, it); // Array element type is not checked
            if (readFlag(v, it) != Flag::ArrayLength) {
                throw SerializationException{};
            }
            const auto len = readScalarUnchecked<Flag::Uint64>(v, it);
            it = prevIt;
            return len;
        }

        template<Flag TF>
        void readSpan(const std::vector<std::byte>& v, std::vector<std::byte>::const_iterator& it, flag_type_t<TF>* dst, std::uint64_t len) {
            if (readFlag(v, it) != Flag::ArrayOf) {
                throw SerializationException{};
            }
            if (readFlag(v, it) != TF) {
                throw SerializationException{};
            }
            if (readFlag(v, it) != Flag::ArrayLength) {
                throw SerializationException{};
            }
            if (readScalarUnchecked<Flag::Uint64>(v, it) != len) {
                throw SerializationException{};
            }
            for (std::uint64_t i = 0; i < len; ++i) {
                dst[i] = readScalarUnchecked<TF>(v, it);
            }
        }

        // TODO: worry about UTF8
        void writeString(std::vector<std::byte>& v, const std::string& s) {
            const auto data = reinterpret_cast<const std::byte*>(s.data());
            const auto len = static_cast<std::uint64_t>(s.size());
            writeSpan<Flag::Byte>(v, data, len);
        }

        std::string readString(const std::vector<std::byte>& v, std::vector<std::byte>::const_iterator& it) {
            std::string x;
            const auto len = peekSpanLength(v, it);
            x.resize(len, '\0');
            auto data = reinterpret_cast<std::byte*>(x.data());
            readSpan<Flag::Byte>(v, it, data, len);
            return x;
        }

    } // namespace detail

    const char* SerializationException::what() const noexcept {
        return "Serialization failed.";
    }

    void Serializer::serializeFrom(const Panel* panel, const std::vector<const Object*>& whichObjects){
        assert(panel);

        assert(m_objects.empty());
        assert(m_numberInputPegs.empty());
        assert(m_numberOutputPegs.empty());
        assert(m_soundInputPegs.empty());
        assert(m_soundOutputPegs.empty());
        assert(m_numberWires.empty());
        assert(m_soundWires.empty());

        // get pointers to all objects if whichObjects in case is empty
        const auto objects = whichObjects.size() > 0 ? whichObjects : panel->getObjects();

        assert(all_of(
            begin(objects),
            end(objects),
            [&](const Object* op) {
                return op != nullptr && op->getParentPanel() == panel;
            }
        ));
        
        // serialize all objects. Objects serialize their state and register their pegs
        for (const auto& obj : objects) {
            addObject(obj);
        }

        assert(m_numberWires.empty());
        assert(m_soundWires.empty());

        // serialize all wires as simple pairs of peg ids
        for (const auto& w : panel->getSoundWires()) {
            tryAddWire(w);
        }

        for (const auto& w : panel->getNumberWires()) {
            tryAddWire(w);
        }
    }

    std::vector<std::byte> Serializer::dump() const {
        using namespace detail;

        const auto size_of_uint64 = flag_size_v<Flag::Uint64>;

        // section flag, plus number of objects, plus list of size-data pairs
        const auto size_of_objects = 1 + size_of_uint64 + std::accumulate(
            begin(m_objects),
            end(m_objects),
            std::uint64_t{ 0 },
            [&](std::uint64_t acc, const std::vector<std::byte>& v){ return acc + size_of_uint64 + v.size(); }
        );

        const auto size_of_pair = 2 * sizeof(heap_id);

        // section flag, plus number of wires, plus pair for each wire
        const auto size_of_sound_wires = 1 + size_of_uint64 + m_soundWires.size() * size_of_pair;
        const auto size_of_number_wires = 1 + size_of_uint64 + m_numberWires.size() * size_of_pair;

        const auto total_size = size_of_objects + size_of_sound_wires + size_of_number_wires;


        std::vector<std::byte> ret;
        ret.reserve(total_size);

        // dump total number of objects
        writeFlag(ret, Flag::SectionObjects);
        writeScalar<Flag::Uint64>(ret, m_objects.size());
        // dump all objects
        for (const auto& obj : m_objects) {
            // dump size of object data
            writeScalar<Flag::Uint64>(ret, obj.size());
            // dump object data
            copy(begin(obj), end(obj), back_inserter(ret));
        }

        assert(ret.size() == size_of_objects);
        if (ret.size() != size_of_objects) {
            throw SerializationException{};
        }

        // Note: peg ids don't need to be stored explicitly

        // dump number of sound wires
        writeFlag(ret, Flag::SectionSoundWires);
        writeScalar<Flag::Uint64>(ret, m_soundWires.size());
        // dump all sound wires
        for (const auto& w : m_soundWires) {
            writeFlag(ret, Flag::PegID);
            writeScalar<Flag::Uint64>(ret, w.first);
            writeFlag(ret, Flag::PegID);
            writeScalar<Flag::Uint64>(ret, w.second);
        }

        assert(ret.size() == size_of_objects + size_of_sound_wires);

        // dump number of number wires
        writeFlag(ret, Flag::SectionNumberWires);
        writeScalar<Flag::Uint64>(ret, m_numberWires.size());
        // dump all number wires
        for (const auto& w : m_numberWires) {
            writeFlag(ret, Flag::PegID);
            writeScalar<Flag::Uint64>(ret, w.first);
            writeFlag(ret, Flag::PegID);
            writeScalar<Flag::Uint64>(ret, w.second);
        }

        assert(ret.size() == total_size);

        return ret;
    }

    void Serializer::addPeg(const SoundInputPeg* p) {
        assert(p);
        assert(count(begin(m_soundInputPegs), end(m_soundInputPegs), p) == 0);
        auto& currentObj = getCurrentObject();
        const auto id = static_cast<heap_id>(m_soundInputPegs.size());
        m_soundInputPegs.push_back(p);
        detail::writeFlag(currentObj, detail::Flag::PegID);
        detail::writeScalarUnchecked<detail::Flag::Uint64>(currentObj, id);
    }

    void Serializer::addPeg(const SoundOutputPeg* p) {
        assert(p);
        assert(count(begin(m_soundOutputPegs), end(m_soundOutputPegs), p) == 0);
        auto& currentObj = getCurrentObject();
        const auto id = static_cast<heap_id>(m_soundOutputPegs.size());
        m_soundOutputPegs.push_back(p);
        detail::writeFlag(currentObj, detail::Flag::PegID);
        detail::writeScalarUnchecked<detail::Flag::Uint64>(currentObj, id);
    }

    void Serializer::addPeg(const NumberInputPeg* p) {
        assert(p);
        assert(count(begin(m_numberInputPegs), end(m_numberInputPegs), p) == 0);
        auto& currentObj = getCurrentObject();
        const auto id = static_cast<heap_id>(m_numberInputPegs.size());
        m_numberInputPegs.push_back(p);
        detail::writeFlag(currentObj, detail::Flag::PegID);
        detail::writeScalarUnchecked<detail::Flag::Uint64>(currentObj, id);
    }

    void Serializer::addPeg(const NumberOutputPeg* p) {
        assert(p);
        assert(count(begin(m_numberOutputPegs), end(m_numberOutputPegs), p) == 0);
        auto& currentObj = getCurrentObject();
        const auto id = static_cast<heap_id>(m_numberOutputPegs.size());
        m_numberOutputPegs.push_back(p);
        detail::writeFlag(currentObj, detail::Flag::PegID);
        detail::writeScalarUnchecked<detail::Flag::Uint64>(currentObj, id);
    }

    Serializer& Serializer::b(bool x){
        detail::writeScalar<detail::Flag::Bool>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::u8(std::uint8_t x){
        detail::writeScalar<detail::Flag::Uint8>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::u16(std::uint16_t x){
        detail::writeScalar<detail::Flag::Uint16>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::u32(std::uint32_t x){
        detail::writeScalar<detail::Flag::Uint32>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::u64(std::uint64_t x){
        detail::writeScalar<detail::Flag::Uint64>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::i8(std::int8_t x){
        detail::writeScalar<detail::Flag::Int8>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::i16(std::int16_t x){
        detail::writeScalar<detail::Flag::Int16>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::i32(std::int32_t x){
        detail::writeScalar<detail::Flag::Int32>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::i64(std::int64_t x){
        detail::writeScalar<detail::Flag::Int64>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::f32(float x){
        detail::writeScalar<detail::Flag::Float32>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::f64(double x){
        detail::writeScalar<detail::Flag::Float64>(getCurrentObject(), x);
        return *this;
    }

    Serializer& Serializer::str(const std::string& x){
        detail::writeString(getCurrentObject(), x);
        return *this;
    }

    void Serializer::addObject(const Object* o) {
        assert(o);
        m_objects.emplace_back();
        auto& v = getCurrentObject();
        detail::writeFlag(v, detail::Flag::ObjectTypeID);
        str(Deserializer::getObjectIdentifier(o));
        f32(o->left());
        f32(o->top());
        o->serialize(*this);
    }

    bool Serializer::tryAddWire(const SoundWire* w) {
        assert(w);
        const auto hp = w->getHeadPeg();
        const auto tp = w->getTailPeg();
        assert(hp);
        assert(tp);
        const auto iid = findID(hp);
        const auto oid = findID(tp);

        if (!iid.has_value() || !oid.has_value()) {
            return false;
        }

        m_soundWires.push_back({*oid, *iid});
        return true;
    }

    bool Serializer::tryAddWire(const NumberWire* w){
        assert(w);
        const auto hp = w->getHeadPeg();
        const auto tp = w->getTailPeg();
        assert(hp);
        assert(tp);
        const auto iid = findID(hp);
        const auto oid = findID(tp);

        if (!iid.has_value() || !oid.has_value()) {
            return false;
        }

        m_numberWires.push_back({*oid, *iid});
        return true;
    }

    std::optional<Serializer::heap_id> Serializer::findID(const SoundInputPeg* p) const {
        const auto first = begin(m_soundInputPegs);
        const auto last = end(m_soundInputPegs);
        assert(count(first, last, p) <= 1);
        auto it = find(first, last, p);
        if (it == last) {
            return std::nullopt;
        }
        return static_cast<heap_id>(it - first);
    }

    std::optional<Serializer::heap_id> Serializer::findID(const SoundOutputPeg* p) const {
        const auto first = begin(m_soundOutputPegs);
        const auto last = end(m_soundOutputPegs);
        assert(count(first, last, p) <= 1);
        auto it = find(first, last, p);
        if (it == last) {
            return std::nullopt;
        }
        return static_cast<heap_id>(it - first);
    }

    std::optional<Serializer::heap_id> Serializer::findID(const NumberInputPeg* p) const {
        const auto first = begin(m_numberInputPegs);
        const auto last = end(m_numberInputPegs);
        assert(count(first, last, p) <= 1);
        auto it = find(first, last, p);
        if (it == last) {
            return std::nullopt;
        }
        return static_cast<heap_id>(it - first);
    }

    std::optional<Serializer::heap_id> Serializer::findID(const NumberOutputPeg* p) const {
        const auto first = begin(m_numberOutputPegs);
        const auto last = end(m_numberOutputPegs);
        assert(count(first, last, p) <= 1);
        auto it = find(first, last, p);
        if (it == last) {
            return std::nullopt;
        }
        return static_cast<heap_id>(it - first);
    }

    std::vector<std::byte>& Serializer::getCurrentObject() {
        assert(m_objects.size() > 0);
        return m_objects.back();
    }

    Deserializer::Deserializer(const std::vector<std::byte>& buffer)
        : m_objects()
        , m_currentObject(end(m_objects)) {

        using namespace detail;

        assert(buffer.size() > 0);

        auto it = begin(buffer);

        // parse object buffers
        if (readFlag(buffer, it) != Flag::SectionObjects) {
            throw SerializationException{};
        }
        const auto numObjects = readScalar<Flag::Uint64>(buffer, it);
        m_objects.reserve(numObjects);

        for (std::uint64_t i = 0; i < numObjects; ++i) {
            const auto objLen = readScalar<Flag::Uint64>(buffer, it);

            auto objBuf = std::vector<std::byte>(objLen);
            for (std::uint64_t j = 0; j < objLen; ++j) {
                objBuf[j] = *it;
                ++it;
            }
            m_objects.push_back({ std::move(objBuf), {} });

            // point iterators to ends, to make sure nothing is read yet
            m_objects.back().second = end(m_objects.back().first);
            m_currentObject = m_objects.end();
        }

        // parse sound wires
        if (readFlag(buffer, it) != Flag::SectionSoundWires) {
            throw SerializationException{};
        }
        auto numSoundWires = readScalar<Flag::Uint64>(buffer, it);
        m_soundWires.reserve(numSoundWires);
        for (std::uint64_t i = 0; i < numSoundWires; ++i) {
            if (readFlag(buffer, it) != Flag::PegID) {
                throw SerializationException{};
            }
            const auto a = readScalar<Flag::Uint64>(buffer, it);
            if (readFlag(buffer, it) != Flag::PegID) {
                throw SerializationException{};
            }
            const auto b = readScalar<Flag::Uint64>(buffer, it);
            m_soundWires.push_back({a, b});
        }

        // parse number wires
        if (readFlag(buffer, it) != Flag::SectionNumberWires) {
            throw SerializationException{};
        }
        auto numNumberWires = readScalar<Flag::Uint64>(buffer, it);
        m_numberWires.reserve(numNumberWires);
        for (std::uint64_t i = 0; i < numNumberWires; ++i) {
            if (readFlag(buffer, it) != Flag::PegID) {
                throw SerializationException{};
            }
            const auto a = readScalar<Flag::Uint64>(buffer, it);
            if (readFlag(buffer, it) != Flag::PegID) {
                throw SerializationException{};
            }
            const auto b = readScalar<Flag::Uint64>(buffer, it);
            m_numberWires.push_back({a, b});
        }

        if (it != end(buffer)) {
            throw SerializationException{};
        }
    }
    
    std::vector<Object*> Deserializer::deserializeTo(Panel* panel){
        std::vector<Object*> ret;

        // create objects and deserialize them, during which they
        // (should) register their pegs
        assert(m_currentObject == begin(m_objects));
        for (; m_currentObject != end(m_objects); ++m_currentObject) {
            const auto& v = m_currentObject->first;
            auto& it = m_currentObject->second;
            assert(it == end(v));
            it = begin(v);
            auto obj = makeCurrentObject();
            assert(it == end(v));
            const auto ptr = obj.get();
            ret.push_back(ptr);

            // TODO: I hate this
            panel->addObject(ptr);
            panel->adopt(std::move(obj));
        }

        // create and connect all wires to the newly created pegs
        for (const auto& w : m_soundWires) {
            const auto& iid = w.first;
            const auto& oid = w.second;
            const auto hp = findSoundInputPeg(iid);
            const auto tp = findSoundOutputPeg(oid);
            panel->addWire(tp->getOutput(), hp->getInput());
        }

        return ret;
    }

    void Deserializer::addPeg(SoundInputPeg* p) {
        assert(p);
        const auto& v = currentObject();
        auto& it = currentIterator();
        if (detail::readFlag(v, it) != detail::Flag::PegID) {
            throw SerializationException{};
        }
        const auto id = detail::readScalarUnchecked<detail::Flag::Uint64>(v, it);
        assert(m_soundInputPegs.find(id) == end(m_soundInputPegs));
        m_soundInputPegs.emplace(std::make_pair(id, p));
    }

    void Deserializer::addPeg(SoundOutputPeg* p){
        assert(p);
        const auto& v = currentObject();
        auto& it = currentIterator();
        if (detail::readFlag(v, it) != detail::Flag::PegID) {
            throw SerializationException{};
        }
        const auto id = detail::readScalarUnchecked<detail::Flag::Uint64>(v, it);
        assert(m_soundOutputPegs.find(id) == end(m_soundOutputPegs));
        m_soundOutputPegs.emplace(std::make_pair(id, p));
    }

    void Deserializer::addPeg(NumberInputPeg* p){
        assert(p);
        const auto& v = currentObject();
        auto& it = currentIterator();
        if (detail::readFlag(v, it) != detail::Flag::PegID) {
            throw SerializationException{};
        }
        const auto id = detail::readScalarUnchecked<detail::Flag::Uint64>(v, it);
        assert(m_numberInputPegs.find(id) == end(m_numberInputPegs));
        m_numberInputPegs.emplace(std::make_pair(id, p));
    }

    void Deserializer::addPeg(NumberOutputPeg* p){
        assert(p);
        const auto& v = currentObject();
        auto& it = currentIterator();
        if (detail::readFlag(v, it) != detail::Flag::PegID) {
            throw SerializationException{};
        }
        const auto id = detail::readScalarUnchecked<detail::Flag::Uint64>(v, it);
        assert(m_numberOutputPegs.find(id) == end(m_numberOutputPegs));
        m_numberOutputPegs.emplace(std::make_pair(id, p));
    }

    Deserializer& Deserializer::b(bool& x){
        x = detail::readScalar<detail::Flag::Bool>(currentObject(), currentIterator());
        return *this;
    }

    const std::string& Deserializer::getObjectIdentifier(const Object* o){
        assert(o);
        auto tidx = std::type_index(typeid(*o));
        const auto& typeMap = getTypeIDMap();
        auto it = typeMap.find(tidx);
        if (it == end(typeMap)) {
            throw SerializationException{};
        }
        return it->second;
    }

    std::unique_ptr<Object> Deserializer::makeObject(const std::string& identifier) {
        const auto& factoryMap = getFactoryMap();
        auto it = factoryMap.find(identifier);
        if (it == factoryMap.end()) {
            throw SerializationException{};
        }
        return it->second();
    }

    std::unique_ptr<Object> Deserializer::makeCurrentObject(){
        const auto& v = currentObject();
        auto& it = currentIterator();
        if (detail::readFlag(v, it) != detail::Flag::PegID) {
            throw SerializationException{};
        }
        assert(it == begin(v));
        auto id = str();
        auto l = f32();
        auto t = f32();
        auto obj = Deserializer::makeObject(id);
        assert(obj);
        obj->setPos({l, t});
        return obj;
    }

    SoundInputPeg* Deserializer::findSoundInputPeg(heap_id id){
        auto it = m_soundInputPegs.find(id);
        assert(it != end(m_soundInputPegs));
        return it->second;
    }

    SoundOutputPeg* Deserializer::findSoundOutputPeg(heap_id id){
        auto it = m_soundOutputPegs.find(id);
        assert(it != end(m_soundOutputPegs));
        return it->second;
    }

    NumberInputPeg* Deserializer::findNumberInputPeg(heap_id id){
        auto it = m_numberInputPegs.find(id);
        assert(it != end(m_numberInputPegs));
        return it->second;
    }

    NumberOutputPeg* Deserializer::findNumberOutputPeg(heap_id id){
        auto it = m_numberOutputPegs.find(id);
        assert(it != end(m_numberOutputPegs));
        return it->second;
    }

    const std::vector<std::byte>& Deserializer::currentObject() {
        assert(m_currentObject != end(m_objects));
        return m_currentObject->first;
    }

    std::vector<std::byte>::const_iterator& Deserializer::currentIterator() {
        assert(m_currentObject != end(m_objects));
        return m_currentObject->second;
    }

    std::map<std::type_index, std::string>& Deserializer::getTypeIDMap() {
        static std::map<std::type_index, std::string> theMap;
        return theMap;
    }

    std::map<std::string, Deserializer::ObjectCreator>& Deserializer::getFactoryMap() {
        static std::map<std::string, Deserializer::ObjectCreator> theMap;
        return theMap;
    }

    void Deserializer::addSerializable(std::type_index theClass, const std::string& identifier, ObjectCreator creator) {
        auto& typeMap = getTypeIDMap();
        assert(typeMap.find(theClass) == end(typeMap));
        typeMap.insert(make_pair(theClass, identifier));

        auto& factoryMap = getFactoryMap();
        assert(factoryMap.find(identifier) == end(factoryMap));
        factoryMap.insert(make_pair(identifier, creator));
    }

    void Deserializer::removeSerializable(std::type_index theClass){
        auto& typeMap = getTypeIDMap();
        auto it = typeMap.find(theClass);
        assert(it != end(typeMap));
        const auto& identifier = it->second;
        auto& factoryMap = getFactoryMap();
        auto it2 = factoryMap.find(identifier);
        assert(it2 != factoryMap.end());
        factoryMap.erase(it2);
        // NOTE: identifier will be a dangling reference once the class
        // is removed from typeMap, so this is done last
        typeMap.erase(it);
    }

    Serializer& Serializer::i16_span(const std::int16_t* src, std::uint64_t len){
        detail::writeSpan<detail::Flag::Int16>(getCurrentObject(), src, len);
        return *this;
    }

    bool Deserializer::b(){
        return detail::readScalar<detail::Flag::Bool>(currentObject(), currentIterator());
    }

    std::uint8_t Deserializer::u8(){
        return detail::readScalar<detail::Flag::Uint8>(currentObject(), currentIterator());
    }

    std::uint16_t Deserializer::u16(){
        return detail::readScalar<detail::Flag::Uint16>(currentObject(), currentIterator());
    }

    std::uint32_t Deserializer::u32(){
        return detail::readScalar<detail::Flag::Uint32>(currentObject(), currentIterator());
    }

    std::uint64_t Deserializer::u64(){
        return detail::readScalar<detail::Flag::Uint64>(currentObject(), currentIterator());
    }

    std::int8_t Deserializer::i8(){
        return detail::readScalar<detail::Flag::Int8>(currentObject(), currentIterator());
    }

    std::int16_t Deserializer::i16(){
        return detail::readScalar<detail::Flag::Int16>(currentObject(), currentIterator());
    }

    std::int32_t Deserializer::i32(){
        return detail::readScalar<detail::Flag::Int32>(currentObject(), currentIterator());
    }

    std::int64_t Deserializer::i64(){
        return detail::readScalar<detail::Flag::Int64>(currentObject(), currentIterator());
    }

    float Deserializer::f32(){
        return detail::readScalar<detail::Flag::Float32>(currentObject(), currentIterator());
    }

    double Deserializer::f64(){
        return detail::readScalar<detail::Flag::Float64>(currentObject(), currentIterator());
    }

    std::string Deserializer::str(){
        return detail::readString(currentObject(), currentIterator());
    }

    std::uint64_t Deserializer::peekSpanLength(){
        return detail::peekSpanLength(currentObject(), currentIterator());
    }

    Deserializer& Deserializer::i16_span(std::int16_t* dst, std::uint64_t len){
        detail::readSpan<detail::Flag::Int16>(currentObject(), currentIterator(), dst, len);
        return *this;
    }

    std::vector<std::int16_t> Deserializer::i16_vec(){
        auto ret = std::vector<std::int16_t>(peekSpanLength());
        i16_span(ret.data(), ret.size());
        return ret;
    }

} // namespace flui

