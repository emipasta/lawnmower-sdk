#include "cbase.h"
#include "vehicle_base.h"
#include "in_buttons.h"

// Name of our entity's model
#define	ENTITY_MODEL	"models/lawnmower.mdl"

class CVehicleLawnmower;

class CLawnMowerServerVehicle : public CBaseServerVehicle
{
	typedef CBaseServerVehicle BaseClass;

	// IServerVehicle
public:
	void GetVehicleViewPosition(int nRole, Vector *pAbsOrigin, QAngle *pAbsAngles, float *pFOV = NULL);
	virtual void ItemPostFrame(CBasePlayer *pPlayer);

	virtual bool	IsPassengerEntering(void) { return false; }	// NOTE: This mimics the scenario HL2 would have seen
																// what the fuck does this comment mean????
	virtual bool	IsPassengerExiting(void) { return false; }

protected:
	CVehicleLawnmower *GetOwner(void);
};

class CVehicleLawnmower : public CBaseAnimating, public IDrivableVehicle
{
public:
	DECLARE_CLASS(CVehicleLawnmower, CBaseAnimating);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CVehicleLawnmower()
	{
		m_bActive = false;
	}

	~CVehicleLawnmower()
	{
		//fuck this
	}

	void Precache(void);
	void Spawn(void);
	void Think(void);
	int ObjectCaps(void);
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void LawnmowerUse(CBaseEntity *pActivator);

	void PlayerControlInit(CBasePlayer *pPlayer);
	void PlayerControlShutdown(void);
	void ResetUseKey(CBasePlayer *pPlayer);

	void GetVectors(Vector* pForward, Vector* pRight, Vector* pUp) const;

	bool ShouldForceExit() { return m_bForcedExit; }
	void ClearForcedExit() { m_bForcedExit = false; }

	// Inputs
	void InputEnterVehicleImmediate(inputdata_t &inputdata);
	void InputEnterVehicle(inputdata_t &inputdata);
	void InputExitVehicle(inputdata_t &inputdata);
	void InputLock(inputdata_t &inputdata);
	void InputUnlock(inputdata_t &inputdata);
	void InputOpen(inputdata_t &inputdata);
	void InputClose(inputdata_t &inputdata);

	CNetworkHandle(CBasePlayer, m_hPlayer);

// IDrivableVehicle
public:

	virtual bool PassengerShouldReceiveDamage(CTakeDamageInfo &info)
	{
		if (info.GetDamageType() & DMG_VEHICLE)
			return true;

		return (info.GetDamageType() & (DMG_RADIATION | DMG_BLAST)) == 0;
	}

	virtual CBaseEntity *GetDriver(void);
	virtual void ProcessMovement(CBasePlayer *pPlayer, CMoveData *pMoveData) { return; }
	virtual void FinishMove(CBasePlayer *player, CUserCmd *ucmd, CMoveData *move) { return; }
	virtual bool CanEnterVehicle(CBaseEntity *pEntity);
	virtual bool CanExitVehicle(CBaseEntity *pEntity);
	virtual void SetVehicleEntryAnim(bool bOn);
	virtual void SetVehicleExitAnim(bool bOn, Vector vecEyeExitEndpoint) { m_bExitAnimOn = bOn; if (bOn) m_vecEyeExitEndpoint = vecEyeExitEndpoint; }
	virtual void EnterVehicle(CBaseCombatCharacter *pPassenger);

	virtual bool AllowBlockedExit(CBaseCombatCharacter *pPassenger, int nRole) { return true; }
	virtual bool AllowMidairExit(CBaseCombatCharacter *pPassenger, int nRole) { return true; }
	virtual void PreExitVehicle(CBaseCombatCharacter *pPassenger, int nRole) {}
	virtual void ExitVehicle(int nRole);

	virtual void ItemPostFrame(CBasePlayer *pPlayer) {}
	virtual void SetupMove(CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move) {}
	virtual string_t GetVehicleScriptName() { return m_vehicleScript; }

