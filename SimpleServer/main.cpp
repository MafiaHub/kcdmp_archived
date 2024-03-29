#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define LIBRG_IMPLEMENTATION
#include <librg.h>
#include <iostream>
#include <functional>
#include <librg_ext.h>

//bleeee
#include "../KDCPlayground/structs.hpp"

librg_ctx_t ctx = { 0 };
connection_request_t connection_request = { 0 };

void on_librg_connection_request(librg_event_t* evnt) {
	librg_data_rptr(evnt->data, connection_request.name, sizeof(char) * 32);
}

void on_librg_connection_accept(librg_event_t *event) {
	
	librg_entity_control_set(event->ctx, event->entity->id, event->peer);
	//----------[ INIT NEW PLAYER ] -----------
	auto new_player = new player_t();
	new_player->rotation = { 0.0f, 0.0f, 0.3f, 0.0f };
	strcpy(new_player->name, connection_request.name);
	//-----------------------------------------

	event->entity->user_data = (void*)new_player;
	librg_log("[*] <%s> joined server\n", new_player->name);
}

void on_librg_connection_disconnect(librg_event_t *event) {
	auto player = reinterpret_cast<player_t*>(event->entity->user_data);
	librg_log("[*] <%s> disconnected\n", player->name);
}

void on_librg_entityupdate(librg_event_t* evnt) {
	auto player = (player_t *)evnt->entity->user_data;
	librg_data_wptr(evnt->data, &player->rotation, sizeof(zpl_quat));
}

void on_librg_entitycreate(librg_event_t* evnt) {
	auto player = (player_t *)evnt->entity->user_data;
	librg_data_wptr(evnt->data, &player->rotation, sizeof(zpl_quat));
	librg_data_wptr(evnt->data, &player->current_animation, sizeof(animation_t));
	librg_data_wptr(evnt->data, player->name, sizeof(char) * 32);
}

void on_librg_clientstreamer_update(librg_event_t *event) {
	auto player = (player_t*)event->entity->user_data;
	librg_data_rptr(event->data, &player->rotation, sizeof(zpl_quat));
}

int main() {

  printf("KCD MP, yep\n");

	ctx.mode = LIBRG_MODE_SERVER;
	ctx.tick_delay = 32;
	ctx.world_size = zpl_vec3f(50000.0f, 50000.0f, 0.f);
	ctx.max_connections = 128;
	ctx.max_entities = 16000,

	librg_init(&ctx);
	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_librg_connection_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_librg_connection_accept);
	librg_event_add(&ctx, LIBRG_CONNECTION_DISCONNECT, on_librg_connection_disconnect);
	librg_event_add(&ctx, LIBRG_CLIENT_STREAMER_UPDATE, on_librg_clientstreamer_update);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, on_librg_entityupdate);
	librg_event_add(&ctx, LIBRG_ENTITY_CREATE, on_librg_entitycreate);
	
	librg_network_add(&ctx, MSG_PLAYER_ONANIMCHANGE, [](librg_message_t *msg) {
		auto entity = librg_entity_find(&ctx, msg->peer);
		auto player = reinterpret_cast<player_t*>(entity->user_data);
		if (player) {
			librg_data_rptr(msg->data, &player->current_animation, sizeof(animation_t));

			mod_message_send_except(&ctx, MSG_PLAYER_ONANIMCHANGE, msg->peer, [&](librg_data_t* data) {
				librg_data_wu32(data, entity->id);
				librg_data_wptr(data, &player->current_animation, sizeof(animation_t));
			});
		}
	});

	librg_address_t addr = { 7777, NULL };
	librg_network_start(&ctx, addr);

	while (true) {
		librg_tick(&ctx);
		zpl_sleep_ms(1);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);
    return 0;
}