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

	a3_DemoMode5_Final-load.c
	Demo mode implementations: animation scene.

	********************************************
	*** LOADING FOR ANIMATION SCENE MODE     ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode5_Final.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------

inline int a3final_initKeyframeController(a3_Final_KeyframeController* keyCtrl,
	a3ui32 const keyCount, a3ui32 const keyIndex, a3f32 const keyDuration)
{
	if (keyCtrl && keyCount && keyDuration > 0.0f)
	{
		keyCtrl->count = keyCount;
		keyCtrl->index = keyIndex % keyCount;
		keyCtrl->duration = keyDuration;
		keyCtrl->durationInv = 1.0f / keyDuration;
		keyCtrl->time = keyCtrl->param = 0.0f;

		// done
		return 1;
	}
	return -1;
}

inline int a3spatialPoseSetRotation(a3_SceneObjectData* spatialPose, a3f32 const x, a3f32 const y, a3f32 const z)
{
	if (spatialPose)
	{
		spatialPose->euler.x = x;
		spatialPose->euler.y = y;
		spatialPose->euler.z = z;
		return +1;
	}
	return -1;
}

inline int a3spatialPoseSetTranslation(a3_SceneObjectData* spatialPose, a3f32 const x, a3f32 const y, a3f32 const z)
{
	if (spatialPose)
	{
		spatialPose->position.x = x;
		spatialPose->position.y = y;
		spatialPose->position.z = z;
		return +1;
	}
	return -1;
}

inline int a3spatialPoseSetScale(a3_SceneObjectData* spatialPose, a3f32 const x, a3f32 const y, a3f32 const z)
{
	if (spatialPose)
	{
		spatialPose->scale.x = x;
		spatialPose->scale.y = y;
		spatialPose->scale.z = z;
		return +1;
	}
	return -1;
}

inline int a3spatialPoseReset(a3_SceneObjectData* spatialPose)
{
	if (spatialPose)
	{
		spatialPose->euler.x = 0.0f;
		spatialPose->euler.y = 0.0f;
		spatialPose->euler.z = 0.0f;
		spatialPose->position.x = 0.0f;
		spatialPose->position.y = 0.0f;
		spatialPose->position.z = 0.0f;
		spatialPose->scale.x = 1.0f;
		spatialPose->scale.y = 1.0f;
		spatialPose->scale.z = 1.0f;
		return +1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

void a3final_loadValidate(a3_DemoState* demoState, a3_DemoMode5_Final* demoMode)
{
	void a3final_input(a3_DemoState* demoState, a3_DemoMode5_Final* demoMode, a3f64 const dt);
	void a3final_update(a3_DemoState* demoState, a3_DemoMode5_Final* demoMode, a3f64 const dt);
	void a3final_render(a3_DemoState const* demoState, a3_DemoMode5_Final const* demoMode, a3f64 const dt);
	void a3final_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode5_Final* demoMode, a3i32 const asciiKey, a3i32 const state);
	void a3final_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode5_Final* demoMode, a3i32 const asciiKey, a3i32 const state);

	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	}, biasInv = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 1.0f
	};

	// initialize callbacks
	a3_DemoModeCallbacks* const callbacks = demoState->demoModeCallbacks + demoState_modeFinal;
	callbacks->demoMode =		demoMode;
	callbacks->handleInput =	(a3_DemoMode_EventCallback)a3final_input;
	callbacks->handleUpdate =	(a3_DemoMode_EventCallback)a3final_update;
	callbacks->handleRender =	(a3_DemoMode_EventCallback)a3final_render;
	callbacks->handleKeyPress = (a3_DemoMode_InputCallback)a3final_input_keyCharPress;
	callbacks->handleKeyHold =	(a3_DemoMode_InputCallback)a3final_input_keyCharHold;

	// initialize scene object pointers
	a3demo_initSceneObject(demoMode->objgroup_world_root, 0,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_light_main, 1,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_camera_main, 2,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_skybox, 3,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_skeleton, 4,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_teapot, 5,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_torus, 6,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_sphere, 7,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_ground, 8,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);


	// initialize projector pointers
	a3demo_initProjector(demoMode->proj_camera_main,
		demoMode->obj_camera_main->sceneHierarchyIndex, 0,
		demoMode->projectorData, demoMode->projectorMatrixStack, demoMode->sceneObject);

	// initialize point light pointers
	a3demo_initPointLight(demoMode->light_point_main,
		demoMode->obj_light_main->sceneHierarchyIndex, 0,
		demoMode->pointLightData, demoMode->sceneObject);

	// initialize cameras dependent on viewport
	demoMode->proj_camera_main->dataPtr->aspect = demoState->frameAspect;
	a3demo_updateProjector(demoMode->proj_camera_main);
	a3demo_updateProjectorViewMats(demoMode->proj_camera_main);
	a3demo_updateProjectorBiasMats(demoMode->proj_camera_main, bias, biasInv);

	// initialize cameras not dependent on viewport

}