	// If this is a vehicle, returns the vehicle interface
	virtual IServerVehicle *GetServerVehicle() { return &m_ServerVehicle; }

private:
	bool	m_bActive;
	float	m_flNextChangeTime;

	// Entering / Exiting
	bool				m_bLocked;
	CNetworkVar(bool, m_bEnterAnimOn);
	CNetworkVar(bool, m_bExitAnimOn);
	CNetworkVector(m_vecEyeExitEndpoint);
	bool				m_bForcedExit;

	// Vehicle script filename
	// what is a vehicle script file???
	string_t			m_vehicleScript;

	COutputEvent		m_playerOn;
	COutputEvent		m_playerOff;
	COutputEvent		m_OnOpen;
	COutputEvent		m_OnClose;

protected:
	CLawnMowerServerVehicle m_ServerVehicle;
};

LINK_ENTITY_TO_CLASS(vehicle_lawnmower, CVehicleLawnmower);

// Start of our data description for the class
BEGIN_DATADESC(CVehicleLawnmower)

	// Save/restore our active state
	DEFINE_FIELD(m_bActive, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flNextChangeTime, FIELD_TIME),

	// Declare our think function
	DEFINE_THINKFUNC(Think),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_VOID, "Lock", InputLock),
	DEFINE_INPUTFUNC(FIELD_VOID, "Unlock", InputUnlock),
	DEFINE_INPUTFUNC(FIELD_VOID, "EnterVehicle", InputEnterVehicle),
	DEFINE_INPUTFUNC(FIELD_VOID, "EnterVehicleImmediate", InputEnterVehicleImmediate),
	DEFINE_INPUTFUNC(FIELD_VOID, "ExitVehicle", InputExitVehicle),
	DEFINE_INPUTFUNC(FIELD_VOID, "Open", InputOpen),
	DEFINE_INPUTFUNC(FIELD_VOID, "Close", InputClose),

	// Keys
	DEFINE_EMBEDDED(m_ServerVehicle),

	DEFINE_FIELD(m_hPlayer, FIELD_EHANDLE),
	DEFINE_FIELD(m_bEnterAnimOn, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bExitAnimOn, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bForcedExit, FIELD_BOOLEAN),
	DEFINE_FIELD(m_vecEyeExitEndpoint, FIELD_POSITION_VECTOR),

	DEFINE_KEYFIELD(m_vehicleScript, FIELD_STRING, "vehiclescript"),
	DEFINE_KEYFIELD(m_bLocked, FIELD_BOOLEAN, "vehiclelocked"),

	DEFINE_OUTPUT(m_playerOn, "PlayerOn"),
	DEFINE_OUTPUT(m_playerOff, "PlayerOff"),
	DEFINE_OUTPUT(m_OnOpen, "OnOpen"),
	DEFINE_OUTPUT(m_OnClose, "OnClose"),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CVehicleLawnmower, DT_VehicleLawnmower)
	SendPropEHandle(SENDINFO(m_hPlayer)),
	SendPropBool(SENDINFO(m_bEnterAnimOn)),
	SendPropBool(SENDINFO(m_bExitAnimOn)),
	SendPropVector(SENDINFO(m_vecEyeExitEndpoint), -1, SPROP_COORD),
END_SEND_TABLE();

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CVehicleLawnmower::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel(ENTITY_MODEL);

	m_ServerVehicle.Initialize(STRING(m_vehicleScript));
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CVehicleLawnmower::Spawn(void)
{
	Precache();
	SetModel(ENTITY_MODEL);
	SetSolid(SOLID_BBOX);
	UTIL_SetSize(this, Vector(-20, -20, 0), Vector(20, 20, 20));
	SetMoveType(MOVETYPE_FLYGRAVITY);

	BaseClass::Spawn(); // spawn the vehicle??
						// not explained

	SetThink(&CVehicleLawnmower::Think);
	SetNextThink(gpGlobals->curtime);
}

