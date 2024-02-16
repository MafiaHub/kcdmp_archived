#pragma once

/*
* forward declaration of stuff from mp
*/
namespace mp
{
	void on_localplayer_animchanged(animation_t);
}

namespace graphics
{
	inline void draw_names();
}

/*
* Simple entity wrapper class
*/
class Entity {
public:
	void SetPos(const zpl_vec3 & pos) {
		(*(void(__fastcall **)(Entity*, const zpl_vec3 &, uint64_t, uint64_t, __int64))(VTABLE(0x118)))(this, pos, 0, 0, 0);
	}

	zpl_vec3 GetPos() {
		return *(*(zpl_vec3*(__fastcall **)(Entity*))( VTABLE(0x120) ))(this);
	}

	zpl_quat GetRot() {
		return *(*(zpl_quat*(__fastcall **)(Entity*))(VTABLE(0x130)))(this);
	}

	void SetPosRotScale(const zpl_vec3 & pos, const zpl_quat & rot, const zpl_vec3 scale) {

		(*(void(__fastcall **)(Entity*, const zpl_vec3 &, const zpl_quat &, const zpl_vec3 &, int))
			(VTABLE(0x148)))(
				this,
				pos,
				rot,
				scale,
				0
		);
	}
};

/*
* Simple player wrapper class
*/
class Actor {
public:
	Entity* GetEntity() {
		return *(Entity**)(this + 0x38);
	}
};

/*
* Simple function wich gets local player from cry engine :)
*/
Actor* GetLocalPlayer() {

	//get game pointer					 
	/*uint64_t g_Game = *(uint64_t*)(RESOLVE_ADDRESS(0x2CFE6E0));

	//get i game framework 
	uint64_t IGameFramework = (*(__int64(__fastcall **)(__int64))(*(__int64 *)g_Game + 0x80))(g_Game);

	//get client actor from game framework
	return  (*(Actor*(__fastcall **)(__int64))(*(__int64 *)IGameFramework + 0x220))(IGameFramework);;
	*/
	return local_horse;
}

/*
* Used whenever we need to get entity from lua scripting by id
*/
Actor* ActorSytem__GetActorByID(void* actorID) {

	//get game pointer					 
	uint64_t g_Game = *(uint64_t*)(RESOLVE_ADDRESS(0x2CFE6E0));

	//get i game framework 
	uint64_t IGameFramework = (*(__int64(__fastcall **)(__int64))(*(__int64 *)g_Game + 0x80))(g_Game);
	uint64_t ActorSystem = (*(__int64(__fastcall **)(__int64))(*(uint64_t *)IGameFramework + 0xC8))(IGameFramework);

	return (*(Actor*(__fastcall **)(__int64, uint64_t))(*(uint64_t *)ActorSystem + 0x18))(ActorSystem, (UINT_PTR)actorID);
}

/*
* Hooks typedes are global namespace 
*/
typedef __int64(__fastcall *Entity_GetCharacter_t)(Entity* _this, uint64_t val);
Entity_GetCharacter_t Entity_GetCharacter = nullptr;

typedef __int64(__fastcall *ISkeletonAnim__StartAnimationById_t)(uint64_t _this, int animID, uint64_t params);
ISkeletonAnim__StartAnimationById_t ISkeletonAnim__StartAnimationById = nullptr;

typedef __int64(__fastcall *DrawLabelEx_t)(uint64_t _this, const zpl_vec3 &, float, const zpl_vec4 & colors, bool unk1, bool unk2, const char* texPtr);
DrawLabelEx_t DrawLabelEx = nullptr;

/*
* Game hooks section 
*/
namespace game
{
	inline void draw_label(const zpl_vec3 & pos, float size, const zpl_vec4 & color, bool fixed_size, bool center, const char* text) {
		
		uint64_t addr = *(uint64_t*)(RESOLVE_ADDRESS(0x26DDCC0));
		if (addr) {
			DrawLabelEx(addr, pos, size, color, fixed_size, center, text);
		}
	}

	inline zpl_vec3 world_to_screen(zpl_vec3 position) {

		uint64_t addr = *(uint64_t*)(RESOLVE_ADDRESS(0x2C8F2B8));
		zpl_vec3 result;

		(*(void(__fastcall **)(__int64, float, float, float, float*, float*, float*))(*(uint64_t *)addr + 0x2B0))(
			addr,
			position.x,
			position.y,
			position.z,
			&result.x,
			&result.y,
			&result.z);

		return result;
	}

	inline bool skeleton_anim_from_remote(uint64_t anim, Actor* local_ped) {
		
		bool from_remote = false;
		mod_entity_iterate(&ctx, LIBRG_ENTITY_ALIVE, [&](librg_entity_t *entity) {
			if (entity && entity->user_data && local_ped != entity->user_data) {
				
				auto player = reinterpret_cast<player_t*>(entity->user_data);
				auto actorEntity = reinterpret_cast<Actor*>(player->actor)->GetEntity();

				uint64_t ICharacterInstance = Entity_GetCharacter(actorEntity, 0);
				uint64_t ISkeletonAnimation = (*(__int64(__fastcall **)(__int64))(*(uint64_t *)ICharacterInstance + 0x28))(ICharacterInstance);

				if (ISkeletonAnimation == anim)
					from_remote = true;
			}
		});

		return from_remote;
	}
	/* 
	* This hooks allows us to intercept animation states for every entity
	*/
	__int64 ISkeletonAnim__StartAnimationById__hook(uint64_t _this, int animID, uint64_t params) {

		__int64 return_val = 0;
		auto local_ped = GetLocalPlayer();
		if (local_ped != nullptr) {

			// compare sekeleton animation class with local one if equals its local player animation			
			uint64_t ICharacterInstance = Entity_GetCharacter(local_ped->GetEntity(), 0);
			uint64_t ISkeletonAnimation = (*(__int64(__fastcall **)(__int64))(*(uint64_t *)ICharacterInstance + 0x28))(ICharacterInstance);

			if (ISkeletonAnimation == _this) {

				CryCharAnimationParams ourParams = *(CryCharAnimationParams*)params;
				animation_t newAnimation = { static_cast<u32>(animID), ourParams };
				mp::on_localplayer_animchanged(newAnimation);
			}
		}

		if (!skeleton_anim_from_remote(_this, local_ped)) {
			return_val = ISkeletonAnim__StartAnimationById(_this, animID, params);
		}
		
		return return_val;
	}
	
	inline void install_hooks() {
	
		Entity_GetCharacter = (Entity_GetCharacter_t)RESOLVE_ADDRESS(0x2366D4);
		DrawLabelEx			= (DrawLabelEx_t)RESOLVE_ADDRESS(0x128E898);

		hooker::hook(reinterpret_cast<PVOID>(RESOLVE_ADDRESS(0x3C36BC)), &ISkeletonAnim__StartAnimationById__hook, reinterpret_cast<PVOID*>(&ISkeletonAnim__StartAnimationById));
		printf("[*] game::install_hooks()\n");
	}
}
