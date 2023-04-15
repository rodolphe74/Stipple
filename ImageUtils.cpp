#include "ImageUtils.h"
#include <iostream>

#pragma warning(disable : 26451)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize.h"

void
ImageUtils::loadImageAsRGBA(const std::string& filename, rc::Image* image)
{
    int x, y, c;
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &c, 4);
    image->data = data;
    image->w = x;
    image->h = y;
    image->comp = 4;
}

void
ImageUtils::saveImageAsPNG(const std::string& filename, const rc::Image* image)
{
    stbi_write_png(filename.c_str(),
                   image->w,
                   image->h,
                   image->comp,
                   image->data,
                   image->w * image->comp);
}

void
ImageUtils::freeImage(rc::Image* image)
{
    stbi_image_free(image->data);
}

void
ImageUtils::convertRcImageToCImg(rc::Image& source, CImg<uint8_t>& target)
{
    target.assign(source.w, source.h, 4);
    for (int y = 0; y < target.height(); y++) {
        for (int x = 0; x < target.width(); ++x) {
            setCImgPixel(target, ImageUtils::getRGBA(&source, x, y), x, y);
        }
    }
}

void
ImageUtils::convertCImgToRcImage(CImg<uint8_t>& source, rc::Image& target)
{
    for (int y = 0; y < source.height(); y++) {
        for (int x = 0; x < source.width(); ++x) {
            rc::RGBA rgba = getCImgPixel(source, x, y);
            ImageUtils::setRGBA(&target, rgba, x, y);
        }
    }
}

rc::RGBA
ImageUtils::getRGBA(rc::Image* image, int x, int y)
{
    int i = x * image->comp + y * image->w * image->comp;
    uint8_t c = (uint8_t)image->comp;
    rc::RGBA r;
    r.r = c > 0 ? image->data[i] : 0;
    r.g = c > 1 ? image->data[i + 1] : 0;
    r.b = c > 2 ? image->data[i + 2] : 0;
    r.a = c > 3 ? image->data[i + 3] : 0;
    return r;
}

void
ImageUtils::setRGBA(rc::Image* image, rc::RGBA rgba, int x, int y)
{
    int i = x * image->comp + y * image->w * image->comp;
    image->data[i] = rgba.r;
    image->data[i + 1] = rgba.g;
    image->data[i + 2] = rgba.b;
    image->data[i + 3] = rgba.a;
}

void
ImageUtils::setCImgPixel(CImg<uint8_t>& img, rc::RGBA p, int x, int y)
{
    img(x, y, 0) = p.r;
    img(x, y, 1) = p.g;
    img(x, y, 2) = p.b;
    img(x, y, 3) = p.a;
}

rc::RGBA
ImageUtils::getCImgPixel(const CImg<uint8_t>& img, int x, int y)
{
    rc::RGBA p;
    p.r = img(x, y, 0);
    p.g = img(x, y, 1);
    p.b = img(x, y, 2);
    p.a = img(x, y, 3);
    return p;
}

void
ImageUtils::convertImageToGray(const rc::Image* source,
                               rc::Image* target,
                               double gamma,
                               int alpha)
{
    int r, g, b, a;
    int indexImage;
    int grayLevel;
    double lum;
    double gr, gg, gb;
    target->data =
      (uint8_t*)calloc(source->w * source->h * source->comp, sizeof(uint8_t));
    target->h = source->h;
    target->w = source->w;
    target->comp = source->comp;
    for (uint32_t y = 0; y < source->h; y++) {
        for (uint32_t x = 0; x < source->w; x++) {
            indexImage = y * source->w * 4 + x * 4;
            r = source->data[indexImage];
            g = source->data[indexImage + 1];
            b = source->data[indexImage + 2];
            a = source->data[indexImage + 3];

            // Normalize and gamma correct:
            gr = pow(r / 255.0, gamma);
            gg = pow(g / 255.0, gamma);
            gb = pow(b / 255.0, gamma);

            // Calculate luminance:
            lum = 0.2126 * gr + 0.7152 * gg + 0.0722 * gb;
            grayLevel = (uint8_t)ceil(255.0 * pow(lum, 1.0 / gamma));

            // std::cout << "graylevel:" << "  " << "alpha:" << alpha;

            if (target->data) {
                target->data[indexImage] = grayLevel;
                target->data[indexImage + 1] = grayLevel;
                target->data[indexImage + 2] = grayLevel;
                target->data[indexImage + 3] = alpha;
            }
        }
    }
}

