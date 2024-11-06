
/////////////////////////// input_event inlines ///////////////////////////

inline const std::vector<input_event>& inputs::events() const
{
    return events_;
}

inline void inputs::add_event(const input_event& e)
{
    events_.push_back(e);
}

inline bool inputs::empty() const
{
    return events_.empty();
}
