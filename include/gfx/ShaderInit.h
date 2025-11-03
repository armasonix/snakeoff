#pragma once
#include <SFML/Graphics/Shader.hpp>

namespace gfx
{
    inline void initGameShaders(
        sf::Shader& groundDesat, bool& groundDesatReady, float groundSaturation,
        sf::Shader& confuseOverlay, bool& confuseOverlayReady,
        sf::Shader& chromAb, bool& chromAbReady,
        sf::Shader& portalGlow, bool& portalGlowReady)
    {
        // ground desat
        {
            const char* frag = R"(
            uniform sampler2D texture;
            uniform float u_saturation;
            void main()
            {
                vec4 c = texture2D(texture, gl_TexCoord[0].xy);
                float gray = dot(c.rgb, vec3(0.299, 0.587, 0.114));
                vec3 desat = mix(vec3(gray), c.rgb, u_saturation);
                gl_FragColor = vec4(desat, c.a);
            })";
            groundDesatReady = groundDesat.loadFromMemory(frag, sf::Shader::Fragment);
            if (groundDesatReady)
            {
                groundDesat.setUniform("u_saturation", groundSaturation);
                groundDesat.setUniform("texture", sf::Shader::CurrentTexture);
            }
        }

        // confuse overlay
        {
            const char* frag = R"(
            uniform float u_time;
            uniform vec2  u_res;
            void main()
            {
                vec2 uv = gl_FragCoord.xy / u_res;
                float r = 0.5 + 0.5 * sin(u_time + uv.x * 6.2831853);
                float g = 0.5 + 0.5 * sin(u_time + 2.0943951 + uv.y * 6.2831853);
                float b = 0.5 + 0.5 * sin(u_time + 4.1887902 + (uv.x + uv.y) * 3.1415926);
                gl_FragColor = vec4(r, g, b, 0.18);
            })";
            confuseOverlayReady = confuseOverlay.loadFromMemory(frag, sf::Shader::Fragment);
        }

        // chromatic aberration
        {
            const char* frag = R"(
            uniform sampler2D texture;
            uniform vec2  u_res;
            uniform float u_amount;
            uniform float u_time;
            void main()
            {
                vec2 uv = gl_TexCoord[0].xy;
                vec2 center = vec2(0.5, 0.5);
                vec2 d = uv - center;
                float r = length(d) + 1e-6;
                float amtUV = (u_amount / u_res.x) * (0.95 + 0.15 * sin(u_time * 6.2831853));
                vec2 dir = d / r;
                vec2 off = dir * amtUV * r * 2.0;
                float rr = texture2D(texture, uv + off).r;
                float gg = texture2D(texture, uv).g;
                float bb = texture2D(texture, uv - off).b;
                vec4 src = texture2D(texture, uv);
                gl_FragColor = vec4(rr, gg, bb, src.a);
            })";
            chromAbReady = chromAb.loadFromMemory(frag, sf::Shader::Fragment);
            if (chromAbReady)
            {
                chromAb.setUniform("texture", sf::Shader::CurrentTexture);
            }
        }

        // portal emissive glow
        {
            const char* frag = R"(
            uniform sampler2D texture;
            uniform vec3  u_tint;
            uniform float u_strength;
            uniform float u_time;
            uniform vec2  u_texel;
            void main()
            {
                vec2 uv = gl_TexCoord[0].xy;
                vec4 t  = texture2D(texture, uv);
                float a = t.a;
                float core = smoothstep(0.0, 0.7, a);
                vec2 o = u_texel * 1.5;
                float n = 0.0;
                n += texture2D(texture, uv + vec2( o.x, 0.0)).a;
                n += texture2D(texture, uv + vec2(-o.x, 0.0)).a;
                n += texture2D(texture, uv + vec2(0.0,  o.y)).a;
                n += texture2D(texture, uv + vec2(0.0, -o.y)).a;
                float edge = n * 0.25;
                float pulse = 0.65 + 0.35 * sin(u_time);
                float g = (core * 0.85 + edge * 0.65) * pulse * u_strength;
                vec3 glow = u_tint * g;
                gl_FragColor = vec4(glow, g);
            })";
            portalGlowReady = portalGlow.loadFromMemory(frag, sf::Shader::Fragment);
            if (portalGlowReady)
            {
                portalGlow.setUniform("texture", sf::Shader::CurrentTexture);
            }
        }
    }
}