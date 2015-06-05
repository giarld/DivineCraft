varying vec3 position, normal;
varying vec4 ambient, diffuse, lightDirection;

uniform sampler3D tex;

uniform vec4 basicColor;
void main()
{
    vec3 N = normalize(normal);
    // assume directional light

//    gl_MaterialParameters M = gl_FrontMaterial;

    float NdotL = dot(N, lightDirection.xyz);
//    float RdotL = dot(reflect(normalize(position), N), lightDirection.xyz);

    vec4 texColor = texture3D(tex,gl_TexCoord[0].xyz);

    vec4 unlitColor = gl_Color * mix(basicColor, vec4(texColor.xyz, 1.0), texColor.w);
    gl_FragColor = (ambient + diffuse * max(NdotL, 0.0)) * unlitColor ;

//    vec4 color = texture2D(tex,gl_TexCoord[0].xy);
//    gl_FragColor = color;
}
