#include "solosnake/filepath.hpp"
#include "solosnake/image.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/unaliased.hpp"
#include "solosnake/colourtriangle.hpp"
#include <cstdint>
#include <fstream>
#include <cassert>

namespace solosnake
{
    namespace
    {
        typedef std::uint8_t GEByte;

#define DEFAULT_ALPHA ((0u))

        //-------------------------------------------------------------------------
        // Sizes of the different structures.
        //-------------------------------------------------------------------------
#define GE_VERSION3BMPSIZE  (40)
#define GE_ADOBEALPHASIZE   (56)
#define GE_VERSION4BMPSIZE (108)
#define GE_VERSION5BMPSIZE (124)

        //-------------------------------------------------------------------------
        // Compression codes.
        //-------------------------------------------------------------------------
#define GE_BI_RGB           (0L) //!< Currently this is the only type supported by GEDIB_RGBA, uncompressed.
#define GE_BI_RLE8          (1L)
#define GE_BI_RLE4          (2L)
#define GE_BI_BITFIELDS     (3L)

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
#define GE_CALIBRATEDRGB        (0x0)
#define GE_DEVICEDEPENDANTRGB   (0x1)
#define GE_DEVICEDEPENDANTCMYK  (0x2)

        //-------------------------------------------------------------------------
        // Definitions for use with version 5 bitmaps. Note that I do not know if
        // these are the actual values!
        //-------------------------------------------------------------------------

        //! Match Absolute Colorimetric Maintains the white point. Matches the
        //! colors to their nearest color in the destination gamut.
#define GE_LCS_GM_ABS_COLORIMETRIC (0x1)

        //! Graphic Saturation Maintains saturation. Used for business charts
        //! and other situations in which undithered colors are required.
#define GE_LCS_GM_BUSINESS  (0x2)

        //! Proof Relative Colorimetric Maintains colorimetric match. Used for
        //! graphic designs and named colors.
#define GE_LCS_GM_GRAPHICS  (0x3)

        //! Picture Perceptual Maintains contrast. Used for photographs and natural images.
#define GE_LCS_GM_IMAGES    (0x4)

#define GE_BM_IDENTIFIER    (0x4D42)

        //-------------------------------------------------------------------------
        //! See http://en.wikipedia.org/wiki/BMP_file_format
        //-------------------------------------------------------------------------
        struct GE_MSBITMAPFILEHEADERMAGICNUMBER
        {
            GE_MSBITMAPFILEHEADERMAGICNUMBER()
                : bfType( GE_BM_IDENTIFIER )
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
                : bfSize( 0 )
                , bfReserved1and2( 0 )
                , bfOffBits( 0 )
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
                : biSize( GE_VERSION3BMPSIZE )
                , biWidth( 0 )
                , biHeight( 0 )
                , biPlanes( 1 )
                , biBitCount( 24 )
                , biCompression( GE_BI_RGB )
                , biSizeImage( 0 )
                , biXPelsPerMeter( 1 )
                , biYPelsPerMeter( 1 )
                , biClrUsed( 0 )
                , biClrImportant( 0 )
            {
            }

            std::uint32_t   biSize;             // Size of the header structure.
            std::int32_t    biWidth;            // Width of image in pixels.
            std::int32_t    biHeight;           // Height of the image in pixels.
            std::uint16_t   biPlanes;           // Number of colour planes, only 1 is possible.
            std::uint16_t   biBitCount;         // Default is 24 bit.
            std::uint32_t   biCompression;      // The only one we currently support is GE_BI_RGB.
            std::uint32_t   biSizeImage;        // Pixels bytes count.
            std::int32_t    biXPelsPerMeter;    // Vertical resolution of the image in pixels per metre.
            std::int32_t     biYPelsPerMeter;   // Horizontal resolution of the image in pixels per metre.
            std::uint32_t   biClrUsed;          // Number of colours in image.
            std::uint32_t   biClrImportant;     // All colours are to be considered equally important (flag == 0).
        };

        //-------------------------------------------------------------------------
        //! For ADOBE X8 R8 G8 B8 (also exported from GIMP.
        //-------------------------------------------------------------------------
        struct GE_RGBAMASKS
        {
            GE_RGBAMASKS()
                : biRedMask( 0 )
                , biGreenMask( 0 )
                , biBlueMask( 0 )
                , biAlphaMask( 0 )
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
                : rgbaMasks()
                , biCSType( 0 )
                , biRedX( 0 )
                , biRedY( 0 )
                , biRedZ( 0 )
                , biGreenX( 0 )
                , biGreenY( 0 )
                , biGreenZ( 0 )
                , biBlueX( 0 )
                , biBlueY( 0 )
                , biBlueZ( 0 )
                , biGammaRed( 0 )
                , biGammaGreen( 0 )
                , biGammaBlue( 0 )
            {
            }

            GE_RGBAMASKS    rgbaMasks;
            std::uint32_t   biCSType;       //! Colour space type.
            std::uint64_t   biRedX;         //! X Coordinate of red endpoint.
            std::uint64_t   biRedY;         //! Y Coordinate of red endpoint.
            std::uint64_t   biRedZ;         //! Z Coordinate of red endpoint.
            std::uint64_t   biGreenX;       //! X Coordinate of green endpoint.
            std::uint64_t   biGreenY;       //! Y Coordinate of green endpoint.
            std::uint64_t   biGreenZ;       //! Z Coordinate of green endpoint.
            std::uint64_t   biBlueX;        //! X Coordinate of blue endpoint.
            std::uint64_t   biBlueY;        //! Y Coordinate of blue endpoint.
            std::uint64_t   biBlueZ;        //! Z Coordinate of blue endpoint.
            std::uint32_t   biGammaRed;     //! Gamma red coordinate scale value.
            std::uint32_t   biGammaGreen;   //! Gamma blue coordinate scale value.
            std::uint32_t   biGammaBlue;    //! Gamma green coordinate scale value.
        };