void
ImageUtils::getRGBComponents(const rc::Image* source,
                             rc::Image* rTarget,
                             rc::Image* gTarget,
                             rc::Image* bTarget)
{
    int r, g, b, a;
    int indexImage;

    rTarget->data =
      (uint8_t*)calloc(source->w * source->h * source->comp, sizeof(uint8_t));
    rTarget->h = source->h;
    rTarget->w = source->w;
    rTarget->comp = source->comp;

    gTarget->data =
      (uint8_t*)calloc(source->w * source->h * source->comp, sizeof(uint8_t));
    gTarget->h = source->h;
    gTarget->w = source->w;
    gTarget->comp = source->comp;

    bTarget->data =
      (uint8_t*)calloc(source->w * source->h * source->comp, sizeof(uint8_t));
    bTarget->h = source->h;
    bTarget->w = source->w;
    bTarget->comp = source->comp;

    for (uint32_t y = 0; y < source->h; y++) {
        for (uint32_t x = 0; x < source->w; x++) {
            indexImage = y * source->w * 4 + x * 4;
            r = source->data[indexImage];
            g = source->data[indexImage + 1];
            b = source->data[indexImage + 2];
            a = source->data[indexImage + 3];

            if (rTarget->data) {
                rTarget->data[indexImage] = r;
                rTarget->data[indexImage + 1] = r;
                rTarget->data[indexImage + 2] = r;
                rTarget->data[indexImage + 3] = a;
            }

            if (gTarget->data) {
                gTarget->data[indexImage] = g;
                gTarget->data[indexImage + 1] = g;
                gTarget->data[indexImage + 2] = g;
                gTarget->data[indexImage + 3] = a;
            }

            if (bTarget->data) {
                bTarget->data[indexImage] = b;
                bTarget->data[indexImage + 1] = b;
                bTarget->data[indexImage + 2] = b;
                bTarget->data[indexImage + 3] = a;
            }
        }
    }
}

void
ImageUtils::getCMYKComponents(const rc::Image* source,
                              rc::Image* cTarget,
                              rc::Image* mTarget,
                              rc::Image* yTarget,
                              rc::Image* kTarget)
{
    uint8_t r, g, b, a;
    float fc, fm, fy, fk, fw;
    int indexImage;

    cTarget->data =
      (uint8_t*)calloc(source->w * source->h * source->comp, sizeof(uint8_t));
    cTarget->h = source->h;
    cTarget->w = source->w;
    cTarget->comp = source->comp;

    mTarget->data =
      (uint8_t*)calloc(source->w * source->h * source->comp, sizeof(uint8_t));
    mTarget->h = source->h;
    mTarget->w = source->w;
    mTarget->comp = source->comp;

    yTarget->data =
      (uint8_t*)calloc(source->w * source->h * source->comp, sizeof(uint8_t));
    yTarget->h = source->h;
    yTarget->w = source->w;
    yTarget->comp = source->comp;

    kTarget->data =
      (uint8_t*)calloc(source->w * source->h * source->comp, sizeof(uint8_t));
    kTarget->h = source->h;
    kTarget->w = source->w;
    kTarget->comp = source->comp;

    for (uint32_t y = 0; y < source->h; y++) {
        for (uint32_t x = 0; x < source->w; x++) {
            indexImage = y * source->w * 4 + x * 4;
            r = source->data[indexImage];
            g = source->data[indexImage + 1];
            b = source->data[indexImage + 2];
            a = source->data[indexImage + 3];

            fw = std::max(r / 255.0f, std::max(g / 255.0f, b / 255.0f));
            fc = (fw - (r / 255.0f)) / fw;
            fm = (fw - (g / 255.0f)) / fw;
            fy = (fw - (b / 255.0f)) / fw;
            fk = 1 - fw;

            // std::cout << fc << "," << fm << "," << fy << "," << fk <<
            // std::endl;

            if (cTarget->data) {
                cTarget->data[indexImage] = 255 - (uint8_t)roundf(fc * 255.0f);
                cTarget->data[indexImage + 1] =
                  255 - (uint8_t)roundf(fc * 255.0f);
                cTarget->data[indexImage + 2] =
                  255 - (uint8_t)roundf(fc * 255.0f);
                cTarget->data[indexImage + 3] = a;
            }

            if (mTarget->data) {
                mTarget->data[indexImage] = 255 - (uint8_t)roundf(fm * 255.0f);
                mTarget->data[indexImage + 1] =
                  255 - (uint8_t)roundf(fm * 255.0f);
                mTarget->data[indexImage + 2] =
                  255 - (uint8_t)roundf(fm * 255.0f);
                mTarget->data[indexImage + 3] = a;
            }

            if (yTarget->data) {
                yTarget->data[indexImage] = 255 - (uint8_t)roundf(fy * 255.0f);
                yTarget->data[indexImage + 1] =
                  255 - (uint8_t)roundf(fy * 255.0f);
                yTarget->data[indexImage + 2] =
                  255 - (uint8_t)roundf(fy * 255.0f);
                yTarget->data[indexImage + 3] = a;
            }

            if (kTarget->data) {
                kTarget->data[indexImage] = 255 - (uint8_t)roundf(fk * 255.0f);
                kTarget->data[indexImage + 1] =
                  255 - (uint8_t)roundf(fk * 255.0f);
                kTarget->data[indexImage + 2] =
                  255 - (uint8_t)roundf(fk * 255.0f);
                kTarget->data[indexImage + 3] = a;
            }
        }
    }
}

