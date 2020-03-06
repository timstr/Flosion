#include <Flosion/UI/Core/Serialization.hpp>

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/Panel.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>

#include <algorithm>
#include <cassert>
#include <numeric>

// TODO: throw an exception instead of asserting where appropriate

namespace flui {

    namespace detail {

        void appendRawBytes(Blob& v, const unsigned char* src, std::uint64_t bytes) {
            assert(src);
            assert(bytes > 0);
            if (src == nullptr || bytes == 0) {
                throw SerializationException{};
            }
            const auto s = v.size();
            v.resize(s + bytes);
            std::copy(src, src + bytes, begin(v) + s);
        }

        void readRawBytesAndAdvance(const Blob& v, Blob::const_iterator& it, unsigned char* dst, std::uint64_t bytes) {
            const auto good = 
                dst != nullptr
                && bytes > 0
                && it >= begin(v)
                && it < end(v)
                && static_cast<std::uint64_t>(distance(it, end(v))) >= bytes;
            if (!good) {
                throw SerializationException{};
            }
            std::copy(it, it + bytes, dst);
            advance(it, bytes);
        }

        // TODO: prepnd these flags for safety
        enum class TypeFlag : std::uint8_t {
            UChar       = 0x01,
            Bool        = 0x02,
            Uint8       = 0x03,
            Uint16      = 0x04,
            Uint32      = 0x05,
            Uint64      = 0x06,
            Int8        = 0x07,
            Int16       = 0x08,
            Int32       = 0x09,
            Int64       = 0x0A,
            Float32     = 0x0B,
            Float64     = 0x0C,
            UTF8Str     = 0x0D,
            ArrayOf     = 0x0E,
            ArrayLen    = 0x0F
        };

        constexpr int NumFlagRepetititions = 4;

        void appendTypeFlag(Blob& v, TypeFlag f) {
            for (int i = 0; i < NumFlagRepetititions; ++i) {
                v.push_back(static_cast<unsigned char>(f));
            }
        }

        void readAndCheckTypeFlag(const Blob& v, Blob::const_iterator& it, TypeFlag expectedFlag) {
            for (int i = 0; i < NumFlagRepetititions; ++i) {
                if (it == end(v)) {
                    // Too short
                    throw SerializationException{};
                }
                if (static_cast<unsigned char>(expectedFlag) != *it) {
                    // Bad type flag
                    throw SerializationException{};
                }
                ++it;
            }
        }

    } // namespace detail

    const char* SerializationException::what() const noexcept {
        return "Serialization failed.";
    }

    void Serializer::writeBytes(const unsigned char* src, std::uint64_t bytes) {
        assert(src);
        assert(m_objects.size() > 0);
        if (src == nullptr) {
            throw SerializationException{};
        }
        auto& currentObj = m_objects.back();
        detail::appendRawBytes(currentObj, src, bytes);
    }

