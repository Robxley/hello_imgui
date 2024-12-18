#pragma once

#include "imgui.h"

#include <memory>

namespace HelloImGui
{
    class ImageAbstract
    {
    public:
         int Width = 0;
         int Height = 0;
         virtual ImTextureID TextureID() = 0;

        ImageAbstract() = default;
        virtual ~ImageAbstract();

        virtual void _impl_StoreTexture(int width, int height, unsigned char* image_data_rgba) = 0;
        virtual void _impl_UploadTexture(int width, int height, unsigned char* image_data_rgba) {
            _impl_ReleaseTexture();
            _impl_StoreTexture(width, height, image_data_rgba);
        };
        virtual void _impl_ReleaseTexture() = 0;
    };

    using ImageAbstractPtr = std::shared_ptr<ImageAbstract>;
}
