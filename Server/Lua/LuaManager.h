#ifndef __LUA_MANAGER_H__
#define __LUA_MANAGER_H__

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#pragma comment(lib, "lua5.1.lib")

class LuaManager
{
public:
	LuaManager(void);
	~LuaManager(void);

public:
	BOOL	Close();

	BOOL	Init();

	BOOL	Attach(lua_State* L);

	BOOL	Deattch();

	BOOL	LoadAllLua(const char* pszDir);

	BOOL	LoadScriptFile(const char* pszLuaFile);

	BOOL	LoadScriptFile(std::vector<std::string>& vtScriptList);

	BOOL	CallLuaFunction(std::string strFuncName, char* pStrParamSig, ...);

	BOOL	GetStackParams(char* pStrParamSig, ...);

	BOOL    RegisterFunction(const char* libname, const luaL_Reg* l);

	BOOL	RegisterFunction(const char* name, lua_CFunction fn);

	//��ȡ��ջ�ű�����
	BOOL	GetStackValue_Ptr(INT32 nStackIndex, VOID*& ptrValue);
	BOOL	GetStackValue_Int(INT32 nStackIndex, INT32& intValue);
	BOOL	GetStackValue_Double(INT32 nStackIndex, DOUBLE& doubleValue);
	BOOL    GetStackValue_String(INT32 nStackIndex, const CHAR*& strValue);

	//��ȡȫ�ֽű�����
	INT32	GetGlobalVarInt(const char* pszVarName);
	BOOL	GetGlobalVarBoolean(const char* pszVarName);
	DOUBLE  GetGlobalVarDouble(const char* pszVarName);
	const CHAR*	GetGlobalVarString(const char* pszVarName);
	lua_State* GetLuaState();

protected:
	lua_State*			m_pLuaState;
};

#endif