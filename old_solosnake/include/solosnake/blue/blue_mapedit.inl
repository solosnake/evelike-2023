namespace blue
{
    inline void mapedit::do_command( std::unique_ptr<mapedit::command_t> c )
    {
        target_.do_command( std::move(c) );
    }

    inline void mapedit::undo_command()
    {
        target_.undo();
    }

    inline void mapedit::redo_command()
    {
        target_.redo();
    }
}
