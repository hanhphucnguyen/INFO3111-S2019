#version 110
varying vec3 color;		
uniform vec3 triColour;			
void main()
{
    gl_FragColor = vec4(triColour, 1.0);
}