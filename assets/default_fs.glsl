#version 400

in vec3 fs_position;
in vec3 fs_color;
in vec3 fs_normal;

uniform vec3 u_lightPosition;
uniform vec3 u_cameraPosition;
uniform float u_planeDistance;

void main(void) {
    vec3 lightDirection = normalize(u_lightPosition - fs_position);

    vec3 ambient = fs_color;
    vec3 diffuse = vec3(1.0f) * max(dot(fs_normal, lightDirection), 0.0f);
    vec3 specular = vec3(0.5f) * pow(max(dot(fs_normal, normalize(normalize(u_cameraPosition - fs_position) + lightDirection)), 0.0f), 1000.0f);

    float ambientCoef = 1.0f;
    float diffuseCoef = 1.0f;
    float specularCoef = 1.0f;

    vec3 color = clamp(ambientCoef * ambient + diffuseCoef * diffuse + specularCoef * specular, vec3(0.0f), vec3(1.0f));

    gl_FragColor = vec4(color, 1.0f);
    gl_FragDepth = min(length(fs_position - u_cameraPosition) / u_planeDistance, 1.0f);
}