        //-------------------------------------------------------------------------
        //! For version 5.x bmps. the following must be appended to the above two structures.
        //-------------------------------------------------------------------------
        struct GE_MSINFOHEADERV5
        {
            GE_MSINFOHEADERV5()
                : biIntent( 0 )
                , biProfileData( 0 )
                , biProfileSize( 0 )
                , biReserved( 0 )
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
            std::istream& ifile,
            GEByte* const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const bool bottomup )
        {
            //
            // The file pointer should now be at the start of the bits
            // as there is no colour table. We want to read in the bitmap bits
            // in the correct order. Note that DIBs are 32 bit WORD aligned,
            // and zero padded to make life difficult. Oh, and IBM thought it
            // would be nice for them to be stored upside down too. Note also
            // that in Windows a WORD is 16 bit and DWORD is 32 bit.
            //
            if( bottomup )
            {
                for( std::int32_t i = h - 1; i >= 0; --i )
                {
                    ifile.read( reinterpret_cast<char*>( pbuf + ( 4 * w * i ) ), 4 * w ); // Read in Blue, Green, Red, Alpha
                }
            }
            else
            {
                // Optimal! :)
                ifile.read( reinterpret_cast<char*>( pbuf ), 4 * w * h ); // Read in Blue, Green, Red, Alpha
            }
        }

        //-------------------------------------------------------------------------
        //! Reads in the data from a 24 bit uncompressed version 3.x bitmap, top-down
        //! or bottom up. The pbuf is assumed to be a valid buffer of the correct size.
        //-------------------------------------------------------------------------
        void ReadType3toBGR(
            std::istream& ifile,
            GEByte* const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const bool bottomup )
        {
            GEByte* pwork = 0; // Working pointer.
            std::uint32_t pad; // Size of padding in bytes.

            //
            // Find padded width of bitmap in bytes. This is the min no of WORDs
            // that can hold the rgb bytes.
            //
            pad = 4 - ( w * 3 ) % 4;
            pad %= 4;

            //
            // The file pointer should now be at the start of the bits
            // as there is no colour table. We want to read in the bitmap bits
            // in the correct order. Note that DIBs are 32 bit WORD aligned,
            // and zero padded to make life difficult. Oh, and IBM thought it would
            // be nice for them to be stored upside down too. Note also that in Windows
            // a WORD is 16 bit and DWORD is 32 bit.
            //
            if( bottomup )
            {
                if( pad )
                {
                    for( std::int32_t i = h - 1; i >= 0; --i )
                    {
                        pwork = pbuf + ( w * i * 3 );

                        ifile.read( reinterpret_cast<char*>( pwork ), 3 * w ); // Read in Blue, Green, Red

                        ifile.ignore( static_cast<std::uint64_t>( pad ) ); // Skip padding bytes.
                    }
                }
                else
                {
                    for( std::int32_t i = h - 1; i >= 0; --i )
                    {
                        ifile.read( reinterpret_cast<char*>( pbuf + ( 3 * w * i ) ), 3 * w ); // Read in Blue, Green, Red
                    }
                }
            }
            else
            {
                if( pad )
                {
                    for( std::int32_t i = 0; i < h; ++i )
                    {
                        pwork = pbuf + ( w * i * 3 );

                        ifile.read( reinterpret_cast<char*>( pwork ), 3 * w ); // Read in Blue, Green, Red

                        ifile.ignore( static_cast<std::uint64_t>( pad ) ); // Skip padding bytes.
                    }
                }
                else
                {
                    // Optimal! :)
                    ifile.read( reinterpret_cast<char*>( pbuf ), 3 * w * h ); // Read in Blue, Green, Red
                }
            }
        }

