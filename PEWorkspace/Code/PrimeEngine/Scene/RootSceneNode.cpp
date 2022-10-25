#define NOMINMAX

#include "RootSceneNode.h"

#include "Light.h"
#include "Wind.h"
#include "DrawList.h"

#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Render/ShaderActions/SetPerFrameConstantsShaderAction.h"
namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(RootSceneNode, SceneNode);

Handle RootSceneNode::s_hTitleInstance;
Handle RootSceneNode::s_hInstance;
Handle RootSceneNode::s_hCurInstance;

void RootSceneNode::Construct(PE::GameContext &context, PE::MemoryArena arena)
{
	Handle h("ROOT_SCENE_NODE", sizeof(RootSceneNode));
	RootSceneNode *pRootSceneNode = new(h) RootSceneNode(context, arena, h);
	pRootSceneNode->addDefaultComponents();
	SetInstance(h);
	
	s_hTitleInstance = Handle("ROOT_SCENE_NODE", sizeof(RootSceneNode));
	RootSceneNode *pTitleRootSceneNode = new(s_hTitleInstance) RootSceneNode(context, arena, h);
	pTitleRootSceneNode->addDefaultComponents();
	
	SetTitleAsCurrent();

	SceneNode* pRoot = RootSceneNode::Instance();

	Handle hWind("WIND", sizeof(Wind));
	Wind* pWind = new(hWind) Wind(context, arena, hWind, Vector3(0, 0, 0), Vector3(0, 0, 0));
	pRoot->m_wind.add(pWind);


	Handle hWind2("WIND", sizeof(Wind));
	Wind* pWind2 = new(hWind2) Wind(context, arena, hWind2, Vector3(0, 0, 0), Vector3(0, 0, 0));
	pRoot->m_wind.add(pWind2);

	Handle hWind3("WIND", sizeof(Wind));
	Wind* pWind3 = new(hWind3) Wind(context, arena, hWind3, Vector3(0, 0, 0), Vector3(0, 0, 0));
	pRoot->m_wind.add(pWind3);
}

void RootSceneNode::addDefaultComponents()
{
	SceneNode::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS, RootSceneNode::do_GATHER_DRAWCALLS);
	PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS_Z_ONLY, RootSceneNode::do_GATHER_DRAWCALLS);
}

//void updateWind(int i, Vector3 pos, Vector3 dir) {
//
//	SceneNode* pRoot = RootSceneNode::Instance();
//	Wind* camWind = pRoot->m_wind.getByIndexUnchecked(i).getObject<Wind>();
//	camWind->update(pos, dir);
//}

