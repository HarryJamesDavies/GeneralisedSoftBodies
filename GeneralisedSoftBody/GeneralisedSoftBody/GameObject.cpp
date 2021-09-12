//=================================================================
//Base Game Object Class
//=================================================================

#include "GameObject.h"
#include "GlobalData.h"

GameObject::GameObject()
{
	//set the Gameobject to the origin with no rotation and unit scaling 
	m_pos = Vector3::Zero;
	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;
	m_scale = Vector3::One;

	m_worldMat = Matrix::Identity;
	m_fudge = Matrix::Identity;
	m_name = "None";
	m_tag = ObjectTag::OT_NULL;
	m_layer = ObjectLayer::OL_NULL;
	m_drawable = true;
}

GameObject::~GameObject()
{

}

void GameObject::Tick(GlobalData* _GD)
{
	//build up the world matrix depending on the new position of the GameObject
	//the assumption is that this class will be inherited by the class that ACTUALLY changes this
	Matrix  scaleMat = Matrix::CreateScale(m_scale);
	m_rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll); //possible not the best way of doing this!
	Matrix  transMat = Matrix::CreateTranslation(m_pos);

	m_worldMat = m_fudge * scaleMat * m_rotMat * transMat;
}