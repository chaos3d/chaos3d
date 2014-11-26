attribute vec4 position;
attribute lowp vec2 uv;
uniform mat4 c_ProjViewMat;
varying lowp vec2 uvVaring;
varying lowp float alpha;

void main() {
    gl_Position = c_ProjViewMat * vec4(position.xyz, 1.);
    uvVaring = uv;
    alpha = position.w;
}

