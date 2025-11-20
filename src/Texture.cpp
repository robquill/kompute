// SPDX-License-Identifier: Apache-2.0

#include "kompute/Texture.hpp"

namespace kp {

vk::DescriptorImageInfo
Texture::constructDescriptorImageInfo()
{
    KP_LOG_DEBUG("Kompute Texture construct descriptor image info size {}",
                 this->memorySize());

    vk::ImageViewCreateInfo viewInfo;
    viewInfo.image = *this->mPrimaryImage;
    viewInfo.format = this->getFormat();
    viewInfo.flags = vk::ImageViewCreateFlags();
    viewInfo.viewType = vk::ImageViewType::e2D;

    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

    // This image object owns the image view
    if (!this->mImageView) {
        mImageView = std::make_shared<vk::ImageView>(
          this->mDevice->createImageView(viewInfo));
    }

    vk::DescriptorImageInfo descriptorInfo;

    descriptorInfo.imageView = *(mImageView.get());
    descriptorInfo.imageLayout = this->mPrimaryImageLayout;
    descriptorInfo.sampler = this->mSampler;
    return descriptorInfo;
}

vk::WriteDescriptorSet
Texture::constructDescriptorSet(vk::DescriptorSet descriptorSet, uint32_t binding)
{
    KP_LOG_DEBUG("Kompute Texture construct descriptor set for binding {}",
                 binding);

    mDescriptorImageInfo = this->constructDescriptorImageInfo();

    return vk::WriteDescriptorSet(descriptorSet,
                                  binding, // Destination binding
                                  0,       // Destination array element
                                  1,       // Descriptor count
                                  vk::DescriptorType::eCombinedImageSampler,
                                  &mDescriptorImageInfo,
                                  nullptr); // Descriptor buffer info
}

vk::ImageUsageFlags
Texture::getPrimaryImageUsageFlags()
{
    switch (this->mMemoryType) {
        case MemoryTypes::eDevice:
        case MemoryTypes::eHost:
        case MemoryTypes::eDeviceAndHost:
            return vk::ImageUsageFlagBits::eSampled |
                   vk::ImageUsageFlagBits::eTransferSrc |
                   vk::ImageUsageFlagBits::eTransferDst;
            break;
        case MemoryTypes::eStorage:
            return vk::ImageUsageFlagBits::eSampled |
                   // You can still copy images to/from storage memory
                   // so set the transfer usage flags here.
                   vk::ImageUsageFlagBits::eTransferSrc |
                   vk::ImageUsageFlagBits::eTransferDst;
            break;
        default:
            throw std::runtime_error("Kompute Image invalid image type");
    }
}

Texture::~Texture()
{
    KP_LOG_DEBUG("Kompute Texture destructor started. Type: {}",
                 Memory::toString(this->memoryType()));

    if (this->mDevice) {
        this->mDevice->destroySampler(mSampler);
        this->destroy();
    }

    KP_LOG_DEBUG("Kompute Texture destructor success");
}

void
Texture::destroy()
{
    KP_LOG_DEBUG("Kompute Texture started destroy()");

    if (!this->mDevice) {
        KP_LOG_WARN(
          "Kompute Texture destructor reached with null Device pointer");
        return;
    }

    if (this->mSampler) {
        KP_LOG_DEBUG("Kompute Texture destroying sampler");
        this->mDevice->destroySampler(this->mSampler);
        this->mSampler = nullptr;
    }

    ImageBase::destroy();

    KP_LOG_DEBUG("Kompute Texture successful destroy()");
}

} // End namespace kp
