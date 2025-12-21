#ifndef IMAGE_FORMATS_HPP
#define IMAGE_FORMATS_HPP

#include "image_base.hpp"
#include <vector>
#include <cstdint>

// JPEG图片类 - 需要实现
class JPEGImage : public Image {
private:
    std::vector<uint8_t> pixelData_;
    int quality_;
    
public:
    JPEGImage(const std::string& filename, int quality = 90);
    bool load() override;
    bool save(const std::string& outputPath) const override;
    std::vector<uint8_t> getPixelData() const override;
    
    void setQuality(int quality);
    int getQuality() const;
    
private:
    bool onLoadSuccess() override;
};

// PNG图片类 - 需要实现
class PNGImage : public Image {
    private:
    std::vector<uint8_t> pixelData_;
    int compressionLevel_; // PNG特有：压缩级别 (0-9)
    
public:
    // 构造函数：默认压缩级别为3
    PNGImage(const std::string& filename, int compressionLevel = 3);
    
    bool load() override;
    bool save(const std::string& outputPath) const override;
    std::vector<uint8_t> getPixelData() const override;
    
    // PNG 特有的参数设置
    void setCompressionLevel(int level);
    int getCompressionLevel() const;
    
private:
    bool onLoadSuccess() override;
};

// BMP图片类 - 需要实现  
class BMPImage : public Image {
private:
    std::vector<uint8_t> pixelData_;
    // BMP 通常不需要保存参数，或者参数很少
    
public:
    BMPImage(const std::string& filename);
    
    bool load() override;
    bool save(const std::string& outputPath) const override;
    std::vector<uint8_t> getPixelData() const override;
    
private:
    bool onLoadSuccess() override;
};

#endif
