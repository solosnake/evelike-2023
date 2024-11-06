#ifndef solosnake_make_font_hpp
#define solosnake_make_font_hpp

//! @file make_font.hpp
//! This file requires linking to FreeType libraries.
//! The default free type libs linked to are assumed to be named
//! freetype.

#ifdef SS_OS_WINDOWS
#ifdef _M_X64
#pragma comment(lib, "freetype64-lib.lib")
#else
#pragma comment(lib, "freetype32-lib.lib")
#endif
#endif

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace solosnake
{
    class image;
    class fontletters;

    ///! Returns an image and a fontletters which refers to that image.
    //! @param fontfile         A font file readable by FreeType.
    //! @param texfilename      Name to use for tex page inside returned
    // fontletters.
    //! @param characters       An array of codepoints to create characters of.
    //! @param font_pixels_size How large the characters should be rendered.
    //! @param pixel_padding    How many pixels of padding should surround each
    // character.
    //! @param verbose          Enable/disable writing information to std::clog.
    std::pair<std::shared_ptr<image>, std::shared_ptr<fontletters>> make_font(
                const std::string& fontfile,
                const std::string& texfilename,
                const std::vector<std::uint32_t>& characters,
                const unsigned int font_pixels_size,
                const unsigned int pixel_padding,
                bool verbose );
}

#endif
