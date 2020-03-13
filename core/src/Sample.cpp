#include <Flosion/Core/Sample.hpp>

namespace flo {

    // Sample

    Sample::Sample() noexcept
        : m_data{} {

    }

    Sample::Sample(float left, float right) noexcept {
        l() = left;
        r() = right;
    }

    Sample::Sample(const SampleProxy& other) noexcept {
        l() = other.l();
        r() = other.r();
    }

    Sample::Sample(const ConstSampleProxy& other) noexcept {
        l() = other.l();
        r() = other.r();
    }

    void Sample::silence(){
        m_data[0] = 0.0f;
        m_data[1] = 0.0f;
    }

    float& Sample::l() noexcept {
        return m_data[0];
    }

    float Sample::l() const noexcept {
        return m_data[0];
    }

    float& Sample::r() noexcept {
        return m_data[1];
    }

    float Sample::r() const noexcept {
        return m_data[1];
    }

    Sample& Sample::operator=(const Sample& other) noexcept {
        l() = other.l();
        r() = other.r();
        return *this;
    }

    Sample& Sample::operator=(const ConstSampleProxy& other) noexcept {
        l() = other.l();
        r() = other.r();
        return *this;
    }

    Sample& Sample::operator+=(const Sample& other) noexcept {
        l() += other.l();
        r() += other.r();
        return *this;
    }

    Sample& Sample::operator+=(const ConstSampleProxy& other) noexcept {
        l() += other.l();
        r() += other.r();
        return *this;
    }

    Sample& Sample::operator-=(const Sample& other) noexcept {
        l() -= other.l();
        r() -= other.r();
        return *this;
    }

    Sample& Sample::operator-=(const SampleProxy& other) noexcept {
        l() -= other.l();
        r() -= other.r();
        return *this;
    }

    Sample& Sample::operator*=(float value) noexcept {
        l() *= value;
        r() *= value;
        return *this;
    }

    Sample& Sample::operator/=(float v) noexcept {
        l() /= v;
        r() /= v;
        return *this;
    }

    Sample Sample::operator-() const noexcept {
        return {-l(), -r()};
    }

    Sample Sample::operator+(const Sample& other) const noexcept {
        return {l() + other.l(), r() + other.r()};
    }

    Sample Sample::operator+(const ConstSampleProxy& other) const noexcept {
        return {l() + other.l(), r() + other.r()};
    }

    Sample Sample::operator-(const Sample& other) const noexcept {
        return {l() - other.l(), r() - other.r()};
    }

    Sample Sample::operator-(const ConstSampleProxy& other) const noexcept {
        return {l() - other.l(), r() - other.r()};
    }

    Sample Sample::operator*(float value) const noexcept {
        return {l() * value, r() * value};
    }

    Sample Sample::operator/(float v) const noexcept {
        return {l() / v, r() / v};
    }

    Sample::operator SampleProxy() & noexcept {
        return SampleProxy(static_cast<float*>(m_data));
    }

    Sample::operator ConstSampleProxy() const & noexcept {
        return ConstSampleProxy(static_cast<const float*>(m_data));
    }

    // SampleProxy

    SampleProxy::SampleProxy(float* ptr) noexcept
        : m_ptr(ptr) {
        
    }

    void SampleProxy::silence(){
        m_ptr[0] = 0.0f;
        m_ptr[1] = 0.0f;
    }

    float& SampleProxy::l() noexcept {
        return m_ptr[0];
    }

    float SampleProxy::l() const noexcept {
        return m_ptr[0];
    }

    float& SampleProxy::r() noexcept {
        return m_ptr[1];
    }

    float SampleProxy::r() const noexcept {
        return m_ptr[1];
    }

    SampleProxy& SampleProxy::operator=(const Sample& other) noexcept {
        l() = other.l();
        r() = other.r();
        return *this;
    }

    SampleProxy& SampleProxy::operator=(const ConstSampleProxy& other) noexcept {
        l() = other.l();
        r() = other.r();
        return *this;
    }

    SampleProxy& SampleProxy::operator+=(const Sample& other) noexcept {
        l() += other.l();
        r() += other.r();
        return *this;
    }

    SampleProxy& SampleProxy::operator+=(const ConstSampleProxy& other) noexcept {
        l() += other.l();
        r() += other.r();
        return *this;
    }

    SampleProxy& SampleProxy::operator-=(const Sample& other) noexcept {
        l() -= other.l();
        r() -= other.r();
        return *this;
    }

    SampleProxy& SampleProxy::operator-=(const ConstSampleProxy& other) noexcept {
        l() -= other.l();
        r() -= other.r();
        return *this;
    }

    SampleProxy& SampleProxy::operator*=(float value) noexcept {
        l() *= value;
        r() *= value;
        return *this;
    }

    SampleProxy& SampleProxy::operator/=(float v) noexcept {
        l() /= v;
        r() /= v;
        return *this;
    }

    Sample SampleProxy::operator-() const noexcept {
        return {-l(), -r()};
    }

    Sample SampleProxy::operator+(const Sample& other) const noexcept {
        return {l() + other.l(), r() + other.r()};
    }

    Sample SampleProxy::operator+(const ConstSampleProxy& other) const noexcept {
        return {l() + other.l(), r() + other.r()};
    }

    Sample SampleProxy::operator-(const Sample& other) const noexcept {
        return {l() - other.l(), r() - other.r()};
    }

    Sample SampleProxy::operator-(const ConstSampleProxy& other) const noexcept {
        return {l() - other.l(), r() - other.r()};
    }

    Sample SampleProxy::operator*(float value) const noexcept {
        return {l() * value, r() * value};
    }

    Sample SampleProxy::operator/(float v) const noexcept {
        return {l() / v, r() / v};
    }

    // ConstSampleProxy

    ConstSampleProxy::ConstSampleProxy(const float* ptr) noexcept
        : m_ptr(ptr) {
    
    }

    ConstSampleProxy::ConstSampleProxy(SampleProxy other) noexcept 
        : m_ptr(other.m_ptr) {
    }

    float ConstSampleProxy::l() const noexcept {
        return m_ptr[0];
    }

    float ConstSampleProxy::r() const noexcept {
        return m_ptr[1];
    }

    Sample ConstSampleProxy::operator-() const noexcept {
        return {-l(), -r()};
    }

    Sample ConstSampleProxy::operator+(const Sample& other) const noexcept {
        return {l() + other.l(), r() + other.r()};
    }

    Sample ConstSampleProxy::operator+(const ConstSampleProxy& other) const noexcept {
        return {l() + other.l(), r() + other.r()};
    }

    Sample ConstSampleProxy::operator-(const Sample& other) const noexcept {
        return {l() - other.l(), r() - other.r()};
    }

    Sample ConstSampleProxy::operator-(const ConstSampleProxy& other) const noexcept {
        return {l() - other.l(), r() - other.r()};
    }

    Sample ConstSampleProxy::operator*(float value) const noexcept {
        return {l() * value, r() * value};
    }

    Sample ConstSampleProxy::operator/(float v) const noexcept {
        return {l() / v, r() / v};
    }

} // namespace flo
