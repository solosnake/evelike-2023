
#ifdef ENDL
#undef ENDL
#endif
#define ENDL "\n"

// nVidia seems to not obey the "shared" request.
#define SS_UNIFORM_LAYOUT "shared"

///////////////////////////////////// MESH /////////////////////////////////////

const char mesh_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "   mat4 ViewProj4x4;                                                                 \n"
      "   mat4 Proj4x4;                                                                     \n"
      "   mat4 View4x4;                                                                     \n"
      "   mat4 ProjInv4x4;                                                                  \n"
      "   vec4 Viewport;                                                                    \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "in vec2 InTexCoord;                                                                  \n"
      "in vec3 InVertex;                                                                    \n"
      "in vec3 InTexTangent;                                                                \n"
      "in vec3 InNormal;                                                                    \n"
      "in vec4 InTransform0;                                                                \n"
      "in vec4 InTransform1;                                                                \n"
      "in vec4 InTransform2;                                                                \n"
      "in vec4 InTransform3;                                                                \n"
      "in vec4 InTeamColour;                                                                \n"
      "in vec4 InEmissive0;                                                                 \n"
      "in vec4 InEmissive1;                                                                 \n"
      "                                                                                     \n"
      "smooth out vec2 TexCoords;                                                           \n"
      "smooth out mat3 TangentToObjSpace;                                                   \n"
      "                                                                                     \n"
      "flat   out vec4 TeamColour;                                                          \n"
      "flat   out vec4 Emissive0;                                                           \n"
      "flat   out vec4 Emissive1;                                                           \n"
      "flat   out mat3 ModelViewRotation;                                                   \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    mat4 Instance4x4  = mat4( InTransform0, InTransform1,                            \n"
      "                              InTransform2, InTransform3 );                          \n"
      "    vec3 bitangent    = normalize(cross(InNormal, InTexTangent));                    \n"
      "                                                                                     \n"
      "    TexCoords         = InTexCoord;                                                  \n"
      "    TangentToObjSpace = mat3(InTexTangent, bitangent, InNormal);                     \n"
      "                                                                                     \n"
      "    TeamColour  = InTeamColour;                                                      \n"
      "    Emissive0   = InEmissive0;                                                       \n"
      "    Emissive1   = InEmissive1;                                                       \n"
      "    ModelViewRotation = mat3( View4x4 * Instance4x4 );                               \n"
      "                                                                                     \n"
      "    gl_Position = ViewProj4x4 * Instance4x4 * vec4(InVertex, 1.0);                   \n"
      "}                                                                                    \n";

const char mesh_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform sampler2D texDiffuseBGRSpecA;                                                \n"
      "uniform sampler2D texNormalsBGEmissRA;                                               \n"
      "                                                                                     \n"
      "smooth in vec2 TexCoords;                                                            \n"
      "smooth in mat3 TangentToObjSpace;                                                    \n"
      "                                                                                     \n"
      "flat   in vec4 TeamColour;                                                           \n"
      "flat   in vec4 Emissive0;                                                            \n"
      "flat   in vec4 Emissive1;                                                            \n"
      "flat   in mat3 ModelViewRotation;                                                    \n"
      "                                                                                     \n"
      "out vec4 oErgbDr;                                                                    \n"
      "out vec4 oDgbWxy;                                                                    \n"
      "out vec4 oNrNgNbSa;                                                                  \n"
      "                                                                                     \n"
      "vec3 Hue(float H)                                                                    \n"
      "{                                                                                    \n"
      "    float R = abs(H * 6.0 - 3.0) - 1.0;                                              \n"
      "    float G = 2 - abs(H * 6.0 - 2.0);                                                \n"
      "    float B = 2 - abs(H * 6.0 - 4.0);                                                \n"
      "    return clamp(vec3(R,G,B), 0.0, 1.0);                                             \n"
      "}                                                                                    \n"
      "                                                                                     \n"
      "vec3 HSVtoRGB(in vec3 HSV)                                                           \n"
      "{                                                                                    \n"
      "    return ((Hue(HSV.x) - 1.0) * HSV.y + 1.0) * HSV.z;                               \n"
      "}                                                                                    \n"
      "                                                                                     \n"
      "vec3 RGBtoHSV(in vec3 RGB)                                                           \n"
      "{                                                                                    \n"
      "    vec3 HSV = vec3(0.0);                                                            \n"
      "    HSV.z = max(RGB.r, max(RGB.g, RGB.b));                                           \n"
      "                                                                                     \n"
      "    float M = min(RGB.r, min(RGB.g, RGB.b));                                         \n"
      "    float C = HSV.z - M;                                                             \n"
      "                                                                                     \n"
      "    if (C != 0.0)                                                                    \n"
      "    {                                                                                \n"
      "        HSV.y = C / HSV.z;                                                           \n"
      "        vec3 Delta = (HSV.z - RGB) / C;                                              \n"
      "        Delta.rgb -= Delta.brg;                                                      \n"
      "        Delta.rg += vec2(2.0,4.0);                                                   \n"
      "        if (RGB.r >= HSV.z)                                                          \n"
      "            HSV.x = Delta.b;                                                         \n"
      "        else if (RGB.g >= HSV.z)                                                     \n"
      "            HSV.x = Delta.r;                                                         \n"
      "        else                                                                         \n"
      "           HSV.x = Delta.g;                                                          \n"
      "       HSV.x = fract(HSV.x / 6.0);                                                   \n"
      "   }                                                                                 \n"
      "                                                                                     \n"
      "   return HSV;                                                                       \n"
      "}                                                                                    \n"
      "                                                                                     \n"
      "vec3 DecompressNormal( in vec2 xy )                                                  \n"
      "{                                                                                    \n"
      "   xy = -1.0 + 2.0 * xy;                                                             \n"
      "   return vec3( xy, sqrt(max( 0.0, 1.0 - dot(xy,xy))) );                             \n"
      "}                                                                                    \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    vec4 InFragDiffSpec = texture( texDiffuseBGRSpecA, TexCoords );                  \n"
      "                                                                                     \n"
      "    float spec = InFragDiffSpec.a;                                                   \n"
      "                                                                                     \n"

