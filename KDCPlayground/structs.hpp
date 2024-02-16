#pragma once
//Structs 

enum {
	MSG_PLAYER_ONANIMCHANGE = 324
};

enum class CA_Interpolation_Type
{
	Linear = 0,
	QuadraticIn,
	QuadraticOut,
	QuadraticInOut,
	SineIn,
	SineOut,
	SineInOut
};

#define NUM_ANIMATION_USER_DATA_SLOTS 8
struct CryCharAnimationParams
{
	CryCharAnimationParams(int _nLayerID = 0, uint32_t _nFlags = 0)
		: m_fTransTime(-1.0f)
		, m_fKeyTime(-1.0f)
		, m_fPlaybackSpeed(1.0f)
		, m_fAllowMultilayerAnim(1.0f)
		, m_nLayerID(_nLayerID)
		, m_nFlags(_nFlags)
		, m_nInterpolationType(CA_Interpolation_Type::Linear)
		, m_nUserToken(0)
		, m_fPlaybackWeight(1.0f)
#if defined(USE_PROTOTYPE_ABS_BLENDING)
		, m_pJointMask(NULL)
#endif //!defined(USE_PROTOTYPE_ABS_BLENDING)
	{
	}
	//! Transition time between two animations.
	float m_fTransTime;

	//! keytime[0-1]. can be used to start a transition animation.
	float m_fKeyTime;

	//! Multiplier for animation-update.
	float m_fPlaybackSpeed;

	//! If this is '1' then we can play animation on higher layers and they overwrite the channels on lower layers.
	float m_fAllowMultilayerAnim;

	//! Specify the layer where to start the animation.
	unsigned int m_nLayerID;

	//! Animation specific weight multiplier, applied on top of the existing layer weight.
	float m_fPlaybackWeight;

	//! Combination of flags defined above.
	unsigned int m_nFlags;

	// Transition Interpolation method.
	CA_Interpolation_Type m_nInterpolationType;

	//! Token specified by the animation calling code for it's own benefit.
	unsigned int m_nUserToken;

	//! a set of weights that are blended together just like the animation is, for calling code's benefit.
	float m_fUserData[NUM_ANIMATION_USER_DATA_SLOTS];

#if defined(USE_PROTOTYPE_ABS_BLENDING)
	const SJointMask* m_pJointMask;
#endif //!defined(USE_PROTOTYPE_ABS_BLENDING)
};

struct connection_request_t {
	char name[32];
};

struct animation_t {
	u32 animation_id;
	CryCharAnimationParams param;
};

struct player_t {
	char name[32];
	zpl_quat rotation;
	zpl_vec3 position;
	animation_t current_animation;
	CryCharAnimationParams anim_params;
	void* actor;
}; 