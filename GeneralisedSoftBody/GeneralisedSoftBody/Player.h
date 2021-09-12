//=================================================================
// Player object that adds physical aspects
//=================================================================

#pragma once
#include "Pawn.h"
#include "GlobalData.h"
#include "ForceField.h"
#include "MeshGO.h"

class Player : public Pawn
{
public:
	Player(float _speed, float _rotSpeed, string _meshFileName, GlobalData* _GD);
	~Player();

	void Tick(GlobalData* _GD);

private:
	//Collisions field
	ForceField* m_forceField;

	//Visible mesh
	MeshGO* m_mesh;
};