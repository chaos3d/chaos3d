uniform sampler2D c_tex1;
varying lowp vec2 uvVaring;
varying lowp float alpha;

void main(void)
{
    gl_FragColor = texture2D(c_tex1, uvVaring) * vec4(1.0, 1.0, 1.0, 1.0);
}
