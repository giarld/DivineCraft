uniform sampler3D tex;

void main()
{
    vec4 color = texture3D(tex,gl_TexCoord[0].xyz);
    gl_FragColor = color;
}
