/*
 * Terrain GLSL fragment shader.
 *
 * "data coordinate"     measured in pixels
 * "texture coordinate"  0.0 - 1.0 scale
 *
 * Y axis is flipped.
 */

// Terrain data
uniform sampler2D txtData;

// Terrain textures
uniform sampler2D txtSolid;
uniform sampler2D txtKill;
uniform sampler2D txtBouncy;
uniform sampler2D txtSlow;
uniform sampler2D txtSticky;
uniform sampler2D txtFinish;

// Size of rendering space (px)
uniform float sizeX;
uniform float sizeY;

// Size of screen (px)
uniform float screenWidth;
uniform float screenHeight;

uniform float cameraX;
uniform float cameraY;

uniform float renderEdges;
uniform vec4 edgeColour;

vec4 sample_terrain_texture(vec4 dataCoord)
{
    vec4 textureCoord = vec4(dataCoord.x / sizeX, (screenHeight - dataCoord.y) / sizeY, dataCoord.z, dataCoord.a);
    vec4 sample = texture2D(txtData, textureCoord.xy);
    return sample * 255;
}

bool edge(vec4 dataCoord)
{
    if (renderEdges == 0)
        return false;

    float rad = 3.0;
    for (float y = -rad; y < rad + 1.0; ++y) {
        for (float x = -rad; x < rad + 1.0; ++x) {
            vec4 probe = vec4(dataCoord.x + x, dataCoord.y + y, dataCoord.z, dataCoord.a);
            vec4 dataPixel = sample_terrain_texture(probe);

            if ((dataPixel.r == 0 && dataPixel.g == 128 && dataPixel.b == 128) ||
                (dataPixel.r == 0 && dataPixel.g == 0 && dataPixel.b == 0)) {
                // Probed empty space or 'kill' terrain
                return true;
            }
        }
    }

    return false;
}

void main()
{
    // Calculate texture lookup coordinates
    vec4 dataCoord;
    dataCoord.x = gl_FragCoord.x + cameraX;
    dataCoord.y = gl_FragCoord.y - cameraY;

    if (dataCoord.x >= cameraX &&
        screenHeight - dataCoord.y >= cameraY &&
        dataCoord.x < cameraX + screenWidth &&
        screenHeight - dataCoord.y < cameraY + screenHeight) {

        vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

        // Lookup pixel in terrain data texture
        vec4 dataPixel = sample_terrain_texture(dataCoord);

        // Determine the terrain type and sample the rendered pixel from the corresponding texture
        if (dataPixel.r == 0 && dataPixel.g == 128 && dataPixel.b == 128) {
            pixel = texture2D(txtKill, gl_TexCoord[0].xy);
            gl_FragColor = gl_Color * pixel;
        }
        else if (dataPixel.r == 255 && dataPixel.g == 255 && dataPixel.b == 255) {
            if (edge(dataCoord))
                pixel = edgeColour;
            else
                pixel = texture2D(txtSolid, gl_TexCoord[0].xy);
            gl_FragColor = gl_Color * pixel;
        }
        else if (dataPixel.r == 128 && dataPixel.g == 128 && dataPixel.b == 128) {
            pixel = texture2D(txtSolid, gl_TexCoord[0].xy);
            gl_FragColor = gl_Color * pixel;
        }
        else if (dataPixel.r == 67 && dataPixel.g == 191 && dataPixel.b == 6) {
            pixel = texture2D(txtBouncy, gl_TexCoord[0].xy);
            gl_FragColor = gl_Color * pixel;
        }
        else if (dataPixel.r == 210 && dataPixel.g == 176 && dataPixel.b == 60) {
            pixel = texture2D(txtSlow, gl_TexCoord[0].xy);
            gl_FragColor = gl_Color * pixel;
        }
        else if (dataPixel.r == 255 && dataPixel.g == 174 && dataPixel.b == 201) {
            pixel = texture2D(txtSticky, gl_TexCoord[0].xy);
            gl_FragColor = gl_Color * pixel;
        }
        else if (dataPixel.r == 237 && dataPixel.g == 28 && dataPixel.b == 36) {
            pixel = texture2D(txtFinish, gl_TexCoord[0].xy);
            gl_FragColor = gl_Color * pixel;
        }
        else
            gl_FragColor.a = 0;
    }
}