void
ImageUtils::resizeImage(const rc::Image* source,
                        rc::Image* target,
                        int32_t newSizeX,
                        int32_t newSizeY)
{
    double ratiow = (double)newSizeX / (double)source->w;
    double ratioh = (double)newSizeY / (double)source->h;
    double ratio = std::min(ratiow, ratioh);

    std::cout << "resizing image" << std::endl;
    std::cout << "image ratio:" << ratio << std::endl;
    std::cout << "image new size:" << (int32_t)round(source->w * ratio) << "x"
              << (int32_t)round(source->h * ratio) << std::endl;

    double w = round(source->w * ratio);
    double h = round(source->h * ratio);
    size_t size = (size_t)(w * h);

    // target->data = (uint8_t *) calloc(w * h * source->comp, sizeof(uint8_t));
    target->data = (uint8_t*)calloc(size * source->comp, sizeof(uint8_t));

    stbir_resize_uint8(source->data,
                       source->w,
                       source->h,
                       0,
                       target->data,
                       (int)w,
                       (int)h,
                       0,
                       source->comp);

    target->comp = source->comp;
    target->w = (int)round(w);
    target->h = (int)round(h);
}

void
ImageUtils::copy(const Image* source, Image* target, int32_t x, int32_t y)
{
    for (uint32_t j = 0; j < source->h; j++) {
        for (uint32_t i = 0; i < source->w; i++) {
            memcpy(target->data +
                     (((x + i) + (y + j) * target->w)) * target->comp,
                   source->data + ((i + j * source->w) * source->comp),
                   source->comp);
        }
    }
}

void
ImageUtils::flipVertically(rc::Image* source)
{
    Image target;
    target.w = source->w;
    target.h = source->h;
    target.comp = source->comp;
    target.data = new uint8_t[target.w * target.h * target.comp];

    int y = 0;
    for (uint32_t j = 0; j < source->h; j++) {
        y = source->h - 1 - j;
        for (uint32_t i = 0; i < source->w; i++) {
            memcpy(target.data + ((i + y * target.w)) * target.comp,
                   source->data + ((i + j * source->w) * source->comp),
                   source->comp);
        }
    }
    copy(&target, source, 0, 0);
    freeImage(&target);
}

void
ImageUtils::saveTextureAsPNG(const std::string& filename,
                             const sf::RenderTexture& rt,
                             bool flipVert)
{
    sf::Image i = rt.getTexture().copyToImage();
    const uint8_t* pixels = i.getPixelsPtr();

    rc::Image sfmlImage;
    sfmlImage.h = i.getSize().y;
    sfmlImage.w = i.getSize().x;
    sfmlImage.comp = 4;
    size_t pixelsSize = sfmlImage.h * sfmlImage.w * sfmlImage.comp;
    uint8_t* pixelsCopy = new uint8_t[pixelsSize];
    memcpy(pixelsCopy, pixels, pixelsSize);
    sfmlImage.data = pixelsCopy;
    if (flipVert)
        ImageUtils::flipVertically(&sfmlImage);
    ImageUtils::saveImageAsPNG(filename, &sfmlImage);
    ImageUtils::freeImage(&sfmlImage);
}
