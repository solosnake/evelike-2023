namespace solosnake
{
    template<typename T>
    inline command<T>::~command()
    {
    }

    template<typename T>
    inline commandhistory<T>::commandhistory( T&& t )
        : target_( std::move( t ) )
        , done_history_()
        , undone_history_()
    {
        done_history_.reserve( 256u );
        undone_history_.reserve( 128u );
    }

    template<typename T>
    inline void commandhistory<T>::do_cmd( T& target,
                                           std::unique_ptr<command<T>> c,
                                           std::vector<std::unique_ptr<command<T>>>& history )
    {
        auto undoes = c->execute_command_on( target );

        if( undoes )
        {
            history.push_back( std::move( undoes ) );
        }
        else
        {
            history.clear();
        }
    }

    template<typename T>
    inline void commandhistory<T>::undo( T& target,
                                         std::vector<std::unique_ptr<command<T>>>& src,
                                         std::vector<std::unique_ptr<command<T>>>& dst )
    {
        if( ! src.empty() )
        {
            auto c = std::move( src.back() );
            src.pop_back();
            do_cmd( target, std::move( c ), dst );
        }
    }

    template<typename T>
    inline void commandhistory<T>::do_command( std::unique_ptr<command<T>> c )
    {
        do_cmd( target_, std::move( c ), done_history_ );
        undone_history_.clear();
    }


    template<typename T>
    inline void commandhistory<T>::undo()
    {
        undo( target_, done_history_, undone_history_ );
    }

    template<typename T>
    inline void commandhistory<T>::redo()
    {
        undo( target_, undone_history_, done_history_ );
    }

    template<typename T>
    inline bool commandhistory<T>::has_undo_history() const
    {
        return ! done_history_.empty();
    }

    template<typename T>
    inline bool commandhistory<T>::has_redo_history() const
    {
        return ! undone_history_.empty();
    }

    template<typename T>
    inline const T& commandhistory<T>::target() const
    {
        return target_;
    }
}