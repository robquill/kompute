// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Image.hpp"
#include "kompute/Tensor.hpp"
#include "logger/Logger.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <memory>

namespace kp {

/**
 * Image data used in GPU operations.
 *
 * Each image would has a respective Vulkan memory and image, which
 * would be used to store their respective data. The images can be used for GPU
 * data storage or transfer.
 */
class Texture : public Image
{
  public:
    /**
     *  Constructor with data provided which would be used to create the
     *  respective vulkan image and memory.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the image and memory from
     *  @param data Pointer to data that will be used to initialise the image
     *  @param dataSize Size in bytes of the data pointed to by \p data
     *  @param x Width of the image in pixels
     *  @param y Height of the image in pixels
     *  @param numChannels The number of channels in the image
     *  @param dataType Data type for the image which is of type DataTypes
     *  @param memoryType Type for the image which is of type MemoryTypes
     *  @param tiling Tiling mode to use for the image.
     */
    Texture(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              void* data,
              size_t dataSize,
              uint32_t x,
              uint32_t y,
              uint32_t numChannels,
              const DataTypes& dataType,
              vk::ImageTiling tiling,
              const MemoryTypes& memoryType = MemoryTypes::eDevice,
              vk::Filter filter = vk::Filter::eNearest,
              vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge)
      : Image(physicalDevice,
              device,
              data,
              dataSize,
              x,
              y,
              numChannels,
              dataType,
              tiling,
              memoryType)
    {
      vk::SamplerCreateInfo samplerInfo = {};
      samplerInfo.magFilter = filter;
      samplerInfo.minFilter = filter;
      samplerInfo.addressModeU = addressMode;
      samplerInfo.addressModeV = addressMode;
      samplerInfo.addressModeW = addressMode;
      device->createSampler(&samplerInfo, nullptr, &mSampler);
    }

    /**
     *  Constructor with no data provided.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the image and memory from
     *  @param x Width of the image in pixels
     *  @param y Height of the image in pixels
     *  @param dataType Data type for the image which is of type ImageDataTypes
     *  @param memoryType Type for the image which is of type MemoryTypes
     *  @param tiling Tiling mode to use for the image.
     */
    Texture(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              uint32_t x,
              uint32_t y,
              uint32_t numChannels,
              const DataTypes& dataType,
              vk::ImageTiling tiling,
              const MemoryTypes& memoryType = MemoryTypes::eDevice,
              vk::Filter filter = vk::Filter::eNearest,
              vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge)
      : Texture(physicalDevice,
                  device,
                  nullptr,
                  0,
                  x,
                  y,
                  numChannels,
                  dataType,
                  tiling,
                  memoryType,
                  filter,
                  addressMode)
    {
    }

    /**
     *  Constructor with data provided which would be used to create the
     *  respective vulkan image and memory. No tiling has been provided
     *  so will be inferred from \p memoryType.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the image and memory from
     *  @param data Pointer to data that will be used to initialise the image
     *  @param dataSize Size in bytes of the data pointed to by \p data
     *  @param x Width of the image in pixels
     *  @param y Height of the image in pixels
     *  @param numChannels The number of channels in the image
     *  @param dataType Data type for the image which is of type DataTypes
     *  @param memoryType Type for the image which is of type MemoryTypes
     */
    Texture(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              void* data,
              size_t dataSize,
              uint32_t x,
              uint32_t y,
              uint32_t numChannels,
              const DataTypes& dataType,
              const MemoryTypes& memoryType = MemoryTypes::eDevice,
              vk::Filter filter = vk::Filter::eNearest,
              vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge)
      : Image(physicalDevice,
              device,
              data,
              dataSize,
              x,
              y,
              numChannels,
              dataType,
              memoryType)
    {
      vk::SamplerCreateInfo samplerInfo = {};
      samplerInfo.magFilter = filter;
      samplerInfo.minFilter = filter;
      samplerInfo.addressModeU = addressMode;
      samplerInfo.addressModeV = addressMode;
      samplerInfo.addressModeW = addressMode;
      device->createSampler(&samplerInfo, nullptr, &mSampler);
    }

