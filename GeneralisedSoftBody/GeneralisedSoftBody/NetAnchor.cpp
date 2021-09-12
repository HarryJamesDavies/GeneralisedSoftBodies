//==============================================================
//Used in basic spring tests as masses
//==============================================================

#include "NetAnchor.h"
#include "GlobalData.h"

NetAnchor::NetAnchor(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, int _anchorNum, bool _fixed, bool _move) : MeshGO(_fileName, _pd3dDevice, _EF)
{
	m_fudge = Matrix::CreateRotationY(XM_PI);
	m_pos = _pos;
	m_layer = ObjectLayer::OL_NULL;
	m_anchorNum = _anchorNum;
	m_fixed = _fixed;
	m_move = _move;
	m_prevMousePos = Vector2::Zero;
	m_firstFrame = true;
}

NetAnchor::~NetAnchor()
{
	
}

//Update anchors position relative to applied forces
void NetAnchor::Tick(GlobalData* _GD)
{
	if (!m_fixed)
	{
		//Calulates wind, gravity and spring total force
		m_oldVelocity = m_velocity;
		Vector3 overallForce = m_velocity;
		overallForce += (Vector3(0.0f, 10.0f, 0.0f) * _GD->m_physicsManager->m_physicsData.m_gravityCoefficient);
		overallForce += (_GD->m_physicsManager->m_physicsData.m_windDirection * _GD->m_physicsManager->m_physicsData.m_windCoefficient);

		for (auto springs : m_springs)
		{
			overallForce += springs->CalculateForce(_GD);
		}

		//Converts force to velocity and then to displacement
		m_velocity = (m_oldVelocity + (overallForce *_GD->m_dt)) - _GD->m_physicsManager->m_physicsData.m_dragCoefficient * m_velocity;
		m_pos = m_pos + (m_velocity * _GD->m_dt);

		//Limits velocity based on user defined upper velocity limit
		if (m_velocity.Length() > _GD->m_physicsManager->m_physicsData.m_velocityLimit)
		{
			m_velocity.Normalize();
			m_velocity *= _GD->m_physicsManager->m_physicsData.m_velocityLimit;
		}
	}
	else
	{
		//Allows movable anchors to be manipulate dto test springs
		if (m_move)
		{
			auto state = _GD->m_mouse->GetState();

			if (!state.leftButton && !m_firstFrame)
			{
				m_firstFrame = true;
			}
			else if (state.leftButton)
			{
				if (m_firstFrame)
				{
					m_prevMousePos = Vector2(float(state.x), float(state.y));
					m_firstFrame = false;
				}

				if (state.leftButton)
				{
					Vector2 mousePos(float(state.x), float(state.y));
					Vector2 mouseDelta = mousePos - m_prevMousePos;
					m_pos.x += 0.1f * mouseDelta.x;
					m_pos.y -= 0.1f * mouseDelta.y;
					m_prevMousePos = mousePos;
				}
			}
		}
	}

	//apply my base behaviour
	MeshGO::Tick(_GD);
}

//Attaches spring to anchor
void NetAnchor::AddSpring(NetAnchor* _anchorB)
{
	m_springs.push_back(new NetSpring(this, _anchorB));
}