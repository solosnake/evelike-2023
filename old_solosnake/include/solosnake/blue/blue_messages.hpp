#ifndef blue_messages_hpp
#define blue_messages_hpp

#include <string>
#include <cstdint>

namespace blue
{
    //! Base 'class' of message passed between hosts/clients.
    enum BlueMessageType
    {
        UnknownMessage,
        StartingConditionMessage,
        LoadedMessage,
        GameActionsMessage,
        LoadingErrorMessage,
        PlayingErrorMessage
    };
}

#endif
