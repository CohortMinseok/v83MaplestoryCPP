
#include "script_handler.h"

#include "player.hpp"
#include "map.hpp"
#include "playernpc.hpp"
#include "session.hpp"
#include "constants.hpp"

#include <angelscript.h>
#include "scriptstdstring\scriptstdstring.h"
#include "scriptarray\scriptarray.h"

// Implement a simple message callback function for as
void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void as_function_dummy()
{

}

// constants for as

const int as_YES_CONSTANT = kNpcConstantsYes;
const int as_NO_CONSTANT = kNpcConstantsNo;

const int as_EQUIP = kInventoryConstantsTypesEquip;
const int as_USE = kInventoryConstantsTypesUse;
const int as_SETUP = kInventoryConstantsTypesSetup;
const int as_ETC = kInventoryConstantsTypesEtc;
const int as_CASH = kInventoryConstantsTypesCash;

static asIScriptEngine *engine = nullptr;

namespace script_engine
{
	void initialize_angelscript_engine()
	{
		int r;

		// Set the message callback when creating the engine
		engine = asCreateScriptEngine();
		r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		// Register the string type
		RegisterStdString(engine);

		// Register arrays
		RegisterScriptArray(engine, true);

		r = engine->RegisterGlobalProperty("const int YES", (void*)&as_YES_CONSTANT); assert(r >= 0);
		r = engine->RegisterGlobalProperty("const int NO", (void*)&as_NO_CONSTANT); assert(r >= 0);
		r = engine->RegisterGlobalProperty("const int EQUIP", (void*)&as_EQUIP); assert(r >= 0);
		r = engine->RegisterGlobalProperty("const int USE", (void*)&as_USE); assert(r >= 0);
		r = engine->RegisterGlobalProperty("const int SETUP", (void*)&as_SETUP); assert(r >= 0);
		r = engine->RegisterGlobalProperty("const int ETC", (void*)&as_ETC); assert(r >= 0);
		r = engine->RegisterGlobalProperty("const int CASH", (void*)&as_CASH); assert(r >= 0);

		// Player
		r = engine->RegisterObjectType("Player", sizeof(Player), asOBJ_REF); assert(r >= 0);

		r = engine->RegisterObjectBehaviour("Player", asBEHAVE_ADDREF, "void f()", asFUNCTION(as_function_dummy), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Player", asBEHAVE_RELEASE, "void f()", asFUNCTION(as_function_dummy), asCALL_CDECL_OBJLAST); assert(r >= 0);

		// Map
		r = engine->RegisterObjectType("Map", sizeof(Map), asOBJ_REF); assert(r >= 0);

		r = engine->RegisterObjectBehaviour("Map", asBEHAVE_ADDREF, "void f()", asFUNCTION(as_function_dummy), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Map", asBEHAVE_RELEASE, "void f()", asFUNCTION(as_function_dummy), asCALL_CDECL_OBJLAST); assert(r >= 0);

		// functions
		r = engine->RegisterObjectMethod("Player", "int get_selected()", asMETHOD(Player, get_selected), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "void set_selected(int)", asMETHOD(Player, set_selected), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "void send_simple(string)", asMETHOD(Player, send_simple), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "void send_ok(string)", asMETHOD(Player, send_ok), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "void set_npc_variable(string, int)", asMETHOD(Player, set_npc_variable), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "int get_npc_variable(string)", asMETHOD(Player, get_npc_variable), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "int get_mesos()", asMETHOD(Player, get_mesos), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "bool add_mesos(int)", asMETHOD(Player, add_mesos), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "void set_state(int)", asMETHOD(Player, set_state), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "int get_state()", asMETHOD(Player, get_state), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "void send_yes_no(string)", asMETHOD(Player, send_yes_no), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "void set_map(int)", asMETHOD(Player, set_map), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Player", "Map @get_map()", asMETHOD(Player, get_map), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Map", "int get_id()", asMETHOD(Map, get_id), asCALL_THISCALL); assert(r >= 0);
	}
}

void Player::npc_script_handler()
{
	int npc_id = npc_->id_;

	std::string npc_id_string = std::to_string(npc_id);

	// START OF LOADING AND BUILDING

	std::string file_name_string = "scripts\\npcs\\" + npc_id_string + ".as";

	// Load the entire script file into a string buffer

	// Open the file in binary mode
	FILE *f = fopen(file_name_string.c_str(), "rb");

	if (!f)
	{
		return;
	}

	// Determine the size of the file
	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Load and add the script sections to the module
	std::string script;

	// Load the entire file in one call
	script.resize(len);
	fread(&script[0], len, 1, f);

	fclose(f);

	// Create a new script module
	asIScriptModule *mod = engine->GetModule(file_name_string.c_str(), asGM_ALWAYS_CREATE);
	mod->AddScriptSection(file_name_string.c_str(), script.c_str());

	// Build the module
	int r = mod->Build();
	if (r < 0)
	{
		// The build failed. The message stream will have received
		// compiler errors that shows what needs to be fixed
	}

	// END OF LOADING AND BUILDING

	// START OF PREPARING AND EXECUTING

	// Get a script context instance. Usually you'll want to reuse a previously
	// created instance to avoid the overhead of allocating the instance with
	// each call.
	asIScriptContext *ctx = engine->CreateContext();
	// Obtain the function from the module. This should preferrably  
	// be cached if the same function is called multiple times.
	asIScriptFunction *func = engine->GetModule(file_name_string.c_str())->GetFunctionByName("main");
	// Prepare() must be called to allow the context to prepare the stack
	ctx->Prepare(func);
	// Set the function arguments
	ctx->SetArgObject(0, this);
	r = ctx->Execute();
	// Release the context when you're done with it
	ctx->Release();

	// END OF PREPARING AND EXECUTING
}
