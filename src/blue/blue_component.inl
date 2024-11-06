namespace blue
{
    inline std::string_view Component::component_name() const noexcept
    {
        return component_name_;
    }

    inline std::int32_t Component::hull_hitpoints() const noexcept
    {
        return hull_hit_points_;
    }

    inline std::int32_t Component::mass() const noexcept
    {
        return mass_;
    }

    inline ComponentCategory Component::categorisation() const noexcept
    {
        return categorisation_;
    }

    inline const Amount& Component::build_cost() const noexcept
    {
        return build_cost_;
    }

    inline float Component::build_complexity() const noexcept
    {
        return build_complexity_;
    }
}
