#pragma once

#include "RHI_Common.h"
#include "Texture.h"

#include <vector>

namespace qrhi
{
    struct FramebufferAttachment
    {
        Texture* texture = nullptr;
        TextureSubresourceSet subresources = TextureSubresourceSet{0, 1, 0, 1};
        Format format = Format::UNKNOWN;
        bool isReadOnly = false;

        constexpr FramebufferAttachment& setTexture(Texture* t) { texture = t; return *this; }
        constexpr FramebufferAttachment& setSubresources(TextureSubresourceSet s) { subresources = s; return *this; }
        constexpr FramebufferAttachment& setFormat(Format f) { format = f; return *this; }
        constexpr FramebufferAttachment& setReadOnly(bool value) { isReadOnly = value; return *this; }

        [[nodiscard]] bool isValid() const { return texture != nullptr; }
    };

    struct FramebufferDesc
    {
        std::vector<FramebufferAttachment> colorAttachments;
        FramebufferAttachment depthAttachment;

        FramebufferDesc& addColorAttachment(const FramebufferAttachment& attachment) { colorAttachments.push_back(attachment); return *this; }
        FramebufferDesc& addColorAttachment(Texture* texture) { colorAttachments.push_back(FramebufferAttachment{texture}); return *this; }
        FramebufferDesc& setDepthAttachment(const FramebufferAttachment& attachment) { depthAttachment = attachment; return *this; }
        FramebufferDesc& setDepthAttachment(Texture* texture) { depthAttachment = FramebufferAttachment{texture}; return *this; }
    };

    struct FramebufferInfo
    {
        std::vector<Format> colorFormats;
        Format depthFormat = Format::UNKNOWN;
        uint32_t sampleCount = 1;
        uint32_t sampleQuality = 0;
        uint32_t width = 0;
        uint32_t height = 0;

        FramebufferInfo() = default;
        QUEST_API FramebufferInfo(const FramebufferDesc& desc);

        [[nodiscard]] Viewport GetViewport(float minZ = 0.0f, float maxZ = 1.0f) const
        {
            return {0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), minZ, maxZ};
        }

        bool operator==(const FramebufferInfo& other) const
        {
            return formatsEqual(colorFormats, other.colorFormats)
                && depthFormat == other.depthFormat
                && sampleCount == other.sampleCount
                && sampleQuality == other.sampleQuality;
        }
        bool operator !=(const FramebufferInfo& other) const { return !(*this == other); }

    private:
        static bool formatsEqual(const std::vector<Format> a, const std::vector<Format> b)
        {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); i++)
                if (a[i] != b[i]) return false;
            return true;
        }
    };

    class Framebuffer : public Resource
    {
    public:
        [[nodiscard]] virtual const FramebufferDesc GetDesc() const = 0;
        [[nodiscard]] virtual const FramebufferInfo& GetInfo() const = 0;
    };

    using FramebufferHandle = Quest::RefCountPtr<Framebuffer>;
}

namespace std
{
    template<> struct hash<qrhi::FramebufferInfo>
    {
        std::size_t operator()(qrhi::FramebufferInfo const& s) const noexcept
        {
            size_t hash = 0;
            for (auto format : s.colorFormats)
                qrhi::hash_combine(hash, format);
            qrhi::hash_combine(hash, s.depthFormat);
            qrhi::hash_combine(hash, s.sampleCount);
            qrhi::hash_combine(hash, s.sampleQuality);
            return hash;
        }
    };
}