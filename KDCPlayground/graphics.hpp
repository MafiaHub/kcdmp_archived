#pragma once
namespace game
{
	inline zpl_vec3 world_to_screen(zpl_vec3 position);
	inline void draw_label(const zpl_vec3 & pos, float size, const zpl_vec4 & color, bool fixed_size, bool center, const char* text);
}

namespace graphics
{
	inline void draw_names() {

		mod_entity_iterate(&ctx, LIBRG_ENTITY_ALIVE, [&](librg_entity_t *entity) {
			
			if (entity && entity->user_data) {

				auto player			= reinterpret_cast<player_t*>(entity->user_data);
				auto actor			= reinterpret_cast<Actor*>(player->actor);
				auto local_player	= GetLocalPlayer();

				if (local_player && local_player != actor) {

					auto player_pos = actor->GetEntity()->GetPos();
					auto local_pos	= local_player->GetEntity()->GetPos();

					zpl_vec3 sub_vec;
					zpl_vec3_sub(&sub_vec, player_pos, local_pos);
					float distance = zpl_vec3_mag(sub_vec);
					
					if (distance < 15.0f) {	

						player_pos.z += 1.9f;
						zpl_vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
						game::draw_label(player_pos, 1.5f, color, true, true, player->name);
					}
				}
			}
		});
	}
}