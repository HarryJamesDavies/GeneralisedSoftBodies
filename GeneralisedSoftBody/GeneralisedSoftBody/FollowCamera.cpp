//=================================================================
// A camera that will follow a specific obejct at a distance away
//=================================================================

#include "FollowCamera.h"
#include "GlobalData.h"

FollowCamera::FollowCamera(GameObject* _target, Vector3 _dPos, float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _up)
{
	m_name = "FollowCamera";
	m_targetObject = _target;
	m_dPos = _dPos;
}

FollowCamera::~FollowCamera()
{

}

void FollowCamera::Tick(GlobalData* _GD)
{
	//Set up position of camera and target position of camera based on new position and orientation of target object
	Matrix rotCam = Matrix::CreateFromYawPitchRoll(m_targetObject->GetYaw(), m_targetObject->GetPitch(), m_targetObject->GetRoll());
	m_pos = m_targetObject->GetPos() + Vector3::Transform(m_dPos, rotCam);
	m_target = m_targetObject->GetPos();

	Camera::Tick(_GD);

	return;
}