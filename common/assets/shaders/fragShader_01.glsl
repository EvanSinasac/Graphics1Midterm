// Fragment shader
#version 420

in vec4 fVertexColour;			// The vertex colour from the original model
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

// Replaces gl_FragColor
out vec4 pixelColour;			// RGB A   (0 to 1) 

// The "whole object" colour (diffuse and specular)
uniform vec4 wholeObjectDiffuseColour;	// Whole object diffuse colour
uniform bool bUseWholeObjectDiffuseColour;	// If true, the whole object colour is used (instead of vertex colour)
uniform vec4 wholeObjectSpecularColour;	// Colour of the specular highlight (optional)

// Alpha Transparency value
uniform float wholeObjectAlphaTransparency;

// This is used for wireframe or whole object colour. 
// If bUseDebugColour is TRUE, then the fragment colour is "objectDebugColour".
uniform bool bUseDebugColour;	
uniform vec4 objectDebugColour;		

// This will not modulate the colour by the lighting contribution.
// i.e. shows object colour "as is". 
// Used for wireframe or debug type objects
uniform bool bDontLightObject;			// 1 if you want to AVOID lighting


// This is the camera eye location (update every frame)
uniform vec4 eyeLocation;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;


const int NUMBEROFLIGHTS = 11;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 80 uniforms
// 
// uniform vec4 theLights[0].position;
// uniform vec4 theLights[0].diffuse;
// ...
// uniform vec4 theLights[1].position;
// uniform vec4 theLights[1].diffuse;
// ...

vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );

uniform sampler2D texture_00;		//GL_TEXTURE_2D
uniform sampler2D texture_01;		//GL_TEXTURE_2D
uniform sampler2D texture_02;		//GL_TEXTURE_2D
uniform sampler2D texture_03;		//GL_TEXTURE_2D

uniform sampler2D texture_04;		//GL_TEXTURE_2D
uniform sampler2D texture_05;		//GL_TEXTURE_2D
uniform sampler2D texture_06;		//GL_TEXTURE_2D
uniform sampler2D texture_07;		//GL_TEXTURE_2D

uniform vec4 texture2DRatios0to3;
uniform vec4 texture2DRatios4to7;

// Cube maps for skybox, etc.
uniform samplerCube cubeMap_00;
uniform samplerCube cubeMap_01;
uniform samplerCube cubeMap_02;
uniform samplerCube cubeMap_03;
uniform vec4 cubeMap_Ratios0to3;

// If true, then we only sample from the cube maps
uniform bool bIsSkyBox;

// If this is true, then we will sample the "discardTexture" to
// perform a discard on that pixel
// (we could also do a change in the transparency, or something)
uniform sampler2D discardTexture;
uniform bool bDiscardTransparencyWindowsOn;


// This is used for a discard example 
// Everything inside the "black hole" isn't drawn
uniform float blackHoleSize;

// Skybox or reflection or light probe
//uniform samplerCube skyBox;			// GL_TEXTURE_CUBE_MAP

