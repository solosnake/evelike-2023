#include "solosnake/image.hpp"
#include <cassert>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <png.h>

namespace solosnake
{
    namespace
    {
        void err(const char *e)
        {
            std::cerr << e << std::endl;
        }

        typedef std::uint8_t GEByte;

#define DEFAULT_ALPHA ((0u))

        //-------------------------------------------------------------------------
        // Sizes of the different structures.
        //-------------------------------------------------------------------------
#define GE_VERSION3BMPSIZE (40)
#define GE_ADOBEALPHASIZE (56)
#define GE_VERSION4BMPSIZE (108)
#define GE_VERSION5BMPSIZE (124)

        //-------------------------------------------------------------------------
        // Compression codes.
        //-------------------------------------------------------------------------
#define GE_BI_RGB (0L) //!< Currently this is the only type supported by GEDIB_RGBA, uncompressed.
#define GE_BI_RLE8 (1L)
#define GE_BI_RLE4 (2L)
#define GE_BI_BITFIELDS (3L)

        //-------------------------------------------------------------------------
        // Colour masks for 32 bit rgba: RGB888
        //-------------------------------------------------------------------------
#define GE_RGBA888MASKA (0xFF000000)
#define GE_RGBA888MASKR (0x00FF0000)
#define GE_RGBA888MASKG (0x0000FF00)
#define GE_RGBA888MASKB (0x000000FF)

        //-------------------------------------------------------------------------
        // Standard colour space value. We will only use 0x1.
        //-------------------------------------------------------------------------
#define GE_CALIBRATEDRGB (0x0)
#define GE_DEVICEDEPENDANTRGB (0x1)
#define GE_DEVICEDEPENDANTCMYK (0x2)

        //-------------------------------------------------------------------------
        // Definitions for use with version 5 bitmaps. Note that I do not know if
        // these are the actual values!
        //-------------------------------------------------------------------------

        //! Match Absolute Colorimetric Maintains the white point. Matches the
        //! colors to their nearest color in the destination gamut.
#define GE_LCS_GM_ABS_COLORIMETRIC (0x1)

        //! Graphic Saturation Maintains saturation. Used for business charts
        //! and other situations in which undithered colors are required.
#define GE_LCS_GM_BUSINESS (0x2)

        //! Proof Relative Colorimetric Maintains colorimetric match. Used for
        //! graphic designs and named colors.
#define GE_LCS_GM_GRAPHICS (0x3)

        //! Picture Perceptual Maintains contrast. Used for photographs and natural images.
#define GE_LCS_GM_IMAGES (0x4)

#define GE_BM_IDENTIFIER (0x4D42)

        //-------------------------------------------------------------------------
        //! See http://en.wikipedia.org/wiki/BMP_file_format
        //-------------------------------------------------------------------------
        struct GE_MSBITMAPFILEHEADERMAGICNUMBER
        {
            GE_MSBITMAPFILEHEADERMAGICNUMBER()
                : bfType(GE_BM_IDENTIFIER)
            {
            }
            std::uint16_t bfType;
        };

        //-------------------------------------------------------------------------
        //! Bitmap file header information structure, minus the above magic number
        //! which causes the alignment problems.
        //-------------------------------------------------------------------------
        struct GE_MSBITMAPFILEHEADER2
        {
            GE_MSBITMAPFILEHEADER2()
                : bfSize(0), bfReserved1and2(0), bfOffBits(0)
            {
            }

            std::uint32_t bfSize;
            std::uint32_t bfReserved1and2; // Always zero.
            std::uint32_t bfOffBits;
        };

        //-------------------------------------------------------------------------
        //! This is exactly the same as the version 3.x BITMAPINFOHEADER structure.
        //-------------------------------------------------------------------------
        struct GE_MSBITMAPINFOHEADER // bmih
        {
            GE_MSBITMAPINFOHEADER()
                : biSize(GE_VERSION3BMPSIZE), biWidth(0), biHeight(0), biPlanes(1), biBitCount(24), biCompression(GE_BI_RGB), biSizeImage(0), biXPelsPerMeter(1), biYPelsPerMeter(1), biClrUsed(0), biClrImportant(0)
            {
            }

            std::uint32_t biSize;         // Size of the header structure.
            std::int32_t biWidth;         // Width of Image in pixels.
            std::int32_t biHeight;        // Height of the Image in pixels.
            std::uint16_t biPlanes;       // Number of colour planes, only 1 is possible.
            std::uint16_t biBitCount;     // Default is 24 bit.
            std::uint32_t biCompression;  // The only one we currently support is GE_BI_RGB.
            std::uint32_t biSizeImage;    // Pixels bytes count.
            std::int32_t biXPelsPerMeter; // Vertical resolution of the Image in pixels per metre.
            std::int32_t biYPelsPerMeter; // Horizontal resolution of the Image in pixels per metre.
            std::uint32_t biClrUsed;      // Number of colours in Image.
            std::uint32_t biClrImportant; // All colours are to be considered equally important (flag == 0).
        };

        //-------------------------------------------------------------------------
        //! For ADOBE X8 R8 G8 B8 (also exported from GIMP.
        //-------------------------------------------------------------------------
        struct GE_RGBAMASKS
        {
            GE_RGBAMASKS()
                : biRedMask(0), biGreenMask(0), biBlueMask(0), biAlphaMask(0)
            {
            }

            std::uint32_t biRedMask;   //! Mask identifying bits of red component.
            std::uint32_t biGreenMask; //! Mask identifying bits of green component.
            std::uint32_t biBlueMask;  //! Mask identifying bits of blue component.
            std::uint32_t biAlphaMask; //! Mask identifying bits of alpha component.
        };

        //-------------------------------------------------------------------------
        //! For version 4.x bmps. which are capable of storing 32 bit images containing alpha,
        //! the structure is a 108 byte amalgamation of the above structure with this one. If
        //! the biSize member of the MSBITMAPINFOHEADER read from file equals 108 then we
        //! load this extra structure.
        //-------------------------------------------------------------------------
        struct GE_MSINFOHEADERV4
        {
            GE_MSINFOHEADERV4()
                : rgbaMasks(), biCSType(0), biRedX(0), biRedY(0), biRedZ(0), biGreenX(0), biGreenY(0), biGreenZ(0), biBlueX(0), biBlueY(0), biBlueZ(0), biGammaRed(0), biGammaGreen(0), biGammaBlue(0)
            {
            }

            GE_RGBAMASKS rgbaMasks;
            std::uint32_t biCSType;     //! Colour space type.
            std::uint64_t biRedX;       //! X Coordinate of red endpoint.
            std::uint64_t biRedY;       //! Y Coordinate of red endpoint.
            std::uint64_t biRedZ;       //! Z Coordinate of red endpoint.
            std::uint64_t biGreenX;     //! X Coordinate of green endpoint.
            std::uint64_t biGreenY;     //! Y Coordinate of green endpoint.
            std::uint64_t biGreenZ;     //! Z Coordinate of green endpoint.
            std::uint64_t biBlueX;      //! X Coordinate of blue endpoint.
            std::uint64_t biBlueY;      //! Y Coordinate of blue endpoint.
            std::uint64_t biBlueZ;      //! Z Coordinate of blue endpoint.
            std::uint32_t biGammaRed;   //! Gamma red coordinate scale value.
            std::uint32_t biGammaGreen; //! Gamma blue coordinate scale value.
            std::uint32_t biGammaBlue;  //! Gamma green coordinate scale value.
        };

        //-------------------------------------------------------------------------
        //! For version 5.x bmps. the following must be appended to the above two structures.
        //-------------------------------------------------------------------------
        struct GE_MSINFOHEADERV5
        {
            GE_MSINFOHEADERV5()
                : biIntent(0), biProfileData(0), biProfileSize(0), biReserved(0)
            {
            }

