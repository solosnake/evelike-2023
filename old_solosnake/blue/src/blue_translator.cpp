#include <memory>
#include <string>
#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/languagetexts.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/manifest.hpp"
#include "solosnake/manifest_filepath.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_translator.hpp"
#include "solosnake/blue/blue_user_settings.hpp"

using namespace std;
using namespace solosnake;

namespace blue
{
    namespace
    {
        //! The translations_reader ignores errors, emitting only warnings,
        //! and tries to continue loading translation strings.
        class translations_reader : public ixmlelementreader
        {
        public:

            translations_reader();

            bool can_read_version( const string& ) const override;

            bool read_from_element( const TiXmlElement& ) override;

            shared_ptr<ilanguagetext> texts() const;

        private:

            shared_ptr<languagetexts>   texts_;
        };

        //-------------------------------------------------------------------------

        translations_reader::translations_reader()
            : ixmlelementreader( "translations" )
            , texts_( make_shared<languagetexts>() )
        {
        }

        bool translations_reader::can_read_version( const string& v ) const
        {
            return v >= string( "1.0" );
        }

        bool translations_reader::read_from_element( const TiXmlElement& elem )
        {
            if( elem.ValueStr() == "text" )
            {
                const string* englishText = elem.Attribute( string( "en" ) );
                const string* localText   = elem.Attribute( string( "local" ) );

                if( englishText && localText )
                {
                    texts_->add_translation( *englishText, *localText );
                }
                else
                {
                    if( nullptr == englishText )
                    {
                        ss_wrn( "Missing 'en' attribute in translations element." );
                    }

                    if( nullptr == localText )
                    {
                        ss_wrn( "Missing 'local' attribute in translations element." );
                    }
                }
            }

            return true;
        }

        shared_ptr<ilanguagetext> translations_reader::texts() const
        {
            return texts_;
        }

        //-------------------------------------------------------------------------

        shared_ptr<ilanguagetext> load_languagetexts( const filepath& url )
        {
            translations_reader reader;
            if( ixmlelementreader::read( url.string(), reader ) )
            {
                return reader.texts();
            }
            else
            {
                return shared_ptr<ilanguagetext>();
            }
        }

        shared_ptr<ilanguagetext> try_load_language( const datapaths& paths, const string& langname )
        {
            shared_ptr<ilanguagetext> lang;

            const auto manifesturls = paths.get_translations_manifests();

            for( auto i = manifesturls.cbegin(); i != manifesturls.cend(); ++i )
            {
                manifest m;
                if( ixmlelementreader::read( i->string().c_str(), m ) )
                {
                    if( m.contains( langname ) )
                    {
                        lang = load_languagetexts( manifest_filepath( *i, langname ) );
                        if( lang )
                        {
                            break;
                        }
                    }
                }
                else
                {
                    ss_wrn( "Unable to read translations xml manifest file at", *i );
                }
            }

            return lang;
        }

        //! Class that performs no actual translation.
        class no_op_translator : public solosnake::ilanguagetext
        {
        public:

            utf8text text_for( const utf8text& txt ) const override // final
            {
                return txt;
            }
        };
    }

    translator::translator( const shared_ptr<user_settings>& settings,
                            const shared_ptr<datapaths>& paths )
        : settings_( settings )
        , language_( make_shared<no_op_translator>() )
        , paths_( paths )
    {
        auto filename = settings_->value( BLUE_GUI_LANGUAGE );

        if( ! change_language( filename ) )
        {
            ss_wrn( "translator failed to load language ", filename, ". Reverting to no translations." );
            change_language( "" );
        }

        assert( settings_->value( BLUE_GUI_LANGUAGE ) == filename );
    }

    translator::~translator()
    {
        ss_dbg( "translator dtor." );
    }

    bool translator::change_language( const string& newlanguagefilename )
    {
        bool changed = false;

        if( newlanguagefilename.empty() )
        {
            settings_->set( BLUE_GUI_LANGUAGE, newlanguagefilename, BLUE_GUI_LANGUAGE_COMMENT );
            language_ = make_shared<no_op_translator>();
            changed   = true;
        }
        else
        {
            auto lang = try_load_language( *paths_, newlanguagefilename );

            if( lang )
            {
                settings_->set( BLUE_GUI_LANGUAGE, newlanguagefilename, BLUE_GUI_LANGUAGE_COMMENT );
                language_ = lang;
                changed   = true;
            }
            else
            {
                ss_wrn( "Unable to located language file ", newlanguagefilename );
            }
        }

        return changed;
    }

    solosnake::utf8text translator::text_for( const solosnake::utf8text& txt ) const
    {
        return language_->text_for( txt );
    }

    std::string translator::current_language_filename() const
    {
        return settings_->value( BLUE_GUI_LANGUAGE );
    }
}
