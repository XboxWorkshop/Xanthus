#include <gpu/resources.h>
#include "render_commands.h"
#include "guest_convert.h"
#include <kernel/function.h>


void AddDestructResource(GuestResource* ressource)
{
	RenderCommand cmd;
	cmd.type = RenderCommandType::DestructResource;
	cmd.destructResource.resource = ressource;
	SendRenderCommand(cmd);
}

void DestructResource(GuestResource* resource)
{
	AddDestructResource(resource);
}

#if XANTHUS_RECOMP
GUEST_FUNCTION_HOOK_STDCALL(sub_3D5DB0, DestructResource);
#endif