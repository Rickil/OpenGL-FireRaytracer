#ifndef POGL_TGA_HH
#define POGL_TGA_HH


#include <cstdint>
#include <vector>
#include <fstream>
#include <cstring>

typedef union PixelInfo
{
    std::uint32_t Colour;
    struct
    {
        std::uint8_t R, G, B, A;
    };
} *PPixelInfo;

class Tga
{
private:
    std::vector<std::uint8_t> Pixels;
    std::vector<std::uint8_t> PixelsRGBA;
    bool ImageCompressed;
    std::uint32_t width, height, size, BitsPerPixel;

public:
    Tga(const char* FilePath);
    std::vector<std::uint8_t> GetRGBA() {return this->PixelsRGBA;}
    std::vector<std::uint8_t> GetPixels() {return this->Pixels;}
    std::uint32_t GetWidth() const {return this->width;}
    std::uint32_t GetHeight() const {return this->height;}
    bool HasAlphaChannel() {return BitsPerPixel == 32;}
};

Tga::Tga(const char* FilePath)
{
    std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
    if (!hFile.is_open()){throw std::invalid_argument("File Not Found.");}

    std::uint8_t Header[18] = {0};
    std::vector<std::uint8_t> ImageData;
    static std::uint8_t DeCompressed[12] = {0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    static std::uint8_t IsCompressed[12] = {0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

    hFile.read(reinterpret_cast<char*>(&Header), sizeof(Header));

    if (!std::memcmp(DeCompressed, &Header, sizeof(DeCompressed)))
    {
        BitsPerPixel = Header[16];
        width  = Header[13] * 256 + Header[12];
        height = Header[15] * 256 + Header[14];
        size  = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
        {
            hFile.close();
            throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit Image.");
        }

        ImageData.resize(size);
        ImageCompressed = false;
        hFile.read(reinterpret_cast<char*>(ImageData.data()), size);
    }
    else if (!std::memcmp(IsCompressed, &Header, sizeof(IsCompressed)))
    {
        BitsPerPixel = Header[16];
        width  = Header[13] * 256 + Header[12];
        height = Header[15] * 256 + Header[14];
        size  = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
        {
            hFile.close();
            throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit Image.");
        }

        PixelInfo Pixel = {0};
        int CurrentByte = 0;
        std::size_t CurrentPixel = 0;
        ImageCompressed = true;
        std::uint8_t ChunkHeader = {0};
        int BytesPerPixel = (BitsPerPixel / 8);
        ImageData.resize(width * height * sizeof(PixelInfo));

        do
        {
            hFile.read(reinterpret_cast<char*>(&ChunkHeader), sizeof(ChunkHeader));

            if(ChunkHeader < 128)
            {
                ++ChunkHeader;
                for(int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
                {
                    hFile.read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

                    ImageData[CurrentByte++] = Pixel.B;
                    ImageData[CurrentByte++] = Pixel.G;
                    ImageData[CurrentByte++] = Pixel.R;
                    if (BitsPerPixel > 24) ImageData[CurrentByte++] = Pixel.A;
                }
            }
            else
            {
                ChunkHeader -= 127;
                hFile.read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

                for(int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
                {
                    ImageData[CurrentByte++] = Pixel.B;
                    ImageData[CurrentByte++] = Pixel.G;
                    ImageData[CurrentByte++] = Pixel.R;
                    if (BitsPerPixel > 24) ImageData[CurrentByte++] = Pixel.A;
                }
            }
        } while(CurrentPixel < (width * height));
    }
    else
    {
        hFile.close();
        throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit TGA File.");
    }

    hFile.close();
    this->Pixels = ImageData;

    this->PixelsRGBA.resize(width*height*4);

    for (std::size_t i = 0; i < width * height; ++i)
    {
        if (BitsPerPixel == 32) {
            std::size_t bgraIndex = i * 4;
            std::size_t rgbaIndex = i * 4;

            this->PixelsRGBA[rgbaIndex + 0] = this->Pixels[bgraIndex + 2];  // R
            this->PixelsRGBA[rgbaIndex + 1] = this->Pixels[bgraIndex + 1];  // G
            this->PixelsRGBA[rgbaIndex + 2] = this->Pixels[bgraIndex + 0];  // B
            this->PixelsRGBA[rgbaIndex + 3] = this->Pixels[bgraIndex + 3];  // A
        } else {
            std::size_t bgraIndex = i * 3;
            std::size_t rgbaIndex = i * 4;

            this->PixelsRGBA[rgbaIndex + 0] = this->Pixels[bgraIndex + 2];  // R
            this->PixelsRGBA[rgbaIndex + 1] = this->Pixels[bgraIndex + 1];  // G
            this->PixelsRGBA[rgbaIndex + 2] = this->Pixels[bgraIndex + 0];  // B
            this->PixelsRGBA[rgbaIndex + 3] = 255;  // A
        }
    }


    int a = 2;
}


#endif //POGL_TGA_HH
