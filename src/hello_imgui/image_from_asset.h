#pragma once
#include "imgui.h"

namespace HelloImGui
{
// @@md#HelloImGui::ImageFromAsset

//
//Images are loaded when first displayed, and then cached
// (they will be freed just before the application exits).
//
//For example, given this files structure:
//```
//├── CMakeLists.txt
//├── assets/
//│         └── my_image.jpg
//└── my_app.main.cpp
//```
//
//then, you can display "my_image.jpg", using:
//
//    ```cpp
//    HelloImGui::ImageFromAsset("my_image.jpg");
//    ```


// `HelloImGui::ImageFromAsset(const char *assetPath, size, ...)`: 
// will display a static image from the assets.
void ImageFromAsset(const char *assetPath, const ImVec2& size = ImVec2(0, 0),
                    const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1),
                    const ImVec4& tint_col = ImVec4(1,1,1,1),
                    const ImVec4& border_col = ImVec4(0,0,0,0));

// `HelloImGui::ImageFromAsset(const char *assetPath, updateCache, size, ...)`: 
// will display a static image from the assets with the ability to update the image cache.
void ImageFromAsset(const char *assetPath, bool updateCache, const ImVec2& size = ImVec2(0, 0),
                    const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1),
                    const ImVec4& tint_col = ImVec4(1,1,1,1),
                    const ImVec4& border_col = ImVec4(0,0,0,0));

struct MemoryImage
{
    unsigned char * image_buffer_rgba = nullptr;
    int width = 0, height = 0;
};

// `HelloImGui::ImageFromMemory(const char *assetName, MemoryImage image, size, ...)`:
// will display a static image from memory, identified by `assetName`.
// This overload will update the cache if image is not nullptr.
void ImageFromMemory(const char *assetName, MemoryImage image,
                    const ImVec2& size = ImVec2(0, 0),
                    const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1),
                    const ImVec4& tint_col = ImVec4(1,1,1,1),
                    const ImVec4& border_col = ImVec4(0,0,0,0));

// `HelloImGui::ImageFromMemory(const char *assetName, size, ...)`
// will display a static image from memory, identified by `assetName`.
// This overload will never update the cache. *FromMemory overloads with MemoryImage have to be called once before using this.
void ImageFromMemory(const char *assetName,
                    const ImVec2& size = ImVec2(0, 0),
                    const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1),
                    const ImVec4& tint_col = ImVec4(1,1,1,1),
                    const ImVec4& border_col = ImVec4(0,0,0,0));                 

// `bool HelloImGui::ImageButtonFromAsset(const char *assetPath, size, ...)`:
// will display a button using an image from the assets.
bool ImageButtonFromAsset(const char *assetPath, const ImVec2& size = ImVec2(0, 0),
                          const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1),
                          int frame_padding = -1,
                          const ImVec4& bg_col = ImVec4(0,0,0,0),
                          const ImVec4& tint_col = ImVec4(1,1,1,1));

// `bool HelloImGui::ImageButtonFromAsset(const char *assetPath, updateCache, size, ...)`:
// will display a button using an image from the assets with the ability to update the image cache.
bool ImageButtonFromAsset(const char *assetPath, bool updateCache, const ImVec2& size = ImVec2(0, 0),
                          const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1),
                          int frame_padding = -1,
                          const ImVec4& bg_col = ImVec4(0,0,0,0),
                          const ImVec4& tint_col = ImVec4(1,1,1,1));

// `bool HelloImGui::ImageButtonFromAsset(const char *assetPath, image, size, ...)`:
// will display a button using an image from memory with the ability to update the image cache (if image is not nullptr).
bool ImageButtonFromMemory(const char *assetName, MemoryImage image, const ImVec2& size = ImVec2(0, 0),
                           const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1),
                           int frame_padding = -1,
                           const ImVec4& bg_col = ImVec4(0,0,0,0),
                           const ImVec4& tint_col = ImVec4(1,1,1,1));

// `bool HelloImGui::ImageButtonFromAsset(const char *assetPath, size, ...)`:
// will display a button using an image from memory.  *FromMemory overloads with MemoryImage have to be called once before using this.
bool ImageButtonFromMemory(const char *assetName, const ImVec2& size = ImVec2(0, 0),
                           const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1),
                           int frame_padding = -1,
                           const ImVec4& bg_col = ImVec4(0,0,0,0),
                           const ImVec4& tint_col = ImVec4(1,1,1,1));

