
varying vec3 position, normal;
varying vec4 specular, ambient, diffuse, lightDirection;

uniform sampler2D tex;
uniform vec4 basicColor;

void main()
{
    vec3 N = normalize(normal);
    // assume directional light

    gl_MaterialParameters M = gl_FrontMaterial;

    float NdotL = dot(N, lightDirection.xyz);
    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    vec3 absN = abs(gl_TexCoord[1].xyz);
    vec3 texCoord;
    if (absN.x > absN.y && absN.x > absN.z)
        texCoord = gl_TexCoord[1].yzx;
    else if (absN.y > absN.z)
        texCoord = gl_TexCoord[1].zxy;
    else
        texCoord = gl_TexCoord[1].xyz;
    texCoord.y *= -sign(texCoord.z);
    texCoord += 0.5;

    vec4 texColor = texture2D(tex, texCoord.xy);

    vec4 unlitColor = gl_Color * mix(basicColor, vec4(texColor.xyz, 1.0), texColor.w);
    gl_FragColor = (ambient + diffuse * max(NdotL, 0.0)) * unlitColor +
                    M.specular * specular * pow(max(RdotL, 0.0), M.shininess);
}
