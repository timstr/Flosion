#pragma once

namespace flo {

    class Sample;
    class SampleProxy;
    class ConstSampleProxy;

    /**
     * Sample is a pair of left and right audio channel samples
     * The data is stored inside the Sample object itself.
     */
    class Sample final {
    public:
        Sample() noexcept;
        Sample(float l, float r) noexcept;
        Sample(const Sample&) noexcept = default;
        explicit Sample(const ConstSampleProxy&) noexcept;
        ~Sample() noexcept = default;

        float& l() noexcept;
        const float& l() const noexcept;

        float& r() noexcept;
        const float& r() const noexcept;

        Sample& operator=(const Sample&) noexcept;
        Sample& operator=(const ConstSampleProxy&) noexcept;

        Sample& operator+=(const Sample&) noexcept;
        Sample& operator+=(const ConstSampleProxy&) noexcept;
        Sample& operator-=(const Sample&) noexcept;
        Sample& operator-=(const SampleProxy&) noexcept;
        Sample& operator*=(float) noexcept;
        
        Sample operator-() const noexcept;
        Sample operator+(const Sample&) const noexcept;
        Sample operator+(const ConstSampleProxy&) const noexcept;
        Sample operator-(const Sample&) const noexcept;
        Sample operator-(const ConstSampleProxy&) const noexcept;
        Sample operator*(float) const noexcept;
        

        operator SampleProxy() & noexcept;
        operator ConstSampleProxy() const & noexcept;

        // NOTE: implicit conversion to proxy is explicitly
        // disabled here for temporary objects, since such
        // a proxy would immediately be pointing to destroyed data
        operator SampleProxy() && = delete;
        operator ConstSampleProxy() const && = delete;

    private:
        float m_data[2];
    };

    /**
     * SampleProxy acts just like a sample and represents the same thing,
     * but its data is pointed to rather than owned.
     * Beware of dangling references.
     */
    class SampleProxy final {
    public:
        SampleProxy() = delete;
        SampleProxy(const SampleProxy&) noexcept = default;
        ~SampleProxy() noexcept = default;

        float& l() noexcept;
        const float& l() const noexcept;

        float& r() noexcept;
        const float& r() const noexcept;

        SampleProxy& operator=(const Sample&) noexcept;
        SampleProxy& operator=(const ConstSampleProxy&) noexcept;

        SampleProxy& operator+=(const Sample&) noexcept;
        SampleProxy& operator+=(const ConstSampleProxy&) noexcept;
        SampleProxy& operator-=(const Sample&) noexcept;
        SampleProxy& operator-=(const ConstSampleProxy&) noexcept;
        SampleProxy& operator*=(float) noexcept;
        SampleProxy& operator*=(const Sample&) noexcept;
        SampleProxy& operator*=(const ConstSampleProxy&) noexcept;
        
        Sample operator-() const noexcept;
        Sample operator+(const Sample&) const noexcept;
        Sample operator+(const ConstSampleProxy&) const noexcept;
        Sample operator-(const Sample&) const noexcept;
        Sample operator-(const ConstSampleProxy&) const noexcept;
        Sample operator*(float) const noexcept;
        Sample operator*(const Sample&) const noexcept;
        Sample operator*(const ConstSampleProxy&) const noexcept;

    private:
        SampleProxy(float* ptr) noexcept;

        float* m_ptr;

        friend class SoundChunk;
        friend class Sample;
    };

    /**
     * ConstSampleProxy is a const-only version of SampleProxy, which acts
     * just like a const-qualified Sample. This exists because a const-qualified
     * SampleProxy can be used to silently copy-construct a non-const SampleProxy
     * that refers to the same sample, effectively stripping away the constness
     * of the data being referred to.
     * As with SampleProxy, the data is not owned but is pointed to.
     * Beware of dangling references.
     */
    class ConstSampleProxy {
    public:
        ConstSampleProxy() = delete;
        ConstSampleProxy(const ConstSampleProxy&) noexcept = default;
        ~ConstSampleProxy() noexcept = default;

        ConstSampleProxy(SampleProxy) noexcept;

        const float& l() const noexcept;

        const float& r() const noexcept;
        
        Sample operator-() const noexcept;
        Sample operator+(const Sample&) const noexcept;
        Sample operator+(const ConstSampleProxy&) const noexcept;
        Sample operator-(const Sample&) const noexcept;
        Sample operator-(const ConstSampleProxy&) const noexcept;
        Sample operator*(float) const noexcept;
        Sample operator*(const Sample&) const noexcept;
        Sample operator*(const ConstSampleProxy&) const noexcept;

    private:
        ConstSampleProxy(const float* ptr) noexcept;

        const float* m_ptr;

        friend class SoundChunk;
        friend class Sample;
    };

} // namespace flo
