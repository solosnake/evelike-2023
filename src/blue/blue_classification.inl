namespace blue
{
    inline Classification::Classification()
    {
        classification_[0] = 'A'; // Total number of components.
        classification_[1] = Classification::CountSeparator;
        classification_[2] = 'A'; // Offensive
        classification_[3] = 'A'; // Defensive
        classification_[4] = 'A'; // Industrial
        classification_[5] = 'A'; // Propulsion
        classification_[6] = 0;   // nul
        classification_[7] = 'A'; // Dual use (hidden by nul).
    }

    inline const char *Classification::c_str() const noexcept
    {
        return classification_;
    }

    inline std::uint16_t Classification::component_count(ComponentCategory c) const noexcept
    {
        return (c < DualUseComponent)
                   ? (std::uint16_t(classification_[2u + c]) - 'A')
                   : (std::uint16_t(classification_[0u]) - 'A');
    }

    inline std::uint16_t Classification::total_components_count() const noexcept
    {
        return classification_[0] - 'A';
    }
}
