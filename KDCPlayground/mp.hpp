#pragma once
namespace mp {

	bool librg_inited = false;
	

	/* 
	* Calls lua function which spawns actor and save it into actor pool under entityId key
	*/
	inline Actor* get_horse() {

		//call fetchLocalPlayer function
		lua_getglobal(lua::L, "getMyHorse");

		//call function
		lua_pcall(lua::L, 0, 1, 0);

		Actor* return_pointer = nullptr;

		void* rawActor = (void*)lua_topointer(lua::L, -1);
		if (rawActor) {
			return_pointer = ActorSytem__GetActorByID(rawActor);
		}

		lua_pop(lua::L, 1);
		return return_pointer;
	}

	inline Actor* spawn_actor(int net_id) {
		
		//call fetchLocalPlayer function
		lua_getglobal(lua::L, "spawnActor");

		//call function
		lua_pushinteger(lua::L, net_id);
		lua_pcall(lua::L, 1, 1, 0);

		Actor* return_pointer = nullptr;

		void* rawActor = (void*)lua_topointer(lua::L, -1);
		if (rawActor) {
			return_pointer =  ActorSytem__GetActorByID(rawActor);
		}

		lua_pop(lua::L, 1);
		return return_pointer;
	}

	inline void remove_actor(int net_id) {

		//call fetchLocalPlayer function
		lua_getglobal(lua::L, "removeActor");

		//call function
		lua_pushinteger(lua::L, net_id);
		lua_pcall(lua::L, 1, 0, 0);
	}

	inline void update_actor(Actor* actor, zpl_vec3 pos, zpl_quat rot) {

		if (GetLocalPlayer() && GetLocalPlayer() == actor)
			return;

		zpl_vec3 default_scale = { 1.0f, 1.0f, 1.0 };
		actor->GetEntity()->SetPosRotScale(pos, rot, default_scale);
	}
	
	inline void playanim_actor(Actor* actor, animation_t animation) {

		uint64_t ICharacterInstance = Entity_GetCharacter(actor->GetEntity(), 0);
		if (ICharacterInstance) {
			uint64_t ISkeletonAnimation = (*(__int64(__fastcall **)(__int64))(*(uint64_t *)ICharacterInstance + 0x28))(ICharacterInstance);
			ISkeletonAnim__StartAnimationById(ISkeletonAnimation, animation.animation_id, (uint64_t)&animation.param);
		}
	}

	/*
	* Librg events
	*/
	void on_librg_connect(librg_event_t* evnt) {
		printf("[*] Connected !\n");
	}