#if 1
      "    vec3 hsv = RGBtoHSV(InFragDiffSpec.rgb);                                         \n"
      "    hsv.r = mod( hsv.r + TeamColour.r, 1.0 );                                        \n"
      "    vec3 diff = HSVtoRGB(hsv);                                                       \n"
#else
      "    vec3 diff = InFragDiffSpec.rgb;                                                  \n"
#endif

      "                                                                                     \n"
      "    vec4 NormalsBGEmissRA = texture( texNormalsBGEmissRA, TexCoords );               \n"
      "    vec2 emissIntensity = NormalsBGEmissRA.ra;                                       \n"
      "    vec2 normalsXY = NormalsBGEmissRA.gb;                                            \n"
      "    vec3 N = ModelViewRotation *                                                     \n"
      "            (TangentToObjSpace * DecompressNormal(normalsXY));                       \n"
      "                                                                                     \n"
      "    oErgbDr.rgb = (emissIntensity.x * Emissive0.rgb) +                               \n"
      "                  (emissIntensity.y * Emissive1.rgb);                                \n"
      "                                                                                     \n"
      "    oNrNgNbSa   = vec4( 0.5 + 0.5 * N, spec );                                       \n"
      "                                                                                     \n"
      "    if( DebugMode == 1u )                                                            \n"
      "    {                                                                                \n"
      "        oDgbWxy.rgb = diff;                                                          \n"
      "    }                                                                                \n"
      "    else if( DebugMode == 2u )                                                       \n"
      "    {                                                                                \n"
      "        vec3 nml  = DecompressNormal(normalsXY);                                     \n"
      "        oDgbWxy.rgb = 0.5 + 0.5 * nml;                                               \n"
      "    }                                                                                \n"
      "    else if( DebugMode == 3u )                                                       \n"
      "    {                                                                                \n"
      "        oDgbWxy.rgb = NormalsBGEmissRA.rrr;                                          \n"
      "    }                                                                                \n"
      "    else if( DebugMode == 4u )                                                       \n"
      "    {                                                                                \n"
      "        oDgbWxy.rgb = NormalsBGEmissRA.aaa;                                          \n"
      "    }                                                                                \n"
      "    else if( DebugMode == 5u )                                                       \n"
      "    {                                                                                \n"
      "        oDgbWxy.rgb = oErgbDr.rgb;                                                   \n"
      "    }                                                                                \n"
      "    else if( DebugMode == 6u )                                                       \n"
      "    {                                                                                \n"
      "        oDgbWxy.rgb = vec3(spec);                                                    \n"
      "    }                                                                                \n"
      "    else if( DebugMode == 7u )                                                       \n"
      "    {                                                                                \n"
      "        vec3 unrotatedN = TangentToObjSpace * DecompressNormal(normalsXY);           \n"
      "        vec3 rotatedN = ModelViewRotation * unrotatedN;                              \n"
      "        oDgbWxy.rgb = 0.5 + 0.5 * rotatedN;                                          \n"
      "    }                                                                                \n"
      "    else if( DebugMode == 10u )                                                      \n"
      "    {                                                                                \n"
      "        oDgbWxy.rgb = vec3(1.0);                                                     \n"
      "    }                                                                                \n"
      "    else                                                                             \n"
      "    {                                                                                \n"
      "       oNrNgNbSa = vec4( 0.5 + 0.5 * N, spec );                                      \n"
      "       oErgbDr   = vec4( emissIntensity.x * Emissive0.rgb +                          \n"
      "                        emissIntensity.y * Emissive1.rgb, diff.r );                  \n"
      "       oDgbWxy   = vec4( diff.gb, vec2(0.0) );                                       \n"
      "    }                                                                                \n"
      "}                                                                                    \n";

#if 0

    // Apply mesh-view (view x instance) rotation to normal:
    "vec3 N    = (ModelViewRotation * (TangentToObjSpace * DecompressNormal(NormalsBGEmissRA.gb)));     \n"
    "oNrNgNbSa = vec4( 0.5 + 0.5 * N, spec );                                                           \n"
    "oErgbDr   = vec4( emissIntensity.x * Emissive0.rgb + emissIntensity.y * Emissive1.rgb, diff.r );   \n"
    "oDgbWxy   = vec4( diff.gb, vec2(0.0) );                                                            \n"

