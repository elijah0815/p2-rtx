#include "std_include.hpp"

namespace components
{
	void __fastcall table_hooks::DrawModelExecute::Detour(void* ecx, void* edx, void* oo, const DrawModelState_t* state, const ModelRenderInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		table_hooks::table.original<FN>(Index)(ecx, edx, oo, state, pInfo, pCustomBoneToWorld);
	}

	model_render::model_render()
	{
		table_hooks::_interface = utils::module_interface.get<table_hooks::IVModelRender*>("engine.dll", "VEngineModel016");

		XASSERT(table_hooks::table.init(table_hooks::_interface) == false);
		XASSERT(table_hooks::table.hook(&table_hooks::DrawModelExecute::Detour, table_hooks::DrawModelExecute::Index) == false);
	}
}