	void on_librg_entity_create(librg_event_t* evnt) {

		auto player = new player_t();
		librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_quat));
		librg_data_rptr(evnt->data, &player->current_animation, sizeof(animation_t));
		librg_data_rptr(evnt->data, &player->name, sizeof(char) * 32);

		//spawn entity & update it to right position
		player->actor = (void*)spawn_actor(evnt->entity->id);
		auto actor = reinterpret_cast<Actor*>(player->actor);

		update_actor(actor, evnt->entity->position, player->rotation);
		playanim_actor(actor, player->current_animation);

		evnt->entity->user_data = (void*)player;
	}

	void on_librg_entity_update(librg_event_t* evnt) {

		auto player = (player_t*)evnt->entity->user_data;
		player->position = evnt->entity->position;
		librg_data_rptr(evnt->data, &player->rotation, sizeof(zpl_quat));
	}

	/* 
	* For now as remove we use teleport to 0, 0, 0, we should improove it
	*/
	void on_librg_entity_remove(librg_event_t* evnt) {
		auto player = reinterpret_cast<player_t*>(evnt->entity->user_data);
		
		if (player) {
			auto entity = reinterpret_cast<Actor*>(player->actor)->GetEntity();
			entity->SetPos(zpl_vec3f_zero());
			
			player = nullptr;
			delete player;
		}
	}

	void on_librg_clientstreamer_update(librg_event_t* evnt) {
	
		auto local_entity = GetLocalPlayer()->GetEntity();
		
		
		auto rotation = local_entity->GetRot();
		evnt->entity->position = local_entity->GetPos();
			
		librg_data_wptr(evnt->data, &rotation, sizeof(zpl_quat));
	}

	/* 
	* Called when player changes animation state
	*/
	inline void on_localplayer_animchanged(animation_t animation) {
		mod_message_send(&ctx, MSG_PLAYER_ONANIMCHANGE, [&](librg_data_t *data) {
			librg_data_wptr(data, &animation, sizeof(animation_t));
		});
	}

	/*
	*  Librg connect to server
	*/
	inline void connect() {
		
		local_horse = get_horse();
		PRINT_HEX("local_horse", local_horse);

		librg_address_t addr = { 7777, (char*)GlobalConfig.server_address.c_str() };
		librg_network_start(&ctx, addr);
	}

	/*
	* Librg init
	*/
	inline void on_init() {

		//Init librg
		ctx.mode = LIBRG_MODE_CLIENT;
		ctx.max_entities = 16;
		ctx.tick_delay = 32;
		ctx.world_size = { 5000.0f, 5000.0f, 0.0f };

		librg_init(&ctx);
		librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_librg_connect);
		librg_event_add(&ctx, LIBRG_ENTITY_CREATE, on_librg_entity_create);
		librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, on_librg_entity_update);
		librg_event_add(&ctx, LIBRG_ENTITY_REMOVE, on_librg_entity_remove);
		librg_event_add(&ctx, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);

		librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, [](librg_event_t *evnt) {
			char nickname[32];
			strcpy(nickname, GlobalConfig.username.c_str());
			librg_data_wptr(evnt->data, nickname, sizeof(char) * 32);
		});

		librg_network_add(&ctx, MSG_PLAYER_ONANIMCHANGE, [](librg_message_t *msg) {
			
			u32 entity_id = librg_data_ru32(msg->data);
			animation_t animation;
			librg_data_rptr(msg->data, &animation, sizeof(animation_t));

			auto geted_entity = librg_entity_fetch(&ctx, entity_id);
			if (geted_entity && geted_entity->user_data) {

				auto player_data = reinterpret_cast<player_t*>(geted_entity->user_data);
				auto geted_actor = reinterpret_cast<Actor*>(player_data->actor);
				playanim_actor(geted_actor, animation);
			}
		});

		librg_inited = true;
	}


	void CQuaternion__Slerp(zpl_quat* out, zpl_quat a, zpl_quat b, float t)
	{
		// benchmarks:
		//    http://jsperf.com/quaternion-slerp-implementations

		float ax = a.x, ay = a.y, az = a.z, aw = a.w,
			bx = b.x, by = b.y, bz = b.z, bw = b.w;

		float omega, cosom, sinom, scale0, scale1;

		// calc cosine
		cosom = ax * bx + ay * by + az * bz + aw * bw;
		// adjust signs (if necessary)
		if (cosom < 0.0)
		{
			cosom = -cosom;
			bx = -bx;
			by = -by;
			bz = -bz;
			bw = -bw;
		}
		// calculate coefficients
		if ((1.0 - cosom) > 0.000001)
		{
			// standard case (slerp)
			omega = acosf(cosom);
			sinom = sinf(omega);
			scale0 = sinf((1.0 - t) * omega) / sinom;
			scale1 = sinf(t * omega) / sinom;
		}
		else
		{
			// "from" and "to" quaternions are very close
			//  ... so we can do a linear interpolation
			scale0 = 1.0 - t;
			scale1 = t;
		}

		// calculate final values
		out->x = scale0 * ax + scale1 * bx;
		out->y = scale0 * ay + scale1 * by;
		out->z = scale0 * az + scale1 * bz;
		out->w = scale0 * aw + scale1 * bw;
	}

	/*
	* Interpolate all our entities
	*/
	void interpolate_entities(float delta_time) {
		// interpolate all entities
		librg_entity_iterate(&ctx, LIBRG_ENTITY_ALIVE, [](librg_ctx_t *ctx, librg_entity_t *entity) {
			if (entity && entity->user_data) {
				auto player = reinterpret_cast<player_t*>(entity->user_data);
				auto actor = reinterpret_cast<Actor*>(player->actor);

				// interpolate position
				zpl_vec3 interpolated_pos;
				zpl_vec3 current_pos = actor->GetEntity()->GetPos();
				zpl_vec3_lerp(&interpolated_pos, current_pos, player->position, 0.4f);

				// interpolate rotation
				zpl_quat interpolated_rot;
				zpl_quat current_rot = actor->GetEntity()->GetRot();
				CQuaternion__Slerp(&interpolated_rot, current_rot, player->rotation, 0.4f);

				// update actor with interpolated pos :)
				update_actor(actor, interpolated_pos, interpolated_rot);
			}
		});
	}

	/* 
	* Tick our librg here 
	*/
	float last_time = 0;
	inline void on_tick() {
		
		float delta_time = zpl_time_now() - last_time;
		if (librg_inited) {
			librg_tick(&ctx);
			interpolate_entities(delta_time);
		}
				
		graphics::draw_names();

		last_time = zpl_time_now();
	}
}