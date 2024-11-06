namespace solosnake
{
    inline const std::string& iwidgetrenderer::stylename() const
    {
        return stylename_;
    }

    inline const rendering_system& iwidgetrenderer::renderingsystem() const
    {
        return *rendering_.get();
    }

    inline rendering_system& iwidgetrenderer::renderingsystem()
    {
        return *rendering_.get();
    }

    inline std::shared_ptr<rendering_system> iwidgetrenderer::renderingsystem_ptr()
    {
        return rendering_;
    }

    inline deferred_renderer& iwidgetrenderer::renderer()
    {
        return *renderer_;
    }
}
