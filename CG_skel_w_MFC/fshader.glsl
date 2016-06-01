#version 330

in vec4 color;

out vec4 fColor;


void main() 
{ 
   //fColor = textureLod( texture, texCoord, 0 );
   //fColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
   fColor = color;
} 