    Serializer& Serializer::operator<<(const std::string& s){
        assert(m_objects.size() > 0);
        const auto len = s.size();
        detail::appendScalar(m_objects.back(), static_cast<std::uint64_t>(len));
        const auto data = reinterpret_cast<const unsigned char*>(s.data());
        detail::appendRawBytes(m_objects.back(), data, len);
        return *this;
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

    std::vector<unsigned char> Serializer::dump() const {
        const auto size_of_uint64 = sizeof(std::uint64_t);

        // number of objects, plus list of size, data pairs
        const auto size_of_objects = size_of_uint64 + std::accumulate(
            begin(m_objects),
            end(m_objects),
            std::uint64_t{ 0 },
            [&](std::uint64_t acc, const std::vector<unsigned char>& v){ return acc + size_of_uint64 + v.size(); }
        );

        const auto size_of_pair = 2 * sizeof(heap_id);

        // number of wires, plus pair for each wire
        const auto size_of_sound_wires = size_of_uint64 + m_soundWires.size() * size_of_pair;
        const auto size_of_number_wires = size_of_uint64 + m_numberWires.size() * size_of_pair;

        const auto total_size = size_of_objects + size_of_sound_wires + size_of_number_wires;


        std::vector<unsigned char> ret;
        ret.reserve(total_size);

        // dump total number of objects
        detail::appendScalar(ret, static_cast<std::uint64_t>(m_objects.size()));
        // dump all objects
        for (const auto& obj : m_objects) {
            // dump size of object data
            detail::appendScalar(ret, static_cast<std::uint64_t>(obj.size()));
            // dump object data
            copy(begin(obj), end(obj), back_inserter(ret));
        }

        assert(ret.size() == size_of_objects);
        if (ret.size() != size_of_objects) {
            throw SerializationException{};
        }

        // Note: peg ids don't need to be stored explicitly

        // dump number of sound wires
        detail::appendScalar(ret, static_cast<std::uint64_t>(m_soundWires.size()));
        // dump all sound wires
        for (const auto& w : m_soundWires) {
            detail::appendScalar(ret, w.first);
            detail::appendScalar(ret, w.second);
        }

        assert(ret.size() == size_of_objects + size_of_sound_wires);

        // dump number of number wires
        detail::appendScalar(ret, static_cast<std::uint64_t>(m_numberWires.size()));
        // dump all number wires
        for (const auto& w : m_numberWires) {
            detail::appendScalar(ret, w.first);
            detail::appendScalar(ret, w.second);
        }

        assert(ret.size() == total_size);

        return ret;
    }

    void Serializer::addPeg(const SoundInputPeg* p) {
        assert(p);
        assert(count(begin(m_soundInputPegs), end(m_soundInputPegs), p) == 0);
        assert(m_objects.size() > 0);
        auto& currentObj = m_objects.back();
        const auto id = static_cast<heap_id>(m_soundInputPegs.size());
        m_soundInputPegs.push_back(p);
        detail::appendScalar(currentObj, id);
    }

    void Serializer::addPeg(const SoundOutputPeg* p) {
        assert(p);
        assert(count(begin(m_soundOutputPegs), end(m_soundOutputPegs), p) == 0);
        assert(m_objects.size() > 0);
        auto& currentObj = m_objects.back();
        const auto id = static_cast<heap_id>(m_soundOutputPegs.size());
        m_soundOutputPegs.push_back(p);
        detail::appendScalar(currentObj, id);
    }

    void Serializer::addPeg(const NumberInputPeg* p) {
        assert(p);
        assert(count(begin(m_numberInputPegs), end(m_numberInputPegs), p) == 0);
        assert(m_objects.size() > 0);
        auto& currentObj = m_objects.back();
        const auto id = static_cast<heap_id>(m_numberInputPegs.size());
        m_numberInputPegs.push_back(p);
        detail::appendScalar(currentObj, id);
    }

    void Serializer::addPeg(const NumberOutputPeg* p) {
        assert(p);
        assert(count(begin(m_numberOutputPegs), end(m_numberOutputPegs), p) == 0);
        assert(m_objects.size() > 0);
        auto& currentObj = m_objects.back();
        const auto id = static_cast<heap_id>(m_numberOutputPegs.size());
        m_numberOutputPegs.push_back(p);
        detail::appendScalar(currentObj, id);
    }

    void Serializer::addObject(const Object* o) {
        assert(o);
        m_objects.emplace_back();
        *this << Deserializer::getObjectIdentifier(o);
        *this << o->left() << o->top();
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

    Deserializer::Deserializer(const std::vector<unsigned char>& buffer)
        : m_objects()
        , m_currentObject(end(m_objects)) {

        assert(buffer.size() > 0);

        auto it = begin(buffer);

        // parse object buffers
        auto numObjects = std::uint64_t{};
        detail::readScalarAndAdvance(buffer, it, numObjects);
        m_objects.reserve(numObjects);

        for (std::uint64_t i = 0; i < numObjects; ++i) {
            auto objLen = std::uint64_t{};
            detail::readScalarAndAdvance(buffer, it, objLen);
            auto objBuf = std::vector<unsigned char>(objLen);
            m_objects.push_back({ std::move(objBuf), {} });

            // point iterators to ends, to make sure nothing is read yet
            m_objects.back().second = end(m_objects.back().first);
            m_currentObject = m_objects.end();
        }

        // parse sound wires
        auto numSoundWires = std::uint64_t{};
        detail::readScalarAndAdvance(buffer, it, numSoundWires);
        m_soundWires.reserve(numSoundWires);
        for (std::uint64_t i = 0; i < numSoundWires; ++i) {
            auto a = heap_id{};
            auto b = heap_id{};
            detail::readScalarAndAdvance(buffer, it, a);
            detail::readScalarAndAdvance(buffer, it, b);
            m_soundWires.push_back({a, b});
        }

        // parse number wires
        auto numNumberWires = std::uint64_t{};
        detail::readScalarAndAdvance(buffer, it, numNumberWires);
        m_numberWires.reserve(numNumberWires);
        for (std::uint64_t i = 0; i < numNumberWires; ++i) {
            auto a = heap_id{};
            auto b = heap_id{};
            detail::readScalarAndAdvance(buffer, it, a);
            detail::readScalarAndAdvance(buffer, it, b);
            m_numberWires.push_back({a, b});
        }

        if (it != end(buffer)) {
            throw SerializationException{};
        }
    }

    void Deserializer::readBytes(unsigned char* dst, std::uint64_t bytes){
        assert(dst);
        assert(m_objects.size() > 0);
        assert(m_currentObject >= m_objects.begin());
        assert(m_currentObject < m_objects.end());
        detail::readRawBytesAndAdvance(m_currentObject->first, m_currentObject->second, dst, bytes);
    }

    std::uint64_t Deserializer::peekSpanCount(){
        assert(m_currentObject != m_objects.end());
        const auto& v = m_currentObject->first;
        auto& it = m_currentObject->second;
        if (it == end(v)) {
            throw SerializationException{};
        }
        const auto oldIt = it;
        auto ret = std::uint64_t{};
        *this >> ret;
        it = oldIt;
        return ret;
    }

    Deserializer& Deserializer::operator>>(std::string& s){
        assert(m_currentObject != end(m_objects));
        const auto& v = m_currentObject->first;
        auto& it = m_currentObject->second;
        assert(it != end(v));
        auto len = std::uint64_t{};
        detail::readScalarAndAdvance(v, it, len);
        s.clear();
        s.resize(len, '\0');
        auto data = reinterpret_cast<unsigned char*>(s.data());
        detail::readRawBytesAndAdvance(v, it, data, len);
        return *this;
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

    void Deserializer::addPeg(SoundInputPeg* p){
        assert(p);
        assert(m_currentObject != m_objects.end());
        auto id = heap_id{};
        detail::readScalarAndAdvance(m_currentObject->first, m_currentObject->second, id);
        assert(m_soundInputPegs.find(id) == end(m_soundInputPegs));
        m_soundInputPegs.emplace(std::make_pair(id, p));
    }

    void Deserializer::addPeg(SoundOutputPeg* p){
        assert(p);
        assert(m_currentObject != m_objects.end());
        auto id = heap_id{};
        detail::readScalarAndAdvance(m_currentObject->first, m_currentObject->second, id);
        assert(m_soundOutputPegs.find(id) == end(m_soundOutputPegs));
        m_soundOutputPegs.emplace(std::make_pair(id, p));
    }

    void Deserializer::addPeg(NumberInputPeg* p){
        assert(p);
        assert(m_currentObject != m_objects.end());
        auto id = heap_id{};
        detail::readScalarAndAdvance(m_currentObject->first, m_currentObject->second, id);
        assert(m_numberInputPegs.find(id) == end(m_numberInputPegs));
        m_numberInputPegs.emplace(std::make_pair(id, p));
    }

    void Deserializer::addPeg(NumberOutputPeg* p){
        assert(p);
        assert(m_currentObject != m_objects.end());
        auto id = heap_id{};
        detail::readScalarAndAdvance(m_currentObject->first, m_currentObject->second, id);
        assert(m_numberOutputPegs.find(id) == end(m_numberOutputPegs));
        m_numberOutputPegs.emplace(std::make_pair(id, p));
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
        assert(m_currentObject != end(m_objects));
        const auto& v = m_currentObject->first;
        auto& it = m_currentObject->second;
        assert(it == begin(v));
        auto id = std::string{};
        auto l = float{};
        auto t = float{};
        *this >> id >> l >> t;;
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

} // namespace flui