#endif

/////////////////////////////////// LIGHTING ///////////////////////////////////

const char lighting_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "   mat4    ViewProj4x4;                                                              \n"
      "   mat4    Proj4x4;                                                                  \n"
      "   mat4    View4x4;                                                                  \n"
      "   mat4    ProjInv4x4;                                                               \n"
      "   vec4    Viewport;                                                                 \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform vec4 lightCentreAndRadius;                                                   \n"
      "                                                                                     \n"
      "in vec3 InVertex;                                                                    \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    if( lightCentreAndRadius.w > 0.0 )                                               \n"
      "    {                                                                                \n"
      "        vec3 v = lightCentreAndRadius.w * InVertex + lightCentreAndRadius.xyz;       \n"
      "        gl_Position = ViewProj4x4 * vec4( v, 1.0 );                                  \n"
      "    }                                                                                \n"
      "    else                                                                             \n"
      "    {                                                                                \n"
      "        gl_Position = vec4( InVertex, 1.0 );                                         \n"
      "    }                                                                                \n"
      "}                                                                                    \n";

const char lighting_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "layout(pixel_center_integer) in vec4 gl_FragCoord;                                   \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "   mat4    ViewProj4x4;                                                              \n"
      "   mat4    Proj4x4;                                                                  \n"
      "   mat4    View4x4;                                                                  \n"
      "   mat4    ProjInv4x4;                                                               \n"
      "   vec4    Viewport;                                                                 \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform sampler2D    texNormalsSpec;                                                 \n"
      "uniform sampler2D    texDepth;                                                       \n"
      "                                                                                     \n"
      "uniform vec4         lightViewCentreAndRadius;                                       \n"
      "uniform vec4         lightColourAndIsPoint;                                          \n"
      "                                                                                     \n"
      "out vec3             fragDiff;                                                       \n"
      "out vec3             fragSpec;                                                       \n"
      "                                                                                     \n"
      "vec3 viewSpaceLocation()                                                             \n"
      "{                                                                                    \n"
      "    vec2 XY  = vec2(-1.0) + (vec2(2.0) * gl_FragCoord.xy / Viewport.zw);             \n"
      "    float tZ = texelFetch( texDepth, ivec2(gl_FragCoord.xy), 0 ).r;                  \n"
      "    float Z  = (2.0 * tZ - gl_DepthRange.near - gl_DepthRange.far) /                 \n"
      "               (gl_DepthRange.far - gl_DepthRange.near);                             \n"
      "                                                                                     \n"
      "    vec4 clipPos;                                                                    \n"
      "    clipPos.w = Proj4x4[3][2] / (Z - (Proj4x4[2][2] / Proj4x4[2][3]));               \n"
      "    clipPos.xyz = vec3(XY,Z) * clipPos.w;                                            \n"
      "                                                                                     \n"
      "    vec4 fragViewPos = ProjInv4x4 * clipPos;                                         \n"
      "                                                                                     \n"
      "    return fragViewPos.xyz / fragViewPos.w;                                          \n"
      "}                                                                                    \n"
      "                                                                                     \n"
      "vec4 LightDirectionAndTypeAttenuation( in vec3 viewpos )                             \n"
      "{                                                                                    \n"
      "    vec4 L;                                                                          \n"
      "                                                                                     \n"
      "    if( lightColourAndIsPoint.w > 0.0 )                                              \n"
      "    {                                                                                \n"
      "        float D = distance( viewpos, lightViewCentreAndRadius.xyz );                 \n"
      "                                                                                     \n"
      "        L.xyz = normalize( lightViewCentreAndRadius.xyz - viewpos );                 \n"
      "        L.w = D < lightViewCentreAndRadius.w ?                                       \n"
#if 0
      // 
      "              1.0 - ( (D * D) / lightViewCentreAndRadius.w ) : 0.0;                  \n"
#else
      // Linear falloff
      "              1.0 - ( (D) / lightViewCentreAndRadius.w ) : 0.0;                      \n"
#endif
      "    }                                                                                \n"
      "    else                                                                             \n"
      "    {                                                                                \n"
      "        L = lightViewCentreAndRadius;                                                \n"
      "    }                                                                                \n"
      "                                                                                     \n"
      "    return L;                                                                        \n"
      "}                                                                                    \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    vec3 fragViewPos = viewSpaceLocation();                                          \n"
      "    vec4 L = LightDirectionAndTypeAttenuation( fragViewPos );                        \n"
      "                                                                                     \n"
      "    if(  L.w > 0.0 )                                                                 \n"
      "    {                                                                                \n"
      "        vec4 normalsSpec = texelFetch(texNormalsSpec, ivec2(gl_FragCoord.xy), 0);    \n"
      "        vec3 normal = vec3(-1.0) + vec3(2.0) * normalsSpec.rgb;                      \n"
      "        vec3 N = normalize(normal);                                                  \n"
      "        float nDotL = clamp( dot(L.xyz,N), 0.0, 1.0 );                               \n"
      "                                                                                     \n"
      "        if( DebugMode == 11u )                                                       \n"
      "        {                                                                            \n"
      "            fragDiff = vec3(1.0);                                                    \n"
      "            fragSpec = vec3(0.0);                                                    \n"
      "        }                                                                            \n"
      "        else if( nDotL > 0.0 )                                                       \n"
      "        {                                                                            \n"
      "            vec3 attenutatedLightColour = lightColourAndIsPoint.rgb * L.w;           \n"
      "            vec3 E = normalize(-fragViewPos);                                        \n"
      "            vec3 R = normalize(-reflect(L.xyz,N));                                   \n"
      "                                                                                     \n"
