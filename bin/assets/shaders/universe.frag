#version 450 core

layout (location=0) out vec4 color;

void main(void)
{
    float x = 2.0 * (gl_PointCoord.x - 0.5);
    float y = 2.0 * (gl_PointCoord.y - 0.5);
    float d = x * x + y * y;
    if(d > 1.0)
    {
        discard;
    }
    else
    {
        float alpha = 1.0 - (d*d);
        color = vec4(0.838, 0.855, 0.912, alpha);
    }
}
