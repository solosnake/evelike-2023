#ifndef test_solosnake_quickloader_hpp
#define test_solosnake_quickloader_hpp

#include <string>
#include <memory>
#include "solosnake/itextureloader.hpp"
#include "solosnake/texturehandle.hpp"

namespace solosnake
{
class deferred_renderer;
class iimg_cache;
}

namespace solosnake_test
{
class quickloader : public solosnake::itextureloader
{
public:
    quickloader(std::shared_ptr<solosnake::deferred_renderer>,
                std::shared_ptr<solosnake::iimg_cache>);

    virtual solosnake::texturehandle_t get_texture(const std::string& texname);

private:
    std::shared_ptr<solosnake::deferred_renderer> renderer_;
    std::shared_ptr<solosnake::iimg_cache> images_;
};
}

#endif