#if 1
      "            float shininess = 192.0;                                                 \n"
      "            float specPow   = normalsSpec.a * pow( max(dot(R,E),0.0), shininess);    \n"
#else
      "            float shininess = 192.0 * normalsSpec.a;                                 \n"
      "            float specPow   = 0.125 * pow( max(dot(R,E),0.0), shininess);            \n"
#endif
      "                                                                                     \n"
      "            if( DebugMode == 8u )                                                    \n"
      "            {                                                                        \n"
      "                fragSpec = vec3(0.0);                                                \n"
      "            }                                                                        \n"
      "            else                                                                     \n"
      "            {                                                                        \n"
      "                fragSpec = clamp(attenutatedLightColour * specPow, 0.0, 1.0);        \n"
      "            }                                                                        \n"
      "                                                                                     \n"
      "            if( DebugMode == 9u )                                                    \n"
      "            {                                                                        \n"
      "                fragDiff = vec3(0.0);                                                \n"
      "            }                                                                        \n"
      "            else                                                                     \n"
      "            {                                                                        \n"
      "                fragDiff = attenutatedLightColour * nDotL;                           \n"
      "            }                                                                        \n"
      "        }                                                                            \n"
      "        else                                                                         \n"
      "        {                                                                            \n"
      "           discard;                                                                  \n"
      "        }                                                                            \n"
      "    }                                                                                \n"
      "    else                                                                             \n"
      "    {                                                                                \n"
      "        if( DebugMode == 11u )                                                       \n"
      "        {                                                                            \n"
      "            fragDiff = vec3(0.5);                                                    \n"
      "            fragSpec = vec3(0.0);                                                    \n"
      "        }                                                                            \n"
      "        else                                                                         \n"
      "        {                                                                            \n"
      "            discard;                                                                 \n"
      "        }                                                                            \n"
      "    }                                                                                \n"
      "}                                                                                    \n";

#if 0
    "        fragSpec = clamp( attenutatedLightColour * specPow, 0.0, 1.0 );" ENDL 
    "        fragDiff = attenutatedLightColour * nDotL;" ENDL    
    " }" ENDL
    "        else {  discard; }"  // Discard because N dot L <= 0
    "}       "
    "else "
    "{  "
    "    discard; " // Discard because its outside radius.
    "}" 

#endif

///////////////////////////////// COMPOSITION //////////////////////////////////

// Fullscreen quad vertex shader:
const char composition_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "in vec3 InPosition;                                                                  \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    gl_Position = vec4( InPosition, 1.0 );                                           \n"
      "}                                                                                    \n";

// Blends the specular and diffuse source images into a single composite image.
const char composition_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform sampler2D texLightingSpec;                                                   \n"
      "uniform sampler2D texLightingDiff;                                                   \n"
      "uniform sampler2D texDgbWxy;                                                         \n"
      "uniform sampler2D texErgbDr;                                                         \n"
      "uniform vec4      ambientLight;                                                      \n"
      "                                                                                     \n"
      "out vec4          FinalColour;                                                       \n"
      "                                                                                     \n"
      "layout(pixel_center_integer) in vec4 gl_FragCoord;                                   \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    vec4 diffColourLight = texelFetch( texLightingDiff,                              \n"
      "                                       ivec2(gl_FragCoord.xy), 0 );                  \n"
      "                                                                                     \n"
      "    vec4 specColourLight = texelFetch( texLightingSpec,                              \n"
      "                                       ivec2(gl_FragCoord.xy), 0 );                  \n"
      "                                                                                     \n"
      "    vec4 diffGBwarpXY    = texelFetch( texDgbWxy,                                    \n"
      "                                       ivec2(gl_FragCoord.xy), 0 );                  \n"
      "                                                                                     \n"
      "    vec4 emissRGBdiffR   = texelFetch( texErgbDr,                                    \n"
      "                                       ivec2(gl_FragCoord.xy), 0 );                  \n"
      "                                                                                     \n"
      "    const int blursize = 1;                                                         \n"
      "                                                                                     \n"
      "    vec4 emissive = vec4(0);                                                         \n"
      "                                                                                     \n"
      "    for( int i = 0 ; i < blursize; ++i )                                             \n"
      "    {                                                                                \n"
      "        for( int j = 0; j < blursize; ++j )                                          \n"
      "        {                                                                            \n"
      "             vec2 duv = vec2(i-blursize/2,j-blursize/2);                             \n"
      "             vec2 offsetuv = (gl_FragCoord.xy + duv) /                               \n"
      "                             textureSize(texErgbDr,0);                               \n"
      "             emissive += texture( texErgbDr, offsetuv );                             \n"
      "        }                                                                            \n"
      "    }                                                                                \n"
      "                                                                                     \n"
      "    emissive /= blursize * blursize;                                                 \n"
      "                                                                                     \n"
      "    if( DebugMode == 11u )                                                           \n"
      "    {                                                                                \n"
      "        FinalColour.rgb = diffColourLight.rgb;                                       \n"
      //"        FinalColour.rgb = vec3(0,0,1);                                               \n"
      "        FinalColour.a   = 0.0;                                                       \n"
      "    }                                                                                \n"
      "    else                                                                             \n"
      "    {                                                                                \n"
      "        if( DebugMode == 10u || DebugMode == 12u  )                                  \n"
      "        {                                                                            \n"
      "             emissive = vec4(0.0);                                                   \n"
      "        }                                                                            \n"
      "                                                                                     \n"
      "        vec3 model      = vec3( emissRGBdiffR.a, diffGBwarpXY.rg );                  \n"
      "        vec3 diffuse    = model * (ambientLight.rgb + diffColourLight.rgb);          \n"
      "                                                                                     \n"
