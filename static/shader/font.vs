#version 410
layout(location = 0) in vec4 vertex;
out vec2 TexCoords;

// uniform mat4 projection;

void main()
{   
    vec4 projection = vec4(0.0f, 800.0f, 0.0f, 600.0f);
    gl_Position = vec4(vertex.xy, 0.0, 600.0);
    TexCoords = vertex.zw;
}  