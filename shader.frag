#version 330 core
in vec3 v_position;
in vec3 light_position;
in vec3 v_normal;
out vec4 FragColor;

void main()
{
    vec3 light_position_wc = vec3(0.0f, 1.0f, 1.0f);
    //vec3 light_position_wc = light_position;
    vec4 light_diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 light_specular = vec4(1.0f, 0.5f, 0.5f, 1.0f);
    vec4 material_diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 material_specular = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    float               material_shininess = 60.0f;

    vec4 color = vec4(0.0);
    vec3 n_wc = normalize(v_normal);
    vec3 l_wc = normalize(light_position_wc - v_position);

    vec3 r_wc = reflect(-l_wc, n_wc);
    vec3 v_wc = v_position;
    float ndotl = max(0.0, dot(n_wc, l_wc));
    color += (ndotl * light_diffuse * material_diffuse );

    float rdotv = max(0.0, dot(r_wc, v_wc) );
    color += (pow(rdotv, material_shininess) * light_specular * material_specular);
    FragColor = color; //vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
