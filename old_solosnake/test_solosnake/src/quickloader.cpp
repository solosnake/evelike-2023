#include "quickloader.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/iimg_cache.hpp"

using namespace solosnake;

namespace solosnake_test
{
    quickloader::quickloader( std::shared_ptr<deferred_renderer> g_rr, std::shared_ptr<iimg_cache> imgs )
        : renderer_( g_rr ), images_( imgs )
    {
    }

    texturehandle_t quickloader::get_texture( const std::string& texname )
    {
        return renderer_->load_texture( images_->get_img_src( texname ) );
    }
}
