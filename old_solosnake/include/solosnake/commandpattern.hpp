#ifndef solosnake_commandpattern_hpp
#define solosnake_commandpattern_hpp

#include <memory>
#include <vector>

namespace solosnake
{
    //! Template implementation of the command pattern (undo/redo). The target T is
    //! the object that the commands act upon.
    template<typename T>
    class command
    {
    public:

        virtual ~command();

        //! Performs a command on T, probably changing it.
        //! If the command can be undone, returns the inverse command.
        //! If the command cannot be undone, returns the empty command ptr.
        virtual std::unique_ptr<command<T>> execute_command_on( T& ) = 0;
    };

    //! Wraps a command target T. The target can only be changed by issuing commands
    //! to it. The 'target' here can also be called the 'model'.
    //! This is a simple do/undo/redo model. A new 'do' command removes all undone history.
    //! This means that not all intermediate states of the model/target are retrievable.
    template<typename T>
    class commandhistory
    {
    public:

        explicit commandhistory( T&& );

        //! Executes the command on the target.
        void do_command( std::unique_ptr<command<T>> );

        //! Undoes the most recent command, restoring the previous state of the target.
        void undo();

        //! Undoes the most recently undone command, restoring the previous state of the target.
        void redo();

        //! Returns true if there is a non-empty stored history of un-doable commands.
        bool has_undo_history() const;

        //! Returns true if there is a non-empty stored history of re-doable commands.
        bool has_redo_history() const;

        //! Returns the target object upon which the commands act.
        const T& target() const;

    private:

        static void do_cmd(
            T&,
            std::unique_ptr<command<T>>,
            std::vector<std::unique_ptr<command<T>>>& history );

        static void undo(
            T&,
            std::vector<std::unique_ptr<command<T>>>& src,
            std::vector<std::unique_ptr<command<T>>>& dst );

    private:

        T                                          target_;
        std::vector<std::unique_ptr<command<T>>>   done_history_;
        std::vector<std::unique_ptr<command<T>>>   undone_history_;
    };
}

#include "solosnake/commandpattern.inl"

#endif