void RootSceneNode::do_GATHER_DRAWCALLS(Events::Event *pEvt)
{
	DrawList *pDrawList = NULL;
	bool zOnly = pEvt->isInstanceOf<Events::Event_GATHER_DRAWCALLS_Z_ONLY>();
	
	pDrawList = zOnly ? DrawList::ZOnlyInstance() : DrawList::Instance();
	
	SceneNode *pRoot = RootSceneNode::Instance();
	Events::Event_GATHER_DRAWCALLS *pDrawEvent = zOnly ? NULL : (Events::Event_GATHER_DRAWCALLS *)(pEvt);
	Events::Event_GATHER_DRAWCALLS_Z_ONLY *pZOnlyDrawEvent = zOnly ? (Events::Event_GATHER_DRAWCALLS_Z_ONLY *)(pEvt) : NULL;

	// set some effect constants here that will be constant per frame

	bool setGlobalValues = true;
	if (!zOnly && pDrawEvent->m_drawOrder != EffectDrawOrder::First)
		setGlobalValues = false;

	if (setGlobalValues)
	{
		// fill in the data object that will be submitted to pipeline
		Handle &h = pDrawList->nextGlobalShaderValue();
		h = Handle("RAW_DATA", sizeof(SetPerFrameConstantsShaderAction));
		SetPerFrameConstantsShaderAction *p = new(h) SetPerFrameConstantsShaderAction(*m_pContext, m_arena);
		p->m_data.gGameTimes[0] = pDrawEvent ? pDrawEvent->m_gameTime : 0;
		p->m_data.gGameTimes[1] = pDrawEvent ? pDrawEvent->m_frameTime : 0;
	}

	// set some effect constants here that will be constant per object group
	// NOTE at this point we have only one object group so we set it on top level per frame

	if (setGlobalValues)
	{
		Handle& hsvPerObjectGroup = pDrawList->nextGlobalShaderValue();
		hsvPerObjectGroup = Handle("RAW_DATA", sizeof(SetPerObjectGroupConstantsShaderAction));
		SetPerObjectGroupConstantsShaderAction* psvPerObjectGroup = new(hsvPerObjectGroup) SetPerObjectGroupConstantsShaderAction(*m_pContext, m_arena);

		psvPerObjectGroup->m_data.gViewProj = pDrawEvent ? pDrawEvent->m_projectionViewTransform : pZOnlyDrawEvent->m_projectionViewTransform;

		psvPerObjectGroup->m_data.gViewInv = pDrawEvent ? pDrawEvent->m_viewInvTransform : Matrix4x4();
		// TODO: fill these in for motion blur
		psvPerObjectGroup->m_data.gPreviousViewProjMatrix = Matrix4x4();
		psvPerObjectGroup->m_data.gViewProjInverseMatrix = Matrix4x4();

		psvPerObjectGroup->m_data.gDoMotionBlur = 0;
		psvPerObjectGroup->m_data.gEyePosW = pDrawEvent ? pDrawEvent->m_eyePos : pZOnlyDrawEvent->m_eyePos;


		// the light that drops shadows is defined by a boolean isShadowCaster in maya light objects
		PrimitiveTypes::UInt32 iDestLight = 0;
		if (pRoot->m_lights.m_size)
		{
			for (PrimitiveTypes::UInt32 i = 0; i < (pRoot->m_lights.m_size); i++) {
				Light* pLight = pRoot->m_lights[i].getObject<Light>();
				if (pLight->castsShadow()) {
					Matrix4x4 worldToView = pLight->m_worldToViewTransform;
					Matrix4x4 lightProjectionViewWorldMatrix = (pLight->m_viewToProjectedTransform * worldToView);
					psvPerObjectGroup->m_data.gLightWVP = lightProjectionViewWorldMatrix;

					psvPerObjectGroup->m_data.gLights[iDestLight] = pLight->m_cbuffer;
					iDestLight++;

					break;
				}
			}
		}
		for (PrimitiveTypes::UInt32 iLight = 0; iLight < pRoot->m_lights.m_size; iLight++)
		{
			Light* pLight = pRoot->m_lights[iLight].getObject<Light>();
			if (pLight->castsShadow())
				continue;
			psvPerObjectGroup->m_data.gLights[iDestLight] = pLight->m_cbuffer;
			iDestLight++;
		}


		PrimitiveTypes::UInt32 iDestWind = 0;
		if (setGlobalValues)
		{
			Handle& hsvWindObjectGroup = pDrawList->nextGlobalWindValue();
			hsvWindObjectGroup = Handle("RAW_DATA", sizeof(WindPerObjectGroupConstantsShaderAction));
			WindPerObjectGroupConstantsShaderAction* psvPerObjectGroup = new(hsvWindObjectGroup) WindPerObjectGroupConstantsShaderAction(*m_pContext, m_arena);

			CameraSceneNode* csn = getFirstComponent<CameraSceneNode>();
			Matrix4x4 camMat = csn->m_worldTransform2;
			Wind* camWind = pRoot->m_wind.getByIndexUnchecked(0).getObject<Wind>();

			camWind->update(camMat.getPos(), camMat.getN());


			//Vector4 pos = (csn->m_worldToViewTransform.getPos().getX(), csn->m_worldToViewTransform.getPos().getY(), csn->m_worldToViewTransform.getPos().getZ(), 1);

			//*rootMat = Matrix4x4(Vector4(, csn->m_worldToViewTransform.getPos(), Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0));

			//csn.m_worldToViewTransform
			
			for (int i = 0; i < 3; ++i) {
				Wind* windData = pRoot->m_wind.getByIndexUnchecked(i).getObject<Wind>();

				psvPerObjectGroup->m_data.gWind[i].pos = windData->m_cbuffer.pos;
				psvPerObjectGroup->m_data.gWind[i].dir = windData->m_cbuffer.dir;

				int j = 5;
			}

			//psvPerObjectGroup->bindToPipeline(NULL);
		}
	}
}
}; // namespace Components
}; // namespace PE
