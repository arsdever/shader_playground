#include <QImage>

#include "texture.hpp"

#include "logger.hpp"

static auto logger = get_logger("opengl");

Texture::Texture() { }

Texture::~Texture() = default;

Texture::Texture(Texture&& other) = default;

Texture& Texture::operator=(Texture&& other) = default;

void Texture::setName(std::string_view name) { _name = name; }

std::string Texture::name() const { return _name; }

unsigned int Texture::width() const { return _width; }

unsigned int Texture::height() const { return _height; }

const unsigned char* Texture::data() const { return _data.data(); }

Texture Texture::fromQImage(QImage image)
{
    logger->info("Creating texture from QImage");
    logger->debug("Image size: {}x{}", image.width(), image.height());
    logger->debug("Image format: {}", image.format());

    Texture texture;
    texture._width = image.width();
    texture._height = image.height();
    texture._data.resize(image.width() * image.height() * 4);

    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            auto pixel = image.pixel(x, y);
            texture._data[ (y * image.width() + x) * 4 + 0 ] = qRed(pixel);
            texture._data[ (y * image.width() + x) * 4 + 1 ] = qGreen(pixel);
            texture._data[ (y * image.width() + x) * 4 + 2 ] = qBlue(pixel);
            texture._data[ (y * image.width() + x) * 4 + 3 ] = qAlpha(pixel);
        }
    }

    logger->debug("Texture size: {}", texture._data.size());

    return texture;
}
