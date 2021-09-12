//=================================================================
// Base object for all user controlled objects
//=================================================================

#include "Pawn.h"
#include "GlobalData.h"
#include "KeyBinding.h"

Pawn::Pawn(float _speed, float _rotSpeed) : GameObject()
{
	m_name = "Pawn";
	m_tag = ObjectTag::OT_PAWN;
	m_layer = ObjectLayer::OL_NULL;
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_speed = _speed;
	m_rotSpeed = _rotSpeed;
}

Pawn::~Pawn()
{

}

void Pawn::Tick(GlobalData* _GD)
{
	//Checks input from the user
	InputHandler(_GD);

	GameObject::Tick(_GD);

	return;
}

//Base input processer for all user controlled objects
void Pawn::InputHandler(GlobalData* _GD)
{
	auto kb = _GD->m_keyboard->GetState();

	//Controls movement of the pawn object
	Vector3 forwardMove = -m_speed * _GD->m_dt * Vector3::Forward;
	Matrix yawMove = Matrix::CreateRotationY(m_yaw + 10.2f);
	forwardMove = Vector3::Transform(forwardMove, yawMove);

	//Checks for displacement inputs
	if (kb.IsKeyDown(KeyBinding::Forward))
	{
		m_pos += forwardMove;
	}
	else if (kb.IsKeyDown(KeyBinding::Backward))
	{
		m_pos -= forwardMove;
	}

	Vector3 upMove = m_speed * _GD->m_dt * Vector3::Up;
	if (kb.IsKeyDown(KeyBinding::Up))
	{
		m_pos += upMove;
	}
	else if (kb.IsKeyDown(KeyBinding::Down))
	{
		m_pos -= upMove;
	}

	//Checks for rotation inputs
	Vector3 rightMove = -m_speed * _GD->m_dt * Vector3::Right;
	rightMove = Vector3::Transform(rightMove, yawMove);
	if (kb.IsKeyDown(KeyBinding::Right))
	{
		m_pos += rightMove;
	}
	else if (kb.IsKeyDown(KeyBinding::Left))
	{
		m_pos -= rightMove;
	}

	float rotMove = m_rotSpeed * _GD->m_dt;
	if (kb.IsKeyDown(KeyBinding::RotRight))
	{
		m_yaw += rotMove;
	}
	else if (kb.IsKeyDown(KeyBinding::RotLeft))
	{
		m_yaw -= rotMove;
	}

	//Prevents excessive movement distance
	float length = m_pos.Length();
	float maxLength = 500.0f;
	if (length > maxLength)
	{
		m_pos *= maxLength / length;
	}

	return;
}