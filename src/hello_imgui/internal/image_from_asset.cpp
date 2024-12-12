#include "hello_imgui/image_from_asset.h"

#include "hello_imgui/internal/image_abstract.h"
#include "hello_imgui/hello_imgui.h"
#include "image_opengl.h"
#include "image_dx11.h"
#include "image_metal.h"
#include "image_vulkan.h"

#include "hello_imgui/image_from_asset.h"
#include "hello_imgui/hello_imgui_assets.h"
#include "hello_imgui/runner_params.h"
#include "imgui.h"
#include "hello_imgui/hello_imgui_assets.h"
#include "hello_imgui/hello_imgui_logger.h"
#include "stb_image.h"

#include <string>
#include <unordered_map>
#include <stdexcept>


namespace HelloImGui
{
    ImVec2 ImageProportionalSize(const ImVec2& askedSize, const ImVec2& imageSize)
    {
        ImVec2 r(askedSize);

        if ((r.x == 0.f) && (r.y == 0.f))
            r = imageSize;
        else if (r.y == 0.f)
            r.y = imageSize.y / imageSize.x * r.x;
        else if (r.x == 0.f)
            r.x = imageSize.x / imageSize.y * r.y;
        return r;
    }

    static std::unordered_map<std::string, ImageAbstractPtr > gImageFromAssetMap;


    static ImageAbstractPtr _GetImageFromMemory(int width, int height, unsigned char* image_data_rgba)
    {

        HelloImGui::RendererBackendType rendererBackendType = HelloImGui::GetRunnerParams()->rendererBackendType;
        ImageAbstractPtr concreteImage;

        #ifdef HELLOIMGUI_HAS_OPENGL
            if (rendererBackendType == RendererBackendType::OpenGL3)
                concreteImage = std::make_shared<ImageOpenGl>();
        #endif
        #if defined(HELLOIMGUI_HAS_METAL)
            if (rendererBackendType == RendererBackendType::Metal)
                concreteImage = std::make_shared<ImageMetal>();
        #endif
        #if defined(HELLOIMGUI_HAS_VULKAN)
            if (rendererBackendType == RendererBackendType::Vulkan)
                concreteImage = std::make_shared<ImageVulkan>();
        #endif
        #if defined(HELLOIMGUI_HAS_DIRECTX11)
            if (rendererBackendType == RendererBackendType::DirectX11)
                concreteImage = std::make_shared<ImageDx11>();
        #endif
        if (concreteImage == nullptr)
        {
            HelloImGui::Log(LogLevel::Warning, "_GetImageFromMemory: not implemented for this rendering backend!");
            return nullptr;
        }

        IM_ASSERT(image_data_rgba != nullptr && width > 0 && height > 0 && "_GetImageFromMemory: image_data_rgba is empty!");
    
        concreteImage->_impl_StoreTexture(width, height, image_data_rgba);

        return concreteImage;
    }

    static bool _UpdateImageFromMemory(ImageAbstractPtr & concreteImage, unsigned char * image_data_rgba, int width, int height)
    {

        if (image_data_rgba == nullptr || width <= 0 || height <= 0)
        {
            IM_ASSERT(false && "_UpdateImageFromMemory: Memory image is empty!");
            throw std::runtime_error("_UpdateImageFromMemory: Memory image is empty!");
        }

        if(!concreteImage) {
            concreteImage = _GetImageFromMemory(width, height, image_data_rgba);
            return true;
        }
        
        concreteImage->_impl_UploadTexture(width, height, image_data_rgba);
        return false;
    }

    static ImageAbstractPtr _GetCachedAssetImage(const char*assetPath, bool updateCache = false)
    {
        
        ImageAbstractPtr concreteImage;
        if (gImageFromAssetMap.find(assetPath) != gImageFromAssetMap.end())
        {
            concreteImage = gImageFromAssetMap.at(assetPath);
            if (updateCache == false)
                return concreteImage;
        }

        unsigned char* image_data_rgba;
        int width, height;
        {
            // Load the image using stbi_load_from_memory
            auto assetData = LoadAssetFileData(assetPath);
            
            IM_ASSERT(assetData.data != nullptr);
            image_data_rgba = stbi_load_from_memory(
                (unsigned char *)assetData.data, (int)assetData.dataSize,
                &width, &height, NULL, 4);
            FreeAssetFileData(&assetData);
        }

        if (image_data_rgba == nullptr)
        {
            IM_ASSERT(false && "_GetCachedAssetImage: Failed to load image!");
            throw std::runtime_error("_GetCachedImage: Failed to load image!");
        }

        if(_UpdateImageFromMemory(concreteImage, image_data_rgba, width, height))   // if a new image pointer was created
            gImageFromAssetMap[assetPath] = concreteImage;

        stbi_image_free(image_data_rgba);

        return concreteImage;
    }

