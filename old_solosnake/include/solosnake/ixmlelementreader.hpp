#ifndef solosnake_ixmlelementreader_hpp
#define solosnake_ixmlelementreader_hpp

#include <string>
#include "solosnake/external/xml.hpp"

namespace solosnake
{
    //! Reads a document which contains a listing of elements.
    //! The document format is considered to be something like:
    //!
    //! @code
    //! <?xml version="1.0" encoding="utf-8" standalone="yes" ?>
    //! <$(GROUPNAME) version="1.0">
    //! <$(ELEMENT) />
    //! <$(ELEMENT) />
    //! <$(ELEMENT) />
    //! ...
    //! </$(GROUPNAME)>
    //! @endcode
    //!
    //! However the ELEMENTs passed in will just be all the elements encountered
    //! inside the GROUPNAME, and it is up to the reader to check that they are the
    //! expected ELEMENTS.
    class ixmlelementreader
    {
    public:

        ixmlelementreader( const std::string& groupName );

        virtual ~ixmlelementreader();

        //! Driver function. Call this with the name of the
        //! TinyXML document to be opened and read.
        static bool read( const char* fileName, ixmlelementreader& );

        //! Driver function. Call this with the name of the
        //! TinyXML document to be opened and read.
        static bool read( const std::string& fileName, ixmlelementreader& );

        //! Driver function. Call this with an opened XML document.
        static bool read( const TiXmlDocument&, ixmlelementreader& );

        //! Driver function. Call this with the root node you want to act on.
        static bool read( const TiXmlNode*, ixmlelementreader& );

    private:

        //! Override this to read and accept or reject the version string.
        //! By default this returns true.
        virtual bool can_read_version( const std::string& ) const;

        //! Override this to read any additional attributes of the group
        //! element. This is called once per document, before any child elements
        //! are read. Returning false to indicate a read error, and the main
        //! read call will return false. Returns true by default.
        //! All elements of the group are passed to the reader.
        virtual bool read_group_element( const TiXmlElement& );

        //! Called every time an element in the group is encountered,
        //! and passes to the user the TinyXML element to read from.
        //! Return false to indicate a read error. Doing so will cause
        //! reading to stop, and the read call will return false.
        //! All elements of the group are passed to the reader.
        virtual bool read_from_element( const TiXmlElement& ) = 0;

        //! Always called at the end of a read, successful or not. This call
        //! can be used e.g. to swap the newly read contents into
        //! the object being deserialised.
        //! If this returns false the static read call returns false.
        //! By default this just returns the value passed in.
        //! All elements of the group are passed to the reader.
        virtual bool read_ended( bool goodRead );

    private:

        std::string groupName_;
    };
}

#endif