        //-------------------------------------------------------------------------
        //! Reads in the data from a 24 bit uncompressed version 3.x bitmap, top-down
        //! or bottom up. The pbuf is assumed to be a valid buffer of the correct size.
        //-------------------------------------------------------------------------
        void ReadType3toRGBA(
            std::istream& ifile,
            GEByte* const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const bool bottomup )
        {
            GEByte* pwork = 0; // Working pointer.

            //
            // The file pointer should now be at the start of the bits
            // as there is no colour table. We want to read in the bitmap bits
            // in the correct order. Note that DIBs are 32 bit WORD aligned,
            // and zero padded to make life difficult. Oh, and IBM thought it would
            // be nice for them to be stored upside down too. Note also that in Windows
            // a WORD is 16 bit and DWORD is 32 bit.
            //
            if( bottomup )
            {
                for( std::int32_t i = h - 1; i >= 0; --i )
                {
                    pwork = pbuf + ( w * i * 4 );

                    for( std::int32_t j = 0, k = 0; j < w; ++j )
                    {
                        ifile.read( reinterpret_cast<char*>( pwork + k + 2 ), 1 ); // Read in Blue.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 1 ), 1 ); // Read in Green.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 0 ), 1 ); // Read in Red.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 3 ), 1 ); // Read in Alpha.
                        k += 4;
                    }
                }
            }
            else
            {
                for( std::int32_t i = 0; i < h; ++i )
                {
                    pwork = pbuf + ( w * i * 4 );

                    for( std::int32_t j = 0, k = 0; j < w; ++j )
                    {
                        ifile.read( reinterpret_cast<char*>( pwork + k + 2 ), 1 ); // Read in Blue.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 1 ), 1 ); // Read in Green.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 0 ), 1 ); // Read in Red.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 3 ), 1 ); // Read in Alpha.
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
            std::istream& ifile,
            GEByte* const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const bool bottomup )
        {
            GEByte* pwork = 0; // Working pointer.
            std::uint32_t pad; // Size of padding in bytes.

            //
            // Find padded width of bitmap in bytes. This is the min no of WORDs
            // that can hold the rgb bytes.
            //
            pad = 4 - ( w * 3 ) % 4;
            pad %= 4;

            //
            // The file pointer should now be at the start of the bits
            // as there is no colour table. We want to read in the bitmap bits
            // in the correct order. Note that DIBs are 32 bit WORD aligned,
            // and zero padded to make life difficult. Oh, and IBM thought it would
            // be nice for them to be stored upside down too. Note also that in Windows
            // a WORD is 16 bit and DWORD is 32 bit.
            //
            if( bottomup )
            {
                for( std::int32_t i = h - 1; i >= 0; --i )
                {
                    pwork = pbuf + ( w * i * 3 );

                    for( std::int32_t j = 0, k = 0; j < w; ++j )
                    {
                        ifile.read( reinterpret_cast<char*>( pwork + k + 2 ), 1 ); // Read in Blue.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 1 ), 1 ); // Read in Green.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 0 ), 1 ); // Read in Red.
                        k += 3;
                    }

                    if( pad )
                    {
                        ifile.ignore( static_cast<std::uint64_t>( pad ) ); // Skip padding bytes.
                    }
                }
            }
            else
            {
                for( std::int32_t i = 0; i < h; ++i )
                {
                    pwork = pbuf + ( w * i * 3 );

                    for( std::int32_t j = 0, k = 0; j < w; ++j )
                    {
                        ifile.read( reinterpret_cast<char*>( pwork + k + 2 ), 1 ); // Read in Blue.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 1 ), 1 ); // Read in Green.
                        ifile.read( reinterpret_cast<char*>( pwork + k + 0 ), 1 ); // Read in Red.
                        k += 3;
                    }

                    if( pad )
                    {
                        ifile.ignore( static_cast<std::uint64_t>( pad ) ); // Skip padding bytes.
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
            std::istream& ifile,
            GEByte* const pbuf,
            const std::int32_t w,
            const std::int32_t h,
            const GE_RGBAMASKS& masks,
            const bool wantBGRA )
        {
            GEByte* pwork = pbuf;

            std::uint32_t colour;
            std::uint32_t c[4] = { masks.biRedMask, masks.biGreenMask, masks.biBlueMask, masks.biAlphaMask };
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

            for( std::int32_t i = h - 1; i >= 0; --i )
            {
                pwork = pbuf + ( w * i * 4 );

                for( std::int32_t j = 0, k = 0; j < w; ++j, k += 4 )
                {
                    ifile.read( reinterpret_cast<char*>( &colour ), 4 ); // Read in the bytes.

                    // "on some platforms, such as Windows, BGRA is preferred."
                    if( wantBGRA )
                    {
                        *( pwork + k + 0 ) = static_cast<GEByte>( ( colour & masks.biBlueMask ) >> shift[2] );
                        *( pwork + k + 1 ) = static_cast<GEByte>( ( colour & masks.biGreenMask ) >> shift[1] );
                        *( pwork + k + 2 ) = static_cast<GEByte>( ( colour & masks.biRedMask ) >> shift[0] );
                        *( pwork + k + 3 ) = static_cast<GEByte>( ( colour & masks.biAlphaMask ) >> shift[3] );
                    }
                    else
                    {
                        // RGBA
                        *( pwork + k + 0 ) = static_cast<GEByte>( ( colour & masks.biRedMask ) >> shift[0] );
                        *( pwork + k + 1 ) = static_cast<GEByte>( ( colour & masks.biGreenMask ) >> shift[1] );
                        *( pwork + k + 2 ) = static_cast<GEByte>( ( colour & masks.biBlueMask ) >> shift[2] );
                        *( pwork + k + 3 ) = static_cast<GEByte>( ( colour & masks.biAlphaMask ) >> shift[3] );
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        //! Copies BGR pixels into BGRA pixels, setting alpha to 0.
        //-------------------------------------------------------------------------
        void widen_pixels( std::vector<GEByte>& pixels, size_t nPixels )
        {
            const bgr* srcBGRbegin = reinterpret_cast<const bgr*>( &pixels[0] );
            bgra* dstBGRAbegin = reinterpret_cast<bgra*>( &pixels[0] );

            const bgr* srcBGR = srcBGRbegin + nPixels;
            bgra* dstBGR = dstBGRAbegin + nPixels;

            do
            {
                --srcBGR;
                --dstBGR;

                *dstBGR = bgra( *srcBGR, DEFAULT_ALPHA );
            }
            while( srcBGR != srcBGRbegin );
        }

        //-------------------------------------------------------------------------
        //!   Reads in the data from binary file ifile. If the file is not a valid bitmap or
        //!   cannot be found returns false. If an error
        //!   occurs the stream position is undefined in relation to the beginning.
        //!   No effort is made to ensure that it is reset. If the
        //!   function succeeds the position is at the after the last byte read (normal).
        //-------------------------------------------------------------------------
        bool Open( std::istream& ifile, bool wantBGR, std::vector<GEByte>& out, unsigned int& width, unsigned int& height, bool widen )
        {
            bool result = false;

            GE_MSBITMAPFILEHEADERMAGICNUMBER bmmn; // 2 byte magic number.
            GE_MSBITMAPFILEHEADER2 bmpfh;          // Bitmap file header of image.
            GE_MSBITMAPINFOHEADER bmpinfo;         // Bitmap info header of image.

            try
            {
                GEByte* pbuf = 0; // Buffer for bitmap data.
                int w = 0;
                int h = 0;
                bool bottomup = true; // Top down or bottom up?

                //
                // Read in bitmap info & header
                //
                ifile.read( reinterpret_cast<char*>( &bmmn ), sizeof( bmmn ) );
                ifile.read( reinterpret_cast<char*>( &bmpfh ), sizeof( bmpfh ) );

                if( false == ifile.good() )
                {
                    ss_err( "bitmap header read failed." );
                    return false;
                }

                ifile.read( reinterpret_cast<char*>( &bmpinfo ), sizeof( bmpinfo ) );

                if( false == ifile.good() )
                {
                    ss_err( "bitmap info read failed." );
                    return false;
                }

                //
                // Check bitmap is good for us before proceeding
                // Note : 0x4D42 == BM
                //
                if( GE_BM_IDENTIFIER != bmmn.bfType )
                {
                    ss_err( "bitmap magic number (0x4d42) not found." );
                    // Not a BMP file.
                    return false;
                }

                assert( bmpinfo.biSize == GE_VERSION3BMPSIZE || bmpinfo.biSize == GE_VERSION4BMPSIZE || bmpinfo.biSize == GE_ADOBEALPHASIZE );

                assert( 24 == bmpinfo.biBitCount || 32 == bmpinfo.biBitCount );
                assert( GE_BI_BITFIELDS == bmpinfo.biCompression || GE_BI_RGB == bmpinfo.biCompression );

                if( 24 != bmpinfo.biBitCount && 32 != bmpinfo.biBitCount )
                {
                    ss_err( "bitmap not 24 or 32 bpp" );
                    return false;
                }

                if( false == ( GE_BI_RGB == bmpinfo.biCompression || GE_BI_BITFIELDS == bmpinfo.biCompression ) )
                {
                    ss_err( "Unable to read compressed bitmaps" );
                    return false;
                }

                //
                // Create buffer and read in bits.
                //
                h = static_cast<std::int32_t>( bmpinfo.biHeight );
                w = abs( bmpinfo.biWidth );
                bottomup = bmpinfo.biWidth > 0 ? true : false;

                if( w <= 0 || h <= 0 )
                {
                    ss_err( "Width or height not positive" );
                    return false;
                }

                std::vector<GEByte> temp;

                switch( bmpinfo.biSize )
                {
                    case GE_VERSION3BMPSIZE:
                    {
                        if( widen || 32 == bmpinfo.biBitCount )
                        {
                            temp.resize( w * h * 4 );
                        }
                        else
                        {
                            temp.resize( w * h * 3 );
                        }

                        pbuf = &temp[0];

                        if( 32 == bmpinfo.biBitCount )
                        {
                            if( wantBGR )
                            {
                                ReadType3toBGRA( ifile, pbuf, w, h, bottomup );
                            }
                            else
                            {
                                ReadType3toRGBA( ifile, pbuf, w, h, bottomup );
                            }
                        }
                        else
                        {
                            if( wantBGR )
                            {
                                ReadType3toBGR( ifile, pbuf, w, h, bottomup );
                            }
                            else
                            {
                                ReadType3toRGB( ifile, pbuf, w, h, bottomup );
                            }
                        }

                        if( widen && ( 32 != bmpinfo.biBitCount ) )
                        {
                            widen_pixels( temp, w * h );
                        }
                    }
                    break;

                    case GE_VERSION4BMPSIZE:
                    {
                        GE_MSINFOHEADERV4 bmp4; // Version 4 bitmap extension.

                        ifile.read( reinterpret_cast<char*>( &bmp4 ), sizeof( bmp4 ) );

                        if( false == ifile.good() )
                        {
                            ss_err( "Error reading bmp GE_VERSION4BMPSIZE" );
                            return false;
                        }

                        temp.resize( w * h * 4 );

                        pbuf = &temp[0];

                        // Type 4 is always 32 bit (RGBA).
                        ReadType4( ifile, pbuf, w, h, bmp4.rgbaMasks, wantBGR );
                    }
                    break;

                    case GE_ADOBEALPHASIZE:
                    {
                        GE_RGBAMASKS adobea; // ADOBE version.

                        ifile.read( reinterpret_cast<char*>( &adobea ), sizeof( adobea ) );

                        if( false == ifile.good() )
                        {
                            ss_err( "Error reading bmp GE_ADOBEALPHASIZE" );
                            return false;
                        }

                        temp.resize( w * h * 4 );

                        pbuf = &temp[0];

                        ReadType4( ifile, pbuf, w, h, adobea, wantBGR );
                    }
                    break;

                    default:
                        return false;
                        break;
                }

                out.swap( temp );
                width = static_cast<size_t>( w );
                height = static_cast<size_t>( h );
                result = true;
            }
            catch( ... )
            {
                ss_err( "Unknown exception readin bmp" );
                result = false;
            }

            return result;
        }

        image::colorformat calculate_format( size_t imagebytessize, unsigned int imgwidth, unsigned int imgheight )
        {
            if( imagebytessize == 0 )
            {
                // Default format.
                return image::format_bgra;
            }
            else
            {
                if( imagebytessize == ( 3 * imgwidth * imgheight ) )
                {
                    return image::format_bgr;
                }
                else if( imagebytessize == ( 4 * imgwidth * imgheight ) )
                {
                    return image::format_bgra;
                }
            }

            ss_throw( "Unknown image format." );
        }

        //-------------------------------------------------------------------------
        //! Saves a 24 bit type 3.x bitmap to file. These types have padding and may be top-down or
        //! bottom up.
        //-------------------------------------------------------------------------
        bool SaveType3_BGR(
            std::ostream& ofile,
            size_t width,
            size_t height,
            const GEByte* data )
        {
            //
            // Find padded width of bitmap in bytes. This is the min no of WORDs
            // that can hold the rgb bytes.
            //
            unsigned int pad = 4 - ( width * 3 ) % 4;
            pad %= 4;

            if( std::numeric_limits<std::uint32_t>::max() < ( height * ( ( width * 3 ) + pad ) ) )
            {
                ss_throw( "Size of bitmap data W x H is out of range for Bitmap file header." );
            }

            const std::uint32_t pixelsSize = static_cast<std::uint32_t>( height * ( ( width * 3 ) + pad ) );

            GE_MSBITMAPFILEHEADERMAGICNUMBER bmpmagic_;
            GE_MSBITMAPFILEHEADER2 bmpfh_;  //!< Bitmap file header of current image.
            GE_MSBITMAPINFOHEADER bmpinfo_; //!< Bitmap info header of current image.

            static_assert( 14 == ( sizeof( GE_MSBITMAPFILEHEADERMAGICNUMBER ) + sizeof( GE_MSBITMAPFILEHEADER2 ) ), "Structures do not add up to required 14-byte Bitmap File Header." );

            bmpfh_.bfSize = sizeof( bmpmagic_ ) + sizeof( bmpfh_ ) + sizeof( bmpinfo_ ) + ( pixelsSize ); // File size in bytes.
            bmpfh_.bfOffBits = sizeof( bmpmagic_ ) + sizeof( bmpfh_ ) + sizeof( bmpinfo_ );       // Offset from file start to image data (in bytes).

            // Fill in Bitmap header. (version 3.x)
            bmpinfo_.biWidth = static_cast<int>( width ); // Width of image in pixels.
            bmpinfo_.biHeight = static_cast<int>( height ); // Height of the image in pixels.
            bmpinfo_.biSizeImage = pixelsSize;            // Pixels bytes count.

            ofile.write( reinterpret_cast<const char*>( &bmpmagic_ ), sizeof( bmpmagic_ ) );
            ofile.write( reinterpret_cast<const char*>( &bmpfh_ ), sizeof( bmpfh_ ) );
            ofile.write( reinterpret_cast<const char*>( &bmpinfo_ ), sizeof( bmpinfo_ ) );

            //
            // Dump bits to file. The bits must be inverted, and the lines padded to be
            // 32 bit WORD aligned. We could store them top down, as a human may prefer however
            // bmp interpreters which do not check for the negative value of height which indicates
            // a top down

            const GEByte* psrc = data; // Working pointer.

            assert( ( ( ( 3 * width ) + pad ) % 4 ) == 0 );

            if( height > 0 )
            {
                for( size_t y = height; y > 0; --y )
                {
                    psrc = data + ( 3 * width * ( y - 1 ) );

                    for( size_t x = 0, k = 0; x < width; ++x, k += 3 )
                    {
                        const GEByte rbg[3] =
                        {
                            *( psrc + k + 0 ),
                            *( psrc + k + 1 ),
                            *( psrc + k + 2 )
                        };

                        ofile.write( reinterpret_cast<const char*>( rbg ), 3 );
                    }

                    for( unsigned int ii = 0; ii < pad; ++ii )
                    {
                        ofile.put( '0' ); // Insert padding bytes.
                    }
                }
            }

            return ofile.good();
        }

        //-------------------------------------------------------------------------
        //! Saves a 32 bit type 3.x bitmap to file.
        //-------------------------------------------------------------------------
        bool SaveType3_BGRA(
            std::ostream& ofile,
            size_t width,
            size_t height,
            const GEByte* data )
        {
            if( std::numeric_limits<std::uint32_t>::max() < ( height * ( ( width * 4 ) ) ) )
            {
                ss_throw( "Size of bitmap data W x H is out of range for Bitmap file header." );
            }

            const std::uint32_t pixelsSize = static_cast<std::uint32_t>( height * ( ( width * 4 ) ) );

            GE_MSBITMAPFILEHEADERMAGICNUMBER bmpmagic_;
            GE_MSBITMAPFILEHEADER2 bmpfh_;  //!< Bitmap file header of current image.
            GE_MSBITMAPINFOHEADER bmpinfo_; //!< Bitmap info header of current image.

            // Fill in file header data.
            bmpmagic_.bfType = 0x4D42; // "BM" identifier.

            static_assert( 14 == ( sizeof( GE_MSBITMAPFILEHEADERMAGICNUMBER ) + sizeof( GE_MSBITMAPFILEHEADER2 ) ), "Structures do not add up to required 14-byte Bitmap File Header." );
            static_assert( GE_VERSION3BMPSIZE == sizeof( bmpinfo_ ), "GE_MSBITMAPFILEHEADER2 is not standard byte size." );

            bmpfh_.bfSize = sizeof( bmpmagic_ ) + sizeof( bmpfh_ ) + sizeof( bmpinfo_ ) + ( pixelsSize ); // File size in bytes.
            bmpfh_.bfReserved1and2 = 0;                                                           // Always zero.
            bmpfh_.bfOffBits = sizeof( bmpmagic_ ) + sizeof( bmpfh_ ) + sizeof( bmpinfo_ );       // Offset from file start to image data (in bytes).

            // Fill in Bitmap header. (version 3.x)
            bmpinfo_.biSize = GE_VERSION3BMPSIZE;         // Size of the header structure.
            bmpinfo_.biWidth = static_cast<int>( width ); // Width of image in pixels.
            bmpinfo_.biHeight = static_cast<int>( height ); // Height of the image in pixels.
            bmpinfo_.biPlanes = 1;                        // Number of colour planes, only 1 is possible.
            bmpinfo_.biBitCount = 32;                     // Default is 24 bit.
            bmpinfo_.biCompression = GE_BI_RGB;           // The only one we currently support.
            bmpinfo_.biSizeImage = pixelsSize;            // Zero when using uncompressed format.
            bmpinfo_.biXPelsPerMeter = 1;                 // Vertical resolution of the image in pixels per metre.
            bmpinfo_.biYPelsPerMeter = 1;                 // Horizontal resolution of the image in pixels per metre.
            bmpinfo_.biClrUsed = 0;                       // Number of colours in image.
            bmpinfo_.biClrImportant = 0;                  // All colours are to be considered equally important (flag == 0).

            ofile.write( reinterpret_cast<const char*>( &bmpmagic_ ), sizeof( bmpmagic_ ) );
            ofile.write( reinterpret_cast<const char*>( &bmpfh_ ), sizeof( bmpfh_ ) );
            ofile.write( reinterpret_cast<const char*>( &bmpinfo_ ), sizeof( bmpinfo_ ) );

            //
            // Dump bits to file. The bits must be inverted, and the lines padded to be
            // 32 bit WORD aligned. We could store them top down, as a human may prefer however
            // bmp interpreters which do not check for the negative value of height which indicates
            // a top down

            if( height > 0u )
            {
                for( size_t y = height; y > 0u; --y )
                {
                    const GEByte* psrc = data + ( width * ( y - 1u ) * 4u );

                    ofile.write( reinterpret_cast<const char*>( psrc ), 4u * width );
                }
            }

            return ofile.good();
        }

        //-------------------------------------------------------------------------
        //! Calls the correct save method.
        //-------------------------------------------------------------------------
        bool SaveType3(
            std::ostream& ofile,
            size_t width,
            size_t height,
            const GEByte* data,
            image::colorformat currentFormat )
        {
            if( currentFormat == image::format_bgr )
            {
                return SaveType3_BGR( ofile, width, height, data );
            }
            else
            {
                return SaveType3_BGRA( ofile, width, height, data );
            }
        }

        //-------------------------------------------------------------------------
        //! Saves a 32 bit type 4.x bitmap to file.
        //-------------------------------------------------------------------------
        bool SaveType4(
            std::ostream& ofile,
            size_t width,
            size_t height,
            const GEByte* data,
            image::colorformat currentFormat )
        {
            if( std::numeric_limits<std::uint32_t>::max() < ( height * width * 4 ) )
            {
                ss_throw( "Size of bitmap data W x H is out of range for Bitmap file header." );
            }

            const std::uint32_t pixelsSize = static_cast<std::uint32_t>( height * width * 4 );

            GE_MSBITMAPFILEHEADERMAGICNUMBER bmpmagic_;
            GE_MSBITMAPFILEHEADER2 bmpfh_;  //!< Bitmap file header of current image.
            GE_MSBITMAPINFOHEADER bmpinfo_; //!< Bitmap info header of current image.
            GE_MSINFOHEADERV4 bmp4_;        //!< Extended bitmap info for versions 4.

            // Fill in file header data.
            bmpmagic_.bfType = 0x4D42; // "BM" identifier.

            static_assert( 14 == ( sizeof( GE_MSBITMAPFILEHEADERMAGICNUMBER ) + sizeof( GE_MSBITMAPFILEHEADER2 ) ), "Structures do not add up to required 14-byte Bitmap File Header." );

            bmpfh_.bfSize = sizeof( bmpmagic_ ) + sizeof( bmpfh_ ) + sizeof( bmpinfo_ ) + sizeof( bmp4_ ) + ( pixelsSize ); // File size in bytes.
            bmpfh_.bfReserved1and2 = 0;                                                                           // Always zero.
            bmpfh_.bfOffBits = sizeof( bmpmagic_ ) + sizeof( bmpfh_ ) + sizeof( bmpinfo_ ) + sizeof( bmp4_ );     // Offset from file start to image data (in bytes).

            // Fill in Bitmap header. (version 3.x)
            bmpinfo_.biSize = GE_VERSION4BMPSIZE;         // Size of the header structure.
            bmpinfo_.biWidth = static_cast<int>( width ); // Width of image in pixels.
            bmpinfo_.biHeight = static_cast<int>( height ); // Height of the image in pixels.
            bmpinfo_.biPlanes = 1;                        // Number of colour planes, only 1 is possible.
            bmpinfo_.biBitCount = 32;                     // Default is 24 bit.
            bmpinfo_.biCompression = GE_BI_RGB;           // The only one we currently support.
            bmpinfo_.biSizeImage = 0;                     // Zero when using uncompressed format.
            bmpinfo_.biXPelsPerMeter = 1;                 // Vertical resolution of the image in pixels per metre.
            bmpinfo_.biYPelsPerMeter = 1;                 // Horizontal resolution of the image in pixels per metre.
            bmpinfo_.biClrUsed = 1 << 24;                 // Number of colours in image.
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

            const size_t w = width;

            if( currentFormat == image::format_bgra )
            {
                const size_t ww = width * 4;

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
                if( currentFormat == image::format_bgr )
                {
                    const size_t ww = width * 3;

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
                    ss_throw( "Unknown format" );
                }
            }

            ofile.flush();

            return ofile.good();
        }

        bool does_layer_exist( const image& src, size_t layer )
        {
            return ( layer <= 2 ) || ( layer <= 3 && src.format() == image::format_bgra );
        }

        void validate_layer_op( const image& src, size_t srcLayer, const image& dest, size_t destLayer )
        {
            if( false == does_layer_exist( src, srcLayer ) )
            {
                ss_throw( "Source layer out of bounds." );
            }

            if( false == does_layer_exist( dest, destLayer ) )
            {
                ss_throw( "Destination layer out of bounds." );
            }

            if( src.height() != dest.height() || src.width() != dest.width() )
            {
                ss_throw( "Source and destination images are not the same sizes." );
            }
        }

    } // Unnamed namespace.

    //-------------------------------------------------------------------------
    /// This is based on code I wrote in 2001, 10 years ago. So far it still
    /// surpasses all "professional" code I have ever worked with.
    //-------------------------------------------------------------------------
    image::image( std::istream& is, bool widen )
        : width_( 0 )
        , height_( 0 )
    {
        const bool loadImagesAsBGRnotRGB = true;

        if( false == Open( is, loadImagesAsBGRnotRGB, img_, width_, height_, widen ) )
        {
            ss_throw( "Failed to load image from stream." );
        }
    }

    image::image( const filepath& img, bool widen )
        : width_( 0 )
        , height_( 0 )
    {
        if( img.empty() )
        {
            ss_throw( "No image file name specified." );
        }

        std::ifstream ifile( img, std::ios::binary );

        if( false == ifile.is_open() )
        {
            ss_err( "Cannot open ", img.string() );
            ss_throw( "Failed to open specified image file." );
        }
        else
        {
            const bool loadImagesAsBGRnotRGB = true;

            if( false == Open( ifile, loadImagesAsBGRnotRGB, img_, width_, height_, widen ) )
            {
                ss_err( "Cannot read ", img.string() );
                ss_throw( "Unable to read the specified image file." );
            }
        }
    }

    image::image( unsigned int w, unsigned int h, image::colorformat f )
        : img_( ( f == image::format_bgr ? ( w * h * 3 ) : ( w * h * 4 ) ), DEFAULT_ALPHA )
        , width_( w )
        , height_( h )
    {
    }

    image::image( unsigned int w, unsigned int h, bgra c )
        : img_( ( w* h * 4 ), c.alpha() )
        , width_( w )
        , height_( h )
    {
        clear_to_bgra( c );
    }

    image::image( unsigned int w, unsigned int h, bgr c )
        : img_( ( w* h * 3 ), DEFAULT_ALPHA )
        , width_( w )
        , height_( h )
    {
        clear_to_bgra( c.blue(), c.green(), c.red(), DEFAULT_ALPHA );
    }

    image::image( const image& rhs )
        : img_( rhs.img_ )
        , width_( rhs.width_ )
        , height_( rhs.height_ )
    {
    }

    bool image::open( const filepath& file, bool widen ) SS_NOEXCEPT
    {
        try
        {
            image other( file, widen );
            swap( other );
            return true;
        }
        catch( ... )
        {
        }

        return false;
    }

    //! Always save as Type 4 bitmap (contains Alpha channel). If the image is 3 channel RGB image
    //! then the alpha channel is filled with value Zero.
    bool image::save( std::ostream& os ) const
    {
        // return SaveType4( os, width_, height_, data(), format() );
        return SaveType3( os, width_, height_, data(), format() );
    }

    //! Convenience method.
    bool image::save( const char* filename ) const
    {
        std::ofstream ofile( filename, std::ios::binary );
        return ofile.is_open() ? save( ofile ) : false;
    }

    //! Convenience method.
    bool image::save( const std::string& filename ) const
    {
        return save( filename.c_str() );
    }

    //! Returns the format of the image data.
    image::colorformat image::format() const
    {
        return calculate_format( img_.size(), width_, height_ );
    }

    const std::uint8_t* image::pixel_at( const size_t x, const size_t y ) const
    {
        return pixel( x, y, *this, this->format() );
    }

    const std::uint8_t* image::pixel( const size_t x, const size_t y, const image& i, const colorformat f )
    {
        assert( x < i.width() );
        assert( y < i.height() );
        assert( f == i.format() );
        assert( f == format_bgr || f == format_bgra );

        return i.data() + ( ( x + y * i.width() ) * ( f == format_bgr ? 3 : 4 ) );
    }

    std::uint8_t* image::pixel( const size_t x, const size_t y, image& i, const colorformat f )
    {
        assert( x < i.width() );
        assert( y < i.height() );
        assert( f == i.format() );
        assert( f == format_bgr || f == format_bgra );

        return i.data() + ( ( x + y * i.width() ) * ( f == format_bgr ? 3 : 4 ) );
    }

    void image::swap( image& other ) SS_NOEXCEPT
    {
        if( &other != this )
        {
            std::swap( width_, other.width_ );
            std::swap( height_, other.height_ );
            img_.swap( other.img_ );
        }
    }

    //! Returns 0 if both image layers are the same. Returns -1 if the first differing
    //! layer element in src is less than its partner element in dest. Returns +1 if
    //! it is greater.
    int image::compare_layer( const image& src,
                              const size_t srcLayer,
                              const image& dest,
                              const size_t destLayer )
    {
        validate_layer_op( src, srcLayer, dest, destLayer );

        if( false == src.empty() )
        {
            const std::uint8_t* psrc = srcLayer + &src.img_[0];
            const std::uint8_t* pdst = destLayer + &dest.img_[0];

            const size_t srcstep = src.format() == image::format_bgra ? 4 : 3;
            const size_t dsttep = dest.format() == image::format_bgra ? 4 : 3;

            size_t n = src.width_ * src.height_;

            do
            {
                if( *psrc != *pdst )
                {
                    return *psrc < *pdst ? -1 : +1;
                }

                psrc += srcstep;
                pdst += dsttep;
            }
            while( --n );
        }

        return 0;
    }

    void image::copy_layer( const image& src,
                            const size_t srcLayer,
                            image& dest,
                            const size_t destLayer )
    {
        validate_layer_op( src, srcLayer, dest, destLayer );

        if( false == src.empty() )
        {
            if( &src.img_[0] != &dest.img_[0] )
            {
                // They are different memory spaces:
                const std::uint8_t* unaliased psrc = srcLayer + &src.img_[0];
                std::uint8_t* unaliased pdst = destLayer + &dest.img_[0];

                const size_t srcstep = src.format() == image::format_bgra ? 4 : 3;
                const size_t dsttep = dest.format() == image::format_bgra ? 4 : 3;

                size_t n = src.width_ * src.height_;

                do
                {
                    *pdst = *psrc;
                    psrc += srcstep;
                    pdst += dsttep;
                }
                while( --n );
            }
            else
            {
                // They are the same image: avoid silly copy:
                if( srcLayer != destLayer )
                {
                    const std::uint8_t* unaliased psrc = srcLayer + &src.img_[0];
                    std::uint8_t* unaliased pdst = destLayer + &dest.img_[0];

                    const size_t srcstep = src.format() == image::format_bgra ? 4 : 3;
                    const size_t dsttep = dest.format() == image::format_bgra ? 4 : 3;

                    size_t n = src.width_ * src.height_;

                    do
                    {
                        *pdst = *psrc;
                        psrc += srcstep;
                        pdst += dsttep;
                    }
                    while( --n );
                }
            }
        }
    }

    void image::clear_layer_to( image& src,
                                const size_t layer,
                                const std::uint8_t value )
    {
        if( false == does_layer_exist( src, layer ) )
        {
            ss_throw( "Layer out of bounds." );
        }

        if( false == src.empty() )
        {
            // They are different memory spaces:
            std::uint8_t* unaliased psrc = layer + &src.img_[0];

            const size_t srcstep = src.format() == image::format_bgra ? 4 : 3;

            size_t n = src.width_ * src.height_;

            do
            {
                *psrc = value;
                psrc += srcstep;
            }
            while( --n );
        }
    }

    void image::flip_vertically()
    {
        *this = flipped();
    }

    image image::flipped() const
    {
        image flipped( width_, height_, format() );

        const unsigned int rowwidth = width_ * ( format() == format_bgr ? 3 : 4 );
        const std::uint8_t* unaliased srcpixels = img_.data();
        std::uint8_t* unaliased dstpixels = flipped.img_.data();

        for( unsigned int dstrow = 0; dstrow < height_; ++dstrow )
        {
            const unsigned int srcrow = height_ - ( dstrow + 1 );

            for( unsigned int i = 0; i < rowwidth; ++i )
            {
                // Copy from src bottom into dest top, moving downwards.
                *( dstpixels + ( rowwidth * dstrow ) + i ) = *( srcpixels + ( rowwidth * srcrow ) + i );
            }
        }

        return flipped;
    }

    namespace
    {
        class color
        {
            float b;
            float g;
            float r;
            float a;

            color( float B, float G, float R, float A )
                : b( B )
                , g( G )
                , r( R )
                , a( A )
            {
            }

        public:
            color()
                : b( 0.0f )
                , g( 0.0f )
                , r( 0.0f )
                , a( 0.0f )
            {
            }
            color( std::uint8_t B, std::uint8_t G, std::uint8_t R, std::uint8_t A )
                : b( B / 255.0f )
                , g( G / 255.0f )
                , r( R / 255.0f )
                , a( A / 255.0f )
            {
            }
            color( const bgra& c )
                : b( c.blue()  / 255.0f )
                , g( c.green() / 255.0f )
                , r( c.red()   / 255.0f )
                , a( c.alpha() / 255.0f )
            {
            }

            inline color operator-( const color& rhs ) const
            {
                return color(
                           b - rhs.b,
                           g - rhs.g,
                           r - rhs.r,
                           a - rhs.a );
            }

            inline color operator+( const color& rhs ) const
            {
                return color(
                           b + rhs.b,
                           g + rhs.g,
                           r + rhs.r,
                           a + rhs.a );
            }

            inline color operator*( const float rhs ) const
            {
                return color(
                           b * rhs,
                           g * rhs,
                           r * rhs,
                           a * rhs );
            }

            inline bgra to_bgra() const
            {
                return bgra(
                           static_cast<std::uint8_t>( b * 255.0f ),
                           static_cast<std::uint8_t>( g * 255.0f ),
                           static_cast<std::uint8_t>( r * 255.0f ),
                           static_cast<std::uint8_t>( a * 255.0f ) );
            }

            inline bgr to_bgr() const
            {
                return bgr(
                           static_cast<std::uint8_t>( b * 255.0f ),
                           static_cast<std::uint8_t>( g * 255.0f ),
                           static_cast<std::uint8_t>( r * 255.0f ) );
            }
        };

        struct edge
        {
            int X1;
            int Y1;
            int X2;
            int Y2;

            edge( int x1, int y1, int x2, int y2 )
            {
                if( y1 < y2 )
                {
                    X1 = x1;
                    Y1 = y1;
                    X2 = x2;
                    Y2 = y2;
                }
                else
                {
                    X1 = x2;
                    Y1 = y2;
                    X2 = x1;
                    Y2 = y1;
                }
            }
        };

        struct span
        {
            int X1;
            int X2;

            span()
                : X1( 0 )
                , X2( 0 )
            {
            }

            span( int x1, int x2 )
                : X1( ( x1 < x2 ) ? x1 : x2 )
                , X2( ( x1 < x2 ) ? x2 : x1 )
            {
            }
        };

        void set_pixel_bgra( image& img, int x, int y, const color& c )
        {
            if( x >= 0 && y >= 0 && static_cast<size_t>( x ) < img.width() && static_cast<size_t>( y ) < img.height() )
            {
                auto px = reinterpret_cast<bgra*>( image::pixel( x, y, img, image::format_bgra ) );
                *px = c.to_bgra();
            }
        }

        void set_pixel_bgr( image& img, int x, int y, const color& c )
        {
            if( x >= 0 && y >= 0 && static_cast<size_t>( x ) < img.width() && static_cast<size_t>( y ) < img.height() )
            {
                auto px = reinterpret_cast<bgr*>( image::pixel( x, y, img, image::format_bgr ) );
                *px = c.to_bgr();
            }
        }

        void draw_span( image& img, image::colorformat fmt, const span& s, int y, const colourtriangle2d& colr )
        {
            assert( ( s.X2 - s.X1 ) != 0 );

            // draw each pixel in the span
            if( fmt == image::format_bgra )
            {
                for( int x = s.X1; x < s.X2; ++x )
                {
                    set_pixel_bgra( img, x, y, colr.get_barycentric_colour( static_cast<float>( x ), static_cast<float>( y ) ) );
                }
            }
            else
            {
                for( int x = s.X1; x < s.X2; ++x )
                {
                    set_pixel_bgr( img, x, y, colr.get_barycentric_colour( static_cast<float>( x ), static_cast<float>( y ) ) );
                }
            }
        }

        void draw_spans_between_edges( image& img, image::colorformat fmt, const edge& e1, const edge& e2, const colourtriangle2d& colr )
        {
            if( e1.Y2 == e1.Y1 || e2.Y2 == e2.Y1 )
            {
                return;
            }

            // calculate difference between the y coordinates
            // of the first edge and return if 0
            float e1ydiff = ( float )( e1.Y2 - e1.Y1 );

            // calculate difference between the y coordinates
            // of the second edge and return if 0
            float e2ydiff = ( float )( e2.Y2 - e2.Y1 );

            // calculate differences between the x coordinates
            // and colors of the points of the edges
            float e1xdiff = ( float )( e1.X2 - e1.X1 );
            float e2xdiff = ( float )( e2.X2 - e2.X1 );

            // calculate factors to use for interpolation
            // with the edges and the step values to increase
            // them by after drawing each span
            float factor1 = ( float )( e2.Y1 - e1.Y1 ) / e1ydiff;
            float factorStep1 = 1.0f / e1ydiff;
            float factor2 = 0.0f;
            float factorStep2 = 1.0f / e2ydiff;

            // loop through the lines between the edges and draw spans
            for( int y = e2.Y1; y < e2.Y2; ++y )
            {
                // create and draw span
                span s( e1.X1 + ( int )( e1xdiff * factor1 ),
                        e2.X1 + ( int )( e2xdiff * factor2 ) );

                if( s.X2 != s.X1 )
                {
                    draw_span( img, fmt, s, y, colr );
                }

                // increase factors
                factor1 += factorStep1;
                factor2 += factorStep2;
            }
        }
    }

    //! Clears entire image to BGRA colour specified. A is not used if
    //! format is BGR.
    void image::clear_to_bgra(
        std::uint8_t b,
        std::uint8_t g,
        std::uint8_t r,
        std::uint8_t a )
    {
        if( false == empty() )
        {
            // They are different memory spaces:
            std::uint8_t* unaliased psrc = &img_[0];

            const size_t n = width_ * height_;

            if( format() == image::format_bgra )
            {
                for( size_t i = 0; i < n; ++i )
                {
                    psrc[i * 4 + 0] = b;
                    psrc[i * 4 + 1] = g;
                    psrc[i * 4 + 2] = r;
                    psrc[i * 4 + 3] = a;
                }
            }
            else
            {
                for( size_t i = 0; i < n; ++i )
                {
                    psrc[i * 3 + 0] = b;
                    psrc[i * 3 + 1] = g;
                    psrc[i * 3 + 2] = r;
                }
            }
        }
    }

    void image::set_pixel( const size_t x, const size_t y, const bgra& c )
    {
        std::uint8_t* p = pixel( x, y, *this, format() );
        p[0] = c.blue();
        p[1] = c.green();
        p[2] = c.red();
        if( format() == format_bgra )
        {
            p[3] = c.alpha();
        }
    }

    //! Classic and possible slow triangle drawing onto the image. Interpolates
    //! the colours across the triangle. If the image is bgr only the alpha
    //! component is unused.
    //! Colour blending between vertices in different triangle might not
    //! produce the same colours along shared edges.
    //! @param x1 Normalised x coordinate of first vertex.
    //! @param x2 Normalised x coordinate of second vertex.
    //! @param x3 Normalised x coordinate of third vertex.
    //! @param y1 Normalised y coordinate of first vertex.
    //! @param y2 Normalised y coordinate of second vertex.
    //! @param y3 Normalised y coordinate of third vertex.
    void image::draw_triangle(
        float x1, float y1, const bgra& c1,
        float x2, float y2, const bgra& c2,
        float x3, float y3, const bgra& c3 )
    {
        const float w = static_cast<float>( width() );
        const float h = static_cast<float>( height() );

        colourtriangle2d colourer(
            w * x1, h * y1, c1,
            w * x2, h * y2, c2,
            w * x3, h * y3, c3 );

        // create edges for the triangle
        edge edges[3] =
        {
            edge( static_cast<int>( x1 * w ), static_cast<int>( y1 * h ), static_cast<int>( x2 * w ), static_cast<int>( y2 * h ) ),
            edge( static_cast<int>( x2 * w ), static_cast<int>( y2 * h ), static_cast<int>( x3 * w ), static_cast<int>( y3 * h ) ),
            edge( static_cast<int>( x3 * w ), static_cast<int>( y3 * h ), static_cast<int>( x1 * w ), static_cast<int>( y1 * h ) )
        };

        int maxLength = 0;
        int longEdge = 0;

        // find edge with the greatest length in the y axis
        for( int i = 0; i < 3; i++ )
        {
            int length = edges[i].Y2 - edges[i].Y1;
            if( length > maxLength )
            {
                maxLength = length;
                longEdge = i;
            }
        }

        int shortEdge1 = ( longEdge + 1 ) % 3;
        int shortEdge2 = ( longEdge + 2 ) % 3;

        // draw spans between edges; the long edge can be drawn
        // with the shorter edges to draw the full triangle
        image::colorformat fmt = format();

        draw_spans_between_edges( *this, fmt, edges[longEdge], edges[shortEdge1], colourer );
        draw_spans_between_edges( *this, fmt, edges[longEdge], edges[shortEdge2], colourer );
    }
}