// `ImTextureID HelloImGui::ImTextureIdFromAsset(assetPath)`:
// will return a texture ID for an image loaded from the assets.
ImTextureID ImTextureIdFromAsset(const char *assetPath);

// `ImTextureID HelloImGui::ImTextureIdFromAsset(assetPath)`:
// will return a texture ID for an image loaded from the assets with the ability to update the image cache.
ImTextureID ImTextureIdFromAsset(const char *assetPath, bool updateCache);

// `ImTextureID HelloImGui::ImTextureIdFromAsset(assetName, image)`:
// will return a texture ID for an image loaded from the memory with the ability to update the image cache (if image is not nullptr).
ImTextureID ImTextureIdFromMemory(const char *assetName,  MemoryImage image);

// `ImTextureID HelloImGui::ImTextureIdFromMemory(assetName,...)`:
// will return a texture ID for an image loaded from the memory.  *FromMemory overloads with MemoryImage have to be called once before using this.
ImTextureID ImTextureIdFromMemory(const char *assetName);

// `ImVec2 HelloImGui::ImageSizeFromAsset(assetPath)`:
// will return the size of an image loaded from the assets.
ImVec2 ImageSizeFromAsset(const char *assetPath);

// `ImVec2 HelloImGui::ImageSizeFromAsset(assetPath)`:
// will return the size of an image loaded from the assets with the ability to update the image cache.
ImVec2 ImageSizeFromAsset(const char *assetPath, bool updateCache);

// `ImVec2 HelloImGui::ImageSizeFromMemory(assetPath, image)`:
// will return the size of an image loaded from the memory .
ImVec2 ImageSizeFromMemory(const char *assetName, MemoryImage image);

// `ImVec2 HelloImGui::ImageSizeFromMemory(assetPath)`:
// will return the size of an image loaded from the memory.  *FromMemory overloads with MemoryImage have to be called once before using this.
ImVec2 ImageSizeFromMemory(const char *assetName);


struct ImageAndSize
{
    ImTextureID textureId = ImTextureID(0);
    ImVec2 size = ImVec2(0.f, 0.f);
};
// `HelloImGui::ImageAndSize HelloImGui::ImageAndSizeFromAsset(assetPath)`:
// will return the texture ID and the size of an image loaded from the assets.
ImageAndSize ImageAndSizeFromAsset(const char *assetPath);

// `HelloImGui::ImageAndSize HelloImGui::ImageAndSizeFromAsset(assetPath, updateCache)`:
// will return the texture ID and the size of an image loaded from the assets with the ability to update the image cache.
ImageAndSize ImageAndSizeFromAsset(const char *assetPath, bool updateCache);

// `HelloImGui::ImageAndSize HelloImGui::ImageAndSizeFromMemory(assetPath, image)`:
// will return the texture ID and the size of an image loaded from the memory.
ImageAndSize ImageAndSizeFromMemory(const char *assetName, MemoryImage image);

// `HelloImGui::ImageAndSize HelloImGui::ImageAndSizeFromMemory(assetPath)`:
// will return the texture ID and the size of an image loaded from the memory.  *FromMemory overloads with MemoryImage have to be called once before using this.
ImageAndSize ImageAndSizeFromMemory(const char *assetName); 


// `ImVec2 HelloImGui::ImageProportionalSize(askedSize, imageSize)`:
//  will return the displayed size of an image.
//     - if askedSize.x or askedSize.y is 0, then the corresponding dimension
//       will be computed from the image size, keeping the aspect ratio.
//     - if askedSize.x>0 and askedSize.y> 0, then the image will be scaled to fit
//       exactly the askedSize, thus potentially changing the aspect ratio.
//  Note: this function is used internally by ImageFromAsset and ImageButtonFromAsset,
//        so you don't need to call it directly.
ImVec2 ImageProportionalSize(const ImVec2& askedSize, const ImVec2& imageSize);

// @@md

namespace internal
{
    void Free_ImageFromAssetMap();
}
}
