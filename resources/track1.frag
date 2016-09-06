#version 120

uniform bool SELECTED;
varying vec2 gsTexCoord;

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
    float yVal = 0.5;
    /*
    if (gsTexCoord.y <= 0.2)
        yVal = gsTexCoord.y / 0.2;
    else if (gsTexCoord.y > 0.8)
        yVal = (1.0 - gsTexCoord.y) / 0.2;
    */
    //if (!SELECTED)
    //    yVal *= 0.5;
    gl_FragColor = gl_Color * yVal;
}
