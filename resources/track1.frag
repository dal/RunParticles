#version 120

uniform bool SELECTED;

varying vec2 gsTexCoord;

varying vec2 gEndpoints[2];
varying vec2 gPosition;

// Return distance from point 'p' to line segment 'a b':
float line_distance(vec2 p, vec2 a, vec2 b)
{
    float dist = distance(a,b);
    vec2 v = normalize(b-a);
    float t = dot(v,p-a);
    vec2 spinePoint;
    if (t > dist) spinePoint = b;
    else if (t > 0.0) spinePoint = a + t*v;
    else spinePoint = a;
    return distance(p,spinePoint);
}

void main(void)
{
    /*
    float d = line_distance(gPosition, gEndpoints[0], gEndpoints[1]);
    float alpha = d;
    if (d < 0)
        gl_FragColor = vec4(1, 0, 0, 1);
    else if (d < 10)
        gl_FragColor = vec4(0, 1, 0, 1);
    else if (d < 1000)
        gl_FragColor = vec4(0, 0, 1, 1);
    else if (d < 10000)
        gl_FragColor = vec4(1, 1, 0, 1);
    else if (d < 100000)
        gl_FragColor = vec4(1, 1, 1, 1);
    else
        gl_FragColor = vec4(0, 0, 0, 1);
    */
    if (!SELECTED) {
        float yVal = gsTexCoord.y;
        if (gsTexCoord.y <= 0.5)
            yVal = gsTexCoord.y / 0.5;
        else if (gsTexCoord.y > 0.5)
            yVal = (1.0 - gsTexCoord.y) / 0.5;
        yVal *= 0.5;
        gl_FragColor = gl_Color * yVal;
    } else {
        gl_FragColor = gl_Color;
    }
    //gl_FragColor.a = alpha;
    //gl_FragColor = vec4(vec3(1.0 - 12.0 * d), 1.0);
}
