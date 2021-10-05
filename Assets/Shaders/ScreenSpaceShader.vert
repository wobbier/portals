$input a_position, a_normal, a_texcoord0, a_tangent, a_bitangent
$output v_color0, v_normal, v_texcoord0

#include "../../Engine/Assets/Shaders/Common.sh"

void main()
{
	vec4 vert = vec4(a_position, 1.0);
	gl_Position = mul(u_modelViewProj, vert);

	v_color0 = vec4(a_normal.x,a_normal.y,a_normal.z,1.0);

	
	v_texcoord0 = gl_Position;

	vec3 normal = a_normal.xyz;
	v_normal = mul(u_model[0], vec4(normalize(normal), 0.0) ).xyz;
}
