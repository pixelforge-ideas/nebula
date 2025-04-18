//------------------------------------------------------------------------------
//  finalize.fx
//  (C) 2013-2021 Individual contributors, See LICENSE file
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/shared.fxh"
#include "lib/techniques.fxh"
#include "lib/shared.fxh"
#include "lib/preetham.fxh"
#include "lib/mie-rayleigh.fxh"

group(BATCH_GROUP) shared constant FinalizeBlock
{
    textureHandle DepthTexture;
    textureHandle ColorTexture;
    textureHandle NormalTexture;
    textureHandle LuminanceTexture;
    textureHandle BloomTexture;
};

sampler_state UpscaleSampler
{
    AddressU = Border;
    AddressV = Border;
    BorderColor = Transparent;
};

sampler_state DefaultSampler
{
    Filter = Point;
    AddressU = Border;
    AddressV = Border;
    BorderColor = Transparent;
};

render_state FinalizeState
{
    CullMode = Back;
    DepthEnabled = false;
    DepthWrite = false;
};

// depth of field samples
#define MAXDOFSAMPLES 23

const vec2 DofSamples[MAXDOFSAMPLES] = {
        vec2( 0.0, 0.0 ),
        vec2( -0.326212, -0.40581  ),
        vec2( -0.840144, -0.07358  ),
        vec2( -0.695914,  0.457137 ),
        vec2( -0.203345,  0.620716 ),
        vec2(  0.96234,  -0.194983 ),
        vec2(  0.473434, -0.480026 ),
        vec2(  0.519456,  0.767022 ),
        vec2(  0.185461, -0.893124 ),
        vec2(  0.507431,  0.064425 ),
        vec2(  0.89642,   0.412458 ),
        vec2( -0.32194,   0.93261f ),
        vec2(  0.326212,  0.40581  ),
        vec2(  0.840144,  0.07358  ),
        vec2(  0.695914, -0.457137 ),
        vec2(  0.203345, -0.620716 ),
        vec2( -0.96234,   0.194983 ),
        vec2( -0.473434,  0.480026 ),
        vec2( -0.519456, -0.767022 ),
        vec2( -0.185461,  0.893124 ),
        vec2( -0.507431, -0.064425 ),
        vec2( -0.89642,  -0.412458 ),
        vec2(  0.32194,  -0.93261f )
};   

//------------------------------------------------------------------------------
/**
*/
shader
void
vsMain(
    [slot=0] in vec3 position,
    [slot=2] in vec2 uv,
    out vec2 UV) 
{
    gl_Position = vec4(position, 1);
    UV = uv;
}

//------------------------------------------------------------------------------
/**
    Compute fogging given a sampled fog intensity value from the depth
    pass and a fog color.
*/
float
Fog(float fogDepth)
{
    return clamp((FogDistances.y - fogDepth) / (FogDistances.y - FogDistances.x), FogColor.a, 1.0);
}

//------------------------------------------------------------------------------
/**
    Get a depth-of-field blurred sample. Set all values to 0 in order to disable DoF
*/
vec4 
DepthOfField(float depth, vec2 uv)
{
    // compute focus-blur param (0 -> no blur, 1 -> max blur)
    float focusDist = DoFDistances.x;
    float focusLength = DoFDistances.y;
    float filterRadius = DoFDistances.z;    
    float focus = saturate(abs(depth - focusDist) / focusLength);
    
    // perform a gaussian blur around uv
    vec3 sampleColor = vec3(0.0f);
    float dofWeight = 1.0f / MAXDOFSAMPLES;
    vec2 pixelSize = RenderTargetParameter[0].Dimensions.zw;
    vec2 uvMul = focus * filterRadius * pixelSize.xy;
    int i;
    for (i = 0; i < MAXDOFSAMPLES; i++)
    {
        sampleColor += sample2DLod(ColorTexture, DefaultSampler, uv + (DofSamples[i] * uvMul), 0).rgb;
    }
    sampleColor *= dofWeight;
    return vec4(sampleColor, 1);
} 

//------------------------------------------------------------------------------
/**
*/
shader
void
psMain(in vec2 UV,
    [color0] out vec4 color) 
{
    // get an averaged depth value        
    float depth = sample2DLod(DepthTexture, DefaultSampler, UV * RenderTargetParameter[0].Scale, 0).r;
    vec4 viewPos = PixelToView(UV, depth, InvProjection);
    vec3 normal = sample2DLod(NormalBuffer, DefaultSampler, UV * RenderTargetParameter[0].Scale, 0).xyz;

    vec4 worldPos = ViewToWorld(viewPos, InvView);
    vec3 viewVec = EyePos.xyz - worldPos.xyz;
    vec3 viewNormal = (View * vec4(normal, 0)).xyz;
    
    vec4 c = vec4(sample2DLod(ColorTexture, DefaultSampler, UV * RenderTargetParameter[0].Scale, 0).rgb, 1.0f);
    
    vec4 bloom = sample2DLod(BloomTexture, UpscaleSampler, UV * RenderTargetParameter[0].Scale, 0);
    //vec4 godray = subpassLoad(InputAttachment1);
    c.rgb = lerp(c.rgb, bloom.rgb, bloom.a);

    vec4 grey = vec4(dot(c.xyz, Luminance.xyz));
    c = Balance * lerp(grey, c, Saturation);
    c.rgb *= FadeValue;

    // Convert color to luminance
    float lumAvg = Time_Random_Luminance_X.z;
    vec3 xyY = RGBToXYY(c.rgb);
    float lp = (xyY.z / (9.6 * lumAvg + 0.0001f)) / max(0.01f, MaxLuminance);

    // Tonemap in luma space
    xyY = ToneMap(xyY, lp, MaxLuminance);

    // Convert back to RGB
    color = vec4(XYYToRGB(xyY), 1);
}

//------------------------------------------------------------------------------
/**
*/
PostEffect(vsMain(), psMain(), FinalizeState);
