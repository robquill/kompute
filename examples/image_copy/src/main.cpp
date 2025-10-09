#include <cstdint> // for uint8_t
#include <cstring> // for memcpy
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <vector>

#include <kompute/Kompute.hpp>
#include <shader/image_copy.hpp>
#include <shader/texture_copy.hpp>
#include <shader/texture_blur.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int
main(int argc, char** argv)
{
    if (argc < 2 || (argc == 2 && (std::string(argv[1]) == "-h" ||
                                   std::string(argv[1]) == "--help"))) {
        std::cout << "Usage: " << argv[0] << " <input.jpg> <output.jpg>\n";
        std::cout << "Options:\n  -h, --help    Show this help message\n";
        return 0;
    }

    std::string inputFile = argv[1];
    kp::Manager mgr;

    // Load JPEG as RGBA F32
    int width, height, channels;
    unsigned char* imgData =
      stbi_load(inputFile.c_str(), &width, &height, &channels, 4); // force RGBA
    if (!imgData) {
        std::cerr << "Failed to load " << inputFile << std::endl;
        return 1;
    }

    size_t imgSize = width * height * 4;
    std::vector<uint8_t> imgRGBA8(imgSize);
    memcpy(imgRGBA8.data(), imgData, imgSize * sizeof(uint8_t));

    std::vector<uint8_t> output(imgSize, 0);

    stbi_image_free(imgData);

    auto imageIn = mgr.imageT<uint8_t>(imgRGBA8, width, height, 4);
    auto imageOut = mgr.imageT<uint8_t>(output, width, height, 4);

    auto textureIn = mgr.textureT<uint8_t>(imgRGBA8, width, height, 4);
    auto textureOut = mgr.imageT<uint8_t>(output, width, height, 4);

    auto textureBlurIn =
      mgr.textureT<uint8_t>(imgRGBA8, width, height, 4, vk::Filter::eLinear);
    auto textureBlurOut = mgr.imageT<uint8_t>(output, width, height, 4);

    const auto image_shader = std::vector<uint32_t>(
      shader::IMAGE_COPY_COMP_SPV.begin(), shader::IMAGE_COPY_COMP_SPV.end());

    auto image_copy = mgr.algorithm(
      { imageIn, imageOut },
      image_shader,
      { static_cast<unsigned int>(width), static_cast<unsigned int>(height) });

    const auto texture_shader =
      std::vector<uint32_t>(shader::TEXTURE_COPY_COMP_SPV.begin(),
                            shader::TEXTURE_COPY_COMP_SPV.end());

    auto texture_copy = mgr.algorithm(
      { textureIn, textureOut },
      texture_shader,
      { static_cast<unsigned int>(width), static_cast<unsigned int>(height) });

    const std::vector<uint32_t> texture_blur_shader =
      std::vector<uint32_t>(shader::TEXTURE_BLUR_COMP_SPV.begin(),
                            shader::TEXTURE_BLUR_COMP_SPV.end());
    auto texture_blur = mgr.algorithm(
      { textureIn, textureBlurOut },
      texture_blur_shader,
      { static_cast<unsigned int>(width), static_cast<unsigned int>(height) });

    // Image copy using image load/store
    mgr.sequence()
      ->record<kp::OpSyncDevice>({ imageIn, imageOut })
      ->record<kp::OpAlgoDispatch>(image_copy)
      ->record<kp::OpSyncLocal>({ imageIn, imageOut })
      ->eval();

    // Texture copy using sampled texture
    mgr.sequence()
      ->record<kp::OpSyncDevice>({ textureIn, textureOut })
      ->record<kp::OpAlgoDispatch>(texture_copy)
      ->record<kp::OpSyncLocal>({ textureIn, textureOut })
      ->eval();

    // Texture blur using bilinear filtering
    mgr.sequence()
      ->record<kp::OpSyncDevice>({ textureIn, textureBlurOut })
      ->record<kp::OpAlgoDispatch>(texture_blur)
      ->record<kp::OpSyncLocal>({ textureIn, textureBlurOut })
      ->eval();

    // Save tensors to JPEG
    std::vector<unsigned char> outImg(imgSize);
    memcpy(outImg.data(), imageOut->vector().data(), imgSize * sizeof(uint8_t));
    if (!stbi_write_jpg(
          "output_image.jpg", width, height, 4, outImg.data(), 100)) {
        std::cerr << "Failed to write output_image.jpg" << std::endl;
        return 1;
    }

    std::cout << "Saved output_image.jpg" << std::endl;

    memcpy(
      outImg.data(), textureOut->vector().data(), imgSize * sizeof(uint8_t));
    if (!stbi_write_jpg(
          "output_texture.jpg", width, height, 4, outImg.data(), 100)) {
        std::cerr << "Failed to write output_texture.jpg" << std::endl;
        return 1;
    }

    std::cout << "Saved output_texture.jpg" << std::endl;

    memcpy(outImg.data(),
           textureBlurOut->vector().data(),
           imgSize * sizeof(uint8_t));
    if (!stbi_write_jpg("output_texture_blur.jpg",
                        width,
                        height,
                        4,
                        outImg.data(),
                        100)) {
        std::cerr << "Failed to write output_texture_blur.jpg" << std::endl;
        return 1;
    }

    std::cout << "Saved output_texture_blur.jpg" << std::endl;

    return 0;
}
