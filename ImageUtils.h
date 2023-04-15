#pragma once

#include "Structs.h"
#include <CImg.h>
#include <SFML/Graphics.hpp>
#include <string>
using namespace cimg_library;

using namespace rc;

class ImageUtils
{
  public:
    static void loadImageAsRGBA(const std::string& filename, rc::Image* image);
    static void saveImageAsPNG(const std::string& filename,
                               const rc::Image* image);
    static void freeImage(rc::Image* image);
    static void convertRcImageToCImg(rc::Image& source, CImg<uint8_t>& target);
    static void convertCImgToRcImage(CImg<uint8_t>& source, rc::Image& target);
    static rc::RGBA getRGBA(rc::Image* image, int x, int y);
    static void setRGBA(rc::Image* image, rc::RGBA rgba, int x, int y);
    static void setCImgPixel(CImg<uint8_t>& img, rc::RGBA p, int x, int y);
    static rc::RGBA getCImgPixel(const CImg<uint8_t>& img, int x, int y);
    static void convertImageToGray(const rc::Image* source,
                                   rc::Image* target,
                                   double gamma,
                                   int alpha);
    static void getRGBComponents(const rc::Image* source,
                                 rc::Image* r,
                                 rc::Image* g,
                                 rc::Image* b);
    static void getCMYKComponents(const rc::Image* source,
                                  rc::Image* c,
                                  rc::Image* m,
                                  rc::Image* y,
                                  rc::Image* k);
    static void resizeImage(const rc::Image* source,
                            rc::Image* target,
                            int32_t newSizeX,
                            int32_t newSizeY);
    static void copy(const rc::Image* source,
                     rc::Image* target,
                     int32_t x,
                     int32_t y);
    static void flipVertically(rc::Image* source);
    static void saveTextureAsPNG(const std::string& filename,
                                 const sf::RenderTexture& rt,
                                 bool flipVert = true);
};
