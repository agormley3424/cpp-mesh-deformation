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

		void Wind::update(Vector3 pos, Vector3 xdir, Vector3 ydir, Vector3 zdir) {
			//memset(&m_cbuffer, 0, sizeof(m_cbuffer));
			m_cbuffer.windSrc.setPos(pos);
			m_cbuffer.windSrc.setU(xdir);
			m_cbuffer.windSrc.setV(ydir);
			m_cbuffer.windSrc.setN(zdir);
		}

	}; // namespace Components
}; // namespace PE