#if 0
      //"        FinalColour.rgb = specColourLight.rgb;                                     \n"
      "        FinalColour.rgb = diffColourLight.rgb;                                       \n"
      //"        FinalColour.rgb = ambientLight.rgb;                                        \n"
#else
      "        FinalColour.rgb = emissRGBdiffR.rgb + emissive.rgb +                         \n"
      "                          specColourLight.rgb + diffuse;                             \n"
#endif
      "        FinalColour.a   = 0.0;                                                       \n"
      "    }                                                                                \n"

      "}                                                                                    \n";

/////////////////////////////////// SKYBOX ////////////////////////////////////

const char skybox_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "   mat4    ViewProj4x4;                                                              \n"
      "   mat4    Proj4x4;                                                                  \n"
      "   mat4    View4x4;                                                                  \n"
      "   mat4    ProjInv4x4;                                                               \n"
      "   vec4    Viewport;                                                                 \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform mat3 SkyboxRotation3x3;                                                      \n"
      "                                                                                     \n"
      "in vec3 InPosition;                                                                  \n"
      "                                                                                     \n"
      "out vec3 TexCoords;                                                                  \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "  vec4 pos    = Proj4x4 * vec4( mat3(View4x4) * InPosition, 1.0);                    \n"
      "  gl_Position = pos.xyww;                                                            \n"
      "  TexCoords   = SkyboxRotation3x3 * InPosition;                                      \n"
      "}                                                                                    \n";

const char skybox_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform samplerCube skyboxTexture;                                                   \n"
      "                                                                                     \n"
      "in vec3 TexCoords;                                                                   \n"
      "                                                                                     \n"
      "out vec4 FinalColour;                                                                \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    FinalColour = texture(skyboxTexture, TexCoords);                                 \n"
      "}                                                                                    \n";

////////////////////////////////// SUN CORONAS ///////////////////////////////////

const char sun_coronas_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "    mat4    ViewProj4x4;                                                             \n"
      "    mat4    Proj4x4;                                                                 \n"
      "    mat4    View4x4;                                                                 \n"
      "    mat4    ProjInv4x4;                                                              \n"
      "    vec4    Viewport;                                                                \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform sampler2D EmissRGBDiffRTexture;                                              \n"
      "                                                                                     \n"
      "in  vec3 InXYZ;                                                                      \n"
      "in  vec3 InCentreXYZ;                                                                \n"
      "in  vec2 InUV;                                                                       \n"
      "                                                                                     \n"
      "flat out vec2 SunXY;                                                                 \n"
      "out vec2 UV;                                                                         \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    gl_Position = ViewProj4x4 * vec4(InXYZ, 1.0);                                    \n"
      "    vec4 centreXYZW = ViewProj4x4 * vec4(InCentreXYZ, 1.0);                          \n"
      "    vec2 centreXY = 0.5 * (vec2(1.0) + (centreXYZW.xy / centreXYZW.w));              \n"
      "                                                                                     \n"
      "    UV    = InUV;                                                                    \n"
      "    SunXY = centreXY * Viewport.zw / textureSize(EmissRGBDiffRTexture, 0);           \n"
      "}                                                                                    \n";

