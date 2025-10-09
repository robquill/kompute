#include <iostream>
#include <memory>
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
    std::vector<float> imgF32(imgSize);
    for (size_t i = 0; i < imgSize; ++i) {
        imgF32[i] = imgData[i] / 255.0f;
    }

    std::vector<float> output(imgSize, 0.0f);

    stbi_image_free(imgData);

    std::shared_ptr<kp::ImageT<float>> imageIn =
      mgr.image(imgF32, width, height, 4);
    std::shared_ptr<kp::ImageT<float>> imageOut =
      mgr.image(output, width, height, 4);

    std::shared_ptr<kp::TextureT<float>> textureIn =
      mgr.texture(imgF32, width, height, 4);
    std::shared_ptr<kp::ImageT<float>> textureOut =
      mgr.image(output, width, height, 4);

    std::shared_ptr<kp::TextureT<float>> textureBilinearIn =
      mgr.texture(imgF32, width, height, 4, vk::Filter::eLinear);
    std::shared_ptr<kp::ImageT<float>> textureBilinearOut =
      mgr.image(output, width, height, 4);

    const std::vector<uint32_t> image_shader = std::vector<uint32_t>(
      shader::IMAGE_COPY_COMP_SPV.begin(), shader::IMAGE_COPY_COMP_SPV.end());

    std::shared_ptr<kp::Algorithm> image_copy = mgr.algorithm(
      { imageIn, imageOut },
      image_shader,
      { static_cast<unsigned int>(width), static_cast<unsigned int>(height) });

    const std::vector<uint32_t> texture_shader =
      std::vector<uint32_t>(shader::TEXTURE_COPY_COMP_SPV.begin(),
                            shader::TEXTURE_COPY_COMP_SPV.end());

    std::shared_ptr<kp::Algorithm> texture_copy = mgr.algorithm(
      { textureIn, textureOut },
      texture_shader,
      { static_cast<unsigned int>(width), static_cast<unsigned int>(height) });

    const std::vector<uint32_t> texture_blur_shader =
      std::vector<uint32_t>(shader::TEXTURE_BLUR_COMP_SPV.begin(),
                            shader::TEXTURE_BLUR_COMP_SPV.end());
    std::shared_ptr<kp::Algorithm> texture_blur = mgr.algorithm(
      { textureIn, textureBilinearOut },
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

    // Texture copy using sampled texture bilinear filtering
    mgr.sequence()
      ->record<kp::OpSyncDevice>({ textureIn, textureBilinearOut })
      ->record<kp::OpAlgoDispatch>(texture_blur)
      ->record<kp::OpSyncLocal>({ textureIn, textureBilinearOut })
      ->eval();

    // Save tensors to JPEG
    std::vector<unsigned char> outImg(imgSize);
    size_t i = 0;
    for (const float& elem : imageOut->vector()) {
        outImg[i] = static_cast<unsigned char>(
          std::max(0.0f, std::min(1.0f, elem)) * 255.0f);
        i++;
    }

    if (!stbi_write_jpg(
          "output_image.jpg", width, height, 4, outImg.data(), 100)) {
        std::cerr << "Failed to write output_image.jpg" << std::endl;
        return 1;
    }

    std::cout << "Saved output_image.jpg" << std::endl;

    i = 0;
    for (const float& elem : textureOut->vector()) {
        outImg[i] = static_cast<unsigned char>(
          std::max(0.0f, std::min(1.0f, elem)) * 255.0f);
        i++;
    }

    if (!stbi_write_jpg(
          "output_texture.jpg", width, height, 4, outImg.data(), 100)) {
        std::cerr << "Failed to write output_texture.jpg" << std::endl;
        return 1;
    }

    std::cout << "Saved output_texture.jpg" << std::endl;

    i = 0;
    for (const float& elem : textureBilinearOut->vector()) {
        outImg[i] = static_cast<unsigned char>(
          std::max(0.0f, std::min(1.0f, elem)) * 255.0f);
        i++;
    }

    if (!stbi_write_jpg("output_texture_bilinear.jpg",
                        width,
                        height,
                        4,
                        outImg.data(),
                        100)) {
        std::cerr << "Failed to write output_texture_bilinear.jpg" << std::endl;
        return 1;
    }

    std::cout << "Saved output_texture_bilinear.jpg" << std::endl;

    return 0;
}