    /**
     *  Constructor with no data provided. No tiling has been provided
     *  so will be inferred from \p memoryType.
     *
     *  @param physicalDevice The physical device to use to fetch properties
     *  @param device The device to use to create the image and memory from
     *  @param x Width of the image in pixels
     *  @param y Height of the image in pixels
     *  @param dataType Data type for the image which is of type ImageDataTypes
     *  @param memoryType Type for the image which is of type MemoryTypes
     */
    Texture(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              uint32_t x,
              uint32_t y,
              uint32_t numChannels,
              const DataTypes& dataType,
              const MemoryTypes& memoryType = MemoryTypes::eDevice,
              vk::Filter filter = vk::Filter::eNearest,
              vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge)
      : Texture(physicalDevice,
                  device,
                  nullptr,
                  0,
                  x,
                  y,
                  numChannels,
                  dataType,
                  memoryType,
                  filter,
                  addressMode)
    {
    }

    /**
     * @brief Make Texture uncopyable
     *
     */
    Texture(const Texture&) = delete;
    Texture(const Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(const Texture&&) = delete;

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    virtual ~Texture();

    /**
     * Adds this object to a Vulkan descriptor set at \p binding.
     *
     * @param descriptorSet The descriptor set to add to.
     * @param binding The binding number to use.
     * @return Add this object to a descriptor set at \p binding.
     */
    vk::WriteDescriptorSet constructDescriptorSet(
      vk::DescriptorSet descriptorSet,
      uint32_t binding) override;

    private:
      vk::DescriptorImageInfo constructDescriptorImageInfo();

      vk::Sampler mSampler;
};

template<typename T>
class TextureT : public Texture
{
  public:
    TextureT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
               std::shared_ptr<vk::Device> device,
               const std::vector<T>& data,
               uint32_t x,
               uint32_t y,
               uint32_t numChannels,
               vk::ImageTiling tiling,
               const MemoryTypes& imageType = MemoryTypes::eDevice,
               vk::Filter filter = vk::Filter::eNearest,
               vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge)
      : Texture(physicalDevice,
                  device,
                  (void*)data.data(),
                  data.size(),
                  x,
                  y,
                  numChannels,
                  Memory::dataType<T>(),
                  tiling,
                  imageType,
                  filter,
                  addressMode)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute textureT constructor with data size {}, x {}, "
                     "y {}, and num channels {}",
                     data.size(),
                     x,
                     y,
                     numChannels);
    }

    TextureT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
               std::shared_ptr<vk::Device> device,
               const std::vector<T>& data,
               uint32_t x,
               uint32_t y,
               uint32_t numChannels,
               const MemoryTypes& imageType = MemoryTypes::eDevice,
               vk::Filter filter = vk::Filter::eNearest,
               vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge)
      : Texture(physicalDevice,
                  device,
                  (void*)data.data(),
                  data.size(),
                  x,
                  y,
                  numChannels,
                  Memory::dataType<T>(),
                  imageType,
                  filter,
                  addressMode)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute textureT constructor with data size {}, x {}, "
                     "y {}, and num channels {}",
                     data.size(),
                     x,
                     y,
                     numChannels);
    }

    TextureT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
               std::shared_ptr<vk::Device> device,
               uint32_t x,
               uint32_t y,
               uint32_t numChannels,
               vk::ImageTiling tiling,
               const MemoryTypes& imageType = MemoryTypes::eDevice,
               vk::Filter filter = vk::Filter::eNearest,
               vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge)
      : Texture(physicalDevice,
                  device,
                  x,
                  y,
                  numChannels,
                  Memory::dataType<T>(),
                  tiling,
                  imageType,
                  filter,
                  addressMode)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute textureT constructor with no data, x {}, "
                     "y {}, and num channels {}",
                     x,
                     y,
                     numChannels);
    }

    TextureT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
               std::shared_ptr<vk::Device> device,
               uint32_t x,
               uint32_t y,
               uint32_t numChannels,
               const MemoryTypes& imageType = MemoryTypes::eDevice,
               vk::Filter filter = vk::Filter::eNearest,
               vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge)
      : Texture(physicalDevice,
                  device,
                  x,
                  y,
                  numChannels,
                  Memory::dataType<T>(),
                  imageType,
                  filter,
                  addressMode)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute textureT constructor with no data, x {}, "
                     "y {}, and num channels {}",
                     x,
                     y,
                     numChannels);
    }

    ~TextureT() { KP_LOG_DEBUG("Kompute TextureT destructor"); }

    std::vector<T> vector() { return Memory::vector<T>(); }

    T& operator[](int index) { return *(Memory::data<T>() + index); }
};

} // End namespace kp