const char sun_coronas_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "    mat4    ViewProj4x4;                                                             \n"
      "    mat4    Proj4x4;                                                                 \n"
      "    mat4    View4x4;                                                                 \n"
      "    mat4    ProjInv4x4;                                                              \n"
      "    vec4    Viewport;                                                                \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform sampler2D EmissRGBDiffRTexture;                                              \n"
      "                                                                                     \n"
      "flat in vec2 SunXY;                                                                  \n"
      "in vec2 UV;                                                                          \n"
      "                                                                                     \n"
      "out vec4 oLightSpecRGBX;                                                             \n"
      "                                                                                     \n"
      "const int NUM_SAMPLES = 100;                                                         \n"
      "const float density   = 1.1;                                                         \n"
      "const float decay     = 0.925;                                                       \n"
      "const float exposure  = 0.95;                                                        \n"
      "const float weight    = 1.0;                                                         \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "   vec2 fragXY = gl_FragCoord.xy / textureSize(EmissRGBDiffRTexture, 0);             \n"
      "                                                                                     \n"
      "   vec2 deltaTextCoord = (fragXY - SunXY) /  float(NUM_SAMPLES) * density;           \n"
      "                                                                                     \n"
      "   vec2 textCoord  = fragXY;                                                         \n"
      "                                                                                     \n"
      "   float illuminationDecay = 1.0;                                                    \n"
      "                                                                                     \n"
      "   vec4 fragColor = vec4(0);                                                         \n"
      "                                                                                     \n"
      "   for(int i=0; i < NUM_SAMPLES ; i++)                                               \n"
      "   {                                                                                 \n"
      "       textCoord         -= deltaTextCoord;                                          \n"
      "       vec4 sample        = texture(EmissRGBDiffRTexture, textCoord);                \n"
      "       sample            *= illuminationDecay * weight;                              \n"
      "       illuminationDecay *= decay;                                                   \n"
      "       fragColor         += sample;                                                  \n"
      "   }                                                                                 \n"
      "                                                                                     \n"
      "   float falloff  = 1.0 - length( UV * 2.0 - vec2(1.0) );                            \n"
      "   oLightSpecRGBX = exposure * falloff * fragColor;                                  \n"
      "}                                                                                    \n";

////////////////////////////////// SUN SPHERES ///////////////////////////////////

const char sun_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "   mat4    ViewProj4x4;                                                              \n"
      "   mat4    Proj4x4;                                                                  \n"
      "   mat4    View4x4;                                                                  \n"
      "   mat4    ProjInv4x4;                                                               \n"
      "   vec4    Viewport;                                                                 \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "in  vec4 XYZAngle;                                                                   \n"
      "in  vec4 UVNoiseMoveAB;                                                              \n"
      "                                                                                     \n"
      "flat out mat3  Rotation3x3;                                                          \n"
      "flat out vec2  NoiseMove;                                                            \n"
      "                                                                                     \n"
      "out vec2 UV;                                                                         \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    gl_Position = ViewProj4x4 * vec4(XYZAngle.xyz, 1.0);                             \n"
      "    UV = UVNoiseMoveAB.xy;                                                           \n"
      "    NoiseMove = UVNoiseMoveAB.zw;                                                    \n"
      "    float s = sin(XYZAngle[3]);                                                      \n"
      "    float c = cos(XYZAngle[3]);                                                      \n"
      "    Rotation3x3[0][0] = c;                                                           \n"
      "    Rotation3x3[0][1] = 0;                                                           \n"
      "    Rotation3x3[0][2] = s;                                                           \n"
      "    Rotation3x3[1][0] = 0;                                                           \n"
      "    Rotation3x3[1][1] = 1;                                                           \n"
      "    Rotation3x3[1][2] = 0;                                                           \n"
      "    Rotation3x3[2][0] = -s;                                                          \n"
      "    Rotation3x3[2][1] = 0;                                                           \n"
      "    Rotation3x3[2][2] = c;                                                           \n"
      "}                                                                                    \n";

const char sun_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "    mat4    ViewProj4x4;                                                             \n"
      "    mat4    Proj4x4;                                                                 \n"
      "    mat4    View4x4;                                                                 \n"
      "    mat4    ProjInv4x4;                                                              \n"
      "    vec4    Viewport;                                                                \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform sampler3D NoiseTexture;                                                      \n"
      "uniform sampler2D SunRampTexture;                                                    \n"
      "                                                                                     \n"
      "flat in mat3 Rotation3x3;                                                            \n"
      "flat in vec2 NoiseMove;                                                              \n"
      "in vec2 UV;                                                                          \n"
      "                                                                                     \n"
      "out vec4 oLightSpecRGBX;                                                             \n"
      "out vec4 oLightDiffRGBX;                                                             \n"
      "out vec4 oEmissRGBDiffR;                                                             \n"
      "out vec4 oDiffGBWarpXY;                                                              \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    vec2 p = UV * 2.0 - vec2(1.0);                                                   \n"
      "    float d = length(p);                                                             \n"
      "                                                                                     \n"
      "    if(d > 1.0) discard;                                                             \n"
      "                                                                                     \n"
      "    float z = sqrt( 1.0 - d * d );                                                   \n"
      "    vec4 uvwx = vec4( p, z, 0 ) * View4x4;                                           \n"
      "                                                                                     \n"
      "    vec3 noiseuvw = vec3(NoiseMove,0) + Rotation3x3 * uvwx.xyz;                      \n"
      "    vec4 perlin = texture(NoiseTexture, noiseuvw );                                  \n"
      "                                                                                     \n"
      "    float n = abs(perlin[0] - 0.25)   +                                              \n"
      "              abs(perlin[1] - 0.125)  +                                              \n"
      "              abs(perlin[2] - 0.0625) +                                              \n"
      "              abs(perlin[3] - 0.03125);                                              \n"
      "                                                                                     \n"
      "    float intensity = clamp(n * 8.5, 0.0, 1.0);                                      \n"
      "    vec4 ramp = texture(SunRampTexture, vec2(intensity,0.0));                        \n"
      "                                                                                     \n"
      "    const float cube_diagonal = sqrt(3.0);                                           \n"
      "    float brightness = length(ramp.rgb) / cube_diagonal;                             \n"
      "                                                                                     \n"
      "    oEmissRGBDiffR = vec4( pow(brightness, 3.75) * ramp.rgb, 0 );                    \n"
      "    oLightSpecRGBX = vec4(0);                                                        \n"
      "    oLightDiffRGBX = vec4(0);                                                        \n"
      "    oDiffGBWarpXY  = vec4(0);                                                        \n"
      "}                                                                                    \n";

