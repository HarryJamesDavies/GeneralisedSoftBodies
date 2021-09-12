//=================================================================
//A simple light defined by a colour and and ambient colour and used as a point light with no drop off in the standard shader
//=================================================================

#include "Light.h"
#include "GlobalData.h"

Light::Light(Vector3 _pos, Color _colour, Color _ambientColour)
{
	m_pos = _pos;
	m_colour = _colour;
	m_ambientColour = _ambientColour;
	m_layer = ObjectLayer::OL_LIGHT;
}

Light::~Light()
{
}