// SPDX-License-Identifier: Apache-2.0

#include "kompute/ImageBase.hpp"
#include "kompute/Image.hpp"

namespace kp {

Image::~Image()
{
    KP_LOG_DEBUG("Kompute Image destructor started. Type: {}",
                 Memory::toString(this->memoryType()));

    if (this->mDevice) {
        this->destroy();
    }

    KP_LOG_DEBUG("Kompute Image destructor success");
}

vk::DescriptorImageInfo
Image::constructDescriptorImageInfo()
{
    KP_LOG_DEBUG("Kompute Image construct descriptor image info size {}",
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
    return descriptorInfo;
}

vk::ImageUsageFlags
Image::getPrimaryImageUsageFlags()
{
    switch (this->mMemoryType) {
        case MemoryTypes::eDevice:
        case MemoryTypes::eHost:
        case MemoryTypes::eDeviceAndHost:
            return vk::ImageUsageFlagBits::eStorage |
                   vk::ImageUsageFlagBits::eTransferSrc |
                   vk::ImageUsageFlagBits::eTransferDst;
            break;
        case MemoryTypes::eStorage:
            return vk::ImageUsageFlagBits::eStorage |
                   // You can still copy images to/from storage memory
                   // so set the transfer usage flags here.
                   vk::ImageUsageFlagBits::eTransferSrc |
                   vk::ImageUsageFlagBits::eTransferDst;
            break;
        default:
            throw std::runtime_error("Kompute Image invalid image type");
    }
}

}