///////////////////////////////////// SFX /////////////////////////////////////

const char sfx_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "   mat4    ViewProj4x4;                                                              \n"
      "   mat4    Proj4x4;                                                                  \n"
      "   mat4    View4x4;                                                                  \n"
      "   mat4    ProjInv4x4;                                                               \n"
      "   vec4    Viewport;                                                                 \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "in vec3 InVertex;                                                                    \n"
      "in vec2 InTexCoord;                                                                  \n"
      "                                                                                     \n"
      "smooth out vec3 TexCoordsZ;                                                          \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    vec4 pos    = View4x4 * vec4(InVertex, 1.0);                                     \n"
      "    TexCoordsZ  = vec3( InTexCoord, pos.z );                                         \n"
      "    gl_Position = ViewProj4x4 * vec4(InVertex, 1.0);                                 \n"
      "}                                                                                    \n";

const char sfx_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "layout(shared) uniform Globals {                                                     \n"
      "   mat4    ViewProj4x4;                                                              \n"
      "   mat4    Proj4x4;                                                                  \n"
      "   mat4    View4x4;                                                                  \n"
      "   mat4    ProjInv4x4;                                                               \n"
      "   vec4    Viewport;                                                                 \n"
      "};                                                                                   \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform vec4 sfxRGBA;                                                                \n"
      "                                                                                     \n"
      "uniform sampler2D texT1;                                                             \n"
      "uniform sampler2D texT2;                                                             \n"
      "uniform sampler2D texDepth;                                                          \n"
      "                                                                                     \n"
      "smooth in vec3 TexCoordsZ;                                                           \n"
      "                                                                                     \n"
      "out vec4 oSceneRGBA;                                                                 \n"
      "out vec4 oDgbWxy;                                                                    \n"
      "                                                                                     \n"
      "vec3 viewSpaceLocation()                                                             \n"
      "{                                                                                    \n"
      "   vec2 XY  = vec2(-1.0) + (vec2(2.0) * gl_FragCoord.xy / Viewport.zw);              \n"
      "                                                                                     \n"
      "   float tZ = texelFetch( texDepth, ivec2(gl_FragCoord.xy), 0 ).r;                   \n"
      "   float Z  = (2.0 * tZ - gl_DepthRange.near - gl_DepthRange.far) /                  \n"
      "              (gl_DepthRange.far - gl_DepthRange.near);                              \n"
      "                                                                                     \n"
      "   vec4 clipPos;                                                                     \n"
      "   clipPos.w = Proj4x4[3][2] / (Z - (Proj4x4[2][2] / Proj4x4[2][3]));                \n"
      "   clipPos.xyz = vec3(XY,Z) * clipPos.w;                                             \n"
      "                                                                                     \n"
      "   vec4 fragViewPos = ProjInv4x4 * clipPos;                                          \n"
      "                                                                                     \n"
      "   return fragViewPos.xyz / fragViewPos.w;                                           \n"
      "}                                                                                    \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    float Z = viewSpaceLocation().z;                                                 \n"
      "    float softness = clamp( (TexCoordsZ.z - Z) * 10.0, 0.0, 1.0 );                   \n"
      "                                                                                     \n"
      "    vec4 diffRGBA = texture(texT1, TexCoordsZ.xy);                                   \n"
      "    float alpha   = softness * sfxRGBA.a * diffRGBA.a;                               \n"
      "                                                                                     \n"
      "    vec2 warped = softness * sfxRGBA.a * texture(texT2, TexCoordsZ.xy).ba;           \n"
      "    oSceneRGBA  = vec4( alpha * sfxRGBA.rgb * diffRGBA.rgb, 0 );                     \n"
      "    oDgbWxy     = vec4( 0, 0, warped );                                              \n"
      "}                                                                                    \n";

///////////////////////////////// POSTEFFECT //////////////////////////////////

// Fullscreen quad vertex shader:
const char posteffect_vert[]
    = "#version 150                                                                         \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "in vec3 InPosition;                                                                  \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    gl_Position = vec4( InPosition, 1.0 );                                           \n"
      "}                                                                                    \n";

