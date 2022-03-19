#version 150

in  vec2 texCoord;
out vec4 fColor;

uniform sampler2D texture;
uniform float d;

void main() 
{ 
    fColor = (
        texture2D(texture, vec2(texCoord.x + d, texCoord.y)) +
        texture2D(texture, vec2(texCoord.x, texCoord.y + d)) +
        texture2D(texture, vec2(texCoord.x - d, texCoord.y)) +
        texture2D(texture, vec2(texCoord.x, texCoord.y - d))
        ) / 4.0;
} 