void main()
{
	// This is the pixel colour on the screen.
	// Just ONE pixel, though.


	if ( bIsSkyBox )
	{
		//pixelColour.a = 1.0f;
		// For some reason if the cube map isn't actually set to
		// a texture unit it returns black when combined with other cubemaps
//		pixelColour.rgb = 
//		( texture( cubeMap_00, fNormal.xyz ).rgb * cubeMap_Ratios0to3.x ) +
//		( texture( cubeMap_01, fNormal.xyz ).rgb * cubeMap_Ratios0to3.y ) +
//		( texture( cubeMap_02, fNormal.xyz ).rgb * cubeMap_Ratios0to3.z ) +
//		( texture( cubeMap_03, fNormal.xyz ).rgb * cubeMap_Ratios0to3.w );
		
		pixelColour.rgba = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if (cubeMap_Ratios0to3.x > 0.0f)
		{
			pixelColour.rgb += ( texture( cubeMap_00, fNormal.xyz ).rgb * cubeMap_Ratios0to3.x );
		}
		if (cubeMap_Ratios0to3.y > 0.0f)
		{
			pixelColour.rgb += ( texture( cubeMap_01, fNormal.xyz ).rgb * cubeMap_Ratios0to3.y );
		}
//		if (cubeMap_Ratios0to3.z > 0.0f)
//		{
//			pixelColour.rgb += ( texture( cubeMap_02, fNormal.xyz ).rgb * cubeMap_Ratios0to3.z );
//		}
//		if (cubeMap_Ratios0to3.w > 0.0f)
//		{
//			pixelColour.rgb += ( texture( cubeMap_03, fNormal.xyz ).rgb * cubeMap_Ratios0to3.w );
//		}

		return;
	}

	// Black hole discard example
//	vec3 blackHoleLocationXYZ = vec3(0.0f, 0.0f, 0.0f);
//	
//	if ( distance(fVertWorldLocation.xyz, blackHoleLocationXYZ) < blackHoleSize )
//	{
//		discard;
//	}

	if (bDiscardTransparencyWindowsOn)
	{
		// Note I'm only sampling from red because I just want
		// to see if ti's "black-ish" coloured...
		vec3 vec3DisSample = texture ( discardTexture, fUVx2.xy ).rgb;
		float discardSample = ( vec3DisSample.r + vec3DisSample.g + vec3DisSample.b )/3.0f;
		//
		if ( discardSample < 0.1f )
		{	// "black enough"
			// DON'T even draw the pixel here
			// The fragment shader simply stops here
			discard;
		}
	}
	
	// Copy model vertex colours?
	vec4 vertexDiffuseColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);//fVertexColour;

	
	// Use model vertex colours or not?
	if ( bUseWholeObjectDiffuseColour )
	{
		vertexDiffuseColour = wholeObjectDiffuseColour;
	}
	
	// Use debug colour?
	if ( bUseDebugColour )
	{
		// Overwrite the vertex colour with this debug colour
		vertexDiffuseColour = objectDebugColour;	
	}

	// I moved it to before the bDontLight so that textures should be set
	// Use the texture values as the "diffuse" colour

	// Makes this "black" but not quite...
	vertexDiffuseColour.rgb *= 0.0001f;

	vertexDiffuseColour.rgb =	texture ( texture_00, fUVx2.xy ).rgb * texture2DRatios0to3.x + 
								texture ( texture_01, fUVx2.xy ).rgb * texture2DRatios0to3.y + 
								texture ( texture_02, fUVx2.xy ).rgb * texture2DRatios0to3.z + 
								texture ( texture_03, fUVx2.xy ).rgb * texture2DRatios0to3.w;
								// + etc... the other 4 texture units
	
	// Used for drawing "debug" objects (usually wireframe)
	if ( bDontLightObject )
	{
		pixelColour = vertexDiffuseColour;
		// Early exit from shader
		return;
	}

	
	
	
	vec4 outColour = calcualteLightContrib( vertexDiffuseColour.rgb,		
	                                        fNormal.xyz, 		// Normal at the vertex (in world coords)
                                            fVertWorldLocation.xyz,	// Vertex WORLD position
											wholeObjectSpecularColour.rgba );
											
	pixelColour = outColour;
	
	// Set the alpha value
	// 0.0 clear
	// 1.0 solid
	//pixelColour.a = 1.0f;
	pixelColour.a = wholeObjectAlphaTransparency;
};



// Calculates the colour of the vertex based on the lighting and vertex information:
vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot( -theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
		
			lightContrib *= dotProduct;		
			
//			finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
			finalObjectColour.rgb += (vertexMaterialColour.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)
			
			return finalObjectColour;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object???s.
		float objectSpecularPower = vertexSpecular.w; 
		
//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
//			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * theLights[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

	}//for(intindex=0...
	
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}
