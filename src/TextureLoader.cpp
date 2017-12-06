#include "TextureLoader.hpp"

bool TextureLoader::_isILinitialized = false;

bool TextureLoader::loadTexture(const char* path, Texture& t)
{
    if (!_isILinitialized)
    {
        ilInit();
        _isILinitialized = true;
    }

    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);

    ilLoadImage(path);

    ILenum Error;
    Error = ilGetError();

    if (Error != IL_NO_ERROR)
    {
        std::wcerr << "DevIL: Failed to load image " << path << ", error: " << Error << std::endl;
        return false;
    }

    //Convert the texture to R8G8B8A8
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    t.width = ilGetInteger(IL_IMAGE_WIDTH);
    t.height = ilGetInteger(IL_IMAGE_HEIGHT);

    t.data = std::shared_ptr<unsigned char>(new unsigned char[4 * t.width*t.height]);
    memcpy(t.data.get(), ilGetData(), 4 * t.width*t.height);

    ilDeleteImage(image);

    return true;
}