// Warps or colorizes etc the composed image.
const char posteffect_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform sampler2D texScene;                                                          \n"
      "uniform sampler2D texWarpBA;                                                         \n"
      "                                                                                     \n"
      "out vec4 FinalColour;                                                                \n"
      "                                                                                     \n"
      "in vec4  gl_FragCoord;                                                               \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    vec2 uv     = gl_FragCoord.xy / textureSize(texScene, 0);                        \n"
      "    vec2 warp   = 0.1 * texture( texWarpBA, uv ).ba;                                 \n"
      "    FinalColour = texture( texScene, warp + uv );                                    \n"
      "}                                                                                    \n";

///////////////////////////////// SCREEN QUADS /////////////////////////////////

const char screen_quads_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "in vec4 InData;                                                                      \n"
      "in vec4 InCornerColour;                                                              \n"
      "                                                                                     \n"
      "smooth out vec4 QuadColour;                                                          \n"
      "smooth out vec2 TexCoords;                                                           \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    gl_Position = vec4( InData.xy, 0.5, 1.0 );                                       \n"
      "    TexCoords   = InData.zw;                                                         \n"
      "    QuadColour  = InCornerColour;                                                    \n"
      "}                                                                                    \n";

const char screen_quads_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform sampler2D screenQuadsPage;                                                   \n"
      "                                                                                     \n"
      "smooth in vec2 TexCoords;                                                            \n"
      "smooth in vec4 QuadColour;                                                           \n"
      "                                                                                     \n"
      "out vec4 FinalColour;                                                                \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    FinalColour = QuadColour * texture( screenQuadsPage, TexCoords );                \n"
      "}                                                                                    \n";

////////////////////////////////// DYNBUFFERS //////////////////////////////////

const char dynbuffer_vert[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform mat4 transform;                                                              \n"
      "                                                                                     \n"
      "in vec4 InColor;                                                                     \n"
      "in vec3 InVertex;                                                                    \n"
      "in vec2 InTexCoord;                                                                  \n"
      "                                                                                     \n"
      "smooth out vec2 TexCoords;                                                           \n"
      "smooth out vec4 Color;                                                               \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    gl_Position = transform * vec4( InVertex.xyz, 1.0 );                             \n"
      "    TexCoords   = InTexCoord;                                                        \n"
      "    Color       = InColor;                                                           \n"
      "}                                                                                    \n";

const char dynbuffer_frag[]
    = "#version 150                                                                         \n"
      "                                                                                     \n"
      "uniform uint DebugMode;                                                              \n"
      "                                                                                     \n"
      "uniform sampler2D texDiffuse;                                                        \n"
      "uniform vec4      blendColour;                                                       \n"
      "                                                                                     \n"
      "smooth in vec2    TexCoords;                                                         \n"
      "smooth in vec4    Color;                                                             \n"
      "                                                                                     \n"
      "out vec4 FinalColour;                                                                \n"
      "                                                                                     \n"
      "void main()                                                                          \n"
      "{                                                                                    \n"
      "    FinalColour = blendColour * Color * texture( texDiffuse, TexCoords );            \n"
      "}                                                                                    \n";

//-------------------------------------------------------------------------

std::string deferred_renderer::internalrenderer::mesh_shader_frag() const
{
    return mesh_frag;
}

std::string deferred_renderer::internalrenderer::mesh_shader_vert() const
{
    return mesh_vert;
}

std::string deferred_renderer::internalrenderer::lighting_shader_frag() const
{
    return lighting_frag;
}

std::string deferred_renderer::internalrenderer::lighting_shader_vert() const
{
    return lighting_vert;
}

std::string deferred_renderer::internalrenderer::skybox_shader_frag() const
{
    return skybox_frag;
}

std::string deferred_renderer::internalrenderer::skybox_shader_vert() const
{
    return skybox_vert;
}

std::string deferred_renderer::internalrenderer::dynbuffer_shader_frag() const
{
    return dynbuffer_frag;
}

std::string deferred_renderer::internalrenderer::dynbuffer_shader_vert() const
{
    return dynbuffer_vert;
}

std::string deferred_renderer::internalrenderer::suns_spheres_shader_frag() const
{
    return sun_frag;
}

std::string deferred_renderer::internalrenderer::suns_spheres_shader_vert() const
{
    return sun_vert;
}

std::string deferred_renderer::internalrenderer::suns_coronas_shader_frag() const
{
    return sun_coronas_frag;
}

std::string deferred_renderer::internalrenderer::suns_coronas_shader_vert() const
{
    return sun_coronas_vert;
}

std::string deferred_renderer::internalrenderer::sfx_shader_frag() const
{
    return sfx_frag;
}

std::string deferred_renderer::internalrenderer::sfx_shader_vert() const
{
    return sfx_vert;
}

std::string deferred_renderer::internalrenderer::composition_shader_frag() const
{
    return composition_frag;
}

std::string deferred_renderer::internalrenderer::composition_shader_vert() const
{
    return composition_vert;
}

std::string deferred_renderer::internalrenderer::posteffect_shader_frag() const
{
    return posteffect_frag;
}

std::string deferred_renderer::internalrenderer::posteffect_shader_vert() const
{
    return posteffect_vert;
}

std::string deferred_renderer::internalrenderer::screen_quads_shader_frag() const
{
    return screen_quads_frag;
}

std::string deferred_renderer::internalrenderer::screen_quads_shader_vert() const
{
    return screen_quads_vert;
}
