//=================================================================
// A camera that will follow a specific obejct at a distance away
//=================================================================

#ifndef _FOLLOW_CAMERA_H_
#define _FOLLOW_CAMERA_H_
#include "Camera.h"

class FollowCamera : public Camera
{
public:
	FollowCamera(GameObject* _target, Vector3 _dpos, float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up);
	virtual ~FollowCamera();

	virtual void Tick(GlobalData* _GD);
	virtual void Draw(DrawData3D* _DD) { return; };

private:
	//Follow this Object
	GameObject* m_targetObject;

	//Distance away from follow object
	Vector3 m_dPos;
};

#endif;