            std::uint32_t biIntent;      //! Rendering intent for bitmap.
            std::uint32_t biProfileData; //! The offset, in bytes, from the beginning of the BITMAPV5HEADER structure to the start of the profile data. If
            std::uint32_t biProfileSize; //! Size, in bytes, of embedded profile data.
            std::uint32_t biReserved;    //! Should be zero.
        };

        //-------------------------------------------------------------------------
        //! Reads in the data from a 24 bit uncompressed version 3.x bitmap, top-down
        //! or bottom up. The pbuf is assumed to be a valid buffer of the correct size.
        //-------------------------------------------------------------------------
        void ReadType3toBGRA(
            std::istream &ifile,
            GEByte *const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const bool bottomup)
        {
            //
            // The file pointer should now be at the start of the bits
            // as there is no colour table. We want to read in the bitmap bits
            // in the correct order. Note that DIBs are 32 bit WORD aligned,
            // and zero padded to make life difficult. Oh, and IBM thought it
            // would be nice for them to be stored upside down too. Note also
            // that in Windows a WORD is 16 bit and DWORD is 32 bit.
            //
            if (bottomup)
            {
                for (std::int32_t i = h - 1; i >= 0; --i)
                {
                    ifile.read(reinterpret_cast<char *>(pbuf + (4 * w * i)), 4 * w); // Read in Blue, Green, Red, Alpha
                }
            }
            else
            {
                // Optimal! :)
                ifile.read(reinterpret_cast<char *>(pbuf), 4 * w * h); // Read in Blue, Green, Red, Alpha
            }
        }

        //-------------------------------------------------------------------------
        //! Reads in the data from a 24 bit uncompressed version 3.x bitmap, top-down
        //! or bottom up. The pbuf is assumed to be a valid buffer of the correct size.
        //-------------------------------------------------------------------------
        void ReadType3toBGR(
            std::istream &ifile,
            GEByte *const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const bool bottomup)
        {
            GEByte *pwork = 0; // Working pointer.
            std::uint32_t pad; // Size of padding in bytes.

            //
            // Find padded width of bitmap in bytes. This is the min no of WORDs
            // that can hold the rgb bytes.
            //
            pad = 4 - (w * 3) % 4;
            pad %= 4;

            //
            // The file pointer should now be at the start of the bits
            // as there is no colour table. We want to read in the bitmap bits
            // in the correct order. Note that DIBs are 32 bit WORD aligned,
            // and zero padded to make life difficult. Oh, and IBM thought it would
            // be nice for them to be stored upside down too. Note also that in Windows
            // a WORD is 16 bit and DWORD is 32 bit.
            //
            if (bottomup)
            {
                if (pad)
                {
                    for (std::int32_t i = h - 1; i >= 0; --i)
                    {
                        pwork = pbuf + (w * i * 3);

                        ifile.read(reinterpret_cast<char *>(pwork), 3 * w); // Read in Blue, Green, Red

                        ifile.ignore(static_cast<std::uint64_t>(pad)); // Skip padding bytes.
                    }
                }
                else
                {
                    for (std::int32_t i = h - 1; i >= 0; --i)
                    {
                        ifile.read(reinterpret_cast<char *>(pbuf + (3 * w * i)), 3 * w); // Read in Blue, Green, Red
                    }
                }
            }
            else
            {
                if (pad)
                {
                    for (std::int32_t i = 0; i < h; ++i)
                    {
                        pwork = pbuf + (w * i * 3);

                        ifile.read(reinterpret_cast<char *>(pwork), 3 * w); // Read in Blue, Green, Red

                        ifile.ignore(static_cast<std::uint64_t>(pad)); // Skip padding bytes.
                    }
                }
                else
                {
                    // Optimal! :)
                    ifile.read(reinterpret_cast<char *>(pbuf), 3 * w * h); // Read in Blue, Green, Red
                }
            }
        }

