#version 420

//uniform mat4 MVP;
uniform mat4 matModel;		// "m"
uniform mat4 matView;		// "v"
uniform mat4 matProj;		// "p"

attribute vec3 vCol;	
attribute vec2 vPos;

out vec3 color;		

void main()
{
	// mvp = p * v * m; 
	mat4 MVP = matProj * matView * matModel;
	
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vCol;
}



