#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/img_cache.hpp"
#include "solosnake/img.hpp"
#include "solosnake/ifilefinder.hpp"

using namespace std;

namespace solosnake
{
    //! Hidden implementation of a cached image. Reloads an image when requested.
    class img_cache::cachedimgsrc : public iimgsrc
    {
    public:

        cachedimgsrc( const std::string& name, const filepath& pth );

        virtual ~cachedimgsrc() SS_NOEXCEPT;

        std::string get_image_name() const override;

        iimg_ptr get_image() override;

        void release_contents();

        static bool compare_name( const shared_ptr<img_cache::cachedimgsrc>& lhs,
                                  const string& name );

        static bool compare( const shared_ptr<img_cache::cachedimgsrc>& lhs,
                             const shared_ptr<img_cache::cachedimgsrc>& rhs );

    private:

        iimg_ptr reload_img();

    private:

        string      name_;
        filepath    path_;
        iimg_ptr    image_;
    };

    //-------------------------------------------------------------------------

    img_cache::cachedimgsrc::cachedimgsrc( const string& name, const filepath& pth )
        : name_( name )
        , path_( pth )
    {
    }

    img_cache::cachedimgsrc::~cachedimgsrc() SS_NOEXCEPT
    {
    }

    std::string img_cache::cachedimgsrc::get_image_name() const
    {
        return name_;
    }

    iimg_ptr img_cache::cachedimgsrc::get_image()
    {
        return image_ ? image_ : reload_img();
    }

    void img_cache::cachedimgsrc::release_contents()
    {
        image_.reset();
    }

    bool img_cache::cachedimgsrc::compare_name(
        const shared_ptr<img_cache::cachedimgsrc>& lhs,
        const string& name )
    {
        return lhs->name_ < name;
    }

    bool img_cache::cachedimgsrc::compare(
        const shared_ptr<img_cache::cachedimgsrc>& lhs,
        const shared_ptr<img_cache::cachedimgsrc>& rhs )
    {
        return lhs->name_ < rhs->name_;
    }


    iimg_ptr img_cache::cachedimgsrc::reload_img()
    {
        image_ = make_shared<img>( path_ );
        return image_;
    }

    //-------------------------------------------------------------------------

    img_cache::img_cache( const shared_ptr<ifilefinder>& ff ) : filefinder_( ff )
    {
        cache_.reserve( 16 );
    }

    img_cache::~img_cache()
    {
    }

    size_t img_cache::size() const
    {
        return cache_.size();
    }

    void img_cache::compact_cache()
    {
        for( size_t i = 0; i < cache_.size(); ++i )
        {
            cache_[i]->release_contents();
        }
    }

    std::shared_ptr<iimgsrc> img_cache::get_img_src( const std::string& name )
    {
        auto i = lower_bound( cache_.cbegin(), cache_.cend(), name, &img_cache::cachedimgsrc::compare_name );

        if( ( i != cache_.cend() ) && ( ( *i )->get_image_name() == name ) )
        {
            return *i;
        }

        // No image source with that name found in cache. Try to add it:
        auto newimg = make_shared<img_cache::cachedimgsrc>( name, filefinder_->get_file( name ) );

        cache_.push_back( newimg );

        // Sort by name to allow use of lower_bound.
        sort( cache_.begin(), cache_.end(), &img_cache::cachedimgsrc::compare );

        return newimg;
    }
}