        //-------------------------------------------------------------------------
        //! Reads in the data from a 24 bit uncompressed version 3.x bitmap, top-down
        //! or bottom up. The pbuf is assumed to be a valid buffer of the correct size.
        //-------------------------------------------------------------------------
        void ReadType3toRGBA(
            std::istream &ifile,
            GEByte *const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const bool bottomup)
        {
            GEByte *pwork = 0; // Working pointer.

            //
            // The file pointer should now be at the start of the bits
            // as there is no colour table. We want to read in the bitmap bits
            // in the correct order. Note that DIBs are 32 bit WORD aligned,
            // and zero padded to make life difficult. Oh, and IBM thought it would
            // be nice for them to be stored upside down too. Note also that in Windows
            // a WORD is 16 bit and DWORD is 32 bit.
            //
            if (bottomup)
            {
                for (std::int32_t i = h - 1; i >= 0; --i)
                {
                    pwork = pbuf + (w * i * 4);

                    for (std::int32_t j = 0, k = 0; j < w; ++j)
                    {
                        ifile.read(reinterpret_cast<char *>(pwork + k + 2), 1); // Read in Blue.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 1), 1); // Read in Green.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 0), 1); // Read in Red.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 3), 1); // Read in Alpha.
                        k += 4;
                    }
                }
            }
            else
            {
                for (std::int32_t i = 0; i < h; ++i)
                {
                    pwork = pbuf + (w * i * 4);

                    for (std::int32_t j = 0, k = 0; j < w; ++j)
                    {
                        ifile.read(reinterpret_cast<char *>(pwork + k + 2), 1); // Read in Blue.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 1), 1); // Read in Green.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 0), 1); // Read in Red.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 3), 1); // Read in Alpha.
                        k += 4;
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        //! Reads in the data from a 24 bit uncompressed version 3.x bitmap, top-down
        //! or bottom up. The pbuf is assumed to be a valid buffer of the correct size.
        //-------------------------------------------------------------------------
        void ReadType3toRGB(
            std::istream &ifile,
            GEByte *const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const bool bottomup)
        {
            GEByte *pwork = 0; // Working pointer.
            std::uint32_t pad; // Size of padding in bytes.

            //
            // Find padded width of bitmap in bytes. This is the min no of WORDs
            // that can hold the rgb bytes.
            //
            pad = 4 - (w * 3) % 4;
            pad %= 4;

            //
            // The file pointer should now be at the start of the bits
            // as there is no colour table. We want to read in the bitmap bits
            // in the correct order. Note that DIBs are 32 bit WORD aligned,
            // and zero padded to make life difficult. Oh, and IBM thought it would
            // be nice for them to be stored upside down too. Note also that in Windows
            // a WORD is 16 bit and DWORD is 32 bit.
            //
            if (bottomup)
            {
                for (std::int32_t i = h - 1; i >= 0; --i)
                {
                    pwork = pbuf + (w * i * 3);

                    for (std::int32_t j = 0, k = 0; j < w; ++j)
                    {
                        ifile.read(reinterpret_cast<char *>(pwork + k + 2), 1); // Read in Blue.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 1), 1); // Read in Green.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 0), 1); // Read in Red.
                        k += 3;
                    }

                    if (pad)
                    {
                        ifile.ignore(static_cast<std::uint64_t>(pad)); // Skip padding bytes.
                    }
                }
            }
            else
            {
                for (std::int32_t i = 0; i < h; ++i)
                {
                    pwork = pbuf + (w * i * 3);

                    for (std::int32_t j = 0, k = 0; j < w; ++j)
                    {
                        ifile.read(reinterpret_cast<char *>(pwork + k + 2), 1); // Read in Blue.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 1), 1); // Read in Green.
                        ifile.read(reinterpret_cast<char *>(pwork + k + 0), 1); // Read in Red.
                        k += 3;
                    }

                    if (pad)
                    {
                        ifile.ignore(static_cast<std::uint64_t>(pad)); // Skip padding bytes.
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        //!  Reads in a type 4 32 bit bitmap. This type has the full 32 bits we need,
        //!  and defines a mask for the red, green, blue and alpha components. As a result
        //!  any mask requests are ignored on the part of the user.
        //-------------------------------------------------------------------------
        void ReadType4(
            std::istream &ifile,
            GEByte *const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const GE_RGBAMASKS &masks,
            const bool wantBGRA)
        {
            GEByte *pwork = pbuf;

            std::uint32_t colour;
            std::uint32_t c[4] = {masks.biRedMask, masks.biGreenMask, masks.biBlueMask, masks.biAlphaMask};
            std::uint32_t shift[4] = {0};

            // Calculate shift numbers for each mask:
            if (c[0])
            {
                while (false == (c[0] & 0x1))
                {
                    c[0] >>= 1;
                    ++shift[0];
                } // Red
            }

            if (c[1])
            {
                while (false == (c[1] & 0x1))
                {
                    c[1] >>= 1;
                    ++shift[1];
                } // Green
            }

            if (c[2])
            {
                while (false == (c[2] & 0x1))
                {
                    c[2] >>= 1;
                    ++shift[2];
                } // Blue
            }

            if (c[3])
            {
                while (false == (c[3] & 0x1))
                {
                    c[3] >>= 1;
                    ++shift[3];
                } // Alpha
            }

            for (std::int32_t i = h - 1; i >= 0; --i)
            {
                pwork = pbuf + (w * i * 4);

                for (std::int32_t j = 0, k = 0; j < w; ++j, k += 4)
                {
                    ifile.read(reinterpret_cast<char *>(&colour), 4); // Read in the bytes.

                    // "on some platforms, such as Windows, BGRA is preferred."
                    if (wantBGRA)
                    {
                        *(pwork + k + 0) = static_cast<GEByte>((colour & masks.biBlueMask) >> shift[2]);
                        *(pwork + k + 1) = static_cast<GEByte>((colour & masks.biGreenMask) >> shift[1]);
                        *(pwork + k + 2) = static_cast<GEByte>((colour & masks.biRedMask) >> shift[0]);
                        *(pwork + k + 3) = static_cast<GEByte>((colour & masks.biAlphaMask) >> shift[3]);
                    }
                    else
                    {
                        // RGBA
                        *(pwork + k + 0) = static_cast<GEByte>((colour & masks.biRedMask) >> shift[0]);
                        *(pwork + k + 1) = static_cast<GEByte>((colour & masks.biGreenMask) >> shift[1]);
                        *(pwork + k + 2) = static_cast<GEByte>((colour & masks.biBlueMask) >> shift[2]);
                        *(pwork + k + 3) = static_cast<GEByte>((colour & masks.biAlphaMask) >> shift[3]);
                    }
                }
            }
        }

        void add_alpha(std::vector<std::uint8_t>& pixels,
                       const std::uint32_t w,
                       const std::uint32_t h)
        {
            assert( pixels.size() == (3u * w * h) );
            pixels.resize( 4u * w * h, DEFAULT_ALPHA );
            if((w * h) > 1)
            {
                for(auto pixel = (w * h); pixel > 0u; --pixel)
                {
                    auto index = pixel - 1u;
                    pixels[ 0u + index * 4u ] = pixels[ 0u + index * 3u ];
                    pixels[ 1u + index * 4u ] = pixels[ 1u + index * 3u ];
                    pixels[ 2u + index * 4u ] = pixels[ 2u + index * 3u ];
                }
            }
        }

        void remove_alpha(std::vector<std::uint8_t>& pixels,
                          const std::uint32_t w,
                          const std::uint32_t h)
        {
            assert( pixels.size() == (4u * w * h) );
            for(auto pixel = 1u; pixel < (w * h); ++pixel)
            {
                pixels[ 0u + pixel * 3u ] = pixels[ 0u + pixel * 4u ];
                pixels[ 1u + pixel * 3u ] = pixels[ 1u + pixel * 4u ];
                pixels[ 2u + pixel * 3u ] = pixels[ 2u + pixel * 4u ];
            }
            pixels.resize( 3u * w * h );
        }

        // Sentinel used to clean up.
        class Sentinel
        {
        public:

            explicit Sentinel(std::function<void(void)> d) : m_dtor(d)
            {
            }

            ~Sentinel() noexcept
            {
                clean_up();
            }

            void clean_up() noexcept
            {
                m_dtor();
                std::function<void(void)> f = [](){};
                m_dtor.swap(f);
            }

        private:
            std::function<void(void)> m_dtor;
        };

        void read_png_data_from_ifstream(png_structp png_ptr, png_bytep data, png_size_t length)
        {
            png_voidp p = png_get_io_ptr(png_ptr);
            static_cast<std::ifstream*>(p)->read(reinterpret_cast<char*>(data), length);
        }

        enum class HandleAlpha
        {
            /// If alpha is present it is kept. RGB stays RGB, RGBA stays RGBA.
            KeepAlpha,
            // If alpha is present it is removed. RGBA becomes RGB.
            RemoveAlpha,
            // If no alpha is present it will be added. RGB becomes RGBA.
            AddAlpha
        };

        /// @param order_BGR     Stores pixels as BGR instead of RGB when true.
        /// @param include_alpha Should output image have the alpha channel. Adds
        ///                      or removes alpha as needed.
        bool OpenPNG(const std::string_view &file_name,
                     const bool order_BGR,
                     const HandleAlpha alpha_handling,
                     std::vector<std::uint8_t>& out,
                     std::uint32_t& width,
                     std::uint32_t& height) noexcept
        {
            out.clear();
            assert(out.empty());

            // to hold first 8 bytes reading from png file to check if it's png file
            unsigned char header[8] = {0};

            std::ifstream ifile(file_name.data(), std::ios::binary);

            if (false == ifile.is_open())
            {
                return false;
            }

            // read the first 8 bytes (more bytes will make it more accurate)
            // but in case we want to keep our file pointer around after this
            // libpng suggests us to only read 8 bytes
            const int cmp_number = 8;
            ifile.read(reinterpret_cast<char*>(&header), 8);
            if (ifile.gcount() != cmp_number)
            {
                return false;
            }

            // check whether magic number matches, and thus it's png file
            if (png_sig_cmp(header, 0, cmp_number) != 0)
            {
                return false;
            }

            // create png structure
            png_structp png_ptr = nullptr;
            png_infop  info_ptr = nullptr;

            auto clean_up = [&]()
            {
                if(png_ptr)
                {
                    if(info_ptr)
                    {
                        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
                    }
                    else
                    {
                        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
                    }
                }
            };

            // Sentinel which handles destroying of png assets
            Sentinel sentinel(clean_up);

            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
            if (png_ptr == nullptr)
            {
                return false;
            }


            // Get PNG info.
            info_ptr = png_create_info_struct(png_ptr);
            if (info_ptr == nullptr)
            {
                return false;
            }

            // We already read first 8 bytes of header.
            png_set_sig_bytes(png_ptr, cmp_number);

            // Set custom data reading callback.
            png_set_read_fn(png_ptr,(png_voidp)&ifile, read_png_data_from_ifstream);

            // Read file info up to image data
            png_read_info(png_ptr, info_ptr);

            // Get PNG details
            width  = png_get_image_width(png_ptr, info_ptr);
            height = png_get_image_height(png_ptr, info_ptr);
            auto bit_depth  = png_get_bit_depth(png_ptr, info_ptr);
            auto color_type = png_get_color_type(png_ptr, info_ptr);
            auto channels   = png_get_channels(png_ptr, info_ptr);
            auto rowbytes   = png_get_rowbytes(png_ptr, info_ptr);

            if (bit_depth != 8)
            {
                err("PNG bit depth was not 8");
                return false;
            }

            bool failed = false;

            switch (color_type)
            {
            case PNG_COLOR_TYPE_GRAY:
                err("Gray PNG not supported.");
                failed = true;
                break;

            case PNG_COLOR_TYPE_GRAY_ALPHA:
                err("Gray alpha PNG not supported.");
                failed = true;
                break;

            case PNG_COLOR_TYPE_PALETTE:
                err("Palette alpha PNG not supported.");
                failed = true;
                break;

            case PNG_COLOR_TYPE_RGB:
                switch(alpha_handling)
                {
                    case HandleAlpha::AddAlpha:
                        out.reserve(4u * width * height);
                        break;

                    case HandleAlpha::KeepAlpha:
                    case HandleAlpha::RemoveAlpha:
                        out.reserve(3u * width * height);
                        break;
                }
                break;

            case PNG_COLOR_TYPE_RGB_ALPHA:
                switch(alpha_handling)
                {
                    case HandleAlpha::AddAlpha:
                    case HandleAlpha::KeepAlpha:
                        out.reserve(4u * width * height);
                        break;

                    case HandleAlpha::RemoveAlpha:
                        out.reserve(3u * width * height);
                        break;
                }
                break;

            default:
                err("PNG format not supported.");
                failed = true;
                break;
            }

            switch (channels)
            {
            case 1:
                err("1 channel PNG not supported.");
                failed = true;
                break;

            case 2:
                err("2 channel PNG not supported.");
                failed = true;
                break;

            case 3:
            case 4:
                break;
            }

            if (failed)
            {
                err("Unsupported PNG.");
                return false;
            }

            // Extract the PNG image bytes (in rows).
            std::vector<png_bytep> row_ptrs(height, nullptr);
            std::vector<std::vector<png_byte>> rows(height);

            // Fill vector of pointers.
            for (auto y = 0u; y < height; ++y)
            {
                rows.at(y).resize(rowbytes);
                row_ptrs.at(y) = rows.at(y).data();
            }

            // read image data
            png_read_image(png_ptr, row_ptrs.data());

            // clear png resource
            sentinel.clean_up();

            // We now have the image data. Transform it into the user
            // image buffer.
            for (auto h = 0u; h < height; ++h)
            {
                auto &row = row_ptrs[h];

                for (auto r = 0u; r < rowbytes; r += channels)
                {
                    if (channels == 3u)
                    {
                        auto red   = row[r + 0u];
                        auto green = row[r + 1u];
                        auto blue  = row[r + 2u];
                        if (order_BGR)
                        {
                            out.push_back(blue);
                            out.push_back(green);
                            out.push_back(red);
                        }
                        else
                        {
                            out.push_back(red);
                            out.push_back(green);
                            out.push_back(blue);
                        }

                        // This is an RGB PNG: add alpha if requested.
                        if (alpha_handling == HandleAlpha::AddAlpha)
                        {
                            out.push_back(DEFAULT_ALPHA);
                        }
                    }
                    else
                    {
                        auto red   = row[r + 0u];
                        auto green = row[r + 1u];
                        auto blue  = row[r + 2u];
                        auto alpha = row[r + 3u];
                        if (order_BGR)
                        {
                            out.push_back(blue);
                            out.push_back(green);
                            out.push_back(red);
                        }
                        else
                        {
                            out.push_back(red);
                            out.push_back(green);
                            out.push_back(blue);
                        }

                        // This is an RGBA PNG: remove alpha if requested.
                        if(alpha_handling != HandleAlpha::RemoveAlpha)
                        {
                            out.push_back(alpha);
                        }
                    }
                }
            }

            return not failed;
        }

        //-------------------------------------------------------------------------
        //!   Reads in the data from file `file_name`. If the file is not a valid
        //!   bitmap or cannot be found returns false. If an error
        //!   occurs the stream position is undefined in relation to the beginning.
        //!   No effort is made to ensure that it is reset. If the
        //!   function succeeds the position is at the after the last byte read (normal).
        //-------------------------------------------------------------------------
        bool OpenBMP(const std::string_view &file_name,
                     const bool order_BGR,
                     const HandleAlpha alpha_handling,
                     std::vector<GEByte> &out,
                     std::uint32_t &width,
                     std::uint32_t &height) noexcept
        {
            std::ifstream ifile(file_name.data(), std::ios::binary);

            if (false == ifile.is_open())
            {
                return false;
            }

            bool result = false;

            GE_MSBITMAPFILEHEADERMAGICNUMBER magic; // 2 byte magic number.
            GE_MSBITMAPFILEHEADER2 bmp_header;          // Bitmap file header of Image.
            GE_MSBITMAPINFOHEADER bmp_info;         // Bitmap info header of Image.

            try
            {
                GEByte *pbuf = 0; // Buffer for bitmap data.
                int w = 0;
                int h = 0;
                bool bottomup = true; // Top down or bottom up?

                //
                // Read in bitmap info & header
                //
                ifile.read(reinterpret_cast<char *>(&magic), sizeof(magic));
                ifile.read(reinterpret_cast<char *>(&bmp_header), sizeof(bmp_header));

                if (false == ifile.good())
                {
                    err("bitmap header read failed.");
                    return false;
                }

                ifile.read(reinterpret_cast<char *>(&bmp_info), sizeof(bmp_info));

                if (false == ifile.good())
                {
                    err("bitmap info read failed.");
                    return false;
                }

                //
                // Check bitmap is good for us before proceeding
                // Note : 0x4D42 == BM
                //
                if (GE_BM_IDENTIFIER != magic.bfType)
                {
                    err("bitmap magic number (0x4d42) not found.");
                    // Not a BMP file.
                    return false;
                }

                assert(bmp_info.biSize == GE_VERSION3BMPSIZE || bmp_info.biSize == GE_VERSION4BMPSIZE || bmp_info.biSize == GE_ADOBEALPHASIZE);

                assert(24 == bmp_info.biBitCount || 32 == bmp_info.biBitCount);
                assert(GE_BI_BITFIELDS == bmp_info.biCompression || GE_BI_RGB == bmp_info.biCompression);

                if (24 != bmp_info.biBitCount && 32 != bmp_info.biBitCount)
                {
                    err("bitmap not 24 or 32 bpp");
                    return false;
                }

                if (false == (GE_BI_RGB == bmp_info.biCompression || GE_BI_BITFIELDS == bmp_info.biCompression))
                {
                    err("Unable to read compressed bitmaps");
                    return false;
                }

                //
                // Create buffer and read in bits.
                //
                h = static_cast<std::int32_t>(bmp_info.biHeight);
                w = abs(bmp_info.biWidth);
                bottomup = bmp_info.biWidth > 0 ? true : false;

                if (w <= 0 || h <= 0)
                {
                    err("Width or height not positive");
                    return false;
                }

                std::vector<GEByte> temp;
                temp.reserve(w * h * 4u);

                switch (bmp_info.biSize)
                {
                case GE_VERSION3BMPSIZE:
                {
                    if (32 == bmp_info.biBitCount)
                    {
                        temp.resize(w * h * 4);
                    }
                    else
                    {
                        temp.resize(w * h * 3);
                    }

                    pbuf = &temp[0];

                    if (32 == bmp_info.biBitCount)
                    {
                        if (order_BGR)
                        {
                            ReadType3toBGRA(ifile, pbuf, w, h, bottomup);
                        }
                        else
                        {
                            ReadType3toRGBA(ifile, pbuf, w, h, bottomup);
                        }
                    }
                    else
                    {
                        if (order_BGR)
                        {
                            ReadType3toBGR(ifile, pbuf, w, h, bottomup);
                        }
                        else
                        {
                            ReadType3toRGB(ifile, pbuf, w, h, bottomup);
                        }
                    }
                }
                break;

                case GE_VERSION4BMPSIZE:
                {
                    GE_MSINFOHEADERV4 bmp4; // Version 4 bitmap extension.

                    ifile.read(reinterpret_cast<char *>(&bmp4), sizeof(bmp4));

                    if (false == ifile.good())
                    {
                        err("Error reading bmp GE_VERSION4BMPSIZE");
                        return false;
                    }

                    temp.resize(w * h * 4);

                    pbuf = &temp[0];

                    // Type 4 is always 32 bit (RGBA).
                    ReadType4(ifile, pbuf, w, h, bmp4.rgbaMasks, order_BGR);
                }
                break;

                case GE_ADOBEALPHASIZE:
                {
                    GE_RGBAMASKS adobe; // ADOBE version.

                    ifile.read(reinterpret_cast<char *>(&adobe), sizeof(adobe));

                    if (false == ifile.good())
                    {
                        err("Error reading bmp GE_ADOBEALPHASIZE");
                        return false;
                    }

                    temp.resize(w * h * 4);

                    pbuf = &temp[0];

                    ReadType4(ifile, pbuf, w, h, adobe, order_BGR);
                }
                break;

                default:
                    return false;
                    break;
                }

                switch(alpha_handling)
                {
                    case HandleAlpha::AddAlpha:
                        if(temp.size() == (3u * w * h))
                        {
                            add_alpha(temp, w, h);
                        }
                    break;

                    case HandleAlpha::RemoveAlpha:
                        if(temp.size() == (4u * w * h))
                        {
                            remove_alpha(temp, w, h);
                        }
                    break;

                    case HandleAlpha::KeepAlpha:
                    break;
                }

                out.swap(temp);
                width  = static_cast<size_t>(w);
                height = static_cast<size_t>(h);
                result = true;
            }
            catch (...)
            {
                err("Unknown exception reading bmp");
                result = false;
            }

            return result;
        }



        void load_image(const std::string_view& filename,
                        std::vector<std::uint8_t>& img,
                        std::uint32_t& width,
                        std::uint32_t& height,
                        const HandleAlpha alpha_handling)
        {
            if (filename.empty())
            {
                throw std::runtime_error("No Image file name specified.");
            }

            const bool loadImagesAsBGRnotRGB = true;

            // Get canonical extension.
            auto extension = std::filesystem::path(filename).extension().string();
            for (auto &c : extension)
            {
                c = static_cast<char>(std::tolower(c));
            }

            bool loaded = false;

            // Use extension to prefer correct loaded.
            if (".png" == extension)
            {
                // Try PNG first, then BMP.
                if (OpenPNG(filename, loadImagesAsBGRnotRGB, alpha_handling, img, width, height))
                {
                    loaded = true;
                }
                else
                {
                    loaded = OpenBMP(filename, loadImagesAsBGRnotRGB, alpha_handling, img, width, height);
                }
            }
            else
            {
                // Try BMP first, then PNG.
                if (OpenBMP(filename, loadImagesAsBGRnotRGB, alpha_handling, img, width, height))
                {
                    loaded = true;
                }
                else
                {
                    loaded = OpenPNG(filename, loadImagesAsBGRnotRGB, alpha_handling, img, width, height);
                }
            }

            if (not loaded)
            {
                throw std::runtime_error(std::string("Failed to load image '") + filename.data() + "'");
            }
        }

        Image::Format calculate_format(const std::size_t image_bytes_size,
                                       const unsigned int img_width,
                                       const unsigned int img_height)
        {
            if (image_bytes_size == 0)
            {
                // Default format.
                return Image::Format::BGRA;
            }
            else
            {
                if (image_bytes_size == (3u * img_width * img_height))
                {
                    return Image::Format::BGR;
                }
                else if (image_bytes_size == (4u * img_width * img_height))
                {
                    return Image::Format::BGRA;
                }
            }

            throw std::runtime_error("Unknown Image format.");
        }

        //-------------------------------------------------------------------------
        //! Saves a 24 bit type 3.x bitmap to file. These types have padding and may be top-down or
        //! bottom up.
        //-------------------------------------------------------------------------
        bool SaveType3_BGR(
            std::ostream &ofile,
            std::size_t width,
            std::size_t height,
            const GEByte *data)
        {
            //
            // Find padded width of bitmap in bytes. This is the min no of WORDs
            // that can hold the rgb bytes.
            //
            unsigned int pad = 4 - (width * 3) % 4;
            pad %= 4;

            if (std::numeric_limits<std::uint32_t>::max() < (height * ((width * 3) + pad)))
            {
                throw std::runtime_error("Size of bitmap data W x H is out of range for Bitmap file header.");
            }

            const std::uint32_t pixelsSize = static_cast<std::uint32_t>(height * ((width * 3) + pad));

            GE_MSBITMAPFILEHEADERMAGICNUMBER bmpmagic_;
            GE_MSBITMAPFILEHEADER2 bmpfh_;  //!< Bitmap file header of current Image.
            GE_MSBITMAPINFOHEADER bmpinfo_; //!< Bitmap info header of current Image.

            static_assert(14 == (sizeof(GE_MSBITMAPFILEHEADERMAGICNUMBER) + sizeof(GE_MSBITMAPFILEHEADER2)), "Structures do not add up to required 14-byte Bitmap File Header.");

            bmpfh_.bfSize = sizeof(bmpmagic_) + sizeof(bmpfh_) + sizeof(bmpinfo_) + (pixelsSize); // File size in bytes.
            bmpfh_.bfOffBits = sizeof(bmpmagic_) + sizeof(bmpfh_) + sizeof(bmpinfo_);             // Offset from file start to Image data (in bytes).

            // Fill in Bitmap header. (version 3.x)
            bmpinfo_.biWidth = static_cast<int>(width);   // Width of Image in pixels.
            bmpinfo_.biHeight = static_cast<int>(height); // Height of the Image in pixels.
            bmpinfo_.biSizeImage = pixelsSize;            // Pixels bytes count.

            ofile.write(reinterpret_cast<const char *>(&bmpmagic_), sizeof(bmpmagic_));
            ofile.write(reinterpret_cast<const char *>(&bmpfh_), sizeof(bmpfh_));
            ofile.write(reinterpret_cast<const char *>(&bmpinfo_), sizeof(bmpinfo_));

            //
            // Dump bits to file. The bits must be inverted, and the lines padded to be
            // 32 bit WORD aligned. We could store them top down, as a human may prefer however
            // bmp interpreters which do not check for the negative value of height which indicates
            // a top down

            const GEByte *psrc = data; // Working pointer.

            assert((((3 * width) + pad) % 4) == 0);

            if (height > 0)
            {
                for (std::size_t y = height; y > 0; --y)
                {
                    psrc = data + (3 * width * (y - 1));

                    for (std::size_t x = 0, k = 0; x < width; ++x, k += 3)
                    {
                        const GEByte rbg[3] =
                            {
                                *(psrc + k + 0),
                                *(psrc + k + 1),
                                *(psrc + k + 2)};

                        ofile.write(reinterpret_cast<const char *>(rbg), 3);
                    }

                    for (unsigned int ii = 0; ii < pad; ++ii)
                    {
                        ofile.put('0'); // Insert padding bytes.
                    }
                }
            }

            return ofile.good();
        }

        //-------------------------------------------------------------------------
        //! Saves a 32 bit type 3.x bitmap to file.
        //-------------------------------------------------------------------------
        bool SaveType3_BGRA(
            std::ostream &ofile,
            std::size_t width,
            std::size_t height,
            const GEByte *data)
        {
            if (std::numeric_limits<std::uint32_t>::max() < (height * ((width * 4))))
            {
                throw std::runtime_error("Size of bitmap data W x H is out of range for Bitmap file header.");
            }

            const std::uint32_t pixelsSize = static_cast<std::uint32_t>(height * ((width * 4)));

            GE_MSBITMAPFILEHEADERMAGICNUMBER bmpmagic_;
            GE_MSBITMAPFILEHEADER2 bmpfh_;  //!< Bitmap file header of current Image.
            GE_MSBITMAPINFOHEADER bmpinfo_; //!< Bitmap info header of current Image.

            // Fill in file header data.
            bmpmagic_.bfType = 0x4D42; // "BM" identifier.

            static_assert(14 == (sizeof(GE_MSBITMAPFILEHEADERMAGICNUMBER) + sizeof(GE_MSBITMAPFILEHEADER2)), "Structures do not add up to required 14-byte Bitmap File Header.");
            static_assert(GE_VERSION3BMPSIZE == sizeof(bmpinfo_), "GE_MSBITMAPFILEHEADER2 is not standard byte size.");

            bmpfh_.bfSize = sizeof(bmpmagic_) + sizeof(bmpfh_) + sizeof(bmpinfo_) + (pixelsSize); // File size in bytes.
            bmpfh_.bfReserved1and2 = 0;                                                           // Always zero.
            bmpfh_.bfOffBits = sizeof(bmpmagic_) + sizeof(bmpfh_) + sizeof(bmpinfo_);             // Offset from file start to Image data (in bytes).

            // Fill in Bitmap header. (version 3.x)
            bmpinfo_.biSize = GE_VERSION3BMPSIZE;         // Size of the header structure.
            bmpinfo_.biWidth = static_cast<int>(width);   // Width of Image in pixels.
            bmpinfo_.biHeight = static_cast<int>(height); // Height of the Image in pixels.
            bmpinfo_.biPlanes = 1;                        // Number of colour planes, only 1 is possible.
            bmpinfo_.biBitCount = 32;                     // Default is 24 bit.
            bmpinfo_.biCompression = GE_BI_RGB;           // The only one we currently support.
            bmpinfo_.biSizeImage = pixelsSize;            // Zero when using uncompressed format.
            bmpinfo_.biXPelsPerMeter = 1;                 // Vertical resolution of the Image in pixels per metre.
            bmpinfo_.biYPelsPerMeter = 1;                 // Horizontal resolution of the Image in pixels per metre.
            bmpinfo_.biClrUsed = 0;                       // Number of colours in Image.
            bmpinfo_.biClrImportant = 0;                  // All colours are to be considered equally important (flag == 0).

            ofile.write(reinterpret_cast<const char *>(&bmpmagic_), sizeof(bmpmagic_));
            ofile.write(reinterpret_cast<const char *>(&bmpfh_), sizeof(bmpfh_));
            ofile.write(reinterpret_cast<const char *>(&bmpinfo_), sizeof(bmpinfo_));

            //
            // Dump bits to file. The bits must be inverted, and the lines padded to be
            // 32 bit WORD aligned. We could store them top down, as a human may prefer however
            // bmp interpreters which do not check for the negative value of height which indicates
            // a top down

            if (height > 0u)
            {
                for (std::size_t y = height; y > 0u; --y)
                {
                    const GEByte *psrc = data + (width * (y - 1u) * 4u);

                    ofile.write(reinterpret_cast<const char *>(psrc), 4u * width);
                }
            }

            return ofile.good();
        }

        //-------------------------------------------------------------------------
        //! Calls the correct save method.
        //-------------------------------------------------------------------------
        bool SaveType3(
            std::ostream &ofile,
            const std::size_t width,
            const std::size_t height,
            const GEByte *data,
            const Image::Format currentFormat)
        {
            if (currentFormat == Image::Format::BGR)
            {
                return SaveType3_BGR(ofile, width, height, data);
            }
            else
            {
                return SaveType3_BGRA(ofile, width, height, data);
            }
        }

#if 0
        //-------------------------------------------------------------------------
        //! Saves a 32 bit type 4.x bitmap to file.
        //-------------------------------------------------------------------------
        bool SaveType4(
            std::ostream& ofile,
            std::size_t width,
            std::size_t height,
            const GEByte* data,
            Image::Format currentFormat )
        {
            if( std::numeric_limits<std::uint32_t>::max() < ( height * width * 4 ) )
            {
                throw std::runtime_error( "Size of bitmap data W x H is out of range for Bitmap file header." );
            }

            const std::uint32_t pixelsSize = static_cast<std::uint32_t>( height * width * 4 );

            GE_MSBITMAPFILEHEADERMAGICNUMBER bmpmagic_;
            GE_MSBITMAPFILEHEADER2 bmpfh_;  //!< Bitmap file header of current Image.
            GE_MSBITMAPINFOHEADER bmpinfo_; //!< Bitmap info header of current Image.
            GE_MSINFOHEADERV4 bmp4_;        //!< Extended bitmap info for versions 4.

            // Fill in file header data.
            bmpmagic_.bfType = 0x4D42; // "BM" identifier.

            static_assert( 14 == ( sizeof( GE_MSBITMAPFILEHEADERMAGICNUMBER ) + sizeof( GE_MSBITMAPFILEHEADER2 ) ), "Structures do not add up to required 14-byte Bitmap File Header." );

            bmpfh_.bfSize = sizeof( bmpmagic_ ) + sizeof( bmpfh_ ) + sizeof( bmpinfo_ ) + sizeof( bmp4_ ) + ( pixelsSize ); // File size in bytes.
            bmpfh_.bfReserved1and2 = 0;                                                                           // Always zero.
            bmpfh_.bfOffBits = sizeof( bmpmagic_ ) + sizeof( bmpfh_ ) + sizeof( bmpinfo_ ) + sizeof( bmp4_ );     // Offset from file start to Image data (in bytes).

            // Fill in Bitmap header. (version 3.x)
            bmpinfo_.biSize = GE_VERSION4BMPSIZE;         // Size of the header structure.
            bmpinfo_.biWidth = static_cast<int>( width ); // Width of Image in pixels.
            bmpinfo_.biHeight = static_cast<int>( height ); // Height of the Image in pixels.
            bmpinfo_.biPlanes = 1;                        // Number of colour planes, only 1 is possible.
            bmpinfo_.biBitCount = 32;                     // Default is 24 bit.
            bmpinfo_.biCompression = GE_BI_RGB;           // The only one we currently support.
            bmpinfo_.biSizeImage = 0;                     // Zero when using uncompressed format.
            bmpinfo_.biXPelsPerMeter = 1;                 // Vertical resolution of the Image in pixels per metre.
            bmpinfo_.biYPelsPerMeter = 1;                 // Horizontal resolution of the Image in pixels per metre.
            bmpinfo_.biClrUsed = 1 << 24;                 // Number of colours in Image.
            bmpinfo_.biClrImportant = 0;                  // All colours are to be considered equally important (flag == 0).

            // Fill in  a correct version 4 extension structure.
            bmp4_.rgbaMasks.biRedMask = GE_RGBA888MASKR;
            bmp4_.rgbaMasks.biGreenMask = GE_RGBA888MASKG;
            bmp4_.rgbaMasks.biBlueMask = GE_RGBA888MASKB;
            bmp4_.rgbaMasks.biAlphaMask = GE_RGBA888MASKA;
            bmp4_.biCSType = GE_DEVICEDEPENDANTRGB;
            bmp4_.biRedX = 0;       // Used only when colour space is calibrated RGB
            bmp4_.biRedY = 0;       // Used only when colour space is calibrated RGB
            bmp4_.biRedZ = 0;       // Used only when colour space is calibrated RGB
            bmp4_.biGreenX = 0;     // Used only when colour space is calibrated RGB
            bmp4_.biGreenY = 0;     // Used only when colour space is calibrated RGB.
            bmp4_.biBlueX = 0;      // Used only when colour space is calibrated RGB
            bmp4_.biBlueY = 0;      // Used only when colour space is calibrated RGB
            bmp4_.biBlueZ = 0;      // Used only when colour space is calibrated RGB
            bmp4_.biGammaRed = 1;   // Gamma red coordinate scale value.
            bmp4_.biGammaGreen = 1; // Gamma blue coordinate scale value.
            bmp4_.biGammaBlue = 1;  // Gamma green coordinate scale value.

            ofile.write( reinterpret_cast<const char*>( &bmpmagic_ ), sizeof( bmpmagic_ ) );
            ofile.write( reinterpret_cast<const char*>( &bmpfh_ ), sizeof( bmpfh_ ) );
            ofile.write( reinterpret_cast<const char*>( &bmpinfo_ ), sizeof( bmpinfo_ ) );
            ofile.write( reinterpret_cast<const char*>( &bmp4_ ), sizeof( bmp4_ ) );

            const GEByte* pwork = data;
            const int h = static_cast<int>( height );

            std::uint32_t colour;
            std::uint32_t c[4] =
            {
                bmp4_.rgbaMasks.biRedMask,
                bmp4_.rgbaMasks.biGreenMask,
                bmp4_.rgbaMasks.biBlueMask,
                bmp4_.rgbaMasks.biAlphaMask
            };

            std::uint32_t shift[4] = { 0 };

            // Calculate shift numbers for each mask:
            if( c[0] )
            {
                while( false == ( c[0] & 0x1 ) )
                {
                    c[0] >>= 1;
                    ++shift[0];
                } // Red
            }

            if( c[1] )
            {
                while( false == ( c[1] & 0x1 ) )
                {
                    c[1] >>= 1;
                    ++shift[1];
                } // Green
            }

            if( c[2] )
            {
                while( false == ( c[2] & 0x1 ) )
                {
                    c[2] >>= 1;
                    ++shift[2];
                } // Blue
            }

            if( c[3] )
            {
                while( false == ( c[3] & 0x1 ) )
                {
                    c[3] >>= 1;
                    ++shift[3];
                } // Alpha
            }

            const std::size_t w = width;

            if( currentFormat == Image::format_bgra )
            {
                const std::size_t ww = width * 4;

                for( int i = h - 1; i >= 0; --i )
                {
                    pwork = data + ( ww * i );

                    for( unsigned int j = 0, k = 0; j < w; ++j, k += 4 )
                    {
                        colour = ( ( *( pwork + k + 0 ) ) << shift[2] ) |
                                 ( ( *( pwork + k + 1 ) ) << shift[1] ) |
                                 ( ( *( pwork + k + 2 ) ) << shift[0] ) |
                                 ( ( *( pwork + k + 3 ) ) << shift[3] );
                        ofile.write( reinterpret_cast<const char*>( &colour ), 4 );
                    }
                }
            }
            else
            {
                if( currentFormat == Image::format_bgr )
                {
                    const std::size_t ww = width * 3;

                    for( int i = h - 1; i >= 0; --i )
                    {
                        pwork = data + ( ww * i );

                        for( unsigned int j = 0, k = 0; j < w; ++j, k += 3 )
                        {
                            // Write BGRA
                            colour = ( ( *( pwork + k + 0 ) ) << shift[2] ) |
                                     ( ( *( pwork + k + 1 ) ) << shift[1] ) |
                                     ( ( *( pwork + k + 2 ) ) << shift[0] ) |
                                     ( ( DEFAULT_ALPHA ) << shift[3] );
                            ofile.write( reinterpret_cast<const char*>( &colour ), 4 );
                        }
                    }
                }
                else
                {
                    throw std::runtime_error( "Unknown format" );
                }
            }

            ofile.flush();

            return ofile.good();
        }
#endif

        bool does_layer_exist(const Image &src, const std::size_t layer)
        {
            return (layer <= 2u) || (layer <= 3u && src.format() == Image::Format::BGRA);
        }

        void validate_layer_op(const Image &src, std::size_t srcLayer, const Image &dest, std::size_t destLayer)
        {
            if (false == does_layer_exist(src, srcLayer))
            {
                throw std::runtime_error("Source layer out of bounds.");
            }

            if (false == does_layer_exist(dest, destLayer))
            {
                throw std::runtime_error("Destination layer out of bounds.");
            }

            if (src.height() != dest.height() || src.width() != dest.width())
            {
                throw std::runtime_error("Source and destination images are not the same sizes.");
            }
        }

    } // Unnamed namespace.

    //-------------------------------------------------------------------------
    /// This is based on code I wrote in 2001, 10 years ago. So far it still
    /// surpasses all "professional" code I have ever worked with.
    //-------------------------------------------------------------------------

    Image::Image(const std::string_view &filename)
        : width_(0u)
        , height_(0u)
    {
        load_image( filename, img_, width_, height_, HandleAlpha::KeepAlpha );
    }

    Image::Image(const std::string_view &filename,
                 const Image::Format format)
        : width_(0u)
        , height_(0u)
    {
        const auto alpha_handling = (format == Image::Format::BGR)
                                  ? HandleAlpha::RemoveAlpha
                                  : HandleAlpha::AddAlpha;

        load_image( filename, img_, width_, height_, alpha_handling );
    }

    Image::Image(const unsigned int w,
                 const unsigned int h,
                 const Image::Format f)
        : img_((f == Image::Format::BGR ? (w * h * 3u) : (w * h * 4u)), DEFAULT_ALPHA)
        , width_(w)
        , height_(h)
    {
    }

    Image::Image(const unsigned int w,
                 const unsigned int h,
                 const Bgra c)
        : img_((w * h * 4u), c.alpha)
        , width_(w)
        , height_(h)
    {
        clear_to_bgra(c);
    }

    Image::Image(const unsigned int w,
                 const unsigned int h,
                 const Bgr c)
        : img_((w * h * 3u))
        , width_(w)
        , height_(h)
    {
        clear_to_bgra(c.blue, c.green, c.red, DEFAULT_ALPHA);
    }

    Image::Image(const unsigned int width,
                 const std::vector<Bgra>& pixels)
        : img_()
        , width_(width)
        , height_()
    {
        if(pixels.empty())
        {
            if(width > 0u)
            {
                throw std::runtime_error("Invalid Image args");
            }
        }
        else
        {
            if(0u != (pixels.size() % width))
            {
                throw std::runtime_error("Invalid Image args");
            }

            height_ = static_cast<std::uint32_t>(pixels.size() / width);

            img_.reserve(4u * pixels.size());

            for(auto i=0u; i < pixels.size(); ++i)
            {
                img_.push_back(pixels[i].blue);
                img_.push_back(pixels[i].green);
                img_.push_back(pixels[i].red);
                img_.push_back(pixels[i].alpha);
            }
        }
    }

    bool Image::open(const std::string_view &file) noexcept
    {
        try
        {
            Image other(file);
            std::swap(*this, other);
            return true;
        }
        catch (...)
        {
        }

        return false;
    }

    bool Image::open(const std::string_view &file,
                     const Image::Format format) noexcept
    {
        try
        {
            Image other(file, format);
            std::swap(*this, other);
            return true;
        }
        catch (...)
        {
        }

        return false;
    }

    //! Always save as Type 4 bitmap (contains Alpha channel). If the Image is 3 channel RGB Image
    //! then the alpha channel is filled with value Zero.
    bool Image::save(const std::string_view &filename) const
    {
        std::ofstream ofile(filename.data(), std::ios::binary);
        if (ofile.is_open())
        {
            // return SaveType4( os, width_, height_, data(), format() );
            return SaveType3(ofile, width_, height_, data(), format());
        }
        else
        {
            return false;
        }
    }

    //! Returns the format of the Image data.
    Image::Format Image::format() const
    {
        return calculate_format(img_.size(), width_, height_);
    }

    const std::uint8_t *Image::pixel_at(const std::size_t x, const std::size_t y) const
    {
        return pixel(x, y, *this, this->format());
    }

    const std::uint8_t *Image::pixel(const std::size_t x,
                                     const std::size_t y,
                                     const Image &i,
                                     const Image::Format f)
    {
        assert(x < i.width());
        assert(y < i.height());
        assert(f == i.format());
        assert(f == Image::Format::BGR || f == Image::Format::BGRA);

        return i.data() + ((x + y * i.width()) * (f == Image::Format::BGR ? 3u : 4u));
    }

    std::uint8_t *Image::pixel(const std::size_t x,
                               const std::size_t y,
                               Image &i,
                               const Image::Format f)
    {
        assert(x < i.width());
        assert(y < i.height());
        assert(f == i.format());
        assert(f == Image::Format::BGR || f == Image::Format::BGRA);

        return i.data() + ((x + y * i.width()) * (f == Image::Format::BGR ? 3u : 4u));
    }

    //! Returns 0 if both Image layers are the same. Returns -1 if the first differing
    //! layer element in src is less than its partner element in dest. Returns +1 if
    //! it is greater.
    int Image::compare_layer(const Image &src,
                             const std::size_t srcLayer,
                             const Image &dest,
                             const std::size_t destLayer)
    {
        validate_layer_op(src, srcLayer, dest, destLayer);

        if (false == src.empty())
        {
            const std::uint8_t *psrc = srcLayer  + &src.img_[0];
            const std::uint8_t *pdst = destLayer + &dest.img_[0];

            const std::size_t srcstep = src.format() == Image::Format::BGRA ? 4u : 3u;
            const std::size_t dsttep = dest.format() == Image::Format::BGRA ? 4u : 3u;

            std::size_t n = src.width_ * src.height_;

            do
            {
                if (*psrc != *pdst)
                {
                    return *psrc < *pdst ? -1 : +1;
                }

                psrc += srcstep;
                pdst += dsttep;
            } while (--n);
        }

        return 0;
    }

    void Image::copy_layer(const Image &src,
                           const std::size_t srcLayer,
                           Image &dest,
                           const std::size_t destLayer)
    {
        validate_layer_op(src, srcLayer, dest, destLayer);

        if (false == src.empty())
        {
            if (&src.img_[0] != &dest.img_[0])
            {
                // They are different memory spaces:
                const std::uint8_t* psrc = srcLayer  + &src.img_[0];
                std::uint8_t* pdst       = destLayer + &dest.img_[0];

                const std::size_t srcstep = src.format()  == Image::Format::BGRA ? 4 : 3;
                const std::size_t dsttep  = dest.format() == Image::Format::BGRA ? 4 : 3;

                std::size_t n = src.width_ * src.height_;

                do
                {
                    *pdst = *psrc;
                    psrc += srcstep;
                    pdst += dsttep;
                } while (--n);
            }
            else
            {
                // They are the same Image: avoid silly copy:
                if (srcLayer != destLayer)
                {
                    const std::uint8_t *psrc = srcLayer + &src.img_[0];
                    std::uint8_t *pdst = destLayer + &dest.img_[0];

                    const std::size_t srcstep = src.format() == Image::Format::BGRA ? 4 : 3;
                    const std::size_t dsttep = dest.format() == Image::Format::BGRA ? 4 : 3;

                    std::size_t n = src.width_ * src.height_;

                    do
                    {
                        *pdst = *psrc;
                        psrc += srcstep;
                        pdst += dsttep;
                    } while (--n);
                }
            }
        }
    }

    void Image::clear_layer_to(Image &src,
                               const std::size_t layer,
                               const std::uint8_t value)
    {
        if (false == does_layer_exist(src, layer))
        {
            throw std::runtime_error("Layer out of bounds.");
        }

        if (false == src.empty())
        {
            // They are different memory spaces:
            std::uint8_t *psrc = layer + &src.img_[0];

            const std::size_t srcstep = src.format() == Image::Format::BGRA ? 4u : 3u;

            std::size_t n = src.width_ * src.height_;

            do
            {
                *psrc = value;
                psrc += srcstep;
            } while (--n);
        }
    }

    void Image::flip_vertically()
    {
        *this = flipped();
    }

    Image Image::flipped() const
    {
        Image flipped(width_, height_, format());

        const unsigned int rowwidth = width_ * (format() == Format::BGRA ? 4u : 3u);
        const std::uint8_t *srcpixels = img_.data();
        std::uint8_t *dstpixels = flipped.img_.data();

        for (unsigned int dstrow = 0; dstrow < height_; ++dstrow)
        {
            const unsigned int srcrow = height_ - (dstrow + 1);

            for (unsigned int i = 0u; i < rowwidth; ++i)
            {
                // Copy from src bottom into dest top, moving downwards.
                *(dstpixels + (rowwidth * dstrow) + i) = *(srcpixels + (rowwidth * srcrow) + i);
            }
        }

        return flipped;
    }

    //! Clears entire Image to BGRA colour specified. A is not used if
    //! format is BGR.
    void Image::clear_to_bgra(std::uint8_t b,
                              std::uint8_t g,
                              std::uint8_t r,
                              std::uint8_t a)
    {
        if (false == empty())
        {
            // They are different memory spaces:
            std::uint8_t *psrc = &img_[0];

            const std::size_t n = width_ * height_;

            if (format() == Image::Format::BGRA)
            {
                for (std::size_t i = 0u; i < n; ++i)
                {
                    psrc[i * 4 + 0] = b;
                    psrc[i * 4 + 1] = g;
                    psrc[i * 4 + 2] = r;
                    psrc[i * 4 + 3] = a;
                }
            }
            else
            {
                for (std::size_t i = 0u; i < n; ++i)
                {
                    psrc[i * 3 + 0] = b;
                    psrc[i * 3 + 1] = g;
                    psrc[i * 3 + 2] = r;
                }
            }
        }
    }

    void Image::set_pixel(const std::size_t x, const std::size_t y, const Bgra &c)
    {
        std::uint8_t *p = pixel(x, y, *this, format());
        p[0] = c.blue;
        p[1] = c.green;
        p[2] = c.red;
        if (format() == Format::BGRA)
        {
            p[3] = c.alpha;
        }
    }

    const std::uint8_t *Image::data() const
    {
        return img_.data();
    }

    std::uint8_t *Image::data()
    {
        return img_.data();
    }

    std::size_t Image::bytes_size() const
    {
        return img_.size();
    }

    std::size_t Image::layers_count() const
    {
        return format() == Image::Format::BGRA ? 4u : 3u;
    }

    std::size_t Image::pixels_count() const
    {
        return width_ * height_;
    }

    std::uint32_t Image::height() const
    {
        return height_;
    }

    std::uint32_t Image::width() const
    {
        return width_;
    }

    bool Image::operator==(const Image &rhs) const
    {
        return width_ == rhs.width_ && height_ == rhs.height_ && img_ == rhs.img_;
    }

    bool Image::operator!=(const Image &rhs) const
    {
        return width_ != rhs.width_ || height_ != rhs.height_ || img_ != rhs.img_;
    }

    bool Image::empty() const
    {
        return img_.empty();
    }

    void Image::clear_to_bgra(const Bgra &c)
    {
        clear_to_bgra(c.blue, c.green, c.red, c.alpha);
    }

    const std::uint8_t *Image::pixels() const
    {
        return data();
    }

    std::uint8_t *Image::pixels()
    {
        return data();
    }
}
