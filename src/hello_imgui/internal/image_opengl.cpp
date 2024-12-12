#ifdef HELLOIMGUI_HAS_OPENGL
#include "image_opengl.h"

#include "imgui.h"


namespace HelloImGui
{

    void _UpdateOrphanTexture(GLuint textureID, int width, int height, unsigned char* image_data_rgba) 
    {
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Orphan the existing texture by reallocating it with nullptr
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // Upload new data using glTexSubImage2D
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data_rgba);
    }


    void ImageOpenGl::_impl_StoreTexture(int width, int height, unsigned char* image_data_rgba)
    {
        auto& self = *this;
        if (self.TextureId == 0)
            glGenTextures(1, &self.TextureId);
        glBindTexture(GL_TEXTURE_2D, self.TextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(HELLOIMGUI_USE_GLES2) || defined(HELLOIMGUI_USE_GLES3)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     width,
                     height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data_rgba);
    }

    void ImageOpenGl::_impl_UploadTexture(int width, int height, unsigned char* image_data_rgba)
    {
        auto& self = *this;
        if (self.TextureId == 0) {
            _impl_StoreTexture(width, height, image_data_rgba);
            return;
        }
        _UpdateOrphanTexture(self.TextureId, width, height, image_data_rgba);
    }


    ImTextureID ImageOpenGl::TextureID()
    {
        auto& self = *this;
        return (ImTextureID)(intptr_t)self.TextureId;
    }


    void ImageOpenGl::_impl_ReleaseTexture()
    {
        glDeleteTextures(1, &TextureId);
        TextureId = 0;
    }

    ImageOpenGl::~ImageOpenGl()
    {
        _impl_ReleaseTexture();
    }

}

#endif // #ifdef HELLOIMGUI_HAS_OPENGL
