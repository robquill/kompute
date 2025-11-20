// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "kompute/ImageBase.hpp"
#include "kompute/Memory.hpp"
#include "kompute/Tensor.hpp"
#include "logger/Logger.hpp"
#include <memory>
#include <string>

namespace kp {

/**
 * Image data used in GPU operations.
 *
 * Each image would has a respective Vulkan memory and image, which
 * would be used to store their respective data. The images can be used for GPU
 * data storage or transfer.
 */
class Image : public ImageBase
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
    Image(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
          std::shared_ptr<vk::Device> device,
          void* data,
          size_t dataSize,
          uint32_t x,
          uint32_t y,
          uint32_t numChannels,
          const DataTypes& dataType,
          vk::ImageTiling tiling,
          const MemoryTypes& memoryType = MemoryTypes::eDevice)
      : ImageBase(physicalDevice, device, dataType, memoryType, x, y)
    {
        if (dataType == DataTypes::eCustom) {
            throw std::runtime_error(
              "Custom data types are not supported for Kompute Images");
        }

        this->mDescriptorType = vk::DescriptorType::eStorageImage;
        init(data, dataSize, numChannels, tiling);
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
    Image(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
          std::shared_ptr<vk::Device> device,
          uint32_t x,
          uint32_t y,
          uint32_t numChannels,
          const DataTypes& dataType,
          vk::ImageTiling tiling,
          const MemoryTypes& memoryType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              nullptr,
              0,
              x,
              y,
              numChannels,
              dataType,
              tiling,
              memoryType)
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
    Image(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
          std::shared_ptr<vk::Device> device,
          void* data,
          size_t dataSize,
          uint32_t x,
          uint32_t y,
          uint32_t numChannels,
          const DataTypes& dataType,
          const MemoryTypes& memoryType = MemoryTypes::eDevice)
      : ImageBase(physicalDevice, device, dataType, memoryType, x, y)
    {
        vk::ImageTiling tiling;

        if (dataType == DataTypes::eCustom) {
            throw std::runtime_error(
              "Custom data types are not supported for Kompute Images");
        }

        if (memoryType == MemoryTypes::eHost ||
            memoryType == MemoryTypes::eDeviceAndHost) {
            // Host-accessible memory must be linear-tiled.
            tiling = vk::ImageTiling::eLinear;
        } else if (memoryType == MemoryTypes::eDevice ||
                   memoryType == MemoryTypes::eStorage) {
            tiling = vk::ImageTiling::eOptimal;
        } else {
            throw std::runtime_error("Kompute Image unsupported memory type");
        }

        this->mDescriptorType = vk::DescriptorType::eStorageImage;
        init(data, dataSize, numChannels, tiling);
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
    Image(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
          std::shared_ptr<vk::Device> device,
          uint32_t x,
          uint32_t y,
          uint32_t numChannels,
          const DataTypes& dataType,
          const MemoryTypes& memoryType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              nullptr,
              0,
              x,
              y,
              numChannels,
              dataType,
              memoryType)
    {
    }

    /**
     * @brief Make Image uncopyable
     *
     */
    Image(const Image&) = delete;
    Image(const Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(const Image&&) = delete;

    /**
     * Destructor which is in charge of freeing vulkan resources unless they
     * have been provided externally.
     */
    virtual ~Image();

    Type type() override { return Type::eImage; }

  protected:
    vk::ImageUsageFlags getPrimaryImageUsageFlags() override;

  private:
    vk::DescriptorImageInfo constructDescriptorImageInfo() override;
};

template<typename T>
class ImageT : public Image
{

  public:
    ImageT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           const std::vector<T>& data,
           uint32_t x,
           uint32_t y,
           uint32_t numChannels,
           vk::ImageTiling tiling,
           const MemoryTypes& imageType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              (void*)data.data(),
              data.size(),
              x,
              y,
              numChannels,
              Memory::dataType<T>(),
              tiling,
              imageType)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute imageT constructor with data size {}, x {}, "
                     "y {}, and num channels {}",
                     data.size(),
                     x,
                     y,
                     numChannels);
    }

    ImageT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           const std::vector<T>& data,
           uint32_t x,
           uint32_t y,
           uint32_t numChannels,
           const MemoryTypes& imageType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              (void*)data.data(),
              data.size(),
              x,
              y,
              numChannels,
              Memory::dataType<T>(),
              imageType)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute imageT constructor with data size {}, x {}, "
                     "y {}, and num channels {}",
                     data.size(),
                     x,
                     y,
                     numChannels);
    }

    ImageT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           uint32_t x,
           uint32_t y,
           uint32_t numChannels,
           vk::ImageTiling tiling,
           const MemoryTypes& imageType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              x,
              y,
              numChannels,
              Memory::dataType<T>(),
              tiling,
              imageType)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute imageT constructor with no data, x {}, "
                     "y {}, and num channels {}",
                     x,
                     y,
                     numChannels);
    }

    ImageT(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
           std::shared_ptr<vk::Device> device,
           uint32_t x,
           uint32_t y,
           uint32_t numChannels,
           const MemoryTypes& imageType = MemoryTypes::eDevice)
      : Image(physicalDevice,
              device,
              x,
              y,
              numChannels,
              Memory::dataType<T>(),
              imageType)
    {
        // Images cannot be created with custom types
        static_assert(Memory::dataType<T>() != DataTypes::eCustom,
                      "Custom data types are not supported for Kompute Images");

        KP_LOG_DEBUG("Kompute imageT constructor with no data, x {}, "
                     "y {}, and num channels {}",
                     x,
                     y,
                     numChannels);
    }

    ~ImageT() { KP_LOG_DEBUG("Kompute imageT destructor"); }

    std::vector<T> vector() { return Memory::vector<T>(); }

    T& operator[](int index) { return *(Memory::data<T>() + index); }
};

} // End namespace kp
