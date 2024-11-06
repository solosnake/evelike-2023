#include <array>
#include <cstdint>

#include "solosnake/decode-64.hpp"

namespace solosnake
{
    namespace
    {
        std::array<std::uint8_t, 128> constexpr decode_table{
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x3E, 0x64, 0x64, 0x64, 0x3F,
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
            0x3C, 0x3D, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
            0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
            0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
            0x17, 0x18, 0x19, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
            0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
            0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
            0x31, 0x32, 0x33, 0x64, 0x64, 0x64, 0x64, 0x64
        };

        std::array<std::uint8_t, 3> decode_quad(char a, char b, char c, char d)
        {
            std::uint32_t const concat_bytes =
                (decode_table[a] << 18) | (decode_table[b] << 12) |
                (decode_table[c] <<  6) | decode_table[d];

            std::uint8_t const byte1 = (concat_bytes >> 16) & 0b1111'1111;
            std::uint8_t const byte2 = (concat_bytes >> 8) & 0b1111'1111;
            std::uint8_t const byte3 = concat_bytes & 0b1111'1111;
            return {byte1, byte2, byte3};
        }
    }

    // See https://www.rfc-editor.org/rfc/rfc4648#section-4
    // (1) The final quantum of encoding input is an integral multiple of 24
    // bits; here, the final unit of encoded output will be an integral multiple
    // of 4 characters with no "=" padding.
    //
    // (2) The final quantum of encoding input is exactly 8 bits; here, the
    // final unit of encoded output will be two characters followed by two "="
    // padding characters.
    //
    // (3) The final quantum of encoding input is exactly 16 bits; here, the
    // final unit of encoded output will be three characters followed by one "="
    // padding character.
    std::vector<std::uint8_t> decode_64(const std::string_view& text64)
    {
        std::vector<std::uint8_t> data;

        const auto quads_count = text64.length()/4u;

        if( quads_count > 0 )
        {
            data.reserve( 3u * quads_count );

            const auto N = quads_count - 1u;

            for(auto i=0u; i < N; ++i)
            {
                auto bytes = decode_quad( text64[ 4u * i + 0u ],
                                          text64[ 4u * i + 1u ],
                                          text64[ 4u * i + 2u ],
                                          text64[ 4u * i + 3u ] );
                data.push_back( bytes[0] );
                data.push_back( bytes[1] );
                data.push_back( bytes[2] );
            }

            // Last block, may contain padding character '='.
            auto c0 = text64[ 4u * N + 0u ];
            auto c1 = text64[ 4u * N + 1u ];
            auto c2 = text64[ 4u * N + 2u ];
            auto c3 = text64[ 4u * N + 3u ];

            auto a2 = (c2 == '=') ? 'A' : c2;
            auto a3 = (c3 == '=') ? 'A' : c3;

            const auto bytes = decode_quad( c0, c1, a2, a3 );

            data.push_back( bytes[0] );

            if(c2 != '=')
            {
                data.push_back( bytes[1] );
            }

            if(c3 != '=')
            {
                data.push_back( bytes[2] );
            }
        }

        return data;
    }
}