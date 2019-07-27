#include <Sample.hpp>

namespace flo {

    float& Sample::l() noexcept {
        return m_data[0];
    }

    const float& Sample::l() const noexcept {
        return m_data[0];
    }

    SampleProxy::SampleProxy(float* ptr) noexcept
        : m_ptr(ptr) {
        
    }

    float& SampleProxy::l() noexcept {
        return m_ptr[0];
    }

    const float& SampleProxy::l() const noexcept {
        return m_ptr[0];
    }

    ConstSampleProxy::ConstSampleProxy(const float* ptr) noexcept
        : m_ptr(ptr) {
    
    }

} // namespace flo
