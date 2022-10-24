#ifndef __PYENGINE_2_0_WIND_H__
#define __PYENGINE_2_0_WIND_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Math/Vector3.h"

#include "WindPerObjectGroupConstantsShaderAction.h"
// Sibling/Children includes

namespace PE {
	namespace Components {

		struct Wind : public Component
		{

			PE_DECLARE_CLASS(Wind);

			// Constructor -------------------------------------------------------------
			Wind(
				PE::GameContext& context,
				PE::MemoryArena arena,
				Handle hMyself,
				Vector3 pos,
				Vector3 dir) : Component(context, arena, hMyself)
			{
				memset(&m_cbuffer, 0, sizeof(m_cbuffer));
				m_cbuffer.pos = Vector4(pos.getX(), pos.getY(), pos.getZ(), 1);
				m_cbuffer.dir = Vector4(dir.getX(), dir.getY(), dir.getZ(), 1);
			}

			void update(Vector3 pos, Vector3 dir);

			virtual ~Wind() {}

			// Component ------------------------------------------------------------

			virtual void addDefaultComponents();

			// Individual events -------------------------------------------------------

			Handle m_hParent;

			WindPerObjectGroupConstantsShaderAction::hlsl_Wind m_cbuffer; // gpu mirror : values that will be set into gpu registers vi shader action hlsl_cbPerObjectGroup_c1
		};

	}; // namespace Components
}; // namespace PE

#endif