void a3final_load(a3_DemoState const* demoState, a3_DemoMode5_Final* demoMode)
{
	a3_SceneObjectComponent const* sceneObject = 0, *sceneObject_parent = 0;
	a3_SceneObjectData* sceneObjectData = 0;

	a3_ProjectorComponent const* projector = 0;
	a3_ProjectorData* projectorData = 0;

	a3_PointLightComponent const* pointLight = 0;
	a3_PointLightData* pointLightData = 0, * pointLightDataEnd = 0;

	a3ui32 i;

	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	const a3real sceneCameraAxisPos = 15.0f;
	const a3vec3 sceneCameraStartPos = {
		+sceneCameraAxisPos,
		-sceneCameraAxisPos,
		+sceneCameraAxisPos + 5.0f,
	};
	const a3vec3 sceneCameraStartEuler = {
		+55.0f,
		+0.0f,
		+45.0f,
	};
	const a3f32 sceneObjectDistance = 8.0f;
	const a3f32 sceneObjectHeight = 2.0f;


	// set up scene hierarchy and scene objects
	a3hierarchyCreate(demoMode->hierarchy_scene, finalMaxCount_sceneObject, 0);

	sceneObject = sceneObject_parent = demoMode->objgroup_world_root;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, -1, "objgroup_world_root");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);

	sceneObject = demoMode->obj_light_main;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_light_main");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.xyz = sceneCameraStartPos;
	sceneObjectData->euler.xyz = sceneCameraStartEuler;

	sceneObject = demoMode->obj_camera_main;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_camera_main");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.xyz = sceneCameraStartPos;
	sceneObjectData->euler.xyz = sceneCameraStartEuler;

	sceneObject = demoMode->obj_skybox;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_skybox");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->scale.x = a3real_oneeighty;
	sceneObjectData->scaleMode = a3scale_uniform;

	sceneObject = demoMode->obj_skeleton;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_skeleton");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = +12.0f;
	sceneObjectData->position.y = +12.0f;

	sceneObject = demoMode->obj_torus;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_torus");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = a3real_zero;
	sceneObjectData->position.y = -a3real_six;
	sceneObjectData->position.z = +a3real_four;
	sceneObjectData->scale.x = a3real_two;
	sceneObjectData->scaleMode = a3scale_uniform;
	//sceneObject->modelMatrixStackPtr->atlasMat = demoState->atlas_marble;

	sceneObject = demoMode->obj_teapot;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_teapot");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = a3real_zero;
	sceneObjectData->position.y = -a3real_six;
	sceneObjectData->position.z = -a3real_two;
	sceneObjectData->scale.x = a3real_two;
	sceneObjectData->scaleMode = a3scale_uniform;

	sceneObject = demoMode->obj_sphere;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_sphere");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = a3real_zero;
	sceneObjectData->position.y = a3real_six;
	sceneObjectData->position.z = -a3real_two;
	sceneObjectData->scale.x = a3real_two;
	sceneObjectData->scaleMode = a3scale_uniform;

	sceneObject = demoMode->obj_ground;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_ground");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.z = -a3real_two;
	sceneObjectData->scale.x = a3real_fortyfive;
	sceneObjectData->scale.y = a3real_fortyfive;
	sceneObjectData->scale.z = a3real_epsilon;
	sceneObjectData->scaleMode = a3scale_nonuniform;


	// set up projectors
	projector = demoMode->proj_camera_main;
	projectorData = projector->dataPtr;
	projectorData->fovy = a3real_fortyfive;
	projectorData->znear = a3real_one;
	projectorData->zfar = a3real_threesixty;
	projectorData->ctrlMoveSpeed = a3real_one;
	projectorData->ctrlRotateSpeed = a3real_two;
	projectorData->ctrlZoomSpeed = a3real_fortyfive;
	projectorData->perspective = a3true;
	a3demo_updateProjector(projector);

	// set up lights
	pointLight = demoMode->light_point_main;
	pointLightData = pointLight->dataPtr;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos = pointLight->sceneObjectPtr->dataPtr->position;
	a3demo_setPointLightRadius(pointLightData, a3real_onehundred*3);


	// set up animation
	a3final_initKeyframeController(demoMode->animMorphTeapot, 5, 0, 2.0f);
	a3final_initKeyframeController(demoMode->animPoseSkel, 3, 0, 3.0f);	// 3 poses animating + base

	{
		// manually set up a skeleton
		// first is the hierarchy: the general non-spatial relationship between bones
		const a3ui32 jointCount = 32;

		// indices of joints, their parents and branching joints
		a3ui32 j, p;
		a3ui32 jointIndex = 0;
		a3i32 jointParentIndex = -1;
		a3i32 rootJointIndex, upperSpineJointIndex, clavicleJointIndex, pelvisJointIndex;

		// pointer to pose
		a3_SceneObjectData* spatialPose = 0;

		// initialize hierarchy
		a3_Hierarchy* hierarchy = demoMode->hierarchy_skel;
		a3hierarchyCreate(hierarchy, jointCount, 0);

		// set up joint relationships
		jointParentIndex = rootJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:root");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_lower");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_mid");
		jointParentIndex = upperSpineJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_upper");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:neck");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:head");
		jointParentIndex = upperSpineJointIndex;
		jointParentIndex = clavicleJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:clavicle");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_r");
		jointParentIndex = clavicleJointIndex;
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_l");
		jointParentIndex = rootJointIndex;
		jointParentIndex = pelvisJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:pelvis");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_r");
		jointParentIndex = pelvisJointIndex;
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_l");


		// calculate hierarchy depth of each joint
		for (j = 0; j < hierarchy->numNodes; ++j)
		{
			p = j;
			i = 0;
			while (hierarchy->nodes[p].parentIndex >= 0)
			{
				p = hierarchy->nodes[p].parentIndex;
				++i;
			}
			demoMode->hierarchyDepth_skel[j] = i;
		}


		// reset all poses
		for (p = 0; p < finalMaxCount_skeletonPose; ++p)
			for (j = 0; j < finalMaxCount_skeletonJoint; ++j)
				a3spatialPoseReset(demoMode->skeletonPose[p] + j);


		// define "bind pose" or "base pose" or the initial transformation
		//	description for each joint (not a literal transform)
		p = 0;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, +3.6f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz | f_channel_scale_xyz | f_channel_translate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_lower");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -5.0f);
		a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +0.1f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_mid");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_upper");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -5.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:neck");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:head");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:clavicle");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, 0.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.1f, +0.1f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, -0.1f, -0.5f);
		demoMode->skeletonChannel[j] = f_channel_translate_yz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +30.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, +0.5f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, -0.5f, -0.1f, -0.5f);
		demoMode->skeletonChannel[j] = f_channel_translate_yz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -30.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, +0.5f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
		a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, -5.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, -0.1f, 0.0f, -0.1f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +1.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +90.0f);
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_r");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +0.2f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +1.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
		a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -90.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);
		demoMode->skeletonChannel[j] = f_channel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_l");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, -0.2f, 0.0f, 0.0f);


		// each remaining pose represents a "delta" from the base
		// initialize the changes where applicable
		// (note: the channels describe which pose components can change)
		p = 1;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetRotation(spatialPose, +45.0f, +60.0f, +90.0f);	// rotate whole figure about all axes

		p = 2;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetScale(spatialPose, 2.0f, 2.0f, 2.0f);	// uniformly scale whole figure up to 200%

		p = 3;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = demoMode->skeletonPose[p] + j;
		a3spatialPoseSetTranslation(spatialPose, +3.0f, +4.0f, +5.0f);	// shift whole figure by some vector
	}

	demoMode->finalWaypointCount = 4;
	for (i = 0; i < finalMaxCount_waypoint; ++i)
	{
		demoMode->finalWaypoint[i] = a3vec4_w;
		demoMode->finalTangent[i] = a3vec4_zero;
	}
	demoMode->finalWaypoint[0].x = +24.0f;
	demoMode->finalWaypoint[0].z = 2.0f;
	demoMode->finalWaypoint[1].y = +24.0f;
	demoMode->finalWaypoint[1].z = 12.0f;
	demoMode->finalWaypoint[2].x = -24.0f;
	demoMode->finalWaypoint[2].z = 2.0f;
	demoMode->finalWaypoint[3].y = -24.0f;
	demoMode->finalWaypoint[3].z = 12.0f;
	demoMode->finalTangent[0].y = +8.0f;
	demoMode->finalTangent[1].x = -8.0f;
	demoMode->finalTangent[2].y = -8.0f;
	demoMode->finalTangent[3].x = +8.0f;

	// animation
	demoMode->finalSegmentDuration = 2.0f;
	demoMode->finalSegmentDurationInv = a3recip(demoMode->finalSegmentDuration);


	// other options
	demoMode->renderMode = final_renderModeDefault;
	demoMode->renderPipeline = final_renderPipeForward;
	demoMode->renderPass = final_renderPassComposite;
	demoMode->renderTarget[final_renderPassScene] = final_renderTargetSceneColor;
	demoMode->renderTargetCount[final_renderPassScene] = final_renderTargetScene_max;
	demoMode->renderTarget[final_renderPassComposite] = final_renderTargetPostColor;
	demoMode->renderTargetCount[final_renderPassComposite] = final_renderTargetPost_max;
}


//-----------------------------------------------------------------------------