    static ImageAbstractPtr _GetCachedMemoryImage(const char*assetName, unsigned char * image_data_rgba, int width, int height)
    {
        ImageAbstractPtr concreteImage;
        if (gImageFromAssetMap.find(assetName) != gImageFromAssetMap.end())
        {
            concreteImage = gImageFromAssetMap.at(assetName);
            if (image_data_rgba == nullptr)     // No need to update
                return concreteImage;
        }

        if (image_data_rgba == nullptr || width <= 0 || height <= 0)
        {
            IM_ASSERT(false && "_GetCachedMemoryImage: Memory image is empty!");
            throw std::runtime_error("_GetCachedMemoryImage: Memory image is empty!");
        }

        if(_UpdateImageFromMemory(concreteImage, image_data_rgba, width, height))
            gImageFromAssetMap[assetName] = concreteImage;

        return concreteImage;
    }

    inline void _CachedImGuiImage(
        ImageAbstractPtr & cachedImage,
        const ImVec2& size,
        const ImVec2& uv0, const ImVec2& uv1,
        const ImVec4& tint_col, const ImVec4& border_col,
        const char *txtFail)
    {
        if (cachedImage == nullptr)
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), txtFail);
            return;
        }
        auto textureId = cachedImage->TextureID();
        auto imageSize = ImVec2((float)cachedImage->Width, (float)cachedImage->Height);
        ImVec2 displayedSize = ImageProportionalSize(size, imageSize);
        ImGui::Image(textureId, displayedSize, uv0, uv1, tint_col, border_col);
    }

    void ImageFromAsset(
        const char *assetPath,
        bool updateCache, 
        const ImVec2& size,
        const ImVec2& uv0, const ImVec2& uv1,
        const ImVec4& tint_col, const ImVec4& border_col)
    {
        auto cachedImage = _GetCachedAssetImage(assetPath, updateCache);
        _CachedImGuiImage(cachedImage, size, uv0, uv1, tint_col, border_col, "ImageFromAsset: fail!");
    }

    void ImageFromAsset(
        const char *assetPath, 
        const ImVec2& size,
        const ImVec2& uv0, const ImVec2& uv1,
        const ImVec4& tint_col, const ImVec4& border_col)
    {
        ImageFromAsset(assetPath, false, size, uv0, uv1, tint_col, border_col);
    }

    void ImageFromMemory(
        const char *assetName, MemoryImage image,
        const ImVec2& size,
        const ImVec2& uv0, const ImVec2& uv1,
        const ImVec4& tint_col, const ImVec4& border_col) 
    {
        auto cachedImage = _GetCachedMemoryImage(assetName, image.image_buffer_rgba, image.width, image.height);
        _CachedImGuiImage(cachedImage, size, uv0, uv1, tint_col, border_col, "ImageFromMemory: fail!");
    }

    void ImageFromMemory(
        const char *assetName,
        const ImVec2& size,
        const ImVec2& uv0, const ImVec2& uv1,
        const ImVec4& tint_col, const ImVec4& border_col) 
    {
        ImageFromMemory(assetName, {}, size, uv0, uv1, tint_col, border_col);
    }


    inline bool _CachedImguiImageButton(
        ImageAbstractPtr & cachedImage,
        const char *str_id,  const ImVec2& size, const ImVec2& uv0,  const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col,
        const char *txtFail)
    {
        if (cachedImage == nullptr)
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), txtFail);
            return false;
        }
        auto textureId = cachedImage->TextureID();
        auto imageSize = ImVec2((float)cachedImage->Width, (float)cachedImage->Height);
        ImVec2 displayedSize = ImageProportionalSize(size, imageSize);
        bool clicked = ImGui::ImageButton(str_id, textureId, displayedSize, uv0, uv1, bg_col, tint_col);
        return clicked;
    }

    bool ImageButtonFromAsset(const char *assetPath, bool updateCache,  const ImVec2& size, const ImVec2& uv0,  const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
    {
        auto cachedImage = _GetCachedAssetImage(assetPath, updateCache);
        return _CachedImguiImageButton(cachedImage, assetPath, size, uv0, uv1, frame_padding, bg_col, tint_col, "ImageButtonFromAsset: fail!");
    }

    bool ImageButtonFromAsset(const char *assetPath, const ImVec2& size, const ImVec2& uv0,  const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
    {
        return ImageButtonFromAsset(assetPath, false, size, uv0, uv1, frame_padding, bg_col, tint_col);
    }

    bool ImageButtonFromMemory(const char *assetName, MemoryImage image, const ImVec2& size, const ImVec2& uv0,  const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
    {
        auto cachedImage = _GetCachedMemoryImage(assetName, image.image_buffer_rgba, image.width, image.height);
        return _CachedImguiImageButton(cachedImage, assetName, size, uv0, uv1, frame_padding, bg_col, tint_col, "ImageButtonFromMemory: fail!");
    }

    bool ImageButtonFromMemory(const char *assetName, const ImVec2& size, const ImVec2& uv0,  const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
    {
        ImageButtonFromMemory(assetName, {}, size, uv0, uv1, frame_padding, bg_col, tint_col);
    }

    ImTextureID ImTextureIdFromAsset(const char *assetPath, bool updateCache)
    {
        auto cachedImage = _GetCachedAssetImage(assetPath, updateCache);
        if (cachedImage == nullptr)
            return ImTextureID(0);
        return cachedImage->TextureID();
    }

    ImTextureID ImTextureIdFromAsset(const char *assetPath)
    {
        return ImTextureIdFromAsset(assetPath, false);
    }

    ImTextureID ImTextureIdFromMemory(const char *assetName, MemoryImage image)
    {
        auto cachedImage = _GetCachedMemoryImage(assetName, image.image_buffer_rgba, image.width, image.height);
        if (cachedImage == nullptr)
            return ImTextureID(0);
        return cachedImage->TextureID();
    }
    
    ImTextureID ImTextureIdFromMemory(const char *assetName) 
    { 
        return ImTextureIdFromMemory(assetName, {});
    }

    ImVec2 ImageSizeFromAsset(const char *assetPath, bool updateCache)
    {
        auto cachedImage = _GetCachedAssetImage(assetPath, updateCache);
        if (cachedImage == nullptr)
            return ImVec2(0.f, 0.f);
        return ImVec2((float)cachedImage->Width, (float)cachedImage->Height);
    }

    ImVec2 ImageSizeFromAsset(const char *assetPath)
    {
        return ImageSizeFromAsset(assetPath, false);
    }

    ImVec2 ImageSizeFromMemory(const char *assetName, MemoryImage image)
    {
        auto cachedImage = _GetCachedMemoryImage(assetName, image.image_buffer_rgba, image.width, image.height);
        if (cachedImage == nullptr)
            return ImVec2(0.f, 0.f);
        return ImVec2((float)cachedImage->Width, (float)cachedImage->Height);
    }

    ImVec2 ImageSizeFromMemory(const char *assetName)
    {
        return ImageSizeFromMemory(assetName, {});
    }


    ImageAndSize ImageAndSizeFromAsset(const char *assetPath, bool updateCache)
    {
        auto cachedImage = _GetCachedAssetImage(assetPath, updateCache);
        if (cachedImage == nullptr)
            return {};
        return {cachedImage->TextureID(), ImVec2((float)cachedImage->Width, (float)cachedImage->Height)};
    }

    ImageAndSize ImageAndSizeFromAsset(const char *assetPath)
    {
        return ImageAndSizeFromAsset(assetPath, false);
    }


    ImageAndSize ImageAndSizeFromMemory(const char *assetName, MemoryImage image)
    {
        auto cachedImage = _GetCachedMemoryImage(assetName, image.image_buffer_rgba, image.width, image.height);
        if (cachedImage == nullptr)
            return {};
        return {cachedImage->TextureID(), ImVec2((float)cachedImage->Width, (float)cachedImage->Height)};
    }

    ImageAndSize ImageAndSizeFromMemory(const char *assetName)
    {
        return ImageAndSizeFromMemory(assetName, {});
    }

    namespace internal
    {
        void Free_ImageFromAssetMap()
        {
            gImageFromAssetMap.clear();
        }
    }

}
