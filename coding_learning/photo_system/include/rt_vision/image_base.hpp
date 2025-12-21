#ifndef IMAGE_BASE_HPP
#define IMAGE_BASE_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

// 图片基类 - 需要完善实现
class Image {
protected:
    std::string filename_;
    int width_, height_;
    bool loaded_;
    
public:
    Image(const std::string& filename);
    virtual ~Image() = default;
    
    // 纯虚函数 - 需要实现
    virtual bool load() = 0;
    virtual bool save(const std::string& outputPath) const = 0;
    virtual std::vector<uint8_t> getPixelData() const = 0;
    
    // 已实现的基础方法
    std::string getFilename() const { return filename_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    bool isLoaded() const { return loaded_; }
    
    // 模板方法 - 需要完善
    bool loadWithValidation();
    
protected:
    virtual bool onLoadSuccess();
};

#endif