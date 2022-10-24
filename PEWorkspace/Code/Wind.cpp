#include "Wind.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Math/CameraOps.h"
namespace PE {
	namespace Components {

		PE_IMPLEMENT_CLASS1(Wind, Component);

		void Wind::addDefaultComponents()
		{
			Component::addDefaultComponents();

		}

		void Wind::update(Vector3 pos, Vector3 dir) {
			//memset(&m_cbuffer, 0, sizeof(m_cbuffer));
			m_cbuffer.pos = Vector4(pos.getX(), pos.getY(), pos.getZ(), 1);
			m_cbuffer.dir = Vector4(dir.getX(), dir.getY(), dir.getZ(), 1);
		}

	}; // namespace Components
}; // namespace PE


