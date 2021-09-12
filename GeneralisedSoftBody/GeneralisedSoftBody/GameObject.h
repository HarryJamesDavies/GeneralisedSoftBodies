//=================================================================
//Base Game Object Class
//=================================================================

#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H
#include <d3d11_1.h>
#include "CommonStates.h"
#include "SimpleMath.h"
#include "Keyboard.h"
#include <list>
#include "ObjectTag.h"
#include "ObjectLayer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// Forward declarations
class Camera;
struct ID3D11DeviceContext;
struct GlobalData;
struct DrawData3D;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Tick(GlobalData* _GD);
	virtual void Draw(DrawData3D* _DD) = 0;

	//getters
	Vector3		GetPos() { return m_pos; }
	Vector3		GetScale() { return m_scale; }

	float		GetPitch() { return m_pitch; }
	float		GetYaw() { return m_yaw; }
	float		GetRoll() { return m_roll; }

	bool GetDrawable() { return m_drawable; }

	std::string GetName() { return m_name; }
	ObjectTag GetTag() { return m_tag; }
	ObjectLayer GetLayer() { return m_layer; }

	//setters
	void		SetPos(Vector3 _pos) { m_pos = _pos; }

	void		SetScale(float _scale) { m_scale = _scale * Vector3::One; }
	void		SetScale(float _x, float _y, float _z) { m_scale = Vector3(_x, _y, _z); }
	void		SetScale(Vector3 _scale) { m_scale = _scale; }

	void		SetPitch(float _pitch) { m_pitch = _pitch; }
	void		SetYaw(float _yaw) { m_yaw = _yaw; }
	void		SetRoll(float _roll) { m_roll = _roll; }
	void		SetPitchYawRoll(float _pitch, float _yaw, float _roll) { m_pitch = _pitch; m_yaw = _yaw; m_roll = _roll; }

	void SetDrawable(bool _drawable) { m_drawable = _drawable; }

	void SetName(std::string _name) { m_name = _name; }
	void SetTag(ObjectTag _tag) { m_tag = _tag; }
	void SetLayer(ObjectLayer _layer) { m_layer = _layer; }


protected:
	//Sets object to be visable in scene
	bool m_drawable;

	//World transform/matrix of this GO and it components
	Matrix m_worldMat;
	Matrix m_rotMat;
	Matrix m_fudge;
	Vector3 m_pos;
	float m_pitch, m_yaw, m_roll;
	Vector3 m_scale;

	//Object identifiers
	std::string m_name;
	ObjectTag m_tag;
	ObjectLayer m_layer;
};

#endif