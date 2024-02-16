#pragma once
std::string lua_mp_script = R"(
--[[
	KCD On-foot synchronization
	animations are synced inside of c++
--]]

local defaultSpawn = {x=2860.6313, y=1773.0573, z=136.9086}

makeEntity = function(npcType, pos, soul) 
	local params = {}
	params.class = npcType
	params.name = "TestPlayer"
	params.position = pos
	params.scale    = {x=1, y=1, z=1}
	params.properties = {}
	if soul ~= nil then
		params.properties.sharedSoulGuid = soul
	end
	return System.SpawnEntity(params)
end

spawnActor = function()
	local newHorse = makeEntity('Horse', defaultSpawn, '4101d3da-859d-1f8d-cdbc-65a2621d7c95')
	local newRider = makeEntity('NPC', defaultSpawn, '40b64892-be82-9366-c837-b9b847a756bd')
	
	Script.SetTimer(500, function(timerId) 
		setupActor(newRider)
		newRider.human:ForceMount(newHorse.id)	
	end)
	
	return newHorse.id
end

addAndEquip = function(entity, stuff)  
	local id = entity.inventory:FindItem(stuff);  
	if (id == nil) then
		id = ItemManager.CreateItem(stuff, 1, 1);
		id = entity.inventory:AddItem(id);
	end

	entity.actor:EquipInventoryItem(id);
	entity.human:DrawWeapon();
end

setupActor = function(actor) 
	actor.inventory:RemoveAllItems(true)
	--actor:SetWorldPos(defaultSpawn)
	
	--Add default inventory & torch as bonus
	addAndEquip(actor, '425f0fc1-64b6-f3c1-e555-844714ae1581')
	addAndEquip(actor, '00000000-0000-0000-0000-00000000001c')
	addAndEquip(actor, '00000000-0000-0000-0000-00000000001c')
	addAndEquip(actor, '4f07bb72-5fa2-8f1e-dd38-3cd78b4785a6')
	addAndEquip(actor, '42e146c8-f2d1-1db1-c44c-7482b0f307b2')
	addAndEquip(actor, '4cea28a0-0814-405a-bf24-4fd711f7eb63')
end

--Night set and setup local player
Calendar.SetWorldTime(3600*24+3600*24.5)
setupActor(g_localActor)

getMyHorse = function() 
	local myHorse = makeEntity('Horse', defaultSpawn, '4669b957-cd91-b597-2f2a-977ba81d1c80')
	g_localActor.human:ForceMount(myHorse.id)
	return myHorse.id	
end

)";