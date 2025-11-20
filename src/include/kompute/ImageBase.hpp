// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "kompute/Memory.hpp"
#include "logger/Logger.hpp"
#include <memory>
#include <string>

namespace kp {

// Forward declarations
class Tensor;
class Image;

/**
 * Base class for Image data used in GPU operations.
 *
 * This class provides the common functionality for both Image and Texture classes.
 * It handles Vulkan memory and image management, which would be used to store
 * their respective data. The images can be used for GPU data storage or transfer.
 */
class ImageBase : public Memory
{
protected:
    /**
     *  Protected constructor - can only be called by derived classes
     */
    ImageBase(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              const DataTypes& dataType,
              const MemoryTypes& memoryType,
              uint32_t x,
              uint32_t y)
      : Memory(physicalDevice, device, dataType, memoryType, x, y)
    {
    }

public:
    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    virtual ~ImageBase();

    /**
     * Destroys and frees the GPU resources which include the image and memory.
     */
    void destroy() override;

    /**
     * Check whether image is initialized based on the created gpu resources.
     *
     * @returns Boolean stating whether image is initialized
     */
    bool isInit() override;

    /**
     * Record a copy operation from another image to this image.
     *
     * @param commandBuffer The command buffer to record the operation
     * @param copyFromImage The image to copy from
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<ImageBase> copyFromImage) override;

    /**
     * Record a copy operation from a tensor to this image.
     *
     * @param commandBuffer The command buffer to record the operation
     * @param copyFromTensor The tensor to copy from
     */
    void recordCopyFrom(const vk::CommandBuffer& commandBuffer,
                        std::shared_ptr<Tensor> copyFromTensor) override;

    /**
     * Records a copy from the staging memory to device memory.
     *
     * @param commandBuffer Command buffer to record the copy operation
     */
    void recordCopyFromStagingToDevice(
      const vk::CommandBuffer& commandBuffer) override;

    /**
     * Records a copy from device memory to the staging memory.
     *
     * @param commandBuffer Command buffer to record the copy operation
     */
    void recordCopyFromDeviceToStaging(
      const vk::CommandBuffer& commandBuffer) override;

    /**
     * Records a memory barrier for the primary image.
     *
     * @param commandBuffer Command buffer to record the barrier
     * @param srcAccessMask Source access mask
     * @param dstAccessMask Destination access mask
     * @param srcStageMask Source pipeline stage mask
     * @param dstStageMask Destination pipeline stage mask
     */
    void recordPrimaryMemoryBarrier(
      const vk::CommandBuffer& commandBuffer,
      vk::AccessFlags srcAccessMask,
      vk::AccessFlags dstAccessMask,
      vk::PipelineStageFlags srcStageMask,
      vk::PipelineStageFlags dstStageMask) override;

    /**
     * Records a memory barrier for the staging image.
     *
     * @param commandBuffer Command buffer to record the barrier
     * @param srcAccessMask Source access mask
     * @param dstAccessMask Destination access mask
     * @param srcStageMask Source pipeline stage mask
     * @param dstStageMask Destination pipeline stage mask
     */
    void recordStagingMemoryBarrier(
      const vk::CommandBuffer& commandBuffer,
      vk::AccessFlags srcAccessMask,
      vk::AccessFlags dstAccessMask,
      vk::PipelineStageFlags srcStageMask,
      vk::PipelineStageFlags dstStageMask) override;

    /**
     * Records a barrier for the primary image with a specific destination layout.
     *
     * @param commandBuffer Command buffer to record the barrier
     * @param srcAccessMask Source access mask
     * @param dstAccessMask Destination access mask
     * @param srcStageMask Source pipeline stage mask
     * @param dstStageMask Destination pipeline stage mask
     * @param dstLayout Destination image layout
     */
    void recordPrimaryImageBarrier(const vk::CommandBuffer& commandBuffer,
                                    vk::AccessFlags srcAccessMask,
                                    vk::AccessFlags dstAccessMask,
                                    vk::PipelineStageFlags srcStageMask,
                                    vk::PipelineStageFlags dstStageMask,
                                    vk::ImageLayout dstLayout);

    /**
     * Constructs a descriptor image info for this image.
     *
     * @returns The descriptor image info
     */
    virtual vk::DescriptorImageInfo constructDescriptorImageInfo() = 0;

    /**
     * Constructs a descriptor set for this image.
     *
     * @param descriptorSet The descriptor set to write to
     * @param binding The binding number
     * @returns The write descriptor set
     */
    virtual vk::WriteDescriptorSet constructDescriptorSet(
      vk::DescriptorSet descriptorSet,
      uint32_t binding) override;

    /**
     * Gets the primary image.
     *
     * @returns The primary image
     */
    std::shared_ptr<vk::Image> getPrimaryImage();

    /**
     * Gets the primary image layout.
     *
     * @returns The primary image layout
     */
    vk::ImageLayout getPrimaryImageLayout();

    /**
     * Gets the number of channels in the image.
     *
     * @returns The number of channels
     */
    uint32_t getNumChannels();

    /**
     * Returns the memory type
     */
    Memory::Type type() override { return Memory::Type::eImage; }

    /**
     * Virtual function to get the primary image usage flags.
     * Must be overridden by derived classes.
     *
     * @returns The primary image usage flags
     */
    virtual vk::ImageUsageFlags getPrimaryImageUsageFlags() = 0;