int CVehicleLawnmower::ObjectCaps()
{
	int caps = BaseClass::ObjectCaps();
	return caps|FCAP_IMPULSE_USE;
}

//-----------------------------------------------------------------------------
// Purpose: Think function to randomly move the entity
//-----------------------------------------------------------------------------
void CVehicleLawnmower::Think(void)
{
	SetNextThink(gpGlobals->curtime + 0.1);
	/*
	// Snap our facing to where we're heading
	Vector velFacing = GetAbsVelocity();
	if (velFacing.IsLengthGreaterThan(1.0f)){
		QAngle angFacing;
		VectorAngles(-velFacing, angFacing);
		SetAbsAngles(angFacing);
	}

	// See if we should change direction again
	if (m_flNextChangeTime < gpGlobals->curtime)
	{
		// Randomly take a new direction and speed
		Vector vecNewVelocity = (RandomVector(-64.0f, 64.0f) * Vector(1.0f, 1.0f, 0.0f)) + Vector(0,0,GetAbsVelocity().z);
		SetAbsVelocity(vecNewVelocity);

		// Randomly change it again within one to three seconds
		m_flNextChangeTime = gpGlobals->curtime + random->RandomFloat(1.0f, 3.0f);
	}
	*/

	if (GetDriver())
	{
		BaseClass::Think();

		// If the enter or exit animation has finished, tell the server vehicle
		// no animation durr
		if (m_bExitAnimOn || m_bEnterAnimOn)
		{
			GetServerVehicle()->HandleEntryExitFinish(m_bExitAnimOn, true);
		}
	}
}

