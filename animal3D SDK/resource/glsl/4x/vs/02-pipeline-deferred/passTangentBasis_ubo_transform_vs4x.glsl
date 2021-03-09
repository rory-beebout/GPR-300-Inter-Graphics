/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein

	///////Modified by Rory Beebout///////
	
	passTangentBasis_ubo_transform_vs4x.glsl
	Calculate and pass tangent basis using uniform buffers.
*/

#version 450

#define MAX_OBJECTS 128

// ****TO-DO:
//	-> declare attributes related to lighting
//		(hint: normal [2], texcoord [8], tangent [10], bitangent [11])
//	-> declare view-space varyings related to lighting
//		(hint: one per attribute)
//	-> calculate final clip-space position and view-space varyings
//		(hint: complete tangent basis [TBNP] transformed to view-space)
//		(hint: texcoord transformed to atlas coordinates in a similar fashion)

struct sModelMatrixStack
{
	mat4 modelMat;						// model matrix (object -> world)
	mat4 modelMatInverse;				// model inverse matrix (world -> object)
	mat4 modelMatInverseTranspose;		// model inverse-transpose matrix (object -> world skewed)
	mat4 modelViewMat;					// model-view matrix (object -> viewer)
	mat4 modelViewMatInverse;			// model-view inverse matrix (viewer -> object)
	mat4 modelViewMatInverseTranspose;	// model-view inverse transpose matrix (object -> viewer skewed)
	mat4 modelViewProjectionMat;		// model-view-projection matrix (object -> clip)
	mat4 atlasMat;						// atlas matrix (texture -> cell)
};
uniform ubTransformStack
{
	sModelMatrixStack uModelMatrixStack[MAX_OBJECTS];
};

layout (location = 0) in vec4 aPosition;
layout (location = 2) in vec3 aNormal;
layout (location = 8) in vec4 aTexcoord;

layout (location = 10) in vec3 aTangent;
layout (location = 11) in vec4 aBiTangent;

uniform int uIndex;

flat out int vVertexID;
flat out int vInstanceID;

//view-space varyings
out vec4 vPosition;
out vec4 vNormal;
out vec4 vTexcoord;
out vec3 vTangent;
out vec4 vBiTangent;

out vec4 vPosition_screen; //pos for screen-space -> G-buffer

out mat3 vTBN;
out mat4 vTBNP;

const mat4 bias = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
	);

void main()
{
	gl_Position = uModelMatrixStack[uIndex].modelViewProjectionMat * aPosition;

	vPosition_screen = bias * gl_Position; //bias projection

	vPosition = uModelMatrixStack[uIndex].modelViewMat * aPosition;
	vNormal = uModelMatrixStack[uIndex].modelViewMatInverseTranspose * vec4(aNormal, 0.0); //w should be zero since no scaling.
	vTexcoord = uModelMatrixStack[uIndex].atlasMat * aTexcoord; //From whole atlas to cell space

	vTangent = normalize(vec3(uModelMatrixStack[uIndex].modelViewMatInverseTranspose * vec4(aTangent, 0.0)));
	vBiTangent = normalize(uModelMatrixStack[uIndex].modelViewMatInverseTranspose * aBiTangent);

	//Calculate tangent, bitangent, normal matrix for converting tangent normal to view space in fragment shader
	vTBN = mat3(normalize(vTangent), 
				normalize(vec3(vBiTangent)), 
				normalize(vNormal));

//	vTBN = mat3(normalize(vec3(uModelMatrixStack[uIndex].modelViewMatInverseTranspose * vec4(aTangent, 0.0))), 
//				normalize(vec3(uModelMatrixStack[uIndex].modelViewMatInverseTranspose * aBiTangent)), 
//				normalize(vec3(vNormal)));


	vVertexID = gl_VertexID;
	vInstanceID = gl_InstanceID;
}
