#ifndef SOLOSNAKE_EVELIKE_VIEW_GL_STATE_HPP
#define SOLOSNAKE_EVELIKE_VIEW_GL_STATE_HPP

#include <memory>
#include "solosnake/opengl.hpp"
#include "solosnake/mouse-ball.hpp"
#include "solosnake/view-projection.hpp"

namespace solosnake::evelike
{
    struct View_gl_state
    {
        solosnake::ViewProjection   view_projection;
        solosnake::Mouseball        mouse_ball;
    };
}

#endif