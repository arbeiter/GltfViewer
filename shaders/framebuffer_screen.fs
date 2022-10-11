#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

vec4 invertColors(vec3 color) {
  vec4 inverted = vec4(vec3(1.0 - color), 1.0);
  return inverted;
}

vec4 grayScale(vec3 color) {
  float average = 0.2126 * color.r + 0.7152 * color.g +  0.0722 * color.b;
  vec4 grayed = vec4(average, average, average, 1.0);
  return grayed;
}

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = col;
}
