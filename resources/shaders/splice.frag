#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texPos;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform vec2 tex1UV0;
uniform vec2 tex1UV1;
uniform vec2 tex2UV0;
uniform vec2 tex2UV1;


void main() {
    vec4 col1 = texture(tex1, texPos);

    vec4 col2 = vec4(0.0, 0.0, 0.0, 0.0);
    if (texPos.x >= tex1UV0.x && texPos.y >= tex1UV0.y &&
        texPos.x < tex1UV1.x && texPos.y < tex1UV1.y) {
            vec2 pos;
            pos.x = (texPos.x - tex1UV0.x) / (tex1UV1.x - tex1UV0.x);
            pos.y = (texPos.y - tex1UV0.y) / (tex1UV1.y - tex1UV0.y);

            pos.x = tex2UV0.x + pos.x * (tex2UV1.x - tex2UV0.x);
            pos.y = tex2UV0.y + pos.y * (tex2UV1.y - tex2UV0.y);

            col2 = texture(tex2, pos);
    }

    FragColor = col1*(1.0-col2.a) + col2*col2.a;
}