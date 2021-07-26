#pragma once

const char* RmlVertexShader = R"(
layout(location = 0) in vec2 in_Pos;
layout(location = 1) in uint in_Color;
layout(location = 2) in vec2 in_TexCoord;

out vec2 TexCoord;
out vec4 Color;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform VertexUniform {
    mat4 Projection;
    mat4 ModelMat;
};

void main() {
    gl_Position = Projection * ModelMat * vec4(in_Pos.x, in_Pos.y, -0.1, 1.0);
    TexCoord = in_TexCoord;
    Color = vec4((in_Color & 0x000000FFu) >> 0u, (in_Color & 0x0000FF00u) >> 8u, (in_Color & 0x00FF0000u) >> 16u, (in_Color & 0xFF000000u)>> 24u) / vec4(256.0f);
}
)";

const char* RmlColorFragmentShader = R"(
in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;

uniform Scissors
{
    vec4 sRect;
    vec4 sSettings;
};

#define StartX sRect.x
#define StartY sRect.y
#define EndX sRect.z
#define EndY sRect.w

#define ScissorEnabled sSettings.x > 0.5
#define ScreenHeight sSettings.y

#define RealPixelX gl_FragCoord.x
#define RealPixelY (ScreenHeight - gl_FragCoord.y)

void main()
{
    if(ScissorEnabled && (
        (RealPixelX < StartX || RealPixelX > EndX) ||
        (RealPixelY < StartY || RealPixelY > EndY)
    )) discard;

    FragColor = Color;
}
)";

const char* RmlTexturedFragmentShader = R"(
in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;

uniform sampler2D Texture;

uniform Scissors
{
    vec4 sRect;
    vec4 sSettings;
};

#define StartX sRect.x
#define StartY sRect.y
#define EndX sRect.z
#define EndY sRect.w

#define ScissorEnabled sSettings.x > 0.5
#define ScreenHeight sSettings.y

#define RealPixelX gl_FragCoord.x
#define RealPixelY (ScreenHeight - gl_FragCoord.y)

void main()
{
    if(ScissorEnabled && (
        (RealPixelX < StartX || RealPixelX > EndX) ||
        (RealPixelY < StartY || RealPixelY > EndY)
    )) discard;

    FragColor = texture(Texture, TexCoord) * Color;
}
)";