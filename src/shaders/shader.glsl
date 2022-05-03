#shader vertex
#version 330 core
layout(location = 0) in vec3 _Position;
layout(location = 1) in vec3 _Colors;
layout(location = 2) in vec2 _TexCoord;

out vec3 colors;
out vec2 texCoord;
void main()
{
	colors = _Colors;
	texCoord = _TexCoord;
	gl_Position = vec4(_Position, 1.0f);
}

#shader fragment
#version 330 core
in vec3 colors;
in vec2 texCoord;

uniform sampler2D texture1;

void main()
{
	gl_FragColor = texture(texture1, texCoord);
}