    /**
     * Virtual function to get the staging image usage flags.
     * Must be overridden by derived classes.
     *
     * @returns The staging image usage flags
     */
    virtual vk::ImageUsageFlags getStagingImageUsageFlags();

    /**
     * Virtual function to get the image format.
     * Must be overridden by derived classes.
     *
     * @returns The image format
     */
    virtual vk::Format getFormat();

protected:
    uint32_t mNumChannels = 0;
    vk::ImageTiling mTiling = vk::ImageTiling::eLinear;

    std::shared_ptr<vk::Image> mPrimaryImage = nullptr;
    bool mFreePrimaryImage = false;
    vk::ImageLayout mPrimaryImageLayout = vk::ImageLayout::eUndefined;

    std::shared_ptr<vk::Image> mStagingImage = nullptr;
    bool mFreeStagingImage = false;
    vk::ImageLayout mStagingImageLayout = vk::ImageLayout::eUndefined;

    std::shared_ptr<vk::ImageView> mImageView = nullptr;
    vk::DescriptorImageInfo mDescriptorImageInfo = {};

    /**
     * Initialize the image with provided data.
     *
     * @param data Pointer to data that will be used to initialize the image
     * @param dataSize Size in bytes of the data pointed to by data
     * @param numChannels The number of channels in the image
     * @param tiling Tiling mode to use for the image
     */
    void init(void* data,
              size_t dataSize,
              uint32_t numChannels,
              vk::ImageTiling tiling);

    /**
     * Function to reserve memory on the image. This does not copy any data, it
     * just reserves memory, similarly to std::vector reserve() method.
     */
    void reserve();

    /**
     * Allocates memory and creates GPU resources for the image.
     */
    void allocateMemoryCreateGPUResources();

    /**
     * Creates a Vulkan image.
     *
     * @param image The image to create
     * @param imageUsageFlags The usage flags for the image
     * @param imageTiling The tiling mode for the image
     */
    void createImage(std::shared_ptr<vk::Image> image,
                     vk::ImageUsageFlags imageUsageFlags,
                     vk::ImageTiling imageTiling);

    /**
     * Allocates and binds memory to an image.
     *
     * @param image The image to bind memory to
     * @param memory The memory to bind
     * @param memoryPropertyFlags The memory property flags
     */
    void allocateBindMemory(std::shared_ptr<vk::Image> image,
                            std::shared_ptr<vk::DeviceMemory> memory,
                            vk::MemoryPropertyFlags memoryPropertyFlags);

    /**
     * Records an image barrier.
     *
     * @param commandBuffer Command buffer to record the barrier
     * @param image The image to apply the barrier to
     * @param srcAccessMask Source access mask
     * @param dstAccessMask Destination access mask
     * @param srcStageMask Source pipeline stage mask
     * @param dstStageMask Destination pipeline stage mask
     * @param srcLayout Source image layout
     * @param dstLayout Destination image layout
     */
    void recordImageMemoryBarrier(const vk::CommandBuffer& commandBuffer,
                                   const vk::Image& image,
                                   vk::AccessFlags srcAccessMask,
                                   vk::AccessFlags dstAccessMask,
                                   vk::PipelineStageFlags srcStageMask,
                                   vk::PipelineStageFlags dstStageMask,
                                   vk::ImageLayout srcLayout,
                                   vk::ImageLayout dstLayout);

    /**
     * Records a barrier for the staging image with a specific destination layout.
     *
     * @param commandBuffer Command buffer to record the barrier
     * @param srcAccessMask Source access mask
     * @param dstAccessMask Destination access mask
     * @param srcStageMask Source pipeline stage mask
     * @param dstStageMask Destination pipeline stage mask
     * @param dstLayout Destination image layout
     */
    void recordStagingImageBarrier(const vk::CommandBuffer& commandBuffer,
                                    vk::AccessFlags srcAccessMask,
                                    vk::AccessFlags dstAccessMask,
                                    vk::PipelineStageFlags srcStageMask,
                                    vk::PipelineStageFlags dstStageMask,
                                    vk::ImageLayout dstLayout);

    /**
     * Records a copy operation between two images.
     *
     * @param commandBuffer Command buffer to record the copy
     * @param srcImage Source image
     * @param dstImage Destination image
     * @param srcLayout Source image layout
     * @param dstLayout Destination image layout
     * @param copyRegion Copy region
     */
    void recordCopyImage(const vk::CommandBuffer& commandBuffer,
                         std::shared_ptr<vk::Image> srcImage,
                         std::shared_ptr<vk::Image> dstImage,
                         vk::ImageLayout srcLayout,
                         vk::ImageLayout dstLayout,
                         vk::ImageCopy copyRegion);

    /**
     * Records a copy operation from a buffer to an image.
     *
     * @param commandBuffer Command buffer to record the copy
     * @param srcBuffer Source buffer
     * @param dstImage Destination image
     * @param dstLayout Destination image layout
     * @param copyRegion Copy region
     */
    void recordCopyImageFromTensor(const vk::CommandBuffer& commandBuffer,
                                    std::shared_ptr<vk::Buffer> srcBuffer,
                                    std::shared_ptr<vk::Image> dstImage,
                                    vk::ImageLayout dstLayout,
                                    vk::BufferImageCopy copyRegion);
};

} // namespace kp
