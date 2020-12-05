#version 330

in vec3 vPos;

out vec3 texCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 pos = projection * view * vec4(vPos, 1.0);
	gl_Position = pos.xyww;
	texCoord = vPos;
}