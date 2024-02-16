#pragma once
namespace hooker
{
	inline void free()
	{
		if (MH_DisableHook(MH_ALL_HOOKS)) { return; };
		if (MH_Uninitialize() != MB_OK)
		{
			MessageBoxA(NULL, "Hooker failed to free !", "Error", MB_OK);
		}
	}

    inline void init()
    {
        if (MH_Initialize() != MH_OK)
        {
            MessageBoxA(NULL, "Hooker failed to init!", "Error", MB_OK);
            return; 
		}
		else printf("Hooked initialized !\n");
    }

    inline void hook(void* target, void* detour, void** original)
    {
        if (MH_CreateHook(target, detour, original) != MH_OK)
        {
            MessageBoxA(NULL, "Hooker failed to hook !", "Error", MB_OK);
            return;
        }

        if (MH_EnableHook(target) != MH_OK)
        {
            MessageBoxA(NULL, "Hooker failed to enable hook hook !", "Error", MB_OK);
            return;
        }
    }
}