void CVehicleLawnmower::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer *pPlayer = ToBasePlayer(pActivator);
	if (!pPlayer)
		return;

	ResetUseKey(pPlayer);

	// what the heck is value here?
	GetServerVehicle()->HandlePassengerEntry(pPlayer, (value > 0));
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CVehicleLawnmower::InputOpen(inputdata_t &inputdata)
{
	Msg("I hate this engine open\n");
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CVehicleLawnmower::InputClose(inputdata_t &inputdata)
{
	// The enter anim closes the pod, so don't do this redundantly!
	if (m_bLocked || m_bEnterAnimOn)
		return;

	Msg("I hate this engine close\n");
}

//-----------------------------------------------------------------------------
// Purpose: reset the use key??? idk what the fuck this is
//-----------------------------------------------------------------------------
void CVehicleLawnmower::ResetUseKey(CBasePlayer *pPlayer)
{
	pPlayer->m_afButtonPressed &= ~IN_USE;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CVehicleLawnmower::GetDriver(void)
{
	return m_hPlayer;
}

//-----------------------------------------------------------------------------
// Purpose: Return true of the player's allowed to enter / exit the vehicle
//-----------------------------------------------------------------------------
bool CVehicleLawnmower::CanEnterVehicle(CBaseEntity *pEntity)
{
	// Prevent entering if the vehicle's being driven by an NPC
	if (GetDriver() && GetDriver() != pEntity)
		return false;

	// Prevent entering if the vehicle's locked
	return !m_bLocked;
}


//-----------------------------------------------------------------------------
// Purpose: Return true of the player is allowed to exit the vehicle.
//-----------------------------------------------------------------------------
bool CVehicleLawnmower::CanExitVehicle(CBaseEntity *pEntity)
{
	// Prevent exiting if the vehicle's locked, rotating, or playing an entry/exit anim.
	return (!m_bLocked && (GetLocalAngularVelocity() == vec3_angle) && !m_bEnterAnimOn && !m_bExitAnimOn);
}

//-----------------------------------------------------------------------------
// Purpose: Prevent the player from entering / exiting the vehicle
//-----------------------------------------------------------------------------
void CVehicleLawnmower::InputLock(inputdata_t &inputdata)
{
	m_bLocked = true;
}


//-----------------------------------------------------------------------------
// Purpose: Allow the player to enter / exit the vehicle
//-----------------------------------------------------------------------------
void CVehicleLawnmower::InputUnlock(inputdata_t &inputdata)
{
	m_bLocked = false;
}

//-----------------------------------------------------------------------------
// Purpose: Force the player to enter the vehicle.
//-----------------------------------------------------------------------------
void CVehicleLawnmower::InputEnterVehicle(inputdata_t &inputdata)
{
	if (m_bEnterAnimOn)
		return;

	// Try the activator first & use them if they are a player.
	CBaseCombatCharacter *pPassenger = ToBaseCombatCharacter(inputdata.pActivator);
	if (pPassenger == NULL)
	{
		// Activator was not a player, just grab the singleplayer player.
		pPassenger = UTIL_PlayerByIndex(1);
		if (pPassenger == NULL)
			return;
	}

	// FIXME: I hate code like this. I should really add a parameter to HandlePassengerEntry
	//		  to allow entry into locked vehicles
	bool bWasLocked = m_bLocked;
	m_bLocked = false;
	GetServerVehicle()->HandlePassengerEntry(pPassenger, true);
	m_bLocked = bWasLocked;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CVehicleLawnmower::InputEnterVehicleImmediate(inputdata_t &inputdata)
{
	if (m_bEnterAnimOn)
		return;

	// Try the activator first & use them if they are a player.
	CBaseCombatCharacter *pPassenger = ToBaseCombatCharacter(inputdata.pActivator);
	if (pPassenger == NULL)
	{
		// Activator was not a player, just grab the singleplayer player.
		pPassenger = UTIL_PlayerByIndex(1);
		if (pPassenger == NULL)
			return;
	}

	CBasePlayer *pPlayer = ToBasePlayer(pPassenger);
	if (pPlayer != NULL)
	{
		if (pPlayer->IsInAVehicle())
		{
			// Force the player out of whatever vehicle they are in.
			pPlayer->LeaveVehicle();
		}

		pPlayer->GetInVehicle(GetServerVehicle(), VEHICLE_ROLE_DRIVER);
	}
	else
	{
		// NPCs are not currently supported - jdw
		Assert(0);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Force the player to exit the vehicle.
//-----------------------------------------------------------------------------
void CVehicleLawnmower::InputExitVehicle(inputdata_t &inputdata)
{
	m_bForcedExit = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVehicleLawnmower::SetVehicleEntryAnim(bool bOn)
{
	m_bEnterAnimOn = bOn;
}

//-----------------------------------------------------------------------------
// Purpose: Vehicles are permanently oriented off angle for vphysics.
//-----------------------------------------------------------------------------
void CVehicleLawnmower::GetVectors(Vector* pForward, Vector* pRight, Vector* pUp) const
{
	// This call is necessary to cause m_rgflCoordinateFrame to be recomputed
	const matrix3x4_t &entityToWorld = EntityToWorldTransform();

	if (pForward != NULL)
	{
		MatrixGetColumn(entityToWorld, 1, *pForward);
	}

	if (pRight != NULL)
	{
		MatrixGetColumn(entityToWorld, 0, *pRight);
	}

	if (pUp != NULL)
	{
		MatrixGetColumn(entityToWorld, 2, *pUp);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVehicleLawnmower::ExitVehicle(int nRole)
{
	CBasePlayer *pPlayer = m_hPlayer;
	if (!pPlayer)
		return;

	m_hPlayer = NULL;
	ResetUseKey(pPlayer);

	m_playerOff.FireOutput(pPlayer, this, 0);
	m_bEnterAnimOn = false;

	m_ServerVehicle.SoundShutdown(1.0);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVehicleLawnmower::EnterVehicle(CBaseCombatCharacter *pPassenger)
{
	if (pPassenger == NULL)
		return;

	CBasePlayer *pPlayer = ToBasePlayer(pPassenger);
	if (pPlayer != NULL)
	{
		// Remove any player who may be in the vehicle at the moment
		if (m_hPlayer)
		{
			ExitVehicle(VEHICLE_ROLE_DRIVER);
		}

		m_hPlayer = pPlayer;
		m_playerOn.FireOutput(pPlayer, this, 0);

		m_ServerVehicle.SoundStart();
	}
	else
	{
		// NPCs are not supported yet - jdw
		Assert(0);
	}
}

//========================================================================================================================================
// CRANE VEHICLE SERVER VEHICLE
//========================================================================================================================================
CVehicleLawnmower *CLawnMowerServerVehicle::GetOwner(void)
{
	return (CVehicleLawnmower *)GetDrivableVehicle();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pPlayer - 
//-----------------------------------------------------------------------------
void CLawnMowerServerVehicle::ItemPostFrame(CBasePlayer *player)
{
	Assert(player == GetDriver());

	GetDrivableVehicle()->ItemPostFrame(player);

	if ((player->m_afButtonPressed & IN_USE) || GetOwner()->ShouldForceExit())
	{
		GetOwner()->ClearForcedExit();
		if (GetDrivableVehicle()->CanExitVehicle(player))
		{
			// Let the vehicle try to play the exit animation
			if (!HandlePassengerExit(player) && (player != NULL))
			{
				player->PlayUseDenySound();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLawnMowerServerVehicle::GetVehicleViewPosition(int nRole, Vector *pAbsOrigin, QAngle *pAbsAngles, float *pFOV /*= NULL*/)
{
	// FIXME: This needs to be reconciled with the other versions of this function!
	Assert(nRole == VEHICLE_ROLE_DRIVER);
	CBasePlayer *pPlayer = ToBasePlayer(GetDrivableVehicle()->GetDriver());
	Assert(pPlayer);

	*pAbsAngles = pPlayer->EyeAngles(); // yuck. this is an in/out parameter.

	float flPitchFactor = 1.0;
	matrix3x4_t vehicleEyePosToWorld;
	Vector vehicleEyeOrigin;
	QAngle vehicleEyeAngles;
	GetOwner()->GetAttachment("vehicle_driver_eyes", vehicleEyeOrigin, vehicleEyeAngles);
	AngleMatrix(vehicleEyeAngles, vehicleEyePosToWorld);

	// Compute the relative rotation between the unperterbed eye attachment + the eye angles
	matrix3x4_t cameraToWorld;
	AngleMatrix(*pAbsAngles, cameraToWorld);

	matrix3x4_t worldToEyePos;
	MatrixInvert(vehicleEyePosToWorld, worldToEyePos);

	matrix3x4_t vehicleCameraToEyePos;
	ConcatTransforms(worldToEyePos, cameraToWorld, vehicleCameraToEyePos);

	// Now perterb the attachment point
	vehicleEyeAngles.x = RemapAngleRange(PITCH_CURVE_ZERO * flPitchFactor, PITCH_CURVE_LINEAR, vehicleEyeAngles.x);
	vehicleEyeAngles.z = RemapAngleRange(ROLL_CURVE_ZERO * flPitchFactor, ROLL_CURVE_LINEAR, vehicleEyeAngles.z);
	AngleMatrix(vehicleEyeAngles, vehicleEyeOrigin, vehicleEyePosToWorld);

	// Now treat the relative eye angles as being relative to this new, perterbed view position...
	matrix3x4_t newCameraToWorld;
	ConcatTransforms(vehicleEyePosToWorld, vehicleCameraToEyePos, newCameraToWorld);

	// output new view abs angles
	MatrixAngles(newCameraToWorld, *pAbsAngles);

	// UNDONE: *pOrigin would already be correct in single player if the HandleView() on the server ran after vphysics
	MatrixGetColumn(newCameraToWorld, 3, *pAbsOrigin);
}
