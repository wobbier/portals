$input v_color0, v_normal, v_texcoord0


#include "../../Engine/Assets/Shaders/Common.sh"

SAMPLER2D(s_texDiffuse, 0);
SAMPLER2D(s_texNormal, 1);
SAMPLER2D(s_texAlpha, 2);
uniform vec4 s_diffuse;
uniform vec4 s_ambient;
uniform vec4 s_sunDirection;
uniform vec4 s_sunDiffuse;
uniform vec4 s_tiling;
uniform vec4 u_skyLuminance;
uniform vec4 s_screenSize;

void main()
{
	// u_viewRect = game viewport size
	// s_screenSize = application window size
	// v_texcoord0 = local to u_viewRect
	float2 texCoord = v_texcoord0.xy / v_texcoord0.w;

	float2 coords = float2(texCoord.x, -texCoord.y);
	float2 scale = u_viewRect.zw / s_screenSize.xy;
	vec2 uvs = (0.5f * coords + 0.5f) * s_tiling.xy;
	uvs = uvs * scale;
	vec4 color = texture2D(s_texDiffuse, uvs) * s_diffuse;

	color.a = toLinear(texture2D(s_texAlpha, uvs)).r;

	vec4 testColor = vec4(u_viewRect.z / s_screenSize.x, 0.0, 0.0, 1.0);
	gl_FragColor = color;
}