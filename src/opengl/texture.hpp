#pragma once

class QImage;

class Texture
{
private:
    Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

public:
    ~Texture();

    Texture(Texture&&);
    Texture& operator=(Texture&&);

    void setName(std::string_view name);
    std::string name() const;

    unsigned int width() const;
    unsigned int height() const;

    const unsigned char* data() const;

public:
    static Texture fromQImage(QImage image);

private:
    std::string _name;
    std::vector<unsigned char> _data;
    unsigned int _width;
    unsigned int _height;
};
