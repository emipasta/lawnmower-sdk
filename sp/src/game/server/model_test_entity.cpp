#include "cbase.h"

// Name of our entity's model
#define	ENTITY_MODEL	"models/gibs/airboat_broken_engine.mdl"

class CModelTestEntity : public CBaseAnimating
{
public:
	DECLARE_CLASS(CModelTestEntity, CBaseAnimating);
	DECLARE_DATADESC();

	CModelTestEntity()
	{
		m_bActive = false;
	}

	void Spawn(void);
	void Precache(void);

	void MoveThink(void);

	// Input function
	void InputToggle(inputdata_t &inputData);

private:

	bool	m_bActive;
	float	m_flNextChangeTime;
};

LINK_ENTITY_TO_CLASS(model_test_entity, CModelTestEntity);

// Start of our data description for the class
BEGIN_DATADESC(CModelTestEntity)

	// Save/restore our active state
	DEFINE_FIELD(m_bActive, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flNextChangeTime, FIELD_TIME),

	// Links our input name from Hammer to our input member function
	DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),

	// Declare our think function
	DEFINE_THINKFUNC(MoveThink),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CModelTestEntity::Precache(void)
{
	PrecacheModel(ENTITY_MODEL);

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CModelTestEntity::Spawn(void)
{
	Precache();

	SetModel(ENTITY_MODEL);
	SetSolid(SOLID_BBOX);
	UTIL_SetSize(this, -Vector(20, 20, 20), Vector(20, 20, 20));
}

//-----------------------------------------------------------------------------
// Purpose: Think function to randomly move the entity
//-----------------------------------------------------------------------------
void CModelTestEntity::MoveThink(void)
{
	// See if we should change direction again
	if (m_flNextChangeTime < gpGlobals->curtime)
	{
		// Randomly take a new direction and speed
		Vector vecNewVelocity = RandomVector(-64.0f, 64.0f);
		SetAbsVelocity(vecNewVelocity);

		// Randomly change it again within one to three seconds
		m_flNextChangeTime = gpGlobals->curtime + random->RandomFloat(1.0f, 3.0f);
	}

	// Snap our facing to where we're heading
	Vector velFacing = GetAbsVelocity();
	QAngle angFacing;
	VectorAngles(velFacing, angFacing);
	SetAbsAngles(angFacing);

	// Think at 20Hz
	SetNextThink(gpGlobals->curtime + 0.05f);
}

//-----------------------------------------------------------------------------
// Purpose: Toggle the movement of the entity
//-----------------------------------------------------------------------------
void CModelTestEntity::InputToggle(inputdata_t &inputData)
{
	// Toggle our active state
	if (!m_bActive)
	{
		// Start thinking
		SetThink(&CModelTestEntity::MoveThink);

		SetNextThink(gpGlobals->curtime + 0.05f);

		// Start moving
		SetMoveType(MOVETYPE_FLY);

		// Force MoveThink() to choose a new speed and direction immediately
		m_flNextChangeTime = gpGlobals->curtime;

		// Update m_bActive to reflect our new state
		m_bActive = true;
	}
	else
	{
		// Stop thinking
		SetThink(NULL);

		// Stop moving
		SetAbsVelocity(vec3_origin);
		SetMoveType(MOVETYPE_NONE);

		m_bActive = false;
	}
}