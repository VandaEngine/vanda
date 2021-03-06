//Copyright (C) 2021 Ehsan Kamrani 
//This file is licensed and distributed under MIT license

#include "stdafx.h"
#include "main.h"
#include "GraphicsEngine/Animation.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <iostream>
#include <sstream>
#include "graphicsEngine\\imagelib.h"
#include <thread>
//Vanda C Functions to be used in Lua scripts
CGeometry* GetGeometryFromScenes(const CChar * name, const CChar * DocURI)
{
	for (CUInt i = 0; i<g_geometries.size(); i++)
	{
		if (ICmp(g_geometries[i]->GetName(), name) &&
			ICmp(g_geometries[i]->GetDocURI(), DocURI))
			return g_geometries[i];
	}
	return NULL;
}

CInt PlaySoundLoop(lua_State *L)
{
	int argc = lua_gettop(L);


	for (int n = 1; n <= argc; ++n)
	{
		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, n));
		StringToUpper(luaToString);
		CBool foundTarget = CFalse;
		for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
		{
			CChar soundName[MAX_NAME_SIZE];
			Cpy(soundName, g_engineStaticSounds[i]->GetName());
			StringToUpper(soundName);

			if (Cmp(soundName, luaToString))
			{
				g_engineStaticSounds[i]->GetSoundSource()->SetLooping(CTrue);
				g_engineStaticSounds[i]->SetLoop( CTrue );
				g_engineStaticSounds[i]->SetPlay(CTrue);
				g_soundSystem->PlayALSound(*(g_engineStaticSounds[i]->GetSoundSource()));
				break;
			}
		}
	}
	return 0; // number of return values
}

CInt PlaySoundOnce(lua_State *L)
{
	int argc = lua_gettop(L);


	for (int n = 1; n <= argc; ++n)
	{
		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, n));
		StringToUpper(luaToString);
		CBool foundTarget = CFalse;
		for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
		{
			CChar soundName[MAX_NAME_SIZE];
			Cpy(soundName, g_engineStaticSounds[i]->GetName());
			StringToUpper(soundName);

			if (Cmp(soundName, luaToString))
			{
				g_engineStaticSounds[i]->GetSoundSource()->SetLooping(CFalse);
				g_engineStaticSounds[i]->SetLoop(CFalse);
				g_engineStaticSounds[i]->SetPlay(CTrue);
				g_soundSystem->PlayALSound(*(g_engineStaticSounds[i]->GetSoundSource()));
				break;
			}
		}
	}
	return 0; // number of return values
}


CInt PauseSound(lua_State *L)
{
	int argc = lua_gettop(L);

	for (int n = 1; n <= argc; ++n)
	{
		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, n));
		StringToUpper(luaToString);
		CBool foundTarget = CFalse;
		for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
		{
			CChar soundName[MAX_NAME_SIZE];
			Cpy(soundName, g_engineStaticSounds[i]->GetName());
			StringToUpper(soundName);

			if (Cmp(soundName, luaToString))
			{
				g_engineStaticSounds[i]->SetPlay(CFalse);
				g_soundSystem->PauseALSound(*(g_engineStaticSounds[i]->GetSoundSource()));
				break;
			}
		}
	}
	return 0; // number of return values
}

CInt StopSound(lua_State *L)
{
	int argc = lua_gettop(L);

	for (int n = 1; n <= argc; ++n)
	{
		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, n));
		StringToUpper(luaToString);
		CBool foundTarget = CFalse;
		for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
		{
			CChar soundName[MAX_NAME_SIZE];
			Cpy(soundName, g_engineStaticSounds[i]->GetName());
			StringToUpper(soundName);

			if (Cmp(soundName, luaToString))
			{
				g_engineStaticSounds[i]->SetPlay(CFalse);
				g_soundSystem->StopALSound(*(g_engineStaticSounds[i]->GetSoundSource()));
				break;
			}
		}
	}
	return 0; // number of return values
}

CInt BlendCycle(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for BlendCycle()", COLOR_RED);
		return 0;
	}
	CScene* scene = NULL;
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp(luaToString, "THIS"))
	{
		if (g_currentInstancePrefab)
		{
			CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_currentInstancePrefab->GetSceneVisible(j)*/)
				{
					scene = g_currentInstancePrefab->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}
					scene->BlendCycle(index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber(L, 4));
				}
			}
			if (!scene)
			{
				//CChar temp[MAX_NAME_SIZE];
				//sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
				//PrintInfo(temp, COLOR_RED);
				return 0;
			}
		}
		else
		{
			//PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_instancePrefab[i]->GetSceneVisible(j)*/)
				{
					scene = g_instancePrefab[i]->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}
					scene->BlendCycle(index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber(L, 4));
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt ClearCycle(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		//PrintInfo("\nPlease specify 3 arguments for ClearCycle()", COLOR_RED);
		return 0;
	}
	CScene* scene = NULL;
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp(luaToString, "THIS"))
	{
		if (g_currentInstancePrefab)
		{
			CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_currentInstancePrefab->GetSceneVisible(j)*/)
				{
					scene = g_currentInstancePrefab->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->ClearCycle(index, (CFloat)lua_tonumber(L, 3));
				}
			}
			if (!scene)
			{
				//CChar temp[MAX_NAME_SIZE];
				//sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
				//PrintInfo(temp, COLOR_RED);
				return 0;
			}
		}
		else
		{
			//PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_instancePrefab[i]->GetSceneVisible(j)*/)
				{
					scene = g_instancePrefab[i]->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->ClearCycle(index, (CFloat)lua_tonumber(L, 3));
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt ExecuteAction(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for ExecuteAction()", COLOR_RED);
		return 0;
	}
	CScene* scene = NULL;
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp(luaToString, "THIS"))
	{
		if (g_currentInstancePrefab)
		{
			CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_currentInstancePrefab->GetSceneVisible(j)*/)
				{
					scene = g_currentInstancePrefab->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					CFloat weight = 1.0f;
					CInt lockResult;
					CBool lock = CFalse;
					if (argc > 4)
						weight = lua_tonumber(L, 5);
					if (argc > 5)
						lockResult = lua_toboolean(L, 6);
					if (lockResult)
						lock = CTrue;
					else
						lock = CFalse;
					scene->ExecuteAction(index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber(L, 4), weight, lock);
				}
			}
			if (!scene)
			{
				//CChar temp[MAX_NAME_SIZE];
				//sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
				//PrintInfo(temp, COLOR_RED);
				return 0;
			}
		}
		else
		{
			//PrintInfo("\nCouldn't find current instance prefab", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_instancePrefab[i]->GetSceneVisible(j)*/)
				{
					scene = g_instancePrefab[i]->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					CFloat weight = 1.0f;
					CInt lockResult;
					CBool lock = CFalse;
					if (argc > 4)
						weight = lua_tonumber(L, 5);
					if (argc > 5)
						lockResult = lua_toboolean(L, 6);
					if (lockResult)
						lock = CTrue;
					else
						lock = CFalse;
					scene->ExecuteAction(index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber(L, 4), weight, lock);
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt ReverseExecuteAction(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		//PrintInfo("\nPlease specify 2 arguments for ReverseExecuteAction()", COLOR_RED);
		return 0;
	}
	CScene* scene = NULL;
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp(luaToString, "THIS"))
	{
		if (g_currentInstancePrefab)
		{
			CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j)/* && g_currentInstancePrefab->GetSceneVisible(j)*/)
				{
					scene = g_currentInstancePrefab->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->ReverseExecuteAction(index);
				}
			}
			if (!scene)
			{
				//CChar temp[MAX_NAME_SIZE];
				//sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
				//PrintInfo(temp, COLOR_RED);
				return 0;
			}
		}
		else
		{
			//PrintInfo("\nCouldn't find current instance prefab", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j)/* && g_instancePrefab[i]->GetSceneVisible(j)*/)
				{
					scene = g_instancePrefab[i]->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->ReverseExecuteAction(index);
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First argument: prefab instance name
//Second argument: animation clip name
CInt RemoveAction(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		//PrintInfo("\nPlease specify 2 arguments for RemoveAction()", COLOR_RED);
		return 0;
	}
	CScene* scene = NULL;
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp(luaToString, "THIS"))
	{
		if (g_currentInstancePrefab)
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_currentInstancePrefab->GetSceneVisible(j)*/)
				{
					scene = g_currentInstancePrefab->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->RemoveAction(index);
				}
			}
			if (!scene)
			{
				//CChar temp[MAX_NAME_SIZE];
				//sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
				//PrintInfo(temp, COLOR_RED);
				return 0;
			}
		}
		else
		{
			//PrintInfo("\nCouldn't find current instance prefab", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_instancePrefab[i]->GetSceneVisible(j)*/)
				{
					scene = g_instancePrefab[i]->GetScene(j);
					scene->SetAnimationStatus(eANIM_PLAY);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->RemoveAction(index);
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First Argument: Prefab Instance Name
//Second Argument: Animation Clip Name
CInt GetAnimationClipDuration(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		//PrintInfo("\nPlease specify 2 arguments for GetAnimationClipDuration()", COLOR_RED);
		return 0;
	}
	CScene* scene = NULL;
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp(luaToString, "THIS"))
	{
		if (g_currentInstancePrefab)
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_currentInstancePrefab->GetSceneVisible(j)*/)
				{
					scene = g_currentInstancePrefab->GetScene(j);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					lua_pushnumber(L, scene->m_animationClips[index]->GetDuration());
					return 1;
				}
			}
		}
		else
		{
			//PrintInfo("\nCouldn't find current instance prefab", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_instancePrefab[i]->GetSceneVisible(j)*/)
				{
					scene = g_instancePrefab[i]->GetScene(j);

					CChar luaToString2[MAX_NAME_SIZE];
					Cpy(luaToString2, lua_tostring(L, 2));
					StringToUpper(luaToString2);

					CBool foundAnimationTarget = CFalse;
					CInt index;
					for (CInt k = 0; k < scene->GetNumClips(); k++)
					{
						CChar animationName[MAX_NAME_SIZE];
						Cpy(animationName, scene->m_animationClips[k]->GetName());
						StringToUpper(animationName);
						if (Cmp(luaToString2, animationName))
						{
							index = k;
							foundAnimationTarget = CTrue;
							break;
						}
					}
					if (!foundAnimationTarget)
					{
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						//PrintInfo(temp, COLOR_RED);
						return 0;
					}

					lua_pushnumber(L, scene->m_animationClips[index]->GetDuration());
					return 1;
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}


CInt ShowPrefabInstance(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 arguments for ShowPrefabInstance()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
			g_currentInstancePrefab->SetVisible(CTrue);
		//else
		//	PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->SetVisible(CTrue);
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt HidePrefabInstance(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 arguments for HidePrefabInstance()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
			g_currentInstancePrefab->SetVisible(CFalse);
		//else
		//	PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->SetVisible(CFalse);
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt PauseAnimations(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 arguments for PauseAnimations()", COLOR_RED);
		return 0;
	}
	CScene* scene = NULL;
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp(luaToString, "THIS"))
	{
		if (g_currentInstancePrefab)
		{
			CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j)/* && g_currentInstancePrefab->GetSceneVisible(j)*/)
				{
					scene = g_currentInstancePrefab->GetScene(j);
					scene->SetAnimationStatus(eANIM_PAUSE);
				}
			}
			if (!scene)
			{
				//CChar temp[MAX_NAME_SIZE];
				//sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
				//PrintInfo(temp, COLOR_RED);
				return 0;
			}
		}
		else
		{
			//PrintInfo("\nCouldn't find current instance prefab", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j)/* && g_instancePrefab[i]->GetSceneVisible(j)*/)
				{
					scene = g_instancePrefab[i]->GetScene(j);
					scene->SetAnimationStatus(eANIM_PAUSE);
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt LoadVScene(lua_State *L)
{
	//if( g_testScript )
	//	return 0;
	g_clickedNew = g_clickedOpen = CTrue;

	int argc = lua_gettop(L);
	if( argc < 1 )
	{
		//PrintInfo( "\nPlease specify 1 argument for LoadVScene()", COLOR_RED );
		return 0;
	}
	Cpy( g_currentVSceneNameViaScript, lua_tostring(L, 1) );
	g_loadSceneViaScript = CTrue;
	return 0;
}

CInt SetCurrentVSceneAsMenu(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		//PrintInfo("\nPlease specify 3 arguments for SetCurrentVSceneAsMenu()", COLOR_RED);
		return 0;
	}
	CInt menuResult;
	CBool menu = CFalse;
	menuResult = lua_toboolean(L, 1); //true or false
	if (menuResult)
		menu = CTrue;
	else
		menu = CFalse;
	g_currentVSceneProperties.m_isMenu = menu;

	CInt pauseResult;
	CBool pause;
	pauseResult = lua_toboolean(L, 2); //true or false
	if (pauseResult)
		pause = CTrue;
	else
		pause = CFalse;
	g_currentVSceneProperties.m_isPause = pause;
	if (!g_currentVSceneProperties.m_isPause)
	{
		g_timer->GetElapsedSeconds(CTrue);

		if (g_main->m_tempAllPlayingSoundSources.size() != 0)
		{
			for (CUInt i = 0; i < g_main->m_tempAllPlayingSoundSources.size(); i++)
			{
				//resume matched resource sounds
				for (CUInt j = 0; j < g_resourceFiles.size(); j++)
				{
					if (g_resourceFiles[j]->GetSoundSource())
					{
						if (Cmp(g_main->m_tempAllPlayingSoundSources[i].c_str(), g_resourceFiles[j]->GetSoundSource()->GetName()))
							g_soundSystem->PlayALSound(*(g_resourceFiles[j]->GetSoundSource()->GetSoundSource()));
					}
				}

				//resume matched static sounds
				for (CUInt j = 0; j < g_engineStaticSounds.size(); j++)
				{
					if (Cmp(g_main->m_tempAllPlayingSoundSources[i].c_str(), g_engineStaticSounds[j]->GetName()))
						g_soundSystem->PlayALSound(*(g_engineStaticSounds[j]->GetSoundSource()));
				}

				//resume matched main character sounds
				if (g_mainCharacter)
				{
					if (Cmp(g_main->m_tempAllPlayingSoundSources[i].c_str(), g_mainCharacter->m_walkSound->GetName()))
						g_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));

					if (Cmp(g_main->m_tempAllPlayingSoundSources[i].c_str(), g_mainCharacter->m_runSound->GetName()))
						g_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));

					if (Cmp(g_main->m_tempAllPlayingSoundSources[i].c_str(), g_mainCharacter->m_jumpSound->GetName()))
						g_soundSystem->PlayALSound(*(g_mainCharacter->m_jumpSound->GetSoundSource()));
				}

				//resume matched ambient sound
				if (g_databaseVariables.m_insertAmbientSound)
				{
					if (Cmp(g_main->m_tempAllPlayingSoundSources[i].c_str(), g_main->m_ambientSound->GetName()))
						g_soundSystem->PlayALSound(*(g_main->m_ambientSound->GetSoundSource()));
				}

			}
		}

	}
	else
	{
		//resource sounds
		for (CUInt i = 0; i < g_resourceFiles.size(); i++)
		{
			if (g_resourceFiles[i]->GetSoundSource())
			{
				ALint state;
				alGetSourcei(g_resourceFiles[i]->GetSoundSource()->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state);
				if (state == AL_PLAYING)
				{
					g_main->m_tempAllPlayingSoundSources.push_back(g_resourceFiles[i]->GetSoundSource()->GetName());
				}
			}
		}

		//static 3D sounds
		for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
		{
			ALint state;
			alGetSourcei(g_engineStaticSounds[i]->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING)
			{
				g_main->m_tempAllPlayingSoundSources.push_back(g_engineStaticSounds[i]->GetName());
			}
		}

		//ambient sound
		if (g_databaseVariables.m_insertAmbientSound)
		{
			ALint state;
			alGetSourcei(g_main->m_ambientSound->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING)
			{
				g_main->m_tempAllPlayingSoundSources.push_back(g_main->m_ambientSound->GetName());
			}
		}

		//main character sounds
		if (g_mainCharacter)
		{
			ALint state1;
			alGetSourcei(g_mainCharacter->m_jumpSound->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state1);
			if (state1 == AL_PLAYING)
			{
				g_main->m_tempAllPlayingSoundSources.push_back(g_mainCharacter->m_jumpSound->GetName());
			}
			ALint state2;
			alGetSourcei(g_mainCharacter->m_walkSound->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state2);
			if (state2 == AL_PLAYING)
			{
				g_main->m_tempAllPlayingSoundSources.push_back(g_mainCharacter->m_walkSound->GetName());
			}
			ALint state3;
			alGetSourcei(g_mainCharacter->m_runSound->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state3);
			if (state3 == AL_PLAYING)
			{
				g_main->m_tempAllPlayingSoundSources.push_back(g_mainCharacter->m_runSound->GetName());
			}
		}
	}

	if (g_currentVSceneProperties.m_isMenu)
	{
		CInt size;
		size = lua_tointeger(L, 3);
		g_currentVSceneProperties.m_cursorSize = size;
		g_main->m_mousePosition.x = (CFloat)g_width / 2.f;
		g_main->m_mousePosition.y = (CFloat)g_height / 2.f;
	}

	return 0;
}

CInt ExitGame(lua_State *L)
{
	g_main->SetExitGame(CTrue);
	return 0;
}

CInt ActivateThirdPersonCamera(lua_State *L)
{
	if (g_mainCharacter)
	{
		g_render.SetActiveInstanceCamera(NULL);
		g_currentCameraType = eCAMERA_PHYSX;
		g_mainCharacter->SetCameraType(ePHYSX_CAMERA_THIRD_PERSON);
		//PrintInfo("\nDefault third person PhysX camera was activated.", COLOR_YELLOW);
		return 0;
	}
	else
	{
		//PrintInfo("\nCouldn't find main character", COLOR_RED);
		return 0;
	}
}

CInt ActivateFirstPersonCamera(lua_State *L)
{
	if (g_mainCharacter)
	{
		g_render.SetActiveInstanceCamera(NULL);
		g_currentCameraType = eCAMERA_PHYSX;
		g_mainCharacter->SetCameraType(ePHYSX_CAMERA_FIRST_PERSON);
		//PrintInfo("\nDefault first person PhysX camera was activated.", COLOR_YELLOW);
		return 0;
	}
	else
	{
		//PrintInfo("\nCouldn't find main character", COLOR_RED);
		return 0;
	}
}

//First Argument: Imported Camera Name
//Second Argument: End Time. Should Be Positive Value. Arbitrary Argument
CInt ActivateImportedCamera(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify at least 1 argument for ActivateImportedCamera()", COLOR_RED);
		return 0;
	}

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundTarget = CFalse;
	for (CUInt i = 0; i < g_importedCameraInstances.size(); i++)
	{
		CChar camera[MAX_NAME_SIZE];
		Cpy(camera, g_importedCameraInstances[i]->m_abstractCamera->GetName());
		StringToUpper(camera);

		if (Cmp(camera, luaToString))
		{
			CFloat end_time = 0.0f;
			CBool enableTimer = CFalse;
			if (argc > 1)
			{
				end_time = (CFloat)lua_tonumber(L, 2);
				if (end_time <= 0.0f)
				{
					enableTimer = CFalse;
					end_time = 0.0f;
				}
				else
				{
					enableTimer = CTrue;
				}
			}

			g_render.SetActiveInstanceCamera(g_importedCameraInstances[i]);
			g_importedCameraInstances[i]->SetIsTimerEnabled(enableTimer);
			g_importedCameraInstances[i]->SetEndTime(end_time);
			g_currentCameraType = eCAMERA_COLLADA;
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nCamera '", g_importedCameraInstances[i]->m_abstractCamera->GetName(), "' was activated.");
			//PrintInfo(temp, COLOR_RED);

			foundTarget = CTrue;
			break;
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nCoudn't find camera '", luaToString, "' to be activated.");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

//First Argument: Prefab Instance Name. Use "this" to refer to current prefab instance
//Second Argument: Imported camera name of prefab
//Third Argument: End Time. Should Be Positive Value. Arbitrary Argument
CInt ActivateImportedCameraOfPrefabInstance(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc < 2)
	{
		//PrintInfo("\nPlease specify at least 2 arguments for ActivateImportedCameraOfPrefabInstance()", COLOR_RED);
		return 0;
	}
	CScene* scene = NULL;
	CBool foundPrefabInstance = CFalse;

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	//camera name of current prefab
	CChar luaToString2[MAX_NAME_SIZE];
	Cpy(luaToString2, lua_tostring(L, 2));
	StringToUpper(luaToString2);

	CBool foundCameraTarget = CFalse;

	if (Cmp(luaToString, "THIS"))
	{
		if (g_currentInstancePrefab)
		{
			CPrefab* prefab = g_currentInstancePrefab->GetPrefab();

			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_currentInstancePrefab->GetSceneVisible(j)*/)
				{
					scene = g_currentInstancePrefab->GetScene(j);

					CUInt index;
					for (CUInt k = 0; k < scene->m_cameraInstances.size(); k++)
					{
						CChar cameraName[MAX_NAME_SIZE];
						Cpy(cameraName, scene->m_cameraInstances[k]->m_abstractCamera->GetPureName());
						StringToUpper(cameraName);
						if (Cmp(luaToString2, cameraName))
						{
							index = k;
							foundCameraTarget = CTrue;
							break;
						}
					}

					if (foundCameraTarget)
					{
						CFloat end_time = 0.0f;
						CBool enableTimer = CFalse;
						if (argc > 2)
						{
							end_time = (CFloat)lua_tonumber(L, 3);
							if (end_time <= 0.0f)
							{
								enableTimer = CFalse;
								end_time = 0.0f;
							}
							else
							{
								enableTimer = CTrue;
							}
						}

						g_render.SetActiveInstanceCamera(scene->m_cameraInstances[index]);
						scene->m_cameraInstances[index]->SetIsTimerEnabled(enableTimer);
						scene->m_cameraInstances[index]->SetEndTime(end_time);
						g_currentCameraType = eCAMERA_COLLADA;
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "%s%s%s", "\nCamera '", scene->m_cameraInstances[index]->m_abstractCamera->GetPureName(), "' was activated.");
						//PrintInfo(temp, COLOR_GREEN);
					}
				}
				if (foundCameraTarget)
					break;
			}
			if (!scene)
			{
				//CChar temp[MAX_NAME_SIZE];
				//sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
				//PrintInfo(temp, COLOR_RED);
				return 0;
			}
			if (!foundCameraTarget)
			{
				//CChar temp[MAX_NAME_SIZE];
				//sprintf(temp, "\n%s%s%s", "Couldn't find the camera '", luaToString2, "'");
				//PrintInfo(temp, COLOR_RED);
				return 0;
			}
		}
		else
		{
			//PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();

			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j) /*&& g_instancePrefab[i]->GetSceneVisible(j)*/)
				{
					scene = g_instancePrefab[i]->GetScene(j);

					CUInt index;
					for (CUInt k = 0; k < scene->m_cameraInstances.size(); k++)
					{
						CChar cameraName[MAX_NAME_SIZE];
						Cpy(cameraName, scene->m_cameraInstances[k]->m_abstractCamera->GetPureName());
						StringToUpper(cameraName);
						if (Cmp(luaToString2, cameraName))
						{
							index = k;
							foundCameraTarget = CTrue;
							break;
						}
					}
					if (foundCameraTarget)
					{
						CFloat end_time = 0.0f;
						CBool enableTimer = CFalse;
						if (argc > 2)
						{
							end_time = (CFloat)lua_tonumber(L, 3);
							if (end_time <= 0.0f)
							{
								enableTimer = CFalse;
								end_time = 0.0f;
							}
							else
							{
								enableTimer = CTrue;
							}
						}

						g_render.SetActiveInstanceCamera(scene->m_cameraInstances[index]);
						scene->m_cameraInstances[index]->SetIsTimerEnabled(enableTimer);
						scene->m_cameraInstances[index]->SetEndTime(end_time);
						g_currentCameraType = eCAMERA_COLLADA;
						//CChar temp[MAX_NAME_SIZE];
						//sprintf(temp, "%s%s%s", "\nCamera '", scene->m_cameraInstances[index]->m_abstractCamera->GetPureName(), "' was activated.");
						//PrintInfo(temp, COLOR_GREEN);
					}
				}
				if (foundCameraTarget)
					break;
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!foundCameraTarget)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find the camera '", luaToString2, "'");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First Argument: Engine Camera Name
//Second Argument: End Time. Should Be Positive Value. Arbitrary Argument

CInt ActivateEngineCamera(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify at least 1 argument for ActivateEngineCamera()", COLOR_RED);
		return 0;
	}

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundTarget = CFalse;
	CInt index = -1;
	for (CUInt i = 0; i < g_engineCameraInstances.size(); i++)
	{
		CChar camera[MAX_NAME_SIZE];
		Cpy(camera, g_engineCameraInstances[i]->m_abstractCamera->GetName());
		StringToUpper(camera);

		if (Cmp(camera, luaToString))
		{
			CFloat end_time = 0.0f;
			CBool enableTimer = CFalse;
			if (argc > 1)
			{
				end_time = (CFloat)lua_tonumber(L, 2);
				if (end_time <= 0.0f)
				{
					enableTimer = CFalse;
					end_time = 0.0f;
				}
				else
				{
					enableTimer = CTrue;
				}
			}

			g_render.SetActiveInstanceCamera(g_engineCameraInstances[i]);
			g_engineCameraInstances[i]->SetActive(CTrue);
			g_engineCameraInstances[i]->SetIsTimerEnabled(enableTimer);
			g_engineCameraInstances[i]->SetEndTime(end_time);
			g_currentCameraType = eCAMERA_ENGINE;
			index = i;
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nCamera '", g_engineCameraInstances[i]->m_abstractCamera->GetName(), "' was activated.");
			//PrintInfo(temp, COLOR_RED);

			foundTarget = CTrue;
			break;
		}
	}
	if (foundTarget)
	{
		for (CUInt i = 0; i < g_engineCameraInstances.size(); i++)
		{
			if (i != index)
				g_engineCameraInstances[i]->SetActive(CFalse);
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nCoudn't find camera '", luaToString, "' to be activated.");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

//Argument: Angle in degrees
CInt SetPhysicsCameraAngle(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraAngle()", COLOR_RED);
		return 0;
	}
	if (g_camera)
	{
		CFloat angle = (CFloat)lua_tonumber(L, 1);
		g_camera->m_cameraManager->SetAngle(angle);
	}
	return 0;
}

CInt GetPhysicsCameraAngle(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	if (g_camera)
	{
		CFloat angle = g_camera->m_cameraManager->GetAngle();
		lua_pushnumber(L, angle);
		return 1;
	}
	return 0;
}

CInt SetPhysicsCameraTilt(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraTilt()", COLOR_RED);
		return 0;
	}
	if (g_camera)
	{
		CFloat value = (CFloat)lua_tonumber(L, 1);
		g_camera->m_perspectiveCameraTilt = value;
		g_camera->m_perspectiveCurrentCameraTilt = value;
	}
	return 0;
}

CInt SetPhysicsCameraMaxTilt(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraMaxTilt()", COLOR_RED);
		return 0;
	}
	if (g_camera)
	{
		CFloat value = (CFloat)lua_tonumber(L, 1);
		g_camera->m_perspectiveCameraMaxTilt = value;
	}
	return 0;
}

CInt SetPhysicsCameraMinTilt(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraMinTilt()", COLOR_RED);
		return 0;
	}
	if (g_camera)
	{
		CFloat value = (CFloat)lua_tonumber(L, 1);
		g_camera->m_perspectiveCameraMinTilt = value;
	}
	return 0;
}

CInt GetPhysicsCameraTilt(lua_State *L)
{
	if (g_camera)
	{
		lua_pushnumber(L, g_camera->m_perspectiveCurrentCameraTilt);
		return 1;
	}

	return 0;
}

CInt GetPhysicsCameraMaxTilt(lua_State *L)
{
	if (g_camera)
	{
		lua_pushnumber(L, g_camera->m_perspectiveCameraMaxTilt);
		return 1;
	}

	return 0;
}

CInt GetPhysicsCameraMinTilt(lua_State *L)
{
	if (g_camera)
	{
		lua_pushnumber(L, g_camera->m_perspectiveCameraMinTilt);
		return 1;
	}

	return 0;
}

CInt SetPhysicsCameraYaw(lua_State *L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraYaw()", COLOR_RED);
		return 0;
	}
	if (g_camera)
	{
		CFloat value = (CFloat)lua_tonumber(L, 1);
		g_camera->m_perspectiveCameraYaw = NxMath::degToRad(value);
	}

	return 0;
}

CInt GetPhysicsCameraYaw(lua_State *L)
{
	if (g_camera)
	{
		lua_pushnumber(L, NxMath::radToDeg(g_camera->m_perspectiveCameraYaw));
		return 1;
	}

	return 0;
}

CInt LoadResource(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 2)
	{
		MessageBoxA(NULL, "Vanda Engine Error", "Please specify 2 arguments for LoadResource()", MB_OK | MB_ICONERROR);
		return 0;
	}

	CChar luaToString1[MAX_NAME_SIZE];
	Cpy(luaToString1, lua_tostring(L, 1));
	StringToUpper(luaToString1); //package name

	CChar luaToString2[MAX_NAME_SIZE];
	Cpy(luaToString2, lua_tostring(L, 2));
	StringToUpper(luaToString2);

	CChar fileName[MAX_NAME_SIZE];
	Cpy(fileName, "Assets/Resources/");

	Append(fileName, luaToString1);
	Append(fileName, "/");
	Append(fileName, luaToString2);

	CChar extension[MAX_NAME_SIZE];
	if (GetFileExtension(luaToString2))
		Cpy(extension, GetFileExtension(luaToString2));
	else
		return 0;

	CChar name[MAX_NAME_SIZE];
	Cpy(name, luaToString1);
	Append(name, "_");
	Append(name, luaToString2);
	StringToUpper(name);

	for (CUInt i = 0; i < g_resourceFiles.size(); i++)
	{
		CChar old_name[MAX_NAME_SIZE];
		Cpy(old_name, g_resourceFiles[i]->GetName());
		StringToUpper(old_name);

		if (Cmp(name, old_name))
			return 0;
	}

	if (Cmp(extension, ".OGG"))
	{
		CResourceFile* oggFile = CNew(CResourceFile);
		if (oggFile->LoadOGGFile(fileName))
		{
			oggFile->SetName(name);
			oggFile->GetSoundSource()->SetName(name);
			oggFile->SetDirectoryName(luaToString1);
			oggFile->SetFileName(luaToString2);
			g_resourceFiles.push_back(oggFile);
		}
		else
		{
			CDelete(oggFile);
		}
	}
	else if (Cmp(extension, ".DDS"))
	{
		CResourceFile* ddsFile = CNew(CResourceFile);
		if (ddsFile->LoadDDSFile(fileName))
		{
			ddsFile->SetName(name);
			ddsFile->SetDirectoryName(luaToString1);
			ddsFile->SetFileName(luaToString2);
			g_resourceFiles.push_back(ddsFile);
		}
		else
		{
			CDelete(ddsFile);
		}
	}
	else if (Cmp(extension, ".AVI"))
	{
		CResourceFile* aviFile = CNew(CResourceFile);
		if (aviFile->LoadAVIFile(fileName))
		{
			aviFile->SetName(name);
			aviFile->SetDirectoryName(luaToString1);
			aviFile->SetFileName(luaToString2);
			g_resourceFiles.push_back(aviFile);
		}
		else
		{
			CDelete(aviFile);
		}
	}

	return 0;
}

CInt DeleteResource(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		//PrintInfo("\nPlease specify 2 arguments for DeleteResource()", COLOR_RED);
		return 0;
	}

	CChar luaToString1[MAX_NAME_SIZE];
	Cpy(luaToString1, lua_tostring(L, 1));
	StringToUpper(luaToString1); //directory name

	CChar luaToString2[MAX_NAME_SIZE];
	Cpy(luaToString2, lua_tostring(L, 2));
	StringToUpper(luaToString2); //file name

	for (CUInt j = 0; j < g_resourceFiles.size(); j++)
	{
		if (Cmp(g_resourceFiles[j]->GetDirectoryName(), luaToString1) && Cmp(g_resourceFiles[j]->GetFileName(), luaToString2))
		{
			CDelete(g_resourceFiles[j]);
			g_resourceFiles.erase(g_resourceFiles.begin() + j);
			break;
		}
	}
	return 0;
}

CInt DeleteAllResources(lua_State* L)
{
	for (CUInt j = 0; j < g_resourceFiles.size(); j++)
		CDelete(g_resourceFiles[j]);
	g_resourceFiles.clear();

	if (g_main && g_main->GetCursorIcon())
		g_main->GetCursorIcon()->SetVisible(CFalse);
	return 0;
}

CInt PlayResourceSoundLoop(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		MessageBoxA(NULL, "Vanda Engine Error", "Please specify at least 1 argument for PlayResourceSoundLoop()", MB_OK | MB_ICONERROR);
		return 0;
	}

	for (int n = 1; n <= argc; ++n)
	{
		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, n));
		StringToUpper(luaToString); //package name

		CBool foundTarget = CFalse;
		for (CUInt i = 0; i < g_resourceFiles.size(); i++)
		{
			if (g_resourceFiles[i]->GetSoundSource())
			{
				CChar string[MAX_NAME_SIZE];
				Cpy(string, g_resourceFiles[i]->GetSoundSource()->GetName());
				StringToUpper(string); //package name

				if (Cmp(string, luaToString))
				{
					g_resourceFiles[i]->GetSoundSource()->GetSoundSource()->SetLooping(CTrue);
					g_soundSystem->PlayALSound(*(g_resourceFiles[i]->GetSoundSource()->GetSoundSource()));
					foundTarget = CTrue;
					break;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nCouldn't find the sound '", lua_tostring(L, n), "' to be played.");
			MessageBoxA(NULL, "Vanda Engine Error", temp, MB_OK | MB_ICONERROR);
		}
	}

	return 0;
}

CInt PlayResourceSoundOnce(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		MessageBoxA(NULL, "Vanda Engine Error", "Please specify at least 1 argument for PlayResourceSoundLoop()", MB_OK | MB_ICONERROR);

		return 0;
	}

	for (int n = 1; n <= argc; ++n)
	{
		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, n));
		StringToUpper(luaToString); //package name

		CBool foundTarget = CFalse;
		for (CUInt i = 0; i < g_resourceFiles.size(); i++)
		{
			if (g_resourceFiles[i]->GetSoundSource())
			{
				CChar string[MAX_NAME_SIZE];
				Cpy(string, g_resourceFiles[i]->GetSoundSource()->GetName());
				StringToUpper(string); //package name

				if (Cmp(string, luaToString))
				{
					g_resourceFiles[i]->GetSoundSource()->GetSoundSource()->SetLooping(CFalse);
					g_soundSystem->PlayALSound(*(g_resourceFiles[i]->GetSoundSource()->GetSoundSource()));

					foundTarget = CTrue;
					break;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nCouldn't find the sound '", lua_tostring(L, n), "' to be played once.");
			MessageBoxA(NULL, "Vanda Engine Error", temp, MB_OK | MB_ICONERROR);
		}
	}

	return 0;
}

CInt StopResourceSound(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		MessageBoxA(NULL, "Vanda Engine Error", "Please specify at least 1 argument for StopResourceSound()", MB_OK | MB_ICONERROR);

		return 0;
	}

	for (int n = 1; n <= argc; ++n)
	{
		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, n));
		StringToUpper(luaToString); //package name

		CBool foundTarget = CFalse;
		for (CUInt i = 0; i < g_resourceFiles.size(); i++)
		{
			if (g_resourceFiles[i]->GetSoundSource())
			{
				CChar string[MAX_NAME_SIZE];
				Cpy(string, g_resourceFiles[i]->GetSoundSource()->GetName());
				StringToUpper(string); //package name

				if (Cmp(string, luaToString))
				{
					g_soundSystem->StopALSound(*(g_resourceFiles[i]->GetSoundSource()->GetSoundSource()));
					foundTarget = CTrue;
					break;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nCouldn't find the sound '", lua_tostring(L, n), "' to be stopped.");
			MessageBoxA(NULL, "Vanda Engine Error", temp, MB_OK | MB_ICONERROR);
		}
	}

	return 0;
}

CInt PauseResourceSound(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		MessageBoxA(NULL, "Vanda Engine Error", "Please specify at least 1 argument for StopResourceSound()", MB_OK | MB_ICONERROR);

		return 0;
	}

	for (int n = 1; n <= argc; ++n)
	{
		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, n));
		StringToUpper(luaToString); //package name

		CBool foundTarget = CFalse;
		for (CUInt i = 0; i < g_resourceFiles.size(); i++)
		{
			if (g_resourceFiles[i]->GetSoundSource())
			{
				CChar string[MAX_NAME_SIZE];
				Cpy(string, g_resourceFiles[i]->GetSoundSource()->GetName());
				StringToUpper(string); //package name

				if (Cmp(string, luaToString))
				{
					g_soundSystem->PauseALSound(*(g_resourceFiles[i]->GetSoundSource()->GetSoundSource()));
					foundTarget = CTrue;
					break;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nCouldn't find the sound '", lua_tostring(L, n), "' to be paused.");
			MessageBoxA(NULL, "Vanda Engine Error", temp, MB_OK | MB_ICONERROR);
		}
	}

	return 0;
}

CInt StopAllResourceSounds(lua_State *L)
{
	for (CUInt i = 0; i < g_resourceFiles.size(); i++)
	{
		if (g_resourceFiles[i]->GetSoundSource())
		{
			g_soundSystem->StopALSound(*(g_resourceFiles[i]->GetSoundSource()->GetSoundSource()));
		}
	}

	return 0;
}

CInt ShowGUI(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		MessageBoxA(NULL, "Vanda Engine 1 Error", "Please specify 1 argument for ShowGUI()", MB_OK | MB_ICONERROR);
		return 0;
	}
	CBool foundTarget = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	for (CUInt i = 0; i < g_guis.size(); i++)
	{
		CChar gui[MAX_NAME_SIZE];
		Cpy(gui, g_guis[i]->GetName());
		StringToUpper(gui);

		if (Cmp(gui, luaToString))
		{
			g_guis[i]->SetVisible(CTrue);

			foundTarget = CTrue;
			break;
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nCoudn't find the GUI '", luaToString, "' to be activated.");
		MessageBoxA(NULL, "Vanda Engine 1 Error", temp, MB_OK | MB_ICONERROR);
	}

	return 0;
}

CInt HideGUI(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		MessageBoxA(NULL, "Vanda Engine 1 Error", "Please specify 1 argument for HideGUI()", MB_OK | MB_ICONERROR);
		return 0;
	}
	CBool foundTarget = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	for (CUInt i = 0; i < g_guis.size(); i++)
	{
		CChar gui[MAX_NAME_SIZE];
		Cpy(gui, g_guis[i]->GetName());
		StringToUpper(gui);

		if (Cmp(gui, luaToString))
		{
			g_guis[i]->SetVisible(CFalse);

			foundTarget = CTrue;
			break;
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nCoudn't find the GUI '", luaToString, "' to be deactivated.");
		MessageBoxA(NULL, "Vanda Engine 1 Error", temp, MB_OK | MB_ICONERROR);
	}

	return 0;
}

CInt ShowCursorIcon(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 2)
	{
		MessageBoxA(NULL, "Vanda Engine 1 Error", "Please specify 2 arguments for ShowCursorIcon()", MB_OK | MB_ICONERROR);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString); //package name

	CBool foundTarget = CFalse;
	for (CUInt i = 0; i < g_resourceFiles.size(); i++)
	{
		if (g_resourceFiles[i]->GetImage())
		{
			CChar string[MAX_NAME_SIZE];
			Cpy(string, g_resourceFiles[i]->GetName());
			StringToUpper(string); //package name

			if (Cmp(string, luaToString))
			{
				g_main->GetCursorIcon()->SetVisible(CTrue);
				g_main->GetCursorIcon()->SetName(string);
				g_main->GetCursorIcon()->SetImage(g_resourceFiles[i]->GetImage());
				g_main->GetCursorIcon()->SetSize(lua_tonumber(L, 2));

				foundTarget = CTrue;
				break;
			}
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nCouldn't find the image '", lua_tostring(L, 1), "' to be shown.");
		MessageBoxA(NULL, "Vanda Engine Error", temp, MB_OK | MB_ICONERROR);
	}

	return 0;
}

CInt HideCursorIcon(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		MessageBoxA(NULL, "Vanda Engine Error", "Please specify 1 argument for HideCursorIcon()", MB_OK | MB_ICONERROR);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString); //package name

	CBool foundTarget = CFalse;
	for (CUInt i = 0; i < g_resourceFiles.size(); i++)
	{
		if (g_resourceFiles[i]->GetImage())
		{
			CChar string[MAX_NAME_SIZE];
			Cpy(string, g_resourceFiles[i]->GetName());
			StringToUpper(string); //package name

			if (Cmp(string, luaToString))
			{
				g_main->GetCursorIcon()->SetVisible(CFalse);
				foundTarget = CTrue;
				break;
			}
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nCouldn't find the image '", lua_tostring(L, 1), "' to be hidden.");
		MessageBoxA(NULL, "Vanda Engine Error", temp, MB_OK | MB_ICONERROR);
	}

	return 0;
}

CInt PrintConsole(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for PrintConsole", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));

	//PrintInfo(luaToString);

	return 0;
}

CInt IsKeyDown(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for IsKeyDown()", COLOR_RED);
		lua_pushboolean(L, 0);
		return 0;
	}

	CChar keyCode[MAX_NAME_SIZE];
	Cpy(keyCode, lua_tostring(L, 1)); //DirecInput Key Code
	StringToUpper(keyCode);

	g_input.Update();

	if (Cmp(keyCode, "0"))
	{
		if (g_input.ButtonDown(0)) //left click
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "1"))
	{
		if (g_input.ButtonDown(1)) //right click
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "2"))
	{
		if (g_input.ButtonDown(2)) //middle button click
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_ESCAPE"))
	{
		if (g_input.KeyDown(DIK_ESCAPE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_1"))
	{
		if (g_input.KeyDown(DIK_1))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_2"))
	{
		if (g_input.KeyDown(DIK_2))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_3"))
	{
		if (g_input.KeyDown(DIK_3))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_4"))
	{
		if (g_input.KeyDown(DIK_4))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_5"))
	{
		if (g_input.KeyDown(DIK_5))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_6"))
	{
		if (g_input.KeyDown(DIK_6))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_7"))
	{
		if (g_input.KeyDown(DIK_7))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_8"))
	{
		if (g_input.KeyDown(DIK_8))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_9"))
	{
		if (g_input.KeyDown(DIK_9))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_0"))
	{
		if (g_input.KeyDown(DIK_0))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_MINUS"))
	{
		if (g_input.KeyDown(DIK_MINUS))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_EQUALS"))
	{
		if (g_input.KeyDown(DIK_EQUALS))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_BACK"))
	{
		if (g_input.KeyDown(DIK_BACK))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_TAB"))
	{
		if (g_input.KeyDown(DIK_TAB))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_Q"))
	{
		if (g_input.KeyDown(DIK_Q))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_W"))
	{
		if (g_input.KeyDown(DIK_W))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_E"))
	{
		if (g_input.KeyDown(DIK_E))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_R"))
	{
		if (g_input.KeyDown(DIK_R))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_T"))
	{
		if (g_input.KeyDown(DIK_T))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_Y"))
	{
		if (g_input.KeyDown(DIK_Y))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_U"))
	{
		if (g_input.KeyDown(DIK_U))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_I"))
	{
		if (g_input.KeyDown(DIK_I))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_O"))
	{
		if (g_input.KeyDown(DIK_O))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_P"))
	{
		if (g_input.KeyDown(DIK_P))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LBRACKET"))
	{
		if (g_input.KeyDown(DIK_LBRACKET))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RBRACKET"))
	{
		if (g_input.KeyDown(DIK_RBRACKET))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RETURN"))
	{
		if (g_input.KeyDown(DIK_RETURN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LCONTROL"))
	{
		if (g_input.KeyDown(DIK_LCONTROL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_A"))
	{
		if (g_input.KeyDown(DIK_A))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_S"))
	{
		if (g_input.KeyDown(DIK_S))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_D"))
	{
		if (g_input.KeyDown(DIK_D))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F"))
	{
		if (g_input.KeyDown(DIK_F))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_G"))
	{
		if (g_input.KeyDown(DIK_G))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_H"))
	{
		if (g_input.KeyDown(DIK_H))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_J"))
	{
		if (g_input.KeyDown(DIK_J))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_K"))
	{
		if (g_input.KeyDown(DIK_K))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_L"))
	{
		if (g_input.KeyDown(DIK_L))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SEMICOLON"))
	{
		if (g_input.KeyDown(DIK_SEMICOLON))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_APOSTROPHE"))
	{
		if (g_input.KeyDown(DIK_APOSTROPHE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_GRAVE"))
	{
		if (g_input.KeyDown(DIK_GRAVE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LSHIFT"))
	{
		if (g_input.KeyDown(DIK_LSHIFT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_BACKSLASH"))
	{
		if (g_input.KeyDown(DIK_BACKSLASH))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_Z"))
	{
		if (g_input.KeyDown(DIK_Z))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_X"))
	{
		if (g_input.KeyDown(DIK_X))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_C"))
	{
		if (g_input.KeyDown(DIK_C))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_V"))
	{
		if (g_input.KeyDown(DIK_V))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_B"))
	{
		if (g_input.KeyDown(DIK_B))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_N"))
	{
		if (g_input.KeyDown(DIK_N))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_M"))
	{
		if (g_input.KeyDown(DIK_M))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_COMMA"))
	{
		if (g_input.KeyDown(DIK_COMMA))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_PERIOD"))
	{
		if (g_input.KeyDown(DIK_PERIOD))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SLASH"))
	{
		if (g_input.KeyDown(DIK_SLASH))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RSHIFT"))
	{
		if (g_input.KeyDown(DIK_RSHIFT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_MULTIPLY"))
	{
		if (g_input.KeyDown(DIK_MULTIPLY))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LMENU"))
	{
		if (g_input.KeyDown(DIK_LMENU))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SPACE"))
	{
		if (g_input.KeyDown(DIK_SPACE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_CAPITAL"))
	{
		if (g_input.KeyDown(DIK_CAPITAL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F1"))
	{
		if (g_input.KeyDown(DIK_F1))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F2"))
	{
		if (g_input.KeyDown(DIK_F2))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F3"))
	{
		if (g_input.KeyDown(DIK_F3))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	if (Cmp(keyCode, "DIK_F4"))
	{
		if (g_input.KeyDown(DIK_F4))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F5"))
	{
		if (g_input.KeyDown(DIK_F5))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F6"))
	{
		if (g_input.KeyDown(DIK_F6))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F7"))
	{
		if (g_input.KeyDown(DIK_F7))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F8"))
	{
		if (g_input.KeyDown(DIK_F8))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F9"))
	{
		if (g_input.KeyDown(DIK_F9))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F10"))
	{
		if (g_input.KeyDown(DIK_F10))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMLOCK"))
	{
		if (g_input.KeyDown(DIK_NUMLOCK))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SCROLL"))
	{
		if (g_input.KeyDown(DIK_SCROLL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD7"))
	{
		if (g_input.KeyDown(DIK_NUMPAD7))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD8"))
	{
		if (g_input.KeyDown(DIK_NUMPAD8))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD9"))
	{
		if (g_input.KeyDown(DIK_NUMPAD9))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SUBTRACT"))
	{
		if (g_input.KeyDown(DIK_SUBTRACT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD4"))
	{
		if (g_input.KeyDown(DIK_NUMPAD4))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD5"))
	{
		if (g_input.KeyDown(DIK_NUMPAD5))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD6"))
	{
		if (g_input.KeyDown(DIK_NUMPAD6))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_ADD"))
	{
		if (g_input.KeyDown(DIK_ADD))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD1"))
	{
		if (g_input.KeyDown(DIK_NUMPAD1))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD2"))
	{
		if (g_input.KeyDown(DIK_NUMPAD2))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD3"))
	{
		if (g_input.KeyDown(DIK_NUMPAD3))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD0"))
	{
		if (g_input.KeyDown(DIK_NUMPAD0))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_DECIMAL"))
	{
		if (g_input.KeyDown(DIK_DECIMAL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F11"))
	{
		if (g_input.KeyDown(DIK_F11))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F12"))
	{
		if (g_input.KeyDown(DIK_F12))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F13"))
	{
		if (g_input.KeyDown(DIK_F13))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F14"))
	{
		if (g_input.KeyDown(DIK_F14))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F15"))
	{
		if (g_input.KeyDown(DIK_F15))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_KANA"))
	{
		if (g_input.KeyDown(DIK_KANA))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_CONVERT"))
	{
		if (g_input.KeyDown(DIK_CONVERT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NOCONVERT"))
	{
		if (g_input.KeyDown(DIK_NOCONVERT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_YEN"))
	{
		if (g_input.KeyDown(DIK_YEN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPADEQUALS"))
	{
		if (g_input.KeyDown(DIK_NUMPADEQUALS))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_CIRCUMFLEX"))
	{
		if (g_input.KeyDown(DIK_CIRCUMFLEX))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_AT"))
	{
		if (g_input.KeyDown(DIK_AT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_COLON"))
	{
		if (g_input.KeyDown(DIK_COLON))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_UNDERLINE"))
	{
		if (g_input.KeyDown(DIK_UNDERLINE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_KANJI"))
	{
		if (g_input.KeyDown(DIK_KANJI))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_STOP"))
	{
		if (g_input.KeyDown(DIK_STOP))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_AX"))
	{
		if (g_input.KeyDown(DIK_AX))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_UNLABELED"))
	{
		if (g_input.KeyDown(DIK_UNLABELED))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPADENTER"))
	{
		if (g_input.KeyDown(DIK_NUMPADENTER))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RCONTROL"))
	{
		if (g_input.KeyDown(DIK_RCONTROL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPADCOMMA"))
	{
		if (g_input.KeyDown(DIK_NUMPADCOMMA))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_DIVIDE"))
	{
		if (g_input.KeyDown(DIK_DIVIDE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SYSRQ"))
	{
		if (g_input.KeyDown(DIK_SYSRQ))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RMENU"))
	{
		if (g_input.KeyDown(DIK_RMENU))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_PAUSE"))
	{
		if (g_input.KeyDown(DIK_PAUSE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_HOME"))
	{
		if (g_input.KeyDown(DIK_HOME))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_UP"))
	{
		if (g_input.KeyDown(DIK_UP))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_PRIOR"))
	{
		if (g_input.KeyDown(DIK_PRIOR))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LEFT"))
	{
		if (g_input.KeyDown(DIK_LEFT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RIGHT"))
	{
		if (g_input.KeyDown(DIK_RIGHT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_END"))
	{
		if (g_input.KeyDown(DIK_END))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_DOWN"))
	{
		if (g_input.KeyDown(DIK_DOWN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NEXT"))
	{
		if (g_input.KeyDown(DIK_NEXT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_INSERT"))
	{
		if (g_input.KeyDown(DIK_INSERT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_DELETE"))
	{
		if (g_input.KeyDown(DIK_DELETE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LWIN"))
	{
		if (g_input.KeyDown(DIK_LWIN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RWIN"))
	{
		if (g_input.KeyDown(DIK_RWIN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_APPS"))
	{
		if (g_input.KeyDown(DIK_APPS))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_POWER"))
	{
		if (g_input.KeyDown(DIK_POWER))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SLEEP"))
	{
		if (g_input.KeyDown(DIK_SLEEP))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}


	lua_pushboolean(L, 0); //false
	return 0;
}

CInt SetSelectionDistance(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetSelectionDistance()", COLOR_RED);
		return 0;
	}
	g_main->SetSelectionDistance(CFloat(lua_tonumber(L, 1)));
	return 0;
}

CInt GetSelectionDistance(lua_State *L)
{
	CFloat selectionDistance = g_main->GetSelectionDistance();
	lua_pushnumber(L, selectionDistance);
	return 1;
}

CInt SelectPrefabInstances(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 argument for SelectPrefabInstances()", COLOR_RED);
		return 0;
	}
	CDouble mouseXPos = CDouble(lua_tonumber(L, 1));
	CDouble mouseYPos = CDouble(lua_tonumber(L, 2));
	CDouble selectionWidth = CDouble(lua_tonumber(L, 3));
	CDouble selectionHeight = CDouble(lua_tonumber(L, 4));

	CChar* selectedName = g_main->SelectPrefabInstances(mouseXPos, mouseYPos, selectionWidth, selectionHeight);
	lua_pushstring(L, selectedName);
	return 1;
}

CInt GetScreenWidth(lua_State* L)
{
	CFloat width = (CFloat)g_width;
	lua_pushnumber(L, width);
	return 1;
}

CInt GetScreenHeight(lua_State* L)
{
	CFloat height = (CFloat)g_height;
	lua_pushnumber(L, height);
	return 1;
}

CInt GetCursorX(lua_State* L)
{
	CFloat posX = g_main->GetCursorX();
	lua_pushnumber(L, posX);
	return 1;

}

CInt GetCursorY(lua_State* L)
{
	CFloat posY = g_main->GetCursorY();
	lua_pushnumber(L, posY);
	return 1;
}

CInt GetElapsedTime(lua_State* L)
{
	lua_pushnumber(L, g_elapsedTime);
	return 1;
}

CInt IsMenuEnabled(lua_State* L)
{
	CBool isMenu = g_currentVSceneProperties.m_isMenu;
	if (isMenu)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}

//First Argument: Physics Actor Name
CInt GetPrefabInstanceNameFromActor(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for GetPrefabInstanceNameFromActor()", COLOR_RED);
		return 0;
	}

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];

	if (lua_tostring(L, 1) == NULL) return 0;

	Cpy(luaToString, lua_tostring(L, 1)); //Physics Actor Name- First Argument

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		if (prefab)
		{
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab->GetHasLod(j))
				{
					CScene* scene = g_instancePrefab[i]->GetScene(j);
					for (CUInt k = 0; k < scene->m_instanceGeometries.size(); k++)
					{
						if (scene->m_instanceGeometries[k]->m_hasPhysX)
						{
							if (Cmp(luaToString, scene->m_instanceGeometries[k]->m_physXName))
							{
								lua_pushstring(L, g_instancePrefab[i]->GetName());
								return 1;
							}
						}
					}
				}
			}
			if (g_instancePrefab[i]->GetHasCollider())
			{
				CScene* scene = g_instancePrefab[i]->GetScene(3);
				for (CUInt k = 0; k < scene->m_instanceGeometries.size(); k++)
				{
					if (scene->m_instanceGeometries[k]->m_hasPhysX)
					{
						if (Cmp(luaToString, scene->m_instanceGeometries[k]->m_physXName))
						{
							lua_pushstring(L, g_instancePrefab[i]->GetName());
							return 1;
						}
					}
				}
			}
		}
	}

	return 0;
}


//First argument: prefab instance name
//2nd, 3rd, and 4th arguments: x positio, y position, z position of prefab instance
CInt TranslatePrefabInstance(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for TranslatePrefabInstance()", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	CFloat xPos = lua_tonumber(L, 2);
	CFloat yPos = lua_tonumber(L, 3);
	CFloat zPos = lua_tonumber(L, 4);
	CVec3f pos(xPos, yPos, zPos);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			if (g_currentInstancePrefab->GetScene(0) && g_currentInstancePrefab->GetScene(0)->IsTransformable())
				g_currentInstancePrefab->SetTranslate(pos);
		}
		else
		{
		//	PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			if (g_instancePrefab[i]->GetScene(0) && g_instancePrefab[i]->GetScene(0)->IsTransformable())
			{
				g_instancePrefab[i]->SetTranslate(pos);
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First argument: prefab instance name
//2nd, 3rd, and 4th arguments: x rotation, y rotation, z rotation of prefab instance
CInt RotatePrefabInstance(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for RotatePrefabInstance()", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	CFloat xRot = lua_tonumber(L, 2);
	CFloat yRot = lua_tonumber(L, 3);
	CFloat zRot = lua_tonumber(L, 4);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			if (g_currentInstancePrefab->GetScene(0) && g_currentInstancePrefab->GetScene(0)->IsTransformable())
			{
				CVec4f rotation(xRot, yRot, zRot, g_currentInstancePrefab->GetRotate().w);
				g_currentInstancePrefab->SetRotate(rotation);
			}
		}
		else
		{
		//	PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			if (g_instancePrefab[i]->GetScene(0) && g_instancePrefab[i]->GetScene(0)->IsTransformable())
			{
				CVec4f rotation(xRot, yRot, zRot, g_instancePrefab[i]->GetRotate().w);
				g_instancePrefab[i]->SetRotate(rotation);
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First argument: prefab instance name
//2nd, 3rd, and 4th arguments: x scale, y scale, z scale of prefab instance
CInt ScalePrefabInstance(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for ScalePrefabInstance()", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	CFloat xScale = lua_tonumber(L, 2);
	CFloat yScale = lua_tonumber(L, 3);
	CFloat zScale = lua_tonumber(L, 4);
	CVec3f scale(xScale, yScale, zScale);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			if (g_currentInstancePrefab->GetScene(0) && g_currentInstancePrefab->GetScene(0)->IsTransformable())
			{
				g_currentInstancePrefab->SetScale(scale);
			}
		}
		else
		{
		//	PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			if (g_instancePrefab[i]->GetScene(0) && g_instancePrefab[i]->GetScene(0)->IsTransformable())
			{
				g_instancePrefab[i]->SetScale(scale);
			}
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetPrefabInstanceTranslate(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 arguments for GetPrefabInstanceTranslate()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			CFloat x = g_currentInstancePrefab->GetTranslate().x;
			CFloat y = g_currentInstancePrefab->GetTranslate().y;
			CFloat z = g_currentInstancePrefab->GetTranslate().z;

			lua_pushnumber(L, x);
			lua_pushnumber(L, y);
			lua_pushnumber(L, z);

			return 3;
		}
		else
		{
			//PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
			return 0;
		}
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CFloat x = g_instancePrefab[i]->GetTranslate().x;
			CFloat y = g_instancePrefab[i]->GetTranslate().y;
			CFloat z = g_instancePrefab[i]->GetTranslate().z;

			lua_pushnumber(L, x);
			lua_pushnumber(L, y);
			lua_pushnumber(L, z);

			return 3;
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetPrefabInstanceRotate(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 arguments for GetPrefabInstanceRotate()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			CFloat x = g_currentInstancePrefab->GetRotate().x;
			CFloat y = g_currentInstancePrefab->GetRotate().y;
			CFloat z = g_currentInstancePrefab->GetRotate().z;

			lua_pushnumber(L, x);
			lua_pushnumber(L, y);
			lua_pushnumber(L, z);

			return 3;
		}
		else
		{
			//PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
			return 0;
		}
	}


	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CFloat x = g_instancePrefab[i]->GetRotate().x;
			CFloat y = g_instancePrefab[i]->GetRotate().y;
			CFloat z = g_instancePrefab[i]->GetRotate().z;

			lua_pushnumber(L, x);
			lua_pushnumber(L, y);
			lua_pushnumber(L, z);

			return 3;
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetPrefabInstanceScale(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 arguments for GetPrefabInstanceScale()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			CFloat x = g_currentInstancePrefab->GetScale().x;
			CFloat y = g_currentInstancePrefab->GetScale().y;
			CFloat z = g_currentInstancePrefab->GetScale().z;

			lua_pushnumber(L, x);
			lua_pushnumber(L, y);
			lua_pushnumber(L, z);

			return 3;
		}
		else
		{
			//PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
			return 0;
		}
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CFloat x = g_instancePrefab[i]->GetScale().x;
			CFloat y = g_instancePrefab[i]->GetScale().y;
			CFloat z = g_instancePrefab[i]->GetScale().z;

			lua_pushnumber(L, x);
			lua_pushnumber(L, y);
			lua_pushnumber(L, z);

			return 3;
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetPrefabInstanceRadius(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 arguments for GetPrefabInstanceRadius()", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			CFloat radius = g_currentInstancePrefab->GetRadius();

			lua_pushnumber(L, radius);

			return 1;
		}
		else
		{
			//PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
			return 0;
		}
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CFloat radius = g_instancePrefab[i]->GetRadius();

			lua_pushnumber(L, radius);

			return 1;
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetDistanceOfPrefabInstanceFromPhysicsCamera(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 arguments for GetDistanceOfPrefabInstanceFromPhysicsCamera()", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			CFloat distance = g_currentInstancePrefab->GetDistanceFromCamera();

			lua_pushnumber(L, distance);

			return 1;
		}
		else
		{
			//PrintInfo("\nCouldn't find current prefab instance", COLOR_RED);
			return 0;
		}
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			CFloat distance = g_instancePrefab[i]->GetDistanceFromCamera();

			lua_pushnumber(L, distance);

			return 1;
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}


//depth of field
CInt EnableDepthOfField(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	g_dofProperties.m_enable = CTrue;
	return 0;
}

CInt DisableDepthOfField(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	g_dofProperties.m_enable = CFalse;
	return 0;
}

CInt SetDepthOfFieldFocalDistance(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDepthOfFieldFocalDistance()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);
	if (value <= 0.0f)
	{
		//PrintInfo("\nPlease specify a positive value for SetDepthOfFieldFocalDistance()", COLOR_RED);
		return 0;
	}

	g_dofProperties.m_dofFocalDistance = value;

	return 0;
}

CInt SetDepthOfFieldFocalRange(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDepthOfFieldFocalRange()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);
	if (value <= 0.0f)
	{
		//PrintInfo("\nPlease specify a positive value for SetDepthOfFieldFocalRange()", COLOR_RED);
		return 0;
	}

	g_dofProperties.m_dofFocalRange = value;

	return 0;
}

//fog
CInt EnableFog(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	g_fogProperties.m_enable = CTrue;
	return 0;
}

CInt DisableFog(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	g_fogProperties.m_enable = CFalse;
	return 0;
}

CInt SetFogColor(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		//PrintInfo("\nPlease specify 3 arguments for SetFogColor()", COLOR_RED);
		return 0;
	}

	CFloat value[4];

	for (CUInt i = 1; i < 4; i++)
	{
		value[i - 1] = (CFloat)lua_tonumber(L, i);
		if (value[i - 1] < 0.0f || value[i - 1] > 1.0f)
		{
			//PrintInfo("\nError: All arguments of SetFogColor() must be between 0.0 and 1.0", COLOR_RED);
			return 0;
		}
		g_fogProperties.m_fogColor[i - 1] = value[i - 1];
	}
	g_fogProperties.m_fogColor[3] = 1.0f;

	CFog fog;
	fog.SetColor(g_fogProperties.m_fogColor);

	return 0;
}

CInt SetFogDensity(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetFogDensity()", COLOR_RED);
		return 0;
	}

	CFloat density;

	density = (CFloat)lua_tonumber(L, 1);

	if (density < 0.0f || density > 1.0f)
	{
		//PrintInfo("\nError: argument of SetFogDensity() must be between 0.0 and 1.0", COLOR_RED);
		return 0;
	}
	g_fogProperties.m_fogDensity = density;

	CFog fog;
	fog.SetDensity(g_fogProperties.m_fogDensity);

	return 0;
}

//bloom
CInt EnableBloom(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	g_bloomProperties.m_enable = CTrue;
	return 0;
}

CInt DisableBloom(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	g_bloomProperties.m_enable = CFalse;
	return 0;
}

CInt SetBloomColor(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		//PrintInfo("\nPlease specify 3 arguments for SetBloomColor()", COLOR_RED);
		return 0;
	}

	CFloat value[3];
	for (CUInt i = 1; i < 4; i++)
	{
		value[i - 1] = (CFloat)lua_tonumber(L, i);
		if (value[i - 1] < 0.0f || value[i - 1] > 1.0f)
		{
			//PrintInfo("\nError: All arguments of SetBloomColor() must be between 0.0 and 1.0", COLOR_RED);
			return 0;
		}
		g_bloomProperties.m_bloomColor[i - 1] = value[i - 1];
	}

	return 0;
}

CInt SetBloomIntensity(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetBloomIntensity()", COLOR_RED);
		return 0;
	}

	CFloat intensity;

	intensity = (CFloat)lua_tonumber(L, 1);
	if (intensity < 0.0f || intensity > 1.0f)
	{
		//PrintInfo("\nError: argument of SetBloomIntensity() must be between 0.0 and 1.0", COLOR_RED);
		return 0;
	}

	g_bloomProperties.m_bloomIntensity = intensity;

	return 0;
}

//shadow
CInt EnableDirectionalShadow(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	g_shadowProperties.m_enable = CTrue;

	return 0;
}

CInt DisableDirectionalShadow(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	g_shadowProperties.m_enable = CFalse;

	return 0;
}

CInt SetDirectionalShadowAlgorithm(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowAlgorithm()", COLOR_RED);
		return 0;
	}
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	if (Cmp(luaToString, "SHADOW_SINGLE_HL"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_SINGLE_HL;
	}
	else if (Cmp(luaToString, "SHADOW_SINGLE"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_SINGLE;
	}
	else if (Cmp(luaToString, "SHADOW_MULTI_LEAK"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_MULTI_LEAK;
	}
	else if (Cmp(luaToString, "SHADOW_MULTI_NOLEAK"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_MULTI_NOLEAK;
	}
	else if (Cmp(luaToString, "SHADOW_PCF"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_PCF;
	}
	else if (Cmp(luaToString, "SHADOW_PCF_TRILIN"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_PCF_TRILIN;
	}
	else if (Cmp(luaToString, "SHADOW_PCF_4TAP"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_PCF_4TAP;
	}
	else if (Cmp(luaToString, "SHADOW_PCF_8TAP"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_PCF_8TAP;
	}
	else if (Cmp(luaToString, "SHADOW_PCF_GAUSSIAN"))
	{
		g_shadowProperties.m_shadowType = eSHADOW_PCF_GAUSSIAN;
	}
	else
	{
		//PrintInfo("\nError: SetDirectionalShadowAlgorithm() argument is invalid", COLOR_RED);
	}

	return 0;
}

CInt SetDirectionalShadowNumberOfSplits(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	if (g_dynamicShadowMap == NULL)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowNumberOfSplits()", COLOR_RED);
		return 0;
	}
	int splits = lua_tonumber(L, 1);

	if (splits == 1)
	{
		g_shadowProperties.m_shadowSplits = eSHADOW_1_SPLIT;
	}
	else if (splits == 2)
	{
		g_shadowProperties.m_shadowSplits = eSHADOW_2_SPLITS;
	}
	else if (splits == 3)
	{
		g_shadowProperties.m_shadowSplits = eSHADOW_3_SPLITS;
	}
	else if (splits == 4)
	{
		g_shadowProperties.m_shadowSplits = eSHADOW_4_SPLITS;
	}
	else
	{
		//PrintInfo("\nError:SetDirectionalShadowNumberOfSplits() argument must be 1, 2, 3 or 4", COLOR_RED);
		return 0;
	}

	switch (g_shadowProperties.m_shadowSplits)
	{
	case eSHADOW_1_SPLIT:
		g_dynamicShadowMap->cur_num_splits = 1;
		break;
	case eSHADOW_2_SPLITS:
		g_dynamicShadowMap->cur_num_splits = 2;
		break;
	case eSHADOW_3_SPLITS:
		g_dynamicShadowMap->cur_num_splits = 3;
		break;
	case eSHADOW_4_SPLITS:
		g_dynamicShadowMap->cur_num_splits = 4;
		break;
	default:
		break;
	}

	return 0;
}

CInt SetDirectionalShadowWeightOfSplits(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	if (g_dynamicShadowMap == NULL)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowWeightOfSplits()", COLOR_RED);
		return 0;
	}

	CFloat weight = (CFloat)lua_tonumber(L, 1);

	g_shadowProperties.m_shadowSplitWeight = weight;

	g_dynamicShadowMap->split_weight = g_shadowProperties.m_shadowSplitWeight;

	return 0;
}

CInt SetDirectionalShadowNearClipPlane(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowNearClipPlane()", COLOR_RED);
		return 0;
	}

	CFloat ncp = (CFloat)lua_tonumber(L, 1);

	if (ncp <= 0.0f)
	{
		//PrintInfo("\nError: SetDirectionalShadowNearClipPlane() argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_shadowProperties.m_shadowNearClipPlane = ncp;

	return 0;
}

CInt SetDirectionalShadowFarClipPlane(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowFarClipPlane()", COLOR_RED);
		return 0;
	}

	CFloat fcp = (CFloat)lua_tonumber(L, 1);

	if (fcp <= 0.0f)
	{
		//PrintInfo("\nError: SetDirectionalShadowFarClipPlane() argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_shadowProperties.m_shadowFarClipPlane = fcp;

	return 0;
}

CInt SetDirectionalShadowResolution(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	if (g_dynamicShadowMap == NULL)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowResolution()", COLOR_RED);
		return 0;
	}
	int resolution = lua_tonumber(L, 1);

	if (resolution == 1024)
	{
		g_shadowProperties.m_shadowResolution = eSHADOW_1024;
	}
	else if (resolution == 2048)
	{
		g_shadowProperties.m_shadowResolution = eSHADOW_2048;
	}
	else if (resolution == 4096)
	{
		g_shadowProperties.m_shadowResolution = eSHADOW_4096;
	}
	else
	{
		//PrintInfo("\nError:SetDirectionalShadowResolution() argument must be 1024, 2048 or 4096", COLOR_RED);
		return 0;
	}

	switch (g_shadowProperties.m_shadowResolution)
	{
	case eSHADOW_1024:
		g_dynamicShadowMap->depth_size = 1024;
		break;
	case eSHADOW_2048:
		g_dynamicShadowMap->depth_size = 2048;
		break;
	case eSHADOW_4096:
		g_dynamicShadowMap->depth_size = 4096;
		break;
	default:
		break;
	}

	g_dynamicShadowMap->RegenerateDepthTex(g_dynamicShadowMap->depth_size);

	return 0;
}

CInt SetDirectionalShadowIntensity(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowIntensity()", COLOR_RED);
		return 0;
	}

	CFloat intensity = (CFloat)lua_tonumber(L, 1);

	if (intensity < 0.0f || intensity > 1.0f)
	{
		//PrintInfo("\nError: SetDirectionalShadowIntensity() argument must be between 0.0 and 1.0", COLOR_RED);
		return 0;
	}

	g_shadowProperties.m_intensity = intensity;

	return 0;
}

CInt SetDirectionalShadowLight(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowLight()", COLOR_RED);
		return 0;
	}
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));

	CBool foundTarget = CFalse;
	for (CUInt j = 0; j < g_engineLights.size(); j++)
	{
		if (g_engineLights[j]->m_abstractLight->GetType() == eLIGHTTYPE_DIRECTIONAL)
		{
			if (Cmp(g_engineLights[j]->m_abstractLight->GetName(), luaToString))
			{
				foundTarget = CTrue;
				break;
			}
		}
	}
	if (!foundTarget)
	{
		//PrintInfo("\nSetDirectionalShadowLight() Error:light name does not exist", COLOR_RED);
		return 0;
	}

	Cpy(g_shadowProperties.m_directionalLightName, luaToString);

	return 0;
}

CInt SetLightAmbient(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for SetLightAmbient()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		//PrintInfo("\nSecond argument of SetLightAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		//PrintInfo("\nThird argument of SetLightAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		//PrintInfo("\nFourth argument of SetLightAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	for (CUInt i = 0; i < g_engineLights.size(); i++)
	{
		CChar lightName[MAX_NAME_SIZE];
		Cpy(lightName, g_engineLights[i]->m_abstractLight->GetName());
		StringToUpper(lightName);

		if (Cmp(luaToString, lightName))
		{
			g_engineLights[i]->m_abstractLight->SetAmbient(Color);
			break;
		}
	}

	return 0;
}

CInt SetLightDiffuse(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for SetLightDiffuse()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		//PrintInfo("\nSecond argument of SetLightDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		//PrintInfo("\nThird argument of SetLightDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		//PrintInfo("\nFourth argument of SetLightDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	for (CUInt i = 0; i < g_engineLights.size(); i++)
	{
		CChar lightName[MAX_NAME_SIZE];
		Cpy(lightName, g_engineLights[i]->m_abstractLight->GetName());
		StringToUpper(lightName);

		if (Cmp(luaToString, lightName))
		{
			g_engineLights[i]->m_abstractLight->SetDiffuse(Color);
			break;
		}
	}

	return 0;
}

CInt SetLightSpecular(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for SetLightSpecular()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		//PrintInfo("\nSecond argument of SetLightSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		//PrintInfo("\nThird argument of SetLightSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		//PrintInfo("\nFourth argument of SetLightSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	for (CUInt i = 0; i < g_engineLights.size(); i++)
	{
		CChar lightName[MAX_NAME_SIZE];
		Cpy(lightName, g_engineLights[i]->m_abstractLight->GetName());
		StringToUpper(lightName);

		if (Cmp(luaToString, lightName))
		{
			g_engineLights[i]->m_abstractLight->SetSpecular(Color);
			break;
		}
	}

	return 0;
}

CInt SetLightShininess(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		//PrintInfo("\nPlease specify 2 arguments for SetLightShininess()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CFloat shininess = (CFloat)lua_tonumber(L, 2);

	for (CUInt i = 0; i < g_engineLights.size(); i++)
	{
		CChar lightName[MAX_NAME_SIZE];
		Cpy(lightName, g_engineLights[i]->m_abstractLight->GetName());
		StringToUpper(lightName);

		if (Cmp(luaToString, lightName))
		{
			g_engineLights[i]->m_abstractLight->SetShininess(shininess);
			break;
		}
	}

	return 0;
}

CInt SetPrefabInstanceAmbient(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for SetPrefabInstanceAmbient()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		//PrintInfo("\nSecond argument of SetPrefabInstanceAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		//PrintInfo("\nThird argument of SetPrefabInstanceAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		//PrintInfo("\nFourth argument of SetPrefabInstanceAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			g_currentInstancePrefab->SetAmbient(Color);
		}
		else
		{
			//PrintInfo("\nSetPrefabInstanceAmbient() Error: Couldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->SetAmbient(Color);
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\nSetPrefabInstanceAmbient() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceDiffuse(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for SetPrefabInstanceDiffuse()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		//PrintInfo("\nSecond argument of SetPrefabInstanceDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		//PrintInfo("\nThird argument of SetPrefabInstanceDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		//PrintInfo("\nFourth argument of SetPrefabInstanceDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			g_currentInstancePrefab->SetDiffuse(Color);
		}
		else
		{
			//PrintInfo("\nSetPrefabInstanceDiffuse() Error: Couldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->SetDiffuse(Color);
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\nSetPrefabInstanceDiffuse() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceSpecular(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for SetPrefabInstanceSpecular()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		//PrintInfo("\nSecond argument of SetPrefabInstanceSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		//PrintInfo("\nThird argument of SetPrefabInstanceSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		//PrintInfo("\nFourth argument of SetPrefabInstanceSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			g_currentInstancePrefab->SetSpecular(Color);
		}
		else
		{
			//PrintInfo("\nSetPrefabInstanceSpecular() Error: Couldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->SetSpecular(Color);
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\nSetPrefabInstanceSpecular() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceEmission(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		//PrintInfo("\nPlease specify 4 arguments for SetPrefabInstanceEmission()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		//PrintInfo("\nSecond argument of SetPrefabInstanceEmission() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		//PrintInfo("\nThird argument of SetPrefabInstanceEmission() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		//PrintInfo("\nFourth argument of SetPrefabInstanceEmission() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			g_currentInstancePrefab->SetEmission(Color);
		}
		else
		{
			//PrintInfo("\nSetPrefabInstanceEmission() Error: Couldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->SetEmission(Color);
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\nSetPrefabInstanceEmission() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceShininess(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		//PrintInfo("\nPlease specify 2 arguments for SetPrefabInstanceShininess()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CFloat shininess = (CFloat)lua_tonumber(L, 2);
	if (shininess < 0.0f)
	{
		//PrintInfo("\nSecond argument of SetPrefabInstanceShininess() must be 0.0 or higher", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			g_currentInstancePrefab->SetShininess(shininess);
		}
		else
		{
			//PrintInfo("\nSetPrefabInstanceShininess() Error: Couldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->SetShininess(shininess);
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\nSetPrefabInstanceShininess() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceTransparency(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		//PrintInfo("\nPlease specify 2 arguments for SetPrefabInstanceTransparency()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CFloat transparency = (CFloat)lua_tonumber(L, 2);
	if (transparency < 0.0f)
	{
		//PrintInfo("\nSecond argument of SetPrefabInstanceTransparency() must be 0.0 or higher", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
		{
			g_currentInstancePrefab->SetTransparency(transparency);
		}
		else
		{
			//PrintInfo("\nSetPrefabInstanceTransparency() Error: Couldn't find current prefab instance", COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->SetTransparency(transparency);
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\nSetPrefabInstanceTransparency() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt EnablePrefabInstanceMaterial(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for EnablePrefabInstanceMaterial()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
			g_currentInstancePrefab->EnableMaterial();
		//else
		//	PrintInfo("\nEnablePrefabInstanceMaterial() Error: Couldn't find current prefab instance", COLOR_RED);
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->EnableMaterial();
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\nEnablePrefabInstanceMaterial() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt DisablePrefabInstanceMaterial(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for DisablePrefabInstanceMaterial()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_currentInstancePrefab)
			g_currentInstancePrefab->DisableMaterial();
		//else
		//	PrintInfo("\nDisablePrefabInstanceMaterial Error: Couldn't find current prefab instance", COLOR_RED);
		return 0;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CChar prefabName[MAX_NAME_SIZE];
		Cpy(prefabName, g_instancePrefab[i]->GetName());
		StringToUpper(prefabName);
		if (Cmp(prefabName, luaToString))
		{
			foundPrefabInstance = CTrue;
			g_instancePrefab[i]->DisableMaterial();
		}
	}
	if (!foundPrefabInstance)
	{
		//CChar temp[MAX_NAME_SIZE];
		//sprintf(temp, "\nDisablePrefabInstanceMaterial() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		//PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt SetPhysicsRestitution(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsRestitution()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetPhysicsRestitution() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fDefaultRestitution = value;
	NxMaterial* defaultMaterial = gPhysXscene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(g_physXProperties.m_fDefaultRestitution);

	for (CUInt i = 0; i < gPhysXscene->getNbActors(); i++)
	{
		NxActor* currentActor = gPhysXscene->getActors()[i];
		if (currentActor->isSleeping())
			currentActor->wakeUp();
		currentActor->getShapes()[0]->setMaterial(defaultMaterial->getMaterialIndex());
	}

	return 0;
}

CInt SetPhysicsSkinWidth(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsSkinWidth()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetPhysicsSkinWidth() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fDefaultSkinWidth = value;
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, g_physXProperties.m_fDefaultSkinWidth);

	for (CUInt i = 0; i < gPhysXscene->getNbActors(); i++)
	{
		NxActor* currentActor = gPhysXscene->getActors()[i];
		if (currentActor->isSleeping())
			currentActor->wakeUp();
		currentActor->getShapes()[0]->setSkinWidth(g_physXProperties.m_fDefaultSkinWidth);
	}

	return 0;
}

CInt SetPhysicsStaticFriction(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsStaticFriction()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetPhysicsStaticFriction() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fDefaultStaticFriction = value;
	NxMaterial* defaultMaterial = gPhysXscene->getMaterialFromIndex(0);
	defaultMaterial->setStaticFriction(g_physXProperties.m_fDefaultStaticFriction);

	for (CUInt i = 0; i < gPhysXscene->getNbActors(); i++)
	{
		NxActor* currentActor = gPhysXscene->getActors()[i];
		if (currentActor->isSleeping())
			currentActor->wakeUp();
		currentActor->getShapes()[0]->setMaterial(defaultMaterial->getMaterialIndex());
	}

	return 0;
}

CInt SetPhysicsDynamicFriction(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicsDynamicFriction()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetPhysicsDynamicFriction() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fDefaultDynamicFriction = value;
	NxMaterial* defaultMaterial = gPhysXscene->getMaterialFromIndex(0);
	defaultMaterial->setDynamicFriction(g_physXProperties.m_fDefaultDynamicFriction);

	for (CUInt i = 0; i < gPhysXscene->getNbActors(); i++)
	{
		NxActor* currentActor = gPhysXscene->getActors()[i];
		if (currentActor->isSleeping())
			currentActor->wakeUp();
		currentActor->getShapes()[0]->setMaterial(defaultMaterial->getMaterialIndex());
	}

	return 0;
}

CInt EnablePhysicsGravity(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	if (!gPhysXscene)
	{
		return 0;
	}

	for (CUInt i = 0; i < gPhysXscene->getNbActors(); i++)
	{
		NxActor* currentActor = gPhysXscene->getActors()[i];
		if (currentActor->isSleeping())
			currentActor->wakeUp();
	}

	g_physXProperties.m_bApplyGravity = CTrue;
	g_nx->m_defaultGravity = NxVec3(g_physXProperties.m_fGravityX, g_physXProperties.m_fGravityY, g_physXProperties.m_fGravityZ);
	gPhysXscene->setGravity(NxVec3(g_nx->m_defaultGravity.x, g_nx->m_defaultGravity.y, g_nx->m_defaultGravity.z));

	return 0;
}

CInt DisablePhysicsGravity(lua_State* L)
{
	//if (g_testScript)
	//	return 0;

	if (!gPhysXscene)
	{
		return 0;
	}

	for (CUInt i = 0; i < gPhysXscene->getNbActors(); i++)
	{
		NxActor* currentActor = gPhysXscene->getActors()[i];
		if (currentActor->isSleeping())
			currentActor->wakeUp();
	}

	g_physXProperties.m_bApplyGravity = CFalse;
	g_nx->m_defaultGravity = NxVec3(0.0f, 0.0f, 0.0f);
	gPhysXscene->setGravity(NxVec3(0.0f));

	return 0;
}

CInt SetPhysicsGravity(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		//PrintInfo("\nPlease specify 3 arguments for SetPhysicsGravity()", COLOR_RED);
		return 0;
	}

	CFloat value1 = (CFloat)lua_tonumber(L, 1);
	CFloat value2 = (CFloat)lua_tonumber(L, 2);
	CFloat value3 = (CFloat)lua_tonumber(L, 3);

	g_physXProperties.m_fGravityX = value1;
	g_physXProperties.m_fGravityY = value2;
	g_physXProperties.m_fGravityZ = value3;

	for (CUInt i = 0; i < gPhysXscene->getNbActors(); i++)
	{
		NxActor* currentActor = gPhysXscene->getActors()[i];
		if (currentActor->isSleeping())
			currentActor->wakeUp();
	}

	if (g_physXProperties.m_bApplyGravity)
	{
		g_nx->m_defaultGravity = NxVec3(g_physXProperties.m_fGravityX, g_physXProperties.m_fGravityY, g_physXProperties.m_fGravityZ);
		gPhysXscene->setGravity(NxVec3(g_nx->m_defaultGravity.x, g_nx->m_defaultGravity.y, g_nx->m_defaultGravity.z));
	}

	return 0;
}

CInt EnablePhysicsGroundPlane(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	g_physXProperties.m_bGroundPlane = CTrue;

	g_nx->ReleaseGroundPlane();

	g_nx->CreateGroundPlane(g_physXProperties.m_fGroundHeight);
	NxVec3 rot0(0, 0, 0);
	NxVec3 rot1(0, 0, 0);
	NxVec3 rot2(0, 0, 0);
	NxMat33 rot(rot0, rot1, rot2);
	g_nx->m_groundBox = g_nx->CreateBox(NxVec3(0.0f, g_physXProperties.m_fGroundHeight, 0.0f), NxVec3(2000.0f, 0.1, 2000.0f), 0, rot, NULL, CFalse, CFalse);

	g_nx->gControllers->reportSceneChanged();
	gPhysXscene->simulate(EPSILON);
	gPhysXscene->flushStream();
	gPhysXscene->fetchResults(NX_ALL_FINISHED, true);

	return 0;
}

CInt DisablePhysicsGroundPlane(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	g_physXProperties.m_bGroundPlane = CFalse;

	g_nx->ReleaseGroundPlane();

	g_nx->gControllers->reportSceneChanged();
	gPhysXscene->simulate(EPSILON);
	gPhysXscene->flushStream();
	gPhysXscene->fetchResults(NX_ALL_FINISHED, true);

	return 0;
}

CInt SetPhysicGroundHeight(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetPhysicGroundHeight()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);
	g_physXProperties.m_fGroundHeight = value;

	if (g_physXProperties.m_bGroundPlane)
	{
		g_nx->ReleaseGroundPlane();

		g_nx->CreateGroundPlane(g_physXProperties.m_fGroundHeight);
		NxVec3 rot0(0, 0, 0);
		NxVec3 rot1(0, 0, 0);
		NxVec3 rot2(0, 0, 0);
		NxMat33 rot(rot0, rot1, rot2);
		g_nx->m_groundBox = g_nx->CreateBox(NxVec3(0.0f, g_physXProperties.m_fGroundHeight, 0.0f), NxVec3(2000.0f, 0.1, 2000.0f), 0, rot, NULL, CFalse, CFalse);
	}

	g_nx->gControllers->reportSceneChanged();
	gPhysXscene->simulate(EPSILON);
	gPhysXscene->flushStream();
	gPhysXscene->fetchResults(NX_ALL_FINISHED, true);

	return 0;
}

CInt SetDistanceBetweenPhysicsCameraAndCharacterController(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetDistanceBetweenPhysicsCameraAndCharacterController()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetDistanceBetweenPhysicsCameraAndCharacterController() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCameraCharacterDistance = value;
	g_nx->gDesiredDistance = value;

	return 0;
}

CInt SetCharacterControllerCapsuleRadius(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetCharacterControllerCapsuleRadius()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetCharacterControllerCapsuleRadius() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCapsuleRadius = value;
	g_nx->gControllers->setRadius(g_physXProperties.m_fCapsuleRadius);

	return 0;
}

CInt SetCharacterControllerCapsuleHeight(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetCharacterControllerCapsuleHeight()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetCharacterControllerCapsuleHeight() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCapsuleHeight = value;
	g_nx->gControllers->setHeight(g_physXProperties.m_fCapsuleHeight);

	return 0;
}

CInt SetCharacterControllerForcePower(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetCharacterControllerForcePower()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetCharacterControllerForcePower() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCharacterPower = value;

	return 0;
}

CInt SetCharacterControllerWalkSpeed(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetCharacterControllerWalkSpeed()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetCharacterControllerWalkSpeed() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCharacterWalkSpeed = value;
	g_nx->gCharacterWalkSpeed = g_physXProperties.m_fCharacterWalkSpeed;

	return 0;
}

CInt SetCharacterControllerRunSpeed(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetCharacterControllerRunSpeed()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetCharacterControllerRunSpeed() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCharacterRunSpeed = value;
	g_nx->gCharacterRunSpeed = g_physXProperties.m_fCharacterRunSpeed;

	return 0;
}

CInt SetCharacterControllerSkinWidth(lua_State* L)
{
	return 0;
}

CInt SetCharacterControllerStepOffset(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetCharacterControllerStepOffset()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetCharacterControllerStepOffset() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCharacterStepOffset = value;
	g_nx->gControllers->setStepOffset(g_physXProperties.m_fCharacterStepOffset);

	return 0;
}

CInt SetCharacterControllerSlopeLimit(lua_State* L)
{
	return 0;
}

CInt SetCharacterControllerJumpPower(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		//PrintInfo("\nPlease specify 1 argument for SetCharacterControllerJumpPower()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		//PrintInfo("\nSetCharacterControllerJumpPower() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fJumpPower = value;

	return 0;
}

CInt EnableCharacterControllerJump(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	g_physXProperties.m_bJumping = CTrue;

	return 0;
}

CInt DisableCharacterControllerJump(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	g_physXProperties.m_bJumping = CFalse;

	return 0;
}

CInt EnablePhysicsDebugMode(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	g_physXProperties.m_bDebugMode = CTrue;
	g_nx->debugMode = g_physXProperties.m_bDebugMode;

	return 0;
}

CInt DisablePhysicsDebugMode(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	g_physXProperties.m_bDebugMode = CFalse;
	g_nx->debugMode = g_physXProperties.m_bDebugMode;

	return 0;
}

CInt SetCharacterControllerPosition(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		//PrintInfo("\nPlease specify 3 arguments for SetCharacterControllerPosition()", COLOR_RED);
		return 0;
	}

	CFloat value1 = (CFloat)lua_tonumber(L, 1);
	CFloat value2 = (CFloat)lua_tonumber(L, 2);
	CFloat value3 = (CFloat)lua_tonumber(L, 3);

	NxExtendedVec3 nxPos(value1, value2, value3);
	g_nx->SetCharacterPos(nxPos);

	return 0;
}

CInt GetCharacterControllerPosition(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	//if (g_testScript)
	//	return 0;

	CFloat x = g_nx->GetFilteredCharacterPos().x;
	CFloat y = g_nx->GetFilteredCharacterPos().y;
	CFloat z = g_nx->GetFilteredCharacterPos().z;

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);

	return 3;
}

CBool CMain::firstIdle = CTrue;
CChar CMain::currentIdleName[MAX_NAME_SIZE];

CMain::CMain()
{
	m_showHelpInfo = CFalse;
	m_timerCounter = 0;
	m_totalElapsedTime = 0;
	m_lockInput = CFalse;
	m_loadScene = CFalse;
	m_soundSystem = NULL;
	m_renderQuery = CFalse;
	Cpy(g_currentZipFileName, "\n");
	m_characterRotationTransition = CFalse;
	m_previousCharacterRotation = 0.0f;
	m_idleCounter = 0.0f;
	m_previousRightButtonDown = CFalse;
	m_previousLeftButtonDown = CFalse;
	m_cursorIcon = CNew(CIcon);
	Cpy(m_previousCharacterAnimationType, "\n");
	m_menuCursorImg = NULL;
	m_publishDebug = CFalse;
	m_exitGame = CFalse;
	m_mousePosition.x = (CFloat)g_width / 2.f;
	m_mousePosition.y = (CFloat)g_height / 2.f;

	m_fSelectionDistance = -1.f;
	m_bSelectionDistance = CFalse;

}

CMain::~CMain()
{
	//CDelete( m_particleSystem );
	m_dof.DeleteFBOs();
	g_octree->ResetState();
	distance_vector.clear();
	sorted_prefabs.clear();

	CDelete(m_cursorIcon);
	DeleteMenuCursorTexture();
}

void CMain::RemoveSelectedScene(CChar* szBuffer, CChar* sceneId)
{
	for (CUInt i = 0; i < g_scene.size(); i++)
	{
		CBool condition = CFalse;
		if (sceneId)
			condition = Cmp(g_scene[i]->m_pureFileName, szBuffer) && Cmp(g_scene[i]->GetDocURI(), sceneId);
		else
			condition = Cmp(g_scene[i]->m_pureFileName, szBuffer);
		if (condition)
		{
			//remove unshared images
			for (CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++)
			{
				CGeometry* m_geo = g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry;
				if (m_geo)
				{
					for (CUInt k = 0; k < m_geo->m_groups.size(); k++)
					{
						for (CUInt l = 0; l < 4; l++)
						{
							CImage* currentImage = NULL;
							if (l == 0 && m_geo->m_groups[k]->m_hasDiffuse && m_geo->m_groups[k]->m_diffuseImg)
							{
								currentImage = m_geo->m_groups[k]->m_diffuseImg;
							}
							else if (l == 1 && m_geo->m_groups[k]->m_hasNormalMap && m_geo->m_groups[k]->m_normalMapImg)
							{
								currentImage = m_geo->m_groups[k]->m_normalMapImg;
							}
							else if (l == 2 && m_geo->m_groups[k]->m_hasGlossMap && m_geo->m_groups[k]->m_glossMapImg)
							{
								currentImage = m_geo->m_groups[k]->m_glossMapImg;
							}
							else if (l == 3 && m_geo->m_groups[k]->m_hasDirtMap && m_geo->m_groups[k]->m_dirtMapImg)
							{
								currentImage = m_geo->m_groups[k]->m_dirtMapImg;
							}
							if (currentImage)
							{
								//see if other scenes use this texture
								CBool foundTarget = CFalse;
								for (CUInt m = 0; m < g_scene.size(); m++)
								{
									CBool condition = CFalse;
									condition = Cmp(g_scene[m]->GetName(), g_scene[i]->GetName()) && Cmp(g_scene[m]->GetDocURI(), g_scene[i]->GetDocURI());

									if (!condition)
									{
										for (CUInt n = 0; n < g_scene[m]->m_instanceGeometries.size(); n++)
										{
											CGeometry* m_geo = g_scene[m]->m_instanceGeometries[n]->m_abstractGeometry;
											for (CUInt o = 0; o < m_geo->m_groups.size(); o++)
											{
												if (m_geo->m_groups[o]->m_hasDiffuse && Cmp(GetAfterPath(currentImage->GetFileName()), GetAfterPath(m_geo->m_groups[o]->m_diffuseImg->GetFileName())))
												{
													foundTarget = CTrue;
													break;
												}
												else if (m_geo->m_groups[o]->m_hasNormalMap && Cmp(GetAfterPath(currentImage->GetFileName()), GetAfterPath(m_geo->m_groups[o]->m_normalMapImg->GetFileName())))
												{
													foundTarget = CTrue;
													break;
												}
												else if (m_geo->m_groups[o]->m_hasGlossMap && Cmp(GetAfterPath(currentImage->GetFileName()), GetAfterPath(m_geo->m_groups[o]->m_glossMapImg->GetFileName())))
												{
													foundTarget = CTrue;
													break;
												}
												else if (m_geo->m_groups[o]->m_hasDirtMap && Cmp(GetAfterPath(currentImage->GetFileName()), GetAfterPath(m_geo->m_groups[o]->m_dirtMapImg->GetFileName())))
												{
													foundTarget = CTrue;
													break;
												}
											}
											if (foundTarget)
												break;
										}
									}
									if (foundTarget)
										break;
								}
								if (!foundTarget)
								{
									for (CUInt p = 0; p < g_images.size(); p++)
									{
										if (Cmp(GetAfterPath(g_images[p]->GetFileName()), GetAfterPath(currentImage->GetFileName())))
										{
											CDelete(currentImage);
											g_images.erase(g_images.begin() + p);
											break;
										}
									}
								} //if (!foundTarget )
							} //if( currentImage )
						} //for( CUInt l = 0; l < 6; l++ )
					} //for groups
				}
			} //for other scenes
			//save functions///////////////////////////////////////////////////
		} //if
	} //for
	//remove unshared geometry
	CBool foundTarget = CFalse;
	for (CUInt i = 0; i < g_scene.size(); i++)
	{
		CBool condition = CFalse;
		if (sceneId)
			condition = Cmp(g_scene[i]->m_pureFileName, szBuffer) && Cmp(g_scene[i]->GetDocURI(), sceneId);
		else
			condition = Cmp(g_scene[i]->m_pureFileName, szBuffer);
		if (condition)
		{
			//delete unshared geometries
			for (CUInt m = 0; m < g_scene[i]->m_instanceGeometries.size(); m++)
			{
				CGeometry* m_geo1 = g_scene[i]->m_instanceGeometries[m]->m_abstractGeometry;
				if (m_geo1)
				{
					for (CUInt j = 0; j < g_scene.size(); j++)
					{
						CBool condition = CFalse;
						if (sceneId)
							condition = Cmp(g_scene[j]->m_pureFileName, szBuffer) && Cmp(g_scene[j]->GetDocURI(), sceneId);
						else
							condition = Cmp(g_scene[j]->m_pureFileName, szBuffer);
						if (!condition)
						{
							//delete unshared geometries
							for (CUInt k = 0; k < g_scene[j]->m_instanceGeometries.size(); k++)
							{
								CGeometry* m_geo2 = g_scene[j]->m_instanceGeometries[k]->m_abstractGeometry;
								if (m_geo2)
								{
									if (ICmp(m_geo1->GetName(), m_geo2->GetName()) &&
										ICmp(m_geo1->GetDocURI(), m_geo2->GetDocURI()))
									{
										foundTarget = CTrue;
										break;
									}
								}
							}
						}
						if (foundTarget)
							break;
					}
				}
				if (foundTarget)
					break;
			}
		}
		if (foundTarget)
			break;
	}
	if (!foundTarget)
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			CBool condition = CFalse;
			if (sceneId)
				condition = Cmp(g_scene[i]->m_pureFileName, szBuffer) && Cmp(g_scene[i]->GetDocURI(), sceneId);
			else
				condition = Cmp(g_scene[i]->m_pureFileName, szBuffer);
			if (condition)
			{
				//delete unshared geometries
				for (CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++)
				{
					CGeometry* m_geo1 = g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry;

					for (CUInt l = 0; l < g_geometries.size(); l++)
					{
						if (ICmp(g_geometries[l]->GetName(), m_geo1->GetName()) &&
							ICmp(g_geometries[l]->GetDocURI(), m_geo1->GetDocURI()))
						{

							CDelete(g_geometries[l]);
							g_geometries.erase(g_geometries.begin() + l);
							break;
						}
					}
				}
				break;
			}
		}
	}

	//remove selected scene
	for (CUInt i = 0; i < g_scene.size(); i++)
	{
		CBool condition = CFalse;
		if (sceneId)
			condition = Cmp(g_scene[i]->m_pureFileName, szBuffer) && Cmp(g_scene[i]->GetDocURI(), sceneId);
		else
			condition = Cmp(g_scene[i]->m_pureFileName, szBuffer);
		if (condition)
		{
			//delete the scene

			CDelete(g_scene[i]);
			//delete the vector that holds the scene
			g_scene.erase(g_scene.begin() + i);
			break;
		}
	}
}

CBool CMain::Init()
{
	//initialize lua/////////////////
	glShadeModel(GL_SMOOTH);										// Enable Smooth Shading
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);							// Black Background
	glEnable(GL_TEXTURE_2D);							
	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK ); 
	glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );
	GLfloat globalAmbientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbientLight );
	glEnable( GL_NORMALIZE );

	CFog fog;
	fog.SetColor( g_fogProperties.m_fogColor );
	fog.SetDensity( g_fogProperties.m_fogDensity );
	//SetTimer( 1, 0 , 0 );
	glDrawBuffer( GL_BACK );
	// I use this variable to load this data once. We may destroy and create the window multiple times,
	// But we don't need to load and unload some data for multiple times
	//initialize audio
	// Position of the listener.
	m_listenerPos[0] = m_listenerPos[1] = m_listenerPos[2] = 0.0f;
	// Velocity of the listener.
	m_listenerVel[0] = m_listenerVel[1] = m_listenerVel[2] =  0.0f;
	// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
	m_listenerOri[0] = 0.0f;
	m_listenerOri[1] = 0.0f;
	m_listenerOri[2] = 1.0f;
	m_listenerOri[3] = 0.0f;
	m_listenerOri[4] = 1.0f;
	m_listenerOri[5] = 0.0f;

	g_soundSystem = m_soundSystem = CNew(COpenALSystem);
	m_soundSystem->Init();

	m_ambientSound = NULL;
	alDistanceModel( AL_INVERSE_DISTANCE );

	g_input.AcquireAll();
	if( !g_input.Initialize( g_window.m_windowGL.hWnd, g_instance, true, IS_USEKEYBOARD | IS_USEMOUSE ) )
	{
		MessageBox( NULL, _T("CMain::Init():Couldn't initialize Input Engine"), _T("VandaEngine Error"), MB_OK );
		return false;
	}

	g_timer = CNew( CTimer );
	if( !g_timer->Init() )
	{
		//Report an error message
		MessageBox( NULL, _T("Couldn't initialize the timer"), _T("VandaEngine Error"), MB_OK | MB_ICONINFORMATION );
		return CFalse; 
	}
	g_nx = CNew( CNovodex );
	if (!g_nx->initNx(10.0, 5.0, 10.0, g_physXProperties.m_fCapsuleRadius, g_physXProperties.m_fCapsuleHeight, g_physXProperties.m_fCharacterSlopeLimit, g_physXProperties.m_fCharacterSkinWidth, g_physXProperties.m_fCharacterStepOffset))
	{
		MessageBox( NULL, _T("Couldn't initialize physX"),_T("VandaEngine Error"), MB_OK | MB_ICONINFORMATION );
		return false;
	}
	g_font = CNew( CFont );
	if( !g_font->Init( _T("Arial"), 13 ) )
	{
		MessageBox( NULL, _T("Couldn't initialize the font"),_T("VandaEngine Error"), MB_OK | MB_ICONINFORMATION );
		return false;
	}

	g_render.Init();
	if (!g_render.m_shaderAvailable)
	{
		MessageBoxA(NULL, "Couldn't initialize shaders", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	g_camera = new CUpdateCamera();

	if (!g_render.SupportForFBOs() || !g_render.SupportForVBOs())
	{
		MessageBoxA(NULL, "Your implementation doesn't support FBO or VBO.\nUpdating the driver of your graphics card may solve the issue", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ilInit();
	iluInit();
	if(GLEW_ARB_color_buffer_float)
	{
		glClampColorARB( GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE );
		glClampColorARB( GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE );
	}

	g_render.m_useBloom = CFalse;
	//bloom effect requires NPOT textures, FBO and shaders
	if( GLEW_ARB_texture_non_power_of_two && g_render.UsingFBOs() && g_options.m_enableFBO && g_render.UsingShader() && g_options.m_enableShader )
	{
		g_bloom = CNew( CBloom );
		g_bloom->CreateEmptyTexture( g_width, g_height, GL_RGB, GL_RGB );
		g_render.m_useBloom = CTrue;
	}

	m_dynamicShadowMap = CNew( CShadowMap );
	g_dynamicShadowMap = m_dynamicShadowMap;
	if( m_dynamicShadowMap->Init() )
		g_render.m_useDynamicShadowMap = CTrue;
	else
	{
		g_render.m_useDynamicShadowMap = CFalse;
	}

	if( !g_useOldRenderingStyle )
		InitFBOs( GL_RGBA, GL_RGBA );

	if( g_render.UsingShader() && g_options.m_enableShader )
	{
		g_render.m_useWaterReflection = CTrue;
	}
	else
	{
		g_render.m_useWaterReflection = CFalse;
	}
	g_octree = CNew( COctree );

	return CTrue;
}

CVoid CMain::Release()
{
	glDeleteTextures(eGBUFFER_NUM_TEXTURES, &m_textureTarget[0] );								
	glDeleteTextures(1, &m_textureTargetSwapLights);								
	glDeleteTextures(1, &m_textureTargetSumLighting);								
	glDeleteTextures(1, &m_textureFogDof);								

	glDeleteFramebuffersEXT( 1, &m_mFboID );
	glDeleteFramebuffersEXT( 1, &m_mFboID2 );
	glDeleteFramebuffersEXT( 1, &m_fboID );
	glDeleteFramebuffersEXT( 1, &m_fboID2 );
	glDeleteFramebuffersEXT( 1, &m_fboIDSum );
	glDeleteFramebuffersEXT( 1, &m_fboIDFogDof );
	glDeleteFramebuffersEXT( 1, &m_mFboIDFogDof );
	CDelete(g_mainCharacter);

	//remove gui
	for (CUInt i = 0; i < g_guiButtons.size(); i++)
	{
		CDelete(g_guiButtons[i]);
	}
	g_guiButtons.clear();

	for (CUInt i = 0; i < g_guiBackgrounds.size(); i++)
	{
		CDelete(g_guiBackgrounds[i]);
	}
	g_guiBackgrounds.clear();

	for (CUInt i = 0; i < g_guiTexts.size(); i++)
	{
		CDelete(g_guiTexts[i]);
	}
	g_guiTexts.clear();

	for (CUInt i = 0; i < g_guis.size(); i++)
	{
		CDelete(g_guis[i]);
	}
	g_guis.clear();

	for (CUInt i = 0; i < g_triggers.size(); i++)
	{
		CDelete(g_triggers[i]);
	}
	g_triggers.clear();

	for( std::vector<CScene*>::iterator it = g_scene.begin(); it != g_scene.end(); it++ )
	{
		CDelete( *it );
	}

	//clear prefabs
	for (CUInt i = 0; i < g_prefab.size(); i++)
	{
		CDelete(g_prefab[i]);
	}
	g_prefab.clear();

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CDelete(g_instancePrefab[i]);
	}
	g_instancePrefab.clear();

	//Clear the vctor objects
	g_scene.clear();
	g_render.Destroy();

	CDelete( m_dynamicShadowMap );
	CDelete( g_skyDome );
	CDelete(g_terrain);
	CDelete(g_startup);

	for( std::vector<CInstanceLight*>::iterator it = g_engineLights.begin(); it != g_engineLights.end(); it++ )
	{
		CDelete( (*it)->m_abstractLight );
		CDelete( *it );
	}
	g_engineLights.clear();

	for (std::vector<CInstanceCamera*>::iterator it = g_engineCameraInstances.begin(); it != g_engineCameraInstances.end(); it++)
	{
		CDelete((*it)->m_abstractCamera);
		CDelete(*it);
	}
	g_engineCameraInstances.clear();

	for( std::vector<CWater*>::iterator it = g_engineWaters.begin(); it != g_engineWaters.end(); it++ )
	{
		CDelete( *it );
	}
	g_engineWaters.clear();

	for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
	{
		CDelete(g_engineStaticSounds[i]);
	}
	g_engineStaticSounds.clear();

	//delete the static sound buffers
	for( std::vector<COpenALSoundBuffer*>::iterator it = g_soundBuffers.begin(); it != g_soundBuffers.end(); it++ )
	{
		CDelete( *it );
	}
	g_soundBuffers.clear();

	for( std::vector<CImage*>::iterator it = g_images.begin(); it != g_images.end(); it++ )
	{
		CDelete( *it );
	}
	if( g_images.size() > 0 )
		g_images.clear();

	CDelete( g_bloom );

	for( std::vector<CImage*>::iterator it = g_waterImages.begin(); it != g_waterImages.end(); it++ )
	{
		CDelete( *it );
	}
	if( g_waterImages.size() > 0 )
		g_waterImages.clear();

	//Delete Resource Files
	for (CUInt j = 0; j < g_resourceFiles.size(); j++)
		CDelete(g_resourceFiles[j]);
	g_resourceFiles.clear();

	if (g_main && g_main->GetCursorIcon())
		g_main->GetCursorIcon()->SetVisible(CFalse);

	//Release Audio
	CDelete( m_ambientSound );
	CDelete(m_soundSystem );

	g_input.Shutdown();
	CDelete( g_timer );
	if( g_nx )
	{
		g_nx->releaseNx();
		CDelete( g_nx )
	}
	if( g_font )
	{
		g_font->Release();
		CDelete( g_font );
	}
	CDelete( g_camera );
}

CVoid CMain::ResetTimer()
{
	elapsedTime = g_timer->GetElapsedSeconds( CTrue ); //start from the beginning
	g_timer->GetFps( 1, CTrue );
	g_elapsedTime = elapsedTime;
	m_timerCounter  = 0;
	m_totalElapsedTime = 0;
}

CVoid CMain::CalculateDistnces(CBool force)
{
	//calculate the distance of instance prefabs from camera
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		if (!force)
		{
			if (!m_calculateDistance && !g_instancePrefab[i]->GetIsAnimated() && !g_instancePrefab[i]->GetIsControlledByPhysX()) continue;
			if (!g_instancePrefab[i]->GetVisible2()) continue;
		}
		//CScene* scene = NULL;

		//CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		//for (CUInt j = 0; j < 3; j++)
		//{
		//	if (prefab && prefab->GetHasLod(j))
		//	{
		//		scene = g_instancePrefab[i]->GetScene(j);
		//		if (scene)
		//			scene->CalculateDistances();
		//	}
		//}
		g_instancePrefab[i]->CalculateDistance();
	}
	if (m_calculateDistance || force)
	{
		for (CUInt i = 0; i < g_engineLights.size(); i++)
		{
			g_engineLights[i]->CalculateDistance();
		}
		for (CUInt i = 0; i < g_engineWaters.size(); i++)
		{
			g_engineWaters[i]->CalculateDistance();
		}
	}
}

CBool CMain::Render()
{
	if (g_clickedOpen)
		return CTrue;
	g_numVerts = 0; //debug info

	if (GetExitGame()) return CFalse; //exit game 

	if (!g_currentVSceneProperties.m_isPause)
	{
		elapsedTime = g_timer->GetElapsedSeconds();
		g_elapsedTime = elapsedTime;
		m_totalElapsedTime += elapsedTime;

		//use multithreading for animations
		/*std::thread t1(&CMain::*/UpdateAnimations();/*, this, false)*/;
		//t1.join();
		UpdateDynamicPhysicsObjects();
	}

	if( !g_useOldRenderingStyle && g_window.m_windowGL.multiSampling && g_options.m_enableFBO)
		g_render.BindFBO(m_mFboID);
	else if( !g_useOldRenderingStyle && !g_window.m_windowGL.multiSampling && g_options.m_enableFBO)
		g_render.BindFBO( m_fboID );

	if( !g_useOldRenderingStyle )
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
		GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT};
		glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);
	}
	else
		glDrawBuffer( GL_BACK );
	m_cameraTypeOfPreviousFrame = m_cameraType;


	if( g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX )
	{
		CInstanceCamera *instanceCamera = g_render.GetDefaultInstanceCamera();
		if (instanceCamera)
		{
			m_cameraType = eCAMERA_DEFAULT_FREE_NO_PHYSX;
			SetInstanceCamera(instanceCamera, g_width / 2.f, g_height / 2.f, instanceCamera->m_abstractCamera->GetAngle(), g_cameraProperties.m_freePerspectiveNCP, g_cameraProperties.m_freePerspectiveFCP);
			m_lockInput = CFalse;
		}
		else
		{
			g_currentCameraType = eCAMERA_PHYSX;
		}
	}
	CInstanceCamera *instanceCamera = g_render.GetActiveInstanceCamera();
	if( g_currentCameraType == eCAMERA_COLLADA )
	{
		if( instanceCamera )		
		{
			CBool enableColladaCamera = CTrue;
			if (instanceCamera->IsTimerEnabled())
			{
				if (!instanceCamera->IncreaseElapsedSeconds(elapsedTime))
					enableColladaCamera = CFalse;
			}
			if (enableColladaCamera)
			{
				m_cameraType = eCAMERA_COLLADA;

				m_lockInput = CTrue;
				CFloat m_fov, m_zNear, m_zFar;

				if (g_cameraProperties.m_readDAECameraFOVFromFile)
					m_fov = instanceCamera->m_abstractCamera->GetYFov();
				else
					m_fov = g_cameraProperties.m_daeCameraFOV;

				if (g_cameraProperties.m_readDAECameraNCPFromFile)
					m_zNear = instanceCamera->m_abstractCamera->GetZNear();
				else
					m_zNear = g_cameraProperties.m_daeCameraNCP;

				if (g_cameraProperties.m_readDAECameraFCPFromFile)
					m_zFar = instanceCamera->m_abstractCamera->GetZFar();
				else
					m_zFar = g_cameraProperties.m_daeCameraFCP;


				SetInstanceCamera(instanceCamera, g_width / 2.f, g_height / 2.f, m_fov, m_zNear, m_zFar);
			}
			else
			{
				g_currentCameraType = eCAMERA_PHYSX;
				g_render.SetActiveInstanceCamera(NULL);
			}
		}
		else
		{
			g_currentCameraType = eCAMERA_PHYSX;
		}
	}
	if (g_currentCameraType == eCAMERA_ENGINE)
	{
		CBool foundTarget = CFalse;
		for (CUInt c = 0; c < g_engineCameraInstances.size(); c++)
		{
			if (g_engineCameraInstances[c]->IsActive())
			{
				if (g_engineCameraInstances[c]->IsTimerEnabled())
				{
					if (g_engineCameraInstances[c]->IncreaseElapsedSeconds(elapsedTime))
					{
						SetInstanceCamera(g_engineCameraInstances[c], g_width / 2.f, g_height / 2.f, g_engineCameraInstances[c]->m_abstractCamera->GetAngle(), g_engineCameraInstances[c]->GetNCP(), g_engineCameraInstances[c]->GetFCP());
						g_render.SetActiveInstanceCamera(g_engineCameraInstances[c]);
						foundTarget = CTrue;
						break;
					}
				}
				else
				{
					SetInstanceCamera(g_engineCameraInstances[c], g_width / 2.f, g_height / 2.f, g_engineCameraInstances[c]->m_abstractCamera->GetAngle(), g_engineCameraInstances[c]->GetNCP(), g_engineCameraInstances[c]->GetFCP());
					g_render.SetActiveInstanceCamera(g_engineCameraInstances[c]);
					foundTarget = CTrue;
					break;
				}
				if (!foundTarget)
				{
					g_engineCameraInstances[c]->SetActive(CFalse);
					g_render.SetActiveInstanceCamera(NULL);
					g_currentCameraType = eCAMERA_PHYSX;
					break;
				}
			}
		}
		if (!foundTarget)
			g_currentCameraType = eCAMERA_PHYSX;
	}
	if (g_currentCameraType == eCAMERA_PHYSX)
	{
		m_cameraType = eCAMERA_PHYSX;

		m_lockInput = CFalse;
		g_camera->m_cameraManager->SetPerspective(g_camera->m_cameraManager->GetAngle(), g_width, g_height, g_cameraProperties.m_playModePerspectiveNCP, g_cameraProperties.m_playModePerspectiveFCP);
		if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
		{
			//shadow
			glGetFloatv(GL_PROJECTION_MATRIX, cam_proj);
		}

		//previous position of character controller
		g_camera->m_perspectiveCharacterPosOfPreviousFrame = g_camera->m_perspectiveCharacterPos;
		g_nx->GetCameraAndCharacterPositions( g_camera->m_perspectiveCameraPitch,
			g_camera->m_perspectiveCameraYaw,
			g_camera->m_perspectiveCameraPos,
			g_camera->m_perspectiveCharacterPos,
			g_camera->m_perspectiveCameraDir );

		g_camera->m_cameraManager->UpdateCameraPosDir( g_camera->m_perspectiveCameraPos,
			g_camera->m_perspectiveCharacterPos,
			g_camera->m_perspectiveCameraDir,
			g_camera->m_perspectiveCurrentCameraTilt );
	}

	//if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	//{
		//shadow
		// store the inverse of the resulting modelview matrix for the shadow computation
		glGetFloatv(GL_MODELVIEW_MATRIX, cam_modelview);
		g_camera->m_cameraManager->GetInverseMatrix( cam_inverse_modelview );
		//////////////
	//}
	g_camera->m_cameraManager->UpdateFrustum();
	g_octree->ResetOctreeGeoCount();
	g_octree->Render(CTrue);
	g_octree->ResetOctreeGeoCount();

	m_calculateDistance = CFalse;
	if ( m_cameraType != m_cameraTypeOfPreviousFrame )
	{
		m_calculateDistance = CTrue;
	}
	//calculate distance between geometries and main camera
	else if( !(g_camera->m_perspectiveCharacterPosOfPreviousFrame.x - g_camera->m_perspectiveCharacterPos.x == 0.0f &&
		g_camera->m_perspectiveCharacterPosOfPreviousFrame.y - g_camera->m_perspectiveCharacterPos.y == 0.0f &&
		g_camera->m_perspectiveCharacterPosOfPreviousFrame.z - g_camera->m_perspectiveCharacterPos.z == 0.0f ) )
	{
		m_calculateDistance = CTrue;
	}
	else if( instanceCamera )
	{
		if( !(g_camera->m_perspectiveCameraPosOfPreviousFrame.x - g_camera->m_perspectiveCameraPos.x == 0.0f &&
			g_camera->m_perspectiveCameraPosOfPreviousFrame.y - g_camera->m_perspectiveCameraPos.y == 0.0f &&
			g_camera->m_perspectiveCameraPosOfPreviousFrame.z - g_camera->m_perspectiveCameraPos.z == 0.0f ) )
		{
			m_calculateDistance = CTrue;
		}
	}

	UpdatePrefabInstanceBB();

	CalculateDistnces();

	//shadow
	const float bias[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f };

	if (!g_useOldRenderingStyle && g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader)
	{
		float light_dir[4] = { g_defaultDirectionalLight.x, g_defaultDirectionalLight.y, g_defaultDirectionalLight.z, 1.0f };
		if (!instanceCamera)
		{
			cam_pos[0] = g_camera->m_perspectiveCameraPos.x;
			cam_pos[1] = g_camera->m_perspectiveCameraPos.y + g_physXProperties.m_fCapsuleHeight;
			cam_pos[2] = g_camera->m_perspectiveCameraPos.z;

			cam_dir[0] = g_camera->m_perspectiveCharacterPos.x - cam_pos[0];
			cam_dir[1] = g_camera->m_perspectiveCharacterPos.y + (g_physXProperties.m_fCapsuleHeight / 2.f) + g_camera->m_perspectiveCurrentCameraTilt - cam_pos[1];
			cam_dir[2] = g_camera->m_perspectiveCharacterPos.z - cam_pos[2];
		}
		if (!Cmp(g_shadowProperties.m_directionalLightName, "\n") && g_shadowProperties.m_enable && g_render.UsingShadowShader())
			m_dynamicShadowMap->MakeShadowMap(cam_pos, cam_dir, light_dir);
	}

	//audio
	m_ambientSourcePos[0] = m_listenerPos[0] = g_camera->m_perspectiveCameraPos.x;
	m_ambientSourcePos[1] = m_listenerPos[1] = g_camera->m_perspectiveCameraPos.y;
	m_ambientSourcePos[2] = m_listenerPos[2] = g_camera->m_perspectiveCameraPos.z;

	m_listenerOri[0] = -g_camera->m_perspectiveCameraDir.x;
	m_listenerOri[1] = g_camera->m_perspectiveCameraDir.y;
	m_listenerOri[2] = -g_camera->m_perspectiveCameraDir.z;

	m_soundSystem->SetListenerPosition( m_listenerPos );
	//m_soundSystem->SetListenerVelocity( m_listenerVel );
	m_soundSystem->SetListenerOrientation( m_listenerOri );
	//set ambient sound
	if(	g_databaseVariables.m_insertAmbientSound )
		m_ambientSound->GetSoundSource()->SetSoundPosition( m_ambientSourcePos );

	g_render.m_useShader = CTrue;

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glPushAttrib(GL_LIGHTING_BIT);
	//create all the waters textures here
	if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
	{
		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
		{
			if (g_engineWaters[i]->GetOutsideFrustom()) continue;

			if (CTrue/*g_currentCameraType == eCAMERA_PHYSX*/)
			{
				if (g_engineWaters[i]->GetVisible() && g_engineWaters[i]->GetQueryVisible())
				{
					g_engineWaters[i]->CreateReflectionTexture(g_waterTextureSize);
					//g_engineWaters[i]->CreateRefractionDepthTexture(g_waterTextureSize );
				}
			}
			else
			{
				g_engineWaters[i]->CreateReflectionTexture(g_waterTextureSize);
			}
		}
	}


	if( !g_useOldRenderingStyle && g_window.m_windowGL.multiSampling && g_options.m_enableFBO)
		g_render.BindFBO(m_mFboID);
	else if( !g_useOldRenderingStyle && !g_window.m_windowGL.multiSampling && g_options.m_enableFBO)
		g_render.BindFBO( m_fboID );

	if( g_useOldRenderingStyle || !g_options.m_enableFBO)
		glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
	else
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//Draw grids of the lower left window (perspective)
	glViewport( 0, 0 , g_width , g_height );// resets the viewport to new dimensions.
	if( g_window.m_windowGL.multiSampling && g_options.m_numSamples )
		glEnable( GL_MULTISAMPLE );
	else
		glDisable( GL_MULTISAMPLE );

	//Pause Sounds
	if (g_currentVSceneProperties.m_isMenu && g_currentVSceneProperties.m_isPause)
	{
		if (g_mainCharacter)
		{
			g_soundSystem->PauseALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
			g_soundSystem->PauseALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
			g_soundSystem->PauseALSound(*(g_mainCharacter->m_jumpSound->GetSoundSource()));
		}
		//resource sounds
		for (CUInt i = 0; i < g_resourceFiles.size(); i++)
		{
			if (g_resourceFiles[i]->GetSoundSource())
			{
				g_soundSystem->PauseALSound(*(g_resourceFiles[i]->GetSoundSource()->GetSoundSource()));
			}
		}

		//static 3D sounds
		for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
		{
			g_soundSystem->PauseALSound(*(g_engineStaticSounds[i]->GetSoundSource()));
		}

		//ambient sound
		if (g_databaseVariables.m_insertAmbientSound)
		{
			g_soundSystem->PauseALSound(*(g_main->m_ambientSound->GetSoundSource()));
		}

	}

	if( !m_loadScene )
		if(!ProcessInputs() )
			return CFalse;

	//render waters here
	CVec3f cameraPos( g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z );

	if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
	{
 		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
		{
			if (g_engineWaters[i]->GetOutsideFrustom()) continue;

			if (CTrue/*g_currentCameraType == eCAMERA_PHYSX*/)
			{
				if (g_engineWaters[i]->GetVisible() && g_engineWaters[i]->GetQueryVisible())
				{
					glUseProgram(g_render.m_waterProgram);
					g_engineWaters[i]->RenderWater(cameraPos, elapsedTime);
					glUseProgram(0);
				}
			}
			else
			{
				glUseProgram(g_render.m_waterProgram);
				g_engineWaters[i]->RenderWater(cameraPos, elapsedTime);
				glUseProgram(0);
			}
		}
	}

	if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	{
		glActiveTexture(GL_TEXTURE7 );
		glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, m_dynamicShadowMap->depth_tex_ar);
		if( g_shadowProperties.m_shadowType > 3 || g_shadowProperties.m_shadowType == 0 /* 0 is debug state*/)
			glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		else 
			glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		//glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		for(int i=m_dynamicShadowMap->cur_num_splits; i<MAX_SPLITS; i++)
			far_bound[i] = 0;

		// for every active split
		for(int i=0; i<m_dynamicShadowMap->cur_num_splits; i++)
		{
			float light_m[16];

			// f[i].fard is originally in eye space - tell's us how far we can see.
			// Here we compute it in camera homogeneous coordinates. Basically, we calculate
			// cam_proj * (0, 0, f[i].fard, 1)^t and then normalize to [0; 1]
			far_bound[i] = 0.5f*(-m_dynamicShadowMap->f[i].fard*cam_proj[10]+cam_proj[14])/m_dynamicShadowMap->f[i].fard + 0.5f;

			// compute a matrix that transforms from camera eye space to light clip space
			// and pass it to the shader through the OpenGL texture matrices, since we
			// don't use them now
			glActiveTexture(GL_TEXTURE0 + (GLenum)i);
			glMatrixMode(GL_TEXTURE);
			glLoadMatrixf(bias);
			glMultMatrixf(m_dynamicShadowMap->shad_cpm[i]);
			// multiply the light's (bias*crop*proj*modelview) by the inverse camera modelview
			// so that we can transform a pixel as seen from the camera
			glMultMatrixf(cam_inverse_modelview);
			
			// compute a normal matrix for the same thing (to transform the normals)
			// Basically, N = ((L)^-1)^-t
			glGetFloatv(GL_TEXTURE_MATRIX, light_m);
			matrix4<float> nm;
			nm.set_value(light_m);
			nm = inverse(nm);
			nm = transpose(nm);

			glActiveTexture(GL_TEXTURE0 + (GLenum)(i+4));
			glMatrixMode(GL_TEXTURE);
			glLoadMatrixf(nm._array);
		}
		glMatrixMode( GL_MODELVIEW );
	}

	g_numLights = 0;
	if (!g_render.UsingShader())
		glEnable( GL_LIGHTING );

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		if (!g_instancePrefab[i]->GetVisible()) continue;
		g_instancePrefab[i]->UpdateScript();
	}
	if (g_startup)
		g_startup->UpdateScript();

	RenderQueries();

	ManageLODs();

	Draw3DObjects();

	if (g_databaseVariables.m_insertAndShowTerrain)
		RenderTerrain();

	if (g_databaseVariables.m_showBoundingBox)
	{
		COpenGLUtility g_glUtil;
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			CVector lineColor(1.0, 1.0, 1.0);
			CVector min(g_instancePrefab[i]->GetMinAABB().x, g_instancePrefab[i]->GetMinAABB().y, g_instancePrefab[i]->GetMinAABB().z);
			CVector max(g_instancePrefab[i]->GetMaxAABB().x, g_instancePrefab[i]->GetMaxAABB().y, g_instancePrefab[i]->GetMaxAABB().z);
			g_glUtil.DrawCWBoxWithLines(min, max, lineColor);
		}
	}
	if (g_databaseVariables.m_showOctree)
	{
		g_octree->Render(CFalse, CFalse);
	}

	if (!g_useOldRenderingStyle &&  g_window.m_windowGL.multiSampling && g_options.m_numSamples && g_options.m_enableFBO)
		g_render.BindForWriting( m_mFboID );
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
		g_render.BindForWriting( m_fboID );

	g_currentInstanceLight = NULL;
	g_totalLights = g_numLights;

	CBool useFog;
	if( g_shadowProperties.m_shadowType == eSHADOW_SINGLE_HL && g_shadowProperties.m_enable && g_render.UsingShadowShader())
		useFog = CFalse;
	else 
		useFog = CTrue;

	if( ( g_fogProperties.m_enable || g_dofProperties.m_enable ) && useFog )
	{
		if( !g_useOldRenderingStyle && g_window.m_windowGL.multiSampling && g_options.m_numSamples && g_options.m_enableFBO)
			g_render.BindForWriting(m_mFboIDFogDof);
		else if(!g_useOldRenderingStyle && g_options.m_enableFBO )
			g_render.BindForWriting( m_fboIDFogDof );
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		g_fogBlurPass = CTrue;

		//g_octree->Render();
		g_main->RenderBakedOctree3DModels();
		if (g_databaseVariables.m_showBoundingBox)
		{
			g_octree->Render(CFalse, CFalse);
		}

		Render3DAnimatedModels( CTrue);
		Render3DModelsControlledByPhysX();
		RenderCharacter(CFalse);
		if (g_databaseVariables.m_insertAndShowTerrain)
			RenderTerrain();

		//render water
		if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
		{
 			for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
			{
				CVec3f waterPoints[4];
				waterPoints[0].x = g_engineWaters[i]->m_sidePoint[0].x;waterPoints[0].y = g_engineWaters[i]->m_sidePoint[0].y;waterPoints[0].z = g_engineWaters[i]->m_sidePoint[0].z;
				waterPoints[1].x = g_engineWaters[i]->m_sidePoint[1].x;waterPoints[1].y = g_engineWaters[i]->m_sidePoint[1].y;waterPoints[1].z = g_engineWaters[i]->m_sidePoint[1].z;
				waterPoints[2].x = g_engineWaters[i]->m_sidePoint[2].x;waterPoints[2].y = g_engineWaters[i]->m_sidePoint[2].y;waterPoints[2].z = g_engineWaters[i]->m_sidePoint[2].z;
				waterPoints[3].x = g_engineWaters[i]->m_sidePoint[3].x;waterPoints[3].y = g_engineWaters[i]->m_sidePoint[3].y;waterPoints[3].z = g_engineWaters[i]->m_sidePoint[3].z;

				if( g_camera->m_cameraManager->IsBoxInFrustum( waterPoints, 4 ) )
				{
					glUseProgram( g_render.m_waterFogBlurProgram );
					glUniform1f(glGetUniformLocation( g_render.m_waterFogBlurProgram , "focalDistance"), m_dof.m_focalDistance);
					glUniform1f(glGetUniformLocation( g_render.m_waterFogBlurProgram , "focalRange"), m_dof.m_focalRange);
					if( g_fogProperties.m_enable && useFog )
						glUniform1i(glGetUniformLocation( g_render.m_waterFogBlurProgram , "enableFog"), CTrue );
					else
						glUniform1i(glGetUniformLocation( g_render.m_waterFogBlurProgram , "enableFog"), CFalse );
					g_engineWaters[i]->RenderWater(cameraPos, elapsedTime );
					glUseProgram( 0 );
				}
			}
		}
		g_fogBlurPass = CFalse;
		BlendFogWithScene();
	}

	if( g_updateOctree && g_scene.size() > 0 )
	{
		//Force Update
		//for( CUInt i = 0 ; i < g_scene.size(); i++ )
		//{
		//	g_render.SetScene( g_scene[i] );
		//	g_render.GetScene()->m_update = CTrue;
		//}

		//update octree
		Render3DModels( CTrue, NULL );
		g_octree->Init();
		g_octree->GetWorldDimensions();
		g_octree->SetName( "octree_root" );
		g_octree->SetLevel(0);
		g_octree->AttachGeometriesToNode();
		g_octree->AttachLightsToGeometries();
		g_updateOctree = CFalse;
		g_main->RenderQueries(CTrue);

		//update shadow max radius
		g_maxInstancePrefabRadius = -1.f;
		for (CUInt j = 0; j < g_instancePrefab.size(); j++)
		{
			if (g_instancePrefab[j]->GetScene(0) && g_instancePrefab[j]->GetScene(0)->CastShadow())
			{
				if (g_instancePrefab[j]->GetRadius() > g_maxInstancePrefabRadius)
					g_maxInstancePrefabRadius = g_instancePrefab[j]->GetRadius();
			}
		}

	}


	if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	{
		//reset matrice
		for( CUInt i = 0; i < 8; i++ )
		{
			glActiveTexture(GL_TEXTURE0 + i );
			glMatrixMode( GL_TEXTURE );
			glLoadIdentity();
		}
		glActiveTexture(GL_TEXTURE0);
	}

	glDisable( GL_LIGHT0 );

	for( CInt i = 0; i <g_numLights ; i++ )
	{
		glDisable( GL_LIGHT0 + i );
	}
	glDisable( GL_LIGHTING ); //just for per vertex lighting
	glMatrixMode( GL_MODELVIEW );

	glPopAttrib();

	
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		g_render.SetScene( g_scene[i] );
		if( !g_render.GetScene()->m_isTrigger )
		{
			for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			{
				g_scene[i]->m_instanceGeometries[j]->m_renderCount = 0;
			}
		}
		if( g_render.GetScene()->m_hasAnimation )
		{
			g_render.GetScene()->m_updateAnimation = CTrue;
		}
	}

	if( g_databaseVariables.m_insertAndShowSky )
	{
		g_skyDome->RenderDome();
	}

	if( g_physXProperties.m_bDebugMode )
		g_nx->debugMode = CTrue;
	else
		g_nx->debugMode = CFalse;

	if( g_nx->debugMode )
		g_nx->debugRenderer();

	if( g_databaseVariables.m_showPerspectiveGrids )
		DrawGrid();
	if( g_databaseVariables.m_showSoundPositions )
	{
		CFloat pos[3];
		for( CUInt i = 0; i < g_engineStaticSounds.size(); i++ )
		{
			 g_engineStaticSounds[i]->GetPosition(pos);
			 g_render.DrawCube( 150.0f, pos );
		}
	}
	glFlush();
	if( !g_useOldRenderingStyle && g_window.m_windowGL.multiSampling && g_options.m_enableFBO)
	{
		g_render.BindForReading(m_mFboID);
		glReadBuffer( GL_COLOR_ATTACHMENT0_EXT );
		g_render.BindForWriting( m_fboID);
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	//m_particleSystem->Render();

	if( !g_useOldRenderingStyle && g_options.m_enableFBO)
	{
		if( g_dofProperties.m_enable )
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			/* Second pass: downsampling */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[0]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);
			glUseProgram(g_render.m_dofProgram[0]);
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[0], "Tex0"), 0 );
			glViewport(0, 0, g_width/2, g_height/2);
			m_dof.DrawQuad();

			/* Third pass: Gaussian filtering along the X axis */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[1]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, m_dof.m_texid[0]);
			glUseProgram(g_render.m_dofProgram[1]);
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[1], "Tex0"), 0 );
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[1], "Width"), g_width );
			m_dof.DrawQuad();

			/* Fourth pass: Gaussian filtering along the Y axis */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[2]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(g_render.m_dofProgram[2]);
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[2], "Tex0"), 0 );
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[2], "Height"), g_height );
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, m_dof.m_texid[1]);
			m_dof.DrawQuad();

			/* Fifth pass: final compositing */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[3]);
			////glDrawBuffer(GL_BACK);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_dof.m_texid[2]);
			glUseProgram(g_render.m_dofProgram[3]);
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[3], "Tex0"), 0 );
			glUniform1i( glGetUniformLocation(g_render.m_dofProgram[3], "Tex1"), 1 );

			glViewport(0, 0, g_width, g_height);

			glBegin(GL_QUADS);
				glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 0.0f);
				glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 0.0f);
				glVertex2d(-1, -1);
				glMultiTexCoord2f(GL_TEXTURE0, 1.0f, 0.0f);
				glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 0.0f);
				glVertex2d(1, -1);
				glMultiTexCoord2f(GL_TEXTURE0, 1.0f, 1.0f);
				glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 1.0f);
				glVertex2d(1, 1);
				glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 1.0f);
				glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 1.0f);
				glVertex2d(-1, 1);
			glEnd();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
		glActiveTexture(GL_TEXTURE0);
		glUseProgram(0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glViewport(0, 0, g_width, g_height);
		glDrawBuffer( GL_BACK );
		glClear( GL_COLOR_BUFFER_BIT ); 
		glPushAttrib( GL_ENABLE_BIT );
		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		glOrtho(0, 1, 0, 1, 0.01, 100);
		glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity();
		glDisable( GL_LIGHTING );
		glDisable(GL_DEPTH_TEST); 
		if( g_dofProperties.m_enable )
			glBindTexture( GL_TEXTURE_2D, m_dof.m_texid[3] );
		else
			glBindTexture( GL_TEXTURE_2D, m_textureTarget[0] );
		glEnable( GL_TEXTURE_2D);
		glBegin(GL_QUADS); 
		glTexCoord2d(0,	0);	glVertex3d(0, 0, -1);
		glTexCoord2d(1, 0);	glVertex3d(1, 0, -1);
		glTexCoord2d(1, 1);	glVertex3d(1, 1, -1);
		glTexCoord2d(0, 1);	glVertex3d(0, 1, -1);
		glEnd();

		if( g_render.m_useBloom && g_bloomProperties.m_enable)
		{
			//if( g_dofProperties.m_enable )
			//	g_bloom->CreateRuntimeTexture( g_width, g_height, m_dof.m_texid[3]  );
			//else
			glPushAttrib( GL_CURRENT_BIT );
			g_bloom->CreateRuntimeTexture( g_width, g_height, m_textureTarget[0]  );

			glEnable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glColor4f( g_bloomProperties.m_bloomColor[0], g_bloomProperties.m_bloomColor[1], g_bloomProperties.m_bloomColor[2], g_bloomProperties.m_bloomIntensity );
			glBindTexture( GL_TEXTURE_2D, g_bloom->m_bloomTexture );
			glBegin(GL_QUADS); 
			glTexCoord2d(0,	0);	glVertex3d(0, 0, -1); 
			glTexCoord2d(1, 0);	glVertex3d(1, 0, -1);
			glTexCoord2d(1, 1);	glVertex3d(1, 1, -1);
			glTexCoord2d(0, 1);	glVertex3d(0, 1, -1);
			glEnd();
			glDisable( GL_BLEND );
			glPopAttrib();
		}
 		//m_dynamicShadowMap->ShowDepthTex();
		DrawGUI();
		glPopAttrib();
		glMatrixMode(GL_PROJECTION); glPopMatrix();
		glMatrixMode(GL_MODELVIEW); glPopMatrix();
	}
	else
	{
		DrawGUI();
	}

	ResetData();

	return CTrue;
}

CVoid CMain::ResetData()
{
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		g_instancePrefab[i]->SetRenderForQuery(CFalse);
		g_instancePrefab[i]->SetRealTimeSceneCheckIsInFrustom(CTrue);
		for (CUInt j = 0; j < 3; j++)
		{
			if (g_instancePrefab[i]->GetPrefab() && g_instancePrefab[i]->GetPrefab()->GetHasLod(j))
			{
				CScene* scene = g_instancePrefab[i]->GetScene(j);
				if (!scene) continue;
				scene->ResetSkinData();
			}
		}
	}
	g_currentInstancePrefab = NULL;
}

CVoid CMain::DrawGrid(CVoid)
{
	glPushAttrib( GL_ENABLE_BIT | GL_CURRENT_BIT );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	//Draw the grid here
	glLineWidth( 1.0f );
	glColor3f( 0.7f, 0.7f, 0.7f );
	glBegin( GL_LINES );
	for( CInt index = -100; index <= 100; index += 1 )
	{
		glVertex3f( -100.0f, 0.0f, (CFloat)index );
		glVertex3f(  100.0f, 0.0f, (CFloat)index );

		glVertex3f( (CFloat)index, 0.0f, -100.0f );
		glVertex3f( (CFloat)index, 0.0f, 100.0f );
	}
	glEnd();
	glLineWidth( 3.0f );
	glBegin( GL_LINES );
	//Draw the axes here
	glColor3f( 1.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 1.0f, 0.0f, 0.0f );
	
	glColor3f( 0.0f, 1.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 1.0f, 0.0f );

	glColor3f( 0.0f, 0.0f, 1.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, 1.0f );

	glEnd();

	glPopAttrib();
}

CBool CMain::GetJumpCurrentEndDuration(CFloat& duration)
{
	if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON /*&& g_currentCameraType == eCAMERA_PHYSX*/) return CFalse;

	if (!g_mainCharacter)
		return CFalse;
	g_currentInstancePrefab = g_mainCharacter->GetInstancePrefab();
	if (!g_currentInstancePrefab)
		return CFalse;
	if (!g_currentInstancePrefab->GetVisible()) return CFalse;

	std::vector<std::string> jumpName = g_mainCharacter->GetJumpName();

	CScene* scene = NULL;

	CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
	for (CUInt j = 0; j < 3; j++)
	{
		if (prefab && prefab->GetHasLod(j) && g_currentInstancePrefab->GetSceneVisible(j))
		{
			scene = g_currentInstancePrefab->GetScene(j);
			break;
		}
	}
	if (!scene) return CFalse;

	CBool foundTarget = CFalse;
	CUInt index = -1;

	for (CInt k = 0; k < scene->GetNumClips(); k++)
	{
		CChar animationName[MAX_NAME_SIZE];
		Cpy(animationName, scene->m_animationClips[k]->GetName());
		StringToUpper(animationName);
		StringToUpper((CChar*)jumpName[0].c_str());
		if (Cmp(jumpName[0].c_str(), animationName))
		{
			index = k;
			foundTarget = CTrue;
			break;
		}
	}
	if (!foundTarget)
	{
		return CFalse;
	}
	duration = scene->m_animationClips[index]->GetEnd() - scene->m_animationClips[index]->GetCurrentAnimationTime();
	return CTrue;
}

CBool CMain::ManageCharacterBlends(CChar* animationType, CChar* IdleAnimationName)
{
	if (Cmp(m_previousCharacterAnimationType, animationType))
		return CFalse;
	else
		Cpy(m_previousCharacterAnimationType, animationType);

	if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON) return CFalse;

	if (!g_mainCharacter)
		return CFalse;
	g_currentInstancePrefab = g_mainCharacter->GetInstancePrefab();
	if (!g_currentInstancePrefab)
		return CFalse;
	if (!g_currentInstancePrefab->GetVisible()) return CFalse;

	std::vector<std::string> idleName;
	std::vector<std::string> walkName;
	std::vector<std::string> jumpName;
	std::vector<std::string> runName;

	for (CUInt i = 0; i < g_mainCharacter->GetIdleName().size(); i++)
		idleName.push_back(g_mainCharacter->GetIdleName()[i]);

	for (CUInt i = 0; i < g_mainCharacter->GetWalkName().size(); i++)
		walkName.push_back(g_mainCharacter->GetWalkName()[i]);

	for (CUInt i = 0; i < g_mainCharacter->GetJumpName().size(); i++)
		jumpName.push_back(g_mainCharacter->GetJumpName()[i]);

	for (CUInt i = 0; i < g_mainCharacter->GetRunName().size(); i++)
		runName.push_back(g_mainCharacter->GetRunName()[i]);

	CScene* scene = NULL;

	CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
	for (CUInt j = 0; j < 3; j++)
	{
		if (prefab && prefab->GetHasLod(j) && g_currentInstancePrefab->GetSceneVisible(j))
		{
			scene = g_currentInstancePrefab->GetScene(j);
			break;
		}
	}
	if (!scene) return CFalse;

	CBool foundTarget = CFalse;
	CUInt index = -1;

	if (Cmp(animationType, "idle") || Cmp(animationType, "more_idle"))
	{
		for (CInt i = 0; i < scene->GetNumClips(); i++)
		{
			if (IdleAnimationName)
			{
				if (Cmp(IdleAnimationName, scene->m_animationClips[i]->GetName()))
				{
					index = i;
					foundTarget = CTrue;
					break;
				}
			}
			else if (Cmp(idleName[0].c_str(), scene->m_animationClips[i]->GetName()))
			{
				index = i;
				foundTarget = CTrue;
				break;
			}
		}
		if (!foundTarget)
		{
			return CFalse;
		}

		if (index != -1)
		{
			scene->SetClipIndex(index);
			scene->BlendCycle(index, 1.0f, g_characterBlendingProperties.m_idleDelayIn);

			for (CInt i = 0; i < scene->GetNumClips(); i++)
			{
				if (i != index)
				{
					//make sure we clear animations
					scene->ClearCycle(i, g_characterBlendingProperties.m_idleDelayIn);
				}
			}
		}
	}
	else if (Cmp(animationType, "walk"))
	{
		for (CInt i = 0; i < scene->GetNumClips(); i++)
		{
			if (Cmp(walkName[0].c_str(), scene->m_animationClips[i]->GetName()))
			{
				index = i;
				foundTarget = CTrue;
				break;
			}
		}
		if (!foundTarget)
		{
			return CFalse;
		}

		if (index != -1)
		{
			scene->SetClipIndex(index);
			scene->BlendCycle(index, 1.0f, g_characterBlendingProperties.m_walkDelayIn);

			for (CInt i = 0; i < scene->GetNumClips(); i++)
			{
				if (i != index)
				{
					//make sure we clear animations
					scene->ClearCycle(i, g_characterBlendingProperties.m_walkDelayIn);
				}
			}
		}
	}
	else if (Cmp(animationType, "run"))
	{
		for (CInt i = 0; i < scene->GetNumClips(); i++)
		{
			if (Cmp(runName[0].c_str(), scene->m_animationClips[i]->GetName()))
			{
				index = i;
				foundTarget = CTrue;
				break;
			}
		}
		if (!foundTarget)
		{
			return CFalse;
		}

		if (index != -1)
		{
			scene->SetClipIndex(index);
			scene->BlendCycle(index, 1.0f, g_characterBlendingProperties.m_runDelayIn);

			for (CInt i = 0; i < scene->GetNumClips(); i++)
			{
				if (i != index)
				{
					//make sure we clear animations
					scene->ClearCycle(i, g_characterBlendingProperties.m_runDelayIn);
				}
			}
		}

	}
	else if (Cmp(animationType, "jump"))
	{
		for (CInt i = 0; i < scene->GetNumClips(); i++)
		{
			if (Cmp(jumpName[0].c_str(), scene->m_animationClips[i]->GetName()))
			{
				index = i;
				foundTarget = CTrue;
				break;
			}
		}
		if (!foundTarget)
		{
			return CFalse;
		}

		if (index != -1)
		{
			scene->SetClipIndex(index);
			scene->ExecuteAction(index, g_characterBlendingProperties.m_jumpDelayIn, g_characterBlendingProperties.m_jumpDelayOut, 1.0, CFalse);

			for (CInt i = 0; i < scene->GetNumClips(); i++)
			{
				if (i != index)
				{
					//make sure we clear animations
					scene->ClearCycle(i, g_characterBlendingProperties.m_jumpDelayIn);
				}
			}
		}

	}
	else if (Cmp(animationType, ""))
	//reset
	{
		for (CInt i = 0; i < scene->GetNumClips(); i++)
		{
			//make sure we clear animations
			scene->ClearCycle(i, 0.0f);
			scene->m_animationClips[i]->SetCurrentTime(0.0f);
		}
	}
	idleName.clear();
	walkName.clear();
	jumpName.clear();
	runName.clear();

	return CTrue;

}

CBool CMain::IsJumping(CBool &isInList)
{
	if (!g_mainCharacter)
		return CFalse;
	g_currentInstancePrefab = g_mainCharacter->GetInstancePrefab();
	if (!g_currentInstancePrefab)
		return CFalse;
	if (!g_currentInstancePrefab->GetVisible()) return CFalse;

	std::vector<std::string> jumpName = g_mainCharacter->GetJumpName();

	CScene* scene = NULL;

	CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
	for (CUInt j = 0; j < 3; j++)
	{
		if (prefab && prefab->GetHasLod(j) && g_currentInstancePrefab->GetSceneVisible(j))
		{
			scene = g_currentInstancePrefab->GetScene(j);
			break;
		}
	}
	if (!scene) return CFalse;
	CBool foundTarget = CFalse;
	for (CUInt j = 0; j < jumpName.size(); j++)
	{
		for (CUInt i = 0; i < scene->m_executeActionList.size(); i++)
		{
			if (Cmp(scene->m_executeActionList[i].c_str(), jumpName[j].c_str()))
			{
				foundTarget = CTrue;
				break;
			}
		}
	}
	if (foundTarget)
		isInList = CTrue;
	else
		isInList = CFalse;
	return CTrue;
}

CBool CMain::ProcessInputs()
{
	if (m_loadScene) return CTrue;

	static CUInt cameraIndex = 0;
	static CBool pKeyDown = CFalse;
	static CBool oKeyDown = CFalse;
	CBool inputUpdated = CFalse;
	if (m_publishDebug)
	{
		g_input.Update();
		inputUpdated = CTrue;
		if (g_input.KeyDown(DIK_P) && !pKeyDown && g_importedCameraInstances.size() > 0)
		{
			pKeyDown = CTrue;
			cameraIndex++;
			if (cameraIndex > g_importedCameraInstances.size() - 1)
				cameraIndex = 0;
			g_render.SetActiveInstanceCamera(g_importedCameraInstances[cameraIndex]);
			g_currentCameraType = eCAMERA_COLLADA;
		}
		if (g_input.KeyUp(DIK_P))
			pKeyDown = CFalse;

		if (g_input.KeyDown(DIK_O) && !oKeyDown && g_importedCameraInstances.size() > 0)
		{
			oKeyDown = CTrue;
			if (cameraIndex > 0)
				cameraIndex--;
			else
				cameraIndex = g_importedCameraInstances.size() - 1;

			g_render.SetActiveInstanceCamera(g_importedCameraInstances[cameraIndex]);
			g_currentCameraType = eCAMERA_COLLADA;

		}
		if (g_input.KeyUp(DIK_O))
			oKeyDown = CFalse;

		if (g_input.KeyDown(DIK_I))
		{
			g_render.SetActiveInstanceCamera(NULL);
			g_currentCameraType = eCAMERA_PHYSX;
		}
		if (g_input.KeyDown(DIK_U))
		{
			g_render.SetActiveInstanceCamera(g_render.GetDefaultInstanceCamera());
			g_currentCameraType = eCAMERA_DEFAULT_FREE_NO_PHYSX;
		}

		static CBool f1KeyDown = CFalse;
		if (g_input.KeyDown(DIK_F1) && !f1KeyDown)
		{
			f1KeyDown = CTrue;
			m_showHelpInfo = !m_showHelpInfo;
		}
		if (g_input.KeyUp(DIK_F1))
			f1KeyDown = CFalse;

		static CBool gKeyDown = CFalse;
		static CBool vKeyDown = CFalse;

		//physics
		if (g_input.KeyDown(DIK_V) && !vKeyDown)
		{
			vKeyDown = CTrue;
			g_physXProperties.m_bDebugMode = !g_physXProperties.m_bDebugMode;
		}
		if (g_input.KeyUp(DIK_V))
			vKeyDown = CFalse;

		//bounding box
		static CBool bKeyDown = CFalse;

		if (g_input.KeyDown(DIK_B) && !bKeyDown)
		{
			bKeyDown = CTrue;
			g_databaseVariables.m_showBoundingBox = !g_databaseVariables.m_showBoundingBox;
		}
		if (g_input.KeyUp(DIK_B))
			bKeyDown = CFalse;

		//octree
		static CBool nKeyDown = CFalse;

		if (g_input.KeyDown(DIK_N) && !nKeyDown)
		{
			nKeyDown = CTrue;
			g_databaseVariables.m_showOctree = !g_databaseVariables.m_showOctree;
		}
		if (g_input.KeyUp(DIK_N))
			nKeyDown = CFalse;

		//gravity
		if (g_input.KeyDown(DIK_G) && !gKeyDown)
		{
			gKeyDown = CTrue;
			g_physXProperties.m_bApplyGravity = !g_physXProperties.m_bApplyGravity;
			if (g_physXProperties.m_bApplyGravity)
				g_nx->m_defaultGravity = NxVec3(g_physXProperties.m_fGravityX, g_physXProperties.m_fGravityY, g_physXProperties.m_fGravityZ);
			else
				g_nx->m_defaultGravity = NxVec3(0.0f);

		}
		if (g_input.KeyUp(DIK_G))
			gKeyDown = CFalse;

	}

	if (m_lockInput)
	{
		ApplyForce(IDLE, elapsedTime);
		EnableIdleAnimations();
		return CTrue;
	}

	if (!inputUpdated)
		g_input.Update();

	//static CBool bEscapeDown = CFalse;

	//if (g_input.KeyDown(DIK_ESCAPE) && !bEscapeDown)
	//{
	//	bEscapeDown = CTrue;
	//	return CFalse;
	//}
	//if (g_input.KeyUp(DIK_ESCAPE))
	//	bEscapeDown = CFalse;

	if (g_currentVSceneProperties.m_isMenu)
	{
		CInt dx, dy;
		g_input.GetMouseMovement(dx, dy);
		if (g_main->m_prevLoadScene)
		{
			dx = dy = 0;
			g_main->m_prevLoadScene = CFalse;
		}
		if (g_input.ButtonDown(0))
		{
			if (!m_previousLeftButtonDown)
			{
				CUInt index = GetSelectedGUI();
				for (CUInt i = 0; i < g_guis.size(); i++)
				{
					for (CUInt k = 0; k < g_guis[i]->m_guiButtons.size(); k++)
					{
						if (g_guis[i]->m_guiButtons[k]->GetIndex() == index)
						{
							if (g_guis[i]->m_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_DISABLE)
							{
								if (g_guis[i]->m_guiButtons[k]->GetHasLeftClickImage())
								{
									g_timer->GetElapsedSeconds2(CTrue);
									g_guis[i]->m_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_LEFT_CLICK);
								}
							}
						}
					}
				}
			}
			m_previousLeftButtonDown = CTrue;
		}
		else
		{
			m_previousLeftButtonDown = CFalse;
		}

		if (g_input.ButtonDown(1))
		{
			if (!m_previousRightButtonDown)
			{
				CUInt index = GetSelectedGUI();
				for (CUInt i = 0; i < g_guis.size(); i++)
				{
					for (CUInt k = 0; k < g_guis[i]->m_guiButtons.size(); k++)
					{
						if (g_guis[i]->m_guiButtons[k]->GetIndex() == index)
						{
							if (g_guis[i]->m_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_DISABLE)
							{
								if (g_guis[i]->m_guiButtons[k]->GetHasRightClickImage())
								{
									g_timer->GetElapsedSeconds2(CTrue);
									g_guis[i]->m_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_RIGHT_CLICK);
								}
							}
						}
					}
				}
			}
			m_previousRightButtonDown = CTrue;
		}
		else
		{
			m_previousRightButtonDown = CFalse;
		}
		if (dx != 0 || dy != 0)
		{
			//mouse moved
			m_selectedGUIIndex = GetSelectedGUI();
			for (CUInt i = 0; i < g_guis.size(); i++)
			{
				for (CUInt k = 0; k < g_guis[i]->m_guiButtons.size(); k++)
				{
					if (g_guis[i]->m_guiButtons[k]->GetIndex() == m_selectedGUIIndex)
					{
						if (g_guis[i]->m_guiButtons[k]->GetCurrentImageType() == eBUTTON_IMAGE_MAIN && g_guis[i]->m_guiButtons[k]->GetHasHoverImage())
						{
							if (g_guis[i]->m_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_HOVER)
							{
								if (m_previuosSelectedGUIIndex != m_selectedGUIIndex)
									g_guis[i]->m_guiButtons[k]->OnSelectMouseHoverScript();
							}

							g_guis[i]->m_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_HOVER);
						}
					}
					else
					{
						g_guis[i]->m_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_MAIN);
					}
				}
			}
			m_previuosSelectedGUIIndex = m_selectedGUIIndex;
		}

		g_main->m_mousePosition.x += dx;
		if (g_main->m_mousePosition.x < 1.0f) g_main->m_mousePosition.x = 1.0f;
		else if (g_main->m_mousePosition.x > g_width) g_main->m_mousePosition.x = g_width;

		g_main->m_mousePosition.y += dy;
		if (g_main->m_mousePosition.y < 0.0f) g_main->m_mousePosition.y = 0.0f;
		else if (g_main->m_mousePosition.y > g_height) g_main->m_mousePosition.y = g_height;

		ApplyForce(IDLE, elapsedTime);
		EnableIdleAnimations();

		return CTrue;
	}

	if( g_currentCameraType == eCAMERA_PHYSX )
	{
		CBool forceApplied = false;
		CBool move = CFalse;
		if( (g_input.KeyDown( DIK_W ) || g_input.KeyDown( DIK_UP )) && (g_input.KeyDown( DIK_D ) || g_input.KeyDown( DIK_RIGHT )) )
		{
			ApplyForce( MOVE_FRONT_RIGHT, elapsedTime );
			forceApplied = true;
			if (!g_nx->gJump)
				move = CTrue;
			if (move)
			{
				if (g_input.KeyDown(DIK_LSHIFT))
				{
					ManageCharacterBlends("run");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
					}
				}
				else
				{
					ManageCharacterBlends("walk");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}
				}
			}

		}
		else	if( (g_input.KeyDown( DIK_W ) || g_input.KeyDown( DIK_UP )) && (g_input.KeyDown( DIK_A ) || g_input.KeyDown( DIK_LEFT )) )
		{
			ApplyForce( MOVE_FRONT_LEFT, elapsedTime );
			forceApplied = true;
			if (!g_nx->gJump)
				move = CTrue;
			if (move)
			{
				if (g_input.KeyDown(DIK_LSHIFT))
				{
					ManageCharacterBlends("run");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
					}
				}
				else
				{
					ManageCharacterBlends("walk");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}
				}
			}

		}
		else if( (g_input.KeyDown( DIK_S ) || g_input.KeyDown( DIK_DOWN )) && (g_input.KeyDown( DIK_D ) || g_input.KeyDown( DIK_RIGHT )) )
		{
			ApplyForce( MOVE_BACK_RIGHT, elapsedTime );
			forceApplied = true;
			if (!g_nx->gJump)
				move = CTrue;
			if (move)
			{
				if (g_input.KeyDown(DIK_LSHIFT))
				{
					ManageCharacterBlends("run");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
					}
				}
				else
				{
					ManageCharacterBlends("walk");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}
				}
			}

		}
		else if( (g_input.KeyDown( DIK_S ) || g_input.KeyDown( DIK_DOWN )) && (g_input.KeyDown( DIK_A ) || g_input.KeyDown( DIK_LEFT )) )
		{
			ApplyForce( MOVE_BACK_LEFT, elapsedTime );
			forceApplied = true;
			if (!g_nx->gJump)
				move = CTrue;
			if (move)
			{
				if (g_input.KeyDown(DIK_LSHIFT))
				{
					ManageCharacterBlends("run");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
					}
				}
				else
				{
					ManageCharacterBlends("walk");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}
				}
			}

		}

		else if( g_input.KeyDown( DIK_W ) || g_input.KeyDown( DIK_UP ) )
		{
			ApplyForce( MOVE_FRONT, elapsedTime );
			forceApplied = true;
			if (!g_nx->gJump)
				move = CTrue;
			if (move)
			{
				if (g_input.KeyDown(DIK_LSHIFT))
				{
					ManageCharacterBlends("run");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
					}
				}
				else
				{
					ManageCharacterBlends("walk");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}
				}
			}

		}
		else if( g_input.KeyDown( DIK_S ) || g_input.KeyDown( DIK_DOWN ) )
		{
			ApplyForce( MOVE_BACK, elapsedTime );
			forceApplied = true;
			if (!g_nx->gJump)
				move = CTrue;
			if (move)
			{
				if (g_input.KeyDown(DIK_LSHIFT))
				{
					ManageCharacterBlends("run");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
					}
				}
				else
				{
					ManageCharacterBlends("walk");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}
				}
			}

		}
		else if( g_input.KeyDown( DIK_A ) || g_input.KeyDown( DIK_LEFT ) )
		{
			ApplyForce( MOVE_LEFT, elapsedTime );
			forceApplied = true;
			if (!g_nx->gJump)
				move = CTrue;
			if (move)
			{
				if (g_input.KeyDown(DIK_LSHIFT))
				{
					ManageCharacterBlends("run");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
					}
				}
				else
				{
					ManageCharacterBlends("walk");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}
				}
			}

		}
		else if( g_input.KeyDown( DIK_D ) || g_input.KeyDown( DIK_RIGHT ) )
		{
			ApplyForce( MOVE_RIGHT, elapsedTime );
			forceApplied = true;
			if (!g_nx->gJump)
				move = CTrue;
			if (move)
			{
				if (g_input.KeyDown(DIK_LSHIFT))
				{
					ManageCharacterBlends("run");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
					}
				}
				else
				{
					ManageCharacterBlends("walk");
					if (g_mainCharacter)
					{
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}
				}
			}

		}
		//else if( g_input.KeyDown( DIK_Q ) )
		//{
		//	ApplyForce( MOVE_UP, elapsedTime );
		//	forceApplied = true;
		//}
		//else if( g_input.KeyDown( DIK_E ) )
		//{
		//	ApplyForce( MOVE_DOWN, elapsedTime );
		//	forceApplied = true;
		//}

		if (!forceApplied || !move)
		{
			if (!forceApplied)
				ApplyForce(IDLE, elapsedTime);
			if (!move && g_mainCharacter)
			{
				CBool idle = CFalse;
				if (!g_nx->gJump)
					idle = CTrue;
				CBool jumping = CFalse;
				if (IsJumping(jumping) && jumping)
				{
					CFloat duration = 1.0f;
					if (GetJumpCurrentEndDuration(duration) && duration <= g_characterBlendingProperties.m_jumpDelayOut)
						idle = CTrue;
				}
				//Fix Bug: if we don't have any active animation:
				CInt index = 0;
				for (CUInt i = 0; i < g_mainCharacter->GetInstancePrefab()->GetScene(0)->m_animationClips.size(); i++)
				{
					if (g_mainCharacter->GetInstancePrefab()->GetScene(0)->m_animationClips[i]->GetAnimationStatus() == eANIM_NONE)
					{
						index++;
					}
				}
				if (index == g_mainCharacter->GetInstancePrefab()->GetScene(0)->m_animationClips.size())
				{
					Cpy(m_previousCharacterAnimationType, "");
					idle = CTrue;
				}

				if (idle)
				{
					if (!Cmp(m_previousCharacterAnimationType, "idle") && !Cmp(m_previousCharacterAnimationType, "more_idle"))
					{
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
					}

					std::vector<std::string> idleName = g_mainCharacter->GetIdleName();

					if (!Cmp(m_previousCharacterAnimationType, "idle") && !Cmp(m_previousCharacterAnimationType, "more_idle"))
					{
						if (ManageCharacterBlends("idle"))
						{
							Cpy(currentIdleName, (CChar*)idleName[0].c_str());
							m_idleCounter = 0.0f;
							firstIdle = CTrue;
						}
					}
					if (firstIdle)
						m_idleCounter += elapsedTime;
					else if (idleName.size() > 1)
					{
						g_currentInstancePrefab = g_mainCharacter->GetInstancePrefab();
						CScene* scene = NULL;
						CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
						for (CUInt j = 0; j < 3; j++)
						{
							if (prefab && prefab->GetHasLod(j) && g_currentInstancePrefab->GetSceneVisible(j))
							{
								scene = g_currentInstancePrefab->GetScene(j);
								break;
							}
						}

						for (CInt j = 0; j < scene->GetNumClips(); j++)
						{
							if (Cmp(currentIdleName, scene->m_animationClips[j]->GetName()))
							{
								if (m_idleCounter < scene->m_animationClips[j]->GetEnd())
								{
									m_idleCounter += elapsedTime;
								}
								else
								{
									if (ManageCharacterBlends("idle"))
									{
										Cpy(currentIdleName, (CChar*)idleName[0].c_str());
										m_idleCounter = 0.0f;
										firstIdle = CTrue;
									}
								}
							}
						}
					}
					if (idleName.size() > 1 && m_idleCounter >= g_mainCharacter->GetIdleDelayForRandomPlay() && firstIdle)
					{
						CInt index = rand() % idleName.size();
						if (index == 0 && idleName.size() > 1)
							index = 1;
						if (ManageCharacterBlends("more_idle", (CChar*)idleName[index].c_str()))
						{
							Cpy(currentIdleName, (CChar*)idleName[index].c_str());
							m_idleCounter = 0.0f;
							firstIdle = CFalse;
						}
					}
				}
			}
		}
		if (forceApplied)
			m_idleCounter = 0.0f;

		if (/*g_physXProperties.m_bApplyGravity &&*/ g_physXProperties.m_bJumping)
			if( g_input.KeyDown( DIK_SPACE ) )
				if (!g_nx->gJump)
				{
					CBool jumping = CFalse;
					if (IsJumping(jumping) && !jumping)
					{
						ManageCharacterBlends("jump");
						g_nx->StartJump(g_physXProperties.m_fJumpPower);
						g_main->m_soundSystem->PlayALSound(*(g_mainCharacter->m_jumpSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));


					}
					else if (!g_mainCharacter)
					{
						g_nx->StartJump(g_physXProperties.m_fJumpPower);
					}
				}

		CInt dx, dy;
		g_input.GetMouseMovement( dx, dy );
		if (g_main->m_prevLoadScene)
		{
			dx = dy = 0;
			g_main->m_prevLoadScene = CFalse;
		}

		if (dx != 0 || dy != 0)
			m_calculateDistance = CTrue;

		if (g_input.ButtonDown(0))
		{
			m_previousLeftButtonDown = CTrue;
		}
		else
		{
			m_previousLeftButtonDown = CFalse;
		}

		if (g_input.ButtonDown(1))
		{
			m_previousRightButtonDown = CTrue;
		}
		else
		{
			m_previousRightButtonDown = CFalse;
		}

		g_main->m_mousePosition.x += dx;
		if (g_main->m_mousePosition.x < 1.0f) g_main->m_mousePosition.x = 1.0f;
		else if (g_main->m_mousePosition.x > g_width) g_main->m_mousePosition.x = g_width;

		g_main->m_mousePosition.y += dy;
		if (g_main->m_mousePosition.y < 0.0f) g_main->m_mousePosition.y = 0.0f;
		else if (g_main->m_mousePosition.y > g_height) g_main->m_mousePosition.y = g_height;

		if( dx > 0 ) 
		{
			g_camera->m_perspectiveCameraYaw -= (CFloat)dx * 0.004f;
		}
		else if( dx < 0 )
		{
			g_camera->m_perspectiveCameraYaw -= (CFloat)dx * 0.004f;
		}
		//if( g_input.ButtonDown(1) )
		//{
		//	if( dy > 0 ) 
		//		g_camera->m_cameraManager->SetZoomOut( elapsedTime * 50.0f );
		//	else if( dy < 0 )
		//		g_camera->m_cameraManager->SetZoomIn( elapsedTime * 50.0f );
		//}
		//else
		//{
			if( dy > 0 ) 
			{
				g_camera->m_perspectiveCurrentCameraTilt -= (CFloat)dy * 0.004f;
				if (g_camera->m_perspectiveCurrentCameraTilt > g_camera->m_perspectiveCameraMaxTilt)
					g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraMaxTilt;
				else if (g_camera->m_perspectiveCurrentCameraTilt < g_camera->m_perspectiveCameraMinTilt)
					g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraMinTilt;

			}
			else if( dy < 0 )
			{
				g_camera->m_perspectiveCurrentCameraTilt -= (CFloat)dy * 0.004f;
				if (g_camera->m_perspectiveCurrentCameraTilt > g_camera->m_perspectiveCameraMaxTilt)
					g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraMaxTilt;
				else if (g_camera->m_perspectiveCurrentCameraTilt < g_camera->m_perspectiveCameraMinTilt)
					g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraMinTilt;

			}

		//}
	}
	else if( g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX )
	{
		ApplyForce(IDLE, elapsedTime);

		CInt dx, dy;
		g_input.GetMouseMovement( dx, dy );
		if (g_main->m_prevLoadScene)
		{
			dx = dy = 0;
			g_main->m_prevLoadScene = CFalse;
		}

		if (dx != 0 || dy != 0)
			m_calculateDistance = CTrue;

		if( g_input.ButtonDown(1) )
		{
			if (dy > 0)
				g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetZoomOut(elapsedTime * 50.0f);
			else if (dy < 0)
				g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetZoomIn(elapsedTime * 50.0f);
		}
		else
		{
			g_render.GetDefaultInstanceCamera()->SetPanAndTilt( -(CFloat)dx * 0.2f, -(CFloat)dy * 0.2f );
		}

		//default speed
		if ((g_input.KeyDown(DIK_LCONTROL) || g_input.KeyDown(DIK_RCONTROL)) &&
			(g_input.KeyDown(DIK_LSHIFT) || g_input.KeyDown(DIK_RSHIFT)))
		{
			if (fabs(g_camera->m_cameraSpeed - DEFAULT_CAMERA_SPEED) > EPSILON)
			{
				g_camera->m_cameraSpeed = DEFAULT_CAMERA_SPEED;
			}
		}
		//default zoom
		if ((g_input.KeyDown(DIK_LCONTROL) || g_input.KeyDown(DIK_RCONTROL)) &&
			g_input.KeyDown(DIK_Z))
		{
			if (fabs(g_render.GetDefaultInstanceCamera()->m_abstractCamera->GetAngle() - DEFAULT_CAMERA_ANGLE) > EPSILON)
			{
				g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetAngle(DEFAULT_CAMERA_ANGLE);
			}
		}

		if (g_input.KeyDown( DIK_S ))
		{
			//Decrease Speed?
			if (g_input.KeyDown(DIK_LCONTROL) || g_input.KeyDown(DIK_RCONTROL))
			{
				g_camera->m_cameraSpeed -= 5.0f;
				if (g_camera->m_cameraSpeed < EPSILON)
				{
					g_camera->m_cameraSpeed += 5.0f;
				}
			}
			else
			{
				// UI code to move the camera closer
				g_render.GetDefaultInstanceCamera()->MoveTransform(elapsedTime * g_camera->m_cameraSpeed, 0.0f, 0.0f);
			}
		}

		if (g_input.KeyDown( DIK_W ))
		{
			if (g_input.KeyDown(DIK_LCONTROL) || g_input.KeyDown(DIK_RCONTROL))
			{
				g_camera->m_cameraSpeed += 5.0f;
			}
			else
			{
				// UI code to move the camera farther away
				g_render.GetDefaultInstanceCamera()->MoveTransform(-elapsedTime* g_camera->m_cameraSpeed, 0.0f, 0.0f);
			}
		}

		if (g_input.KeyDown( DIK_Q ))
		{
			// UI code to move the camera farther up
			g_render.GetDefaultInstanceCamera()->MoveTransform(0.0f, 0.0f, elapsedTime* g_camera->m_cameraSpeed);
		}

		if (g_input.KeyDown( DIK_E ))
		{
			// UI code to move the camera farther down
			g_render.GetDefaultInstanceCamera()->MoveTransform(0.0f, 0.0f, -elapsedTime* g_camera->m_cameraSpeed);
		}

		if (g_input.KeyDown( DIK_D ))
		{
			// UI code to move the camera farther right
			g_render.GetDefaultInstanceCamera()->MoveTransform(0.0f, -elapsedTime* g_camera->m_cameraSpeed, 0.0f);
		}

		if (g_input.KeyDown( DIK_A ))
		{
			// UI code to move the camera farther left
			g_render.GetDefaultInstanceCamera()->MoveTransform(0.0f, elapsedTime* g_camera->m_cameraSpeed, 0.0f);
		}
	}
	else if (g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX || g_currentCameraType == eCAMERA_COLLADA || g_currentCameraType == eCAMERA_ENGINE)
	{
		ApplyForce(IDLE, elapsedTime);
		EnableIdleAnimations(); //for now I enable idle animations for main character everytime PhysX camera is not active
	}
	return CTrue;
}

CVoid CMain::EnableIdleAnimations()
{
	if (g_mainCharacter)
	{
		CBool idle = CFalse;
		if (!g_nx->gJump)
			idle = CTrue;
		CBool jumping = CFalse;
		if (IsJumping(jumping) && jumping)
		{
			CFloat duration = 1.0f;
			if (GetJumpCurrentEndDuration(duration) && duration <= g_characterBlendingProperties.m_jumpDelayOut)
				idle = CTrue;
		}
		//Fix Bug: if we don't have any active animation:
		CInt index = 0;
		for (CUInt i = 0; i < g_mainCharacter->GetInstancePrefab()->GetScene(0)->m_animationClips.size(); i++)
		{
			if (g_mainCharacter->GetInstancePrefab()->GetScene(0)->m_animationClips[i]->GetAnimationStatus() == eANIM_NONE)
			{
				index++;
			}
		}
		if (index == g_mainCharacter->GetInstancePrefab()->GetScene(0)->m_animationClips.size())
		{
			Cpy(m_previousCharacterAnimationType, "");
			idle = CTrue;
		}

		if (idle)
		{
			if (!Cmp(m_previousCharacterAnimationType, "idle") && !Cmp(m_previousCharacterAnimationType, "more_idle"))
			{
				g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
				g_main->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
			}

			std::vector<std::string> idleName = g_mainCharacter->GetIdleName();

			if (!Cmp(m_previousCharacterAnimationType, "idle") && !Cmp(m_previousCharacterAnimationType, "more_idle"))
			{
				if (ManageCharacterBlends("idle"))
				{
					Cpy(currentIdleName, (CChar*)idleName[0].c_str());
					m_idleCounter = 0.0f;
					firstIdle = CTrue;
				}
			}
			if (firstIdle)
				m_idleCounter += elapsedTime;
			else if (idleName.size() > 1)
			{
				g_currentInstancePrefab = g_mainCharacter->GetInstancePrefab();
				CScene* scene = NULL;
				CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
				for (CUInt j = 0; j < 3; j++)
				{
					if (prefab && prefab->GetHasLod(j) && g_currentInstancePrefab->GetSceneVisible(j))
					{
						scene = g_currentInstancePrefab->GetScene(j);
						break;
					}
				}

				for (CInt j = 0; j < scene->GetNumClips(); j++)
				{
					if (Cmp(currentIdleName, scene->m_animationClips[j]->GetName()))
					{
						if (m_idleCounter < scene->m_animationClips[j]->GetEnd())
						{
							m_idleCounter += elapsedTime;
						}
						else
						{
							if (ManageCharacterBlends("idle"))
							{
								Cpy(currentIdleName, (CChar*)idleName[0].c_str());
								m_idleCounter = 0.0f;
								firstIdle = CTrue;
							}
						}
					}
				}
			}
			if (idleName.size() > 1 && m_idleCounter >= g_mainCharacter->GetIdleDelayForRandomPlay() && firstIdle)
			{
				CInt index = rand() % idleName.size();
				if (index == 0 && idleName.size() > 1)
					index = 1;
				if (ManageCharacterBlends("more_idle", (CChar*)idleName[index].c_str()))
				{
					Cpy(currentIdleName, (CChar*)idleName[index].c_str());
					m_idleCounter = 0.0f;
					firstIdle = CFalse;
				}
			}
		}
	}

}

CVoid CMain::ApplyForce( CInt direction, CFloat elapsedTime )
{
	g_nx->runPhysics( NxVec3( 0.0f, 0.0f, 0.0f ), 0.0f, direction, elapsedTime );
}

CBool CMain::Reset()
{
	g_clickedNew = CTrue;
	g_camera->Reset();
	g_dofProperties.Reset();
	g_fogProperties.Reset();
	g_bloomProperties.Reset();
	g_lightProperties.Reset();
	g_currentVSceneProperties.Reset();
	g_instancePrefabLODPercent.Reset();
	g_prefabProperties.Reset();
	g_cameraProperties.Reset();
	g_characterBlendingProperties.Reset();
	g_shadowProperties.Reset();
	g_physXProperties.Reset();
	g_shadowProperties.m_enable = CTrue;
	g_pathProperties.Reset();
	g_extraTexturesNamingConventions.Reset();
	g_sceneManagerObjectsPerSplit = 15;
	//g_options.Reset();
	g_useGlobalAmbientColor = CFalse;
	g_globalAmbientColor.r = g_globalAmbientColor.g = g_globalAmbientColor.b = 0.5f;
	g_globalAmbientColor.a = 1.0f;

	if (g_mainCharacter)
	{
		g_mainCharacter->Destroy();
		g_mainCharacter->Reset();
		g_mainCharacter = NULL;
	}

	//remove gui
	for (CUInt i = 0; i < g_guiButtons.size(); i++)
	{
		CDelete(g_guiButtons[i]);
	}
	g_guiButtons.clear();

	for (CUInt i = 0; i < g_guiBackgrounds.size(); i++)
	{
		CDelete(g_guiBackgrounds[i]);
	}
	g_guiBackgrounds.clear();

	for (CUInt i = 0; i < g_guiTexts.size(); i++)
	{
		CDelete(g_guiTexts[i]);
	}
	g_guiTexts.clear();

	for (CUInt i = 0; i < g_guis.size(); i++)
	{
		CDelete(g_guis[i]);
	}
	g_guis.clear();

	for (CUInt i = 0; i < g_triggers.size(); i++)
	{
		CDelete(g_triggers[i]);
	}
	g_triggers.clear();

	//clear geometries
	for (CUInt i = 0; i < g_geometries.size(); i++)
		CDelete(g_geometries[i]);
	g_geometries.clear();

	//clear prefabs
	for (CUInt i = 0; i < g_prefab.size(); i++)
	{
		CDelete(g_prefab[i]);
	}
	g_prefab.clear();

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CDelete(g_instancePrefab[i]);
	}
	g_instancePrefab.clear();

	g_octree->ResetState();
	g_render.SetScene( NULL );

	if( g_currentCameraType != eCAMERA_PHYSX )
	{
		g_currentCameraType = eCAMERA_PHYSX;
		g_render.SetActiveInstanceCamera(NULL);
		g_main->m_lockInput = CFalse;
	}
	g_main->distance_vector.clear();
	g_main->sorted_prefabs.clear();

	Cpy(g_shadowProperties.m_directionalLightName, "\n" );
	if( g_nx->m_hasScene || gPhysXscene)
	{
		if(!g_clickedOpen)
			ResetPhysX();
	}
	for( std::vector<CWater*>::iterator it = g_engineWaters.begin(); it != g_engineWaters.end(); it++ )
	{
		CDelete( *it );
	}
	if( g_engineWaters.size() > 0 )
		g_engineWaters.clear();

	for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
	{
		CDelete(g_engineStaticSounds[i]);
	}
	if (g_engineStaticSounds.size() > 0)
		g_engineStaticSounds.clear();

	//clear the scene
	for (std::vector<CScene*>::iterator it = g_scene.begin(); it != g_scene.end(); it++)
	{
		CDelete(*it);
	}
	//Clear the vctor objects
	if( g_scene.size() > 0 ) 
		g_scene.clear();
	
	//clear all the default engines here
	for( std::vector<CInstanceLight*>::iterator it = g_engineLights.begin(); it != g_engineLights.end(); it++ )
	{
		CDelete( (*it)->m_abstractLight ) ;
		CDelete( *it );
	}
	//Clear the vctor objects
	if( g_engineLights.size() > 0 )
		g_engineLights.clear();

	for (std::vector<CInstanceCamera*>::iterator it = g_engineCameraInstances.begin(); it != g_engineCameraInstances.end(); it++)
	{
		CDelete((*it)->m_abstractCamera);
		CDelete(*it);
	}
	if (g_engineCameraInstances.size() > 0)
		g_engineCameraInstances.clear();

	for( std::vector<CImage*>::iterator it = g_images.begin(); it != g_images.end(); it++ )
	{
		CDelete( *it );
	}

	if( g_images.size() > 0 )
		g_images.clear();

	for( std::vector<CImage*>::iterator it = g_waterImages.begin(); it != g_waterImages.end(); it++ )
	{
		CDelete( *it );
	}
	if( g_waterImages.size() > 0 )
		g_waterImages.clear();

	for( std::vector<COpenALSoundBuffer*>::iterator it = g_soundBuffers.begin(); it != g_soundBuffers.end(); it++ )
	{
		CDelete( *it );
	}
	g_soundBuffers.clear();

	//Delete Resource Files
	for (CUInt j = 0; j < g_resourceFiles.size(); j++)
		CDelete(g_resourceFiles[j]);
	g_resourceFiles.clear();

	if (g_main && g_main->GetCursorIcon())
		g_main->GetCursorIcon()->SetVisible(CFalse);

	CDelete( g_skyDome );

	CDelete(g_terrain);

	CDelete(g_startup);

	//delete ambient sound
	if( g_main->m_ambientSound )
	{
		g_main->m_soundSystem->StopALSound( *(g_main->m_ambientSound->GetSoundSource()) );
		alSourcei(g_main->m_ambientSound->GetSoundSource()->GetSource(), AL_BUFFER, AL_NONE); 
		CDelete( g_main->m_ambientSound );			
	}
	//Reset name indexes which are used for selection
	g_nameIndex = 1;

	g_selectedName = -1; 		//no object has been selected

	Cpy( g_currentVSceneName, "\n" ); //save functions
	Cpy(g_currentVSceneName, "\n"); //save functions
	Cpy(g_currentPrefabName, "\n");
	Cpy(g_currentPrefabPackagePath, "\n");
	Cpy(g_currentPrefabAndSceneName, "\n");
	Cpy(g_currentInstancePrefabName, "\n");

	g_currentInstancePrefab = NULL;
	g_maxInstancePrefabRadius = -1.0f;
	g_clickedNew = CFalse;

	return CTrue;
}

CBool CMain::InsertPrefab(CPrefab* prefab)
{
	CChar packagePath[MAX_NAME_SIZE];
	CChar prefabPath[MAX_NAME_SIZE];
	CChar prefabAndPackageName[MAX_NAME_SIZE];

	Cpy(prefabAndPackageName, prefab->GetName());

	Cpy(packagePath, "Assets/Packages/");
	Append(packagePath, prefab->GetPackageName());
	Append(packagePath, "/");

	Cpy(prefabPath, packagePath);
	Append(prefabPath, prefab->GetPrefabName());
	Append(prefabPath, "/");
	Append(prefabPath, prefabAndPackageName);
	Append(prefabPath, ".vpf");

	Cpy(g_currentPrefabPackagePath, packagePath);
	Cpy(g_currentPrefabName, prefab->GetPrefabName());

	CPrefab* m_newPrefab = prefab;

	FILE *filePtr = fopen(prefabPath, "rb");
	if (filePtr)
	{

		CChar engineName[MAX_NAME_SIZE];
		fread(&engineName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		if (!CmpIn(engineName, "VandaEngine"))
		{
			fclose(filePtr);
			MessageBoxA(NULL, "Invalid VPF file!", "Vanda Engine Error", MB_OK | MB_ICONERROR);
			return CFalse;
		}

		fread(&g_edition, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&g_maxVersion, 1, sizeof(CInt), filePtr);
		fread(&g_minVersion, 1, sizeof(CInt), filePtr);
		fread(&g_bugFixesVersion, 1, sizeof(CInt), filePtr);

		fread(&g_currentPassword, sizeof(CChar), MAX_NAME_SIZE, filePtr);

		//read engine options
		CShadowProperties shadowProperties;
		fread(&shadowProperties, sizeof(CShadowProperties), 1, filePtr);

		CPhysXProperties physXProperties;
		CDOFProperties dofProperties;
		CFogProperties fogProperties;
		CBloomProperties bloomProperties;
		CLightProperties lightProperties;
		CLODProperties lodProperties;
		CCameraProperties cameraProperties;
		CPathProperties pathProperties;
		CCharacterBlendingProperties characterBlendingProperties;
		fread(&physXProperties, sizeof(CPhysXProperties), 1, filePtr);
		fread(&dofProperties, sizeof(CDOFProperties), 1, filePtr);
		fread(&fogProperties, sizeof(CFogProperties), 1, filePtr);
		fread(&bloomProperties, sizeof(CBloomProperties), 1, filePtr);
		fread(&lightProperties, sizeof(CLightProperties), 1, filePtr);
		fread(&lodProperties, sizeof(CLODProperties), 1, filePtr);
		fread(&cameraProperties, sizeof(CCameraProperties), 1, filePtr);

		//fread(&characterBlendingProperties, sizeof(CCharacterBlendingProperties), 1, filePtr);
		fread(&pathProperties, sizeof(CPathProperties), 1, filePtr);
		CBool demo;
		fread(&demo, sizeof(CBool), 1, filePtr);

		CChar banner[MAX_NAME_SIZE];
		fread(&banner, sizeof(CChar), MAX_NAME_SIZE, filePtr);

		CExtraTexturesNamingConventions namingConventions;
		CBool bGlobalAmbientColor;
		CColor4f globalAmbientColor;
		CInt sceneManagersObjectsPerSplit;
		fread(&namingConventions, sizeof(CExtraTexturesNamingConventions), 1, filePtr);
		fread(&bGlobalAmbientColor, sizeof(CBool), 1, filePtr);
		fread(&globalAmbientColor, sizeof(CColor4f), 1, filePtr);
		fread(&sceneManagersObjectsPerSplit, sizeof(CInt), 1, filePtr);

		//read physX 
		CBool insertPhysXScene = CFalse;
		CChar strPhysXSceneName[MAX_NAME_SIZE];
		NxExtendedVec3 characterPos;
		CVec3f cameraInstancePos;
		CVec2f cameraInstancePanTilt;
		CFloat cameraInstanceZoom;
		fread(&insertPhysXScene, sizeof(CBool), 1, filePtr);
		fread(strPhysXSceneName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&characterPos, sizeof(NxExtendedVec3), 1, filePtr);

		fread(&cameraInstancePos, sizeof(CVec3f), 1, filePtr);
		fread(&cameraInstancePanTilt, sizeof(CVec2f), 1, filePtr);
		fread(&cameraInstanceZoom, sizeof(CFloat), 1, filePtr);

		CBool hasScript;
		CChar script[MAX_NAME_SIZE];
		fread(&hasScript, 1, sizeof(CBool), filePtr);
		fread(script, 1, sizeof(CChar) * MAX_NAME_SIZE, filePtr);

		CChar scriptPath[MAX_NAME_SIZE];
		CChar* tempPath = GetAfterPath(script);
		if (hasScript)
			sprintf(scriptPath, "%s%s%s%s", packagePath, prefab->GetPrefabName(), "/Scripts/", tempPath);
		else
			Cpy(scriptPath, "\n");

		prefab->SetHasScript(hasScript);
		prefab->SetScript(scriptPath);

		CChar tempSceneName[MAX_NAME_SIZE];

		CInt tempSceneSize, tempGeoSize;
		fread(&tempSceneSize, sizeof(CInt), 1, filePtr);

		for (CInt i = 0; i < tempSceneSize; i++)
		{
			CInt clipIndex;
			CBool playAnimation, loopAnimation, isVisible, isAlwaysVisible, castShadow, transformable, selectable;
			CInt tempSceneAnimationListSize;

			fread(tempSceneName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
			fread(&clipIndex, 1, sizeof(CInt), filePtr);
			fread(&playAnimation, 1, sizeof(CBool), filePtr);
			fread(&loopAnimation, 1, sizeof(CBool), filePtr);
			fread(&isVisible, 1, sizeof(CBool), filePtr);
			fread(&isAlwaysVisible, 1, sizeof(CBool), filePtr);
			fread(&castShadow, 1, sizeof(CBool), filePtr);
			fread(&transformable, 1, sizeof(CBool), filePtr);
			fread(&selectable, 1, sizeof(CBool), filePtr);

			fread(&tempSceneAnimationListSize, 1, sizeof(CInt), filePtr);

			CScene * tempScene = new CScene();

			for (CInt j = 0; j < tempSceneAnimationListSize; j++)
			{
				CChar name[MAX_NAME_SIZE];
				fread(name, 1, sizeof(CChar) * MAX_NAME_SIZE, filePtr);
				tempScene->m_animationSceneNames.push_back(name);
			}

			fread(&tempGeoSize, sizeof(CInt), 1, filePtr);


			CBool sceneLoaded = CFalse;

			CChar * nameOnly = GetAfterPath(tempSceneName);

			//save functions. it should be copies in WIN32 Project as well
			CChar name[MAX_NAME_SIZE];

			sprintf(name, "%s%s%s%s", packagePath, prefab->GetPrefabName(), "/External Scenes/", nameOnly);

			CChar prefabAndSceneName[MAX_NAME_SIZE];
			sprintf(prefabAndSceneName, "%s%s%s", prefabAndPackageName, "_", nameOnly);
			Cpy(g_currentPrefabAndSceneName, prefabAndSceneName);

			g_scene.push_back(tempScene);

			for (CUInt j = 0; j < tempScene->m_animationSceneNames.size(); j++)
			{
				CChar sceneName[MAX_NAME_SIZE];
				Cpy(sceneName, tempScene->m_animationSceneNames[j].c_str());
				CChar * nameOnly = GetAfterPath(sceneName);
				GetWithoutDot(nameOnly);
				Append(nameOnly, ".vac");
				//save functions. it should be copied in WIN32 Project as well
				CChar name[MAX_NAME_SIZE];
				sprintf(name, "%s%s%s%s", packagePath, g_currentPrefabName, "/External Scenes/", nameOnly);

				CChar clipName[MAX_NAME_SIZE];
				Cpy(clipName, GetAfterPath(sceneName));
				GetWithoutDot(clipName);

				//Load 
				std::ifstream ifs(name, ios_base::binary);
				boost::archive::binary_iarchive ia(ifs);
				CAnimationClip* p;
				ia >> p;
				for (CUInt l = 0; l < p->m_animations.size(); l++)
				{
					if (!tempScene->GetAnimation(p->m_animations[l]->GetName(), p->m_animations[l]->GetDocURI()))
					{
						tempScene->m_animations.push_back(p->m_animations[l]);

						if (p->m_animations[l]->GetChannelSize() > 0)
						{
							p->m_animations[l]->GenerateKeys();
						}
						if (p->m_animations[l]->GetEndTime() > tempScene->GetLastKeyTime())
							tempScene->SetLastKeyTime(p->m_animations[l]->GetEndTime());

					}
				}
				p->SetAnimationStatus(eANIM_NONE);
				p->SetCurrentTime(0.0f);
				p->SetCurrentTime2(0.0f);
				p->SetCurrentWeight(0.0f);
				tempScene->m_animationClips.push_back(p);

				tempScene->m_hasAnimation = CTrue;
			}
			if (tempScene->m_animationSceneNames.size())
				tempScene->SetNumClips(tempScene->m_animationSceneNames.size());

			if (tempScene->m_animationSceneNames.size())
				tempScene->SetLoadAnimation(CFalse);
			if (tempScene->Load(name))
			{
				tempScene->SetLoadAnimation(CTrue);

				tempScene->SetClipIndexForStartup(clipIndex);
				tempScene->m_playAnimation = playAnimation;
				tempScene->m_isVisible = isVisible;
				tempScene->m_alwaysVisible = isAlwaysVisible;
				tempScene->m_loopAnimationAtStartup = loopAnimation;
				tempScene->m_castShadow = castShadow;
				tempScene->m_isTransformable = transformable;
				tempScene->m_isSelectable = selectable;

				if (tempScene->m_isTransformable)
				{
					for (CUInt l = 0; l < tempScene->m_instanceGeometries.size(); l++)
					{
						CGeometry* m_abstractGeometry = tempScene->m_instanceGeometries[l]->m_abstractGeometry;
						m_abstractGeometry->m_hasAnimation = CTrue;
					}
					tempScene->m_hasAnimation = CTrue;
				}

				if (tempScene->m_playAnimation)
				{
					tempScene->SetAnimationStatus(eANIM_PLAY);
					tempScene->SetClipIndex(clipIndex, loopAnimation);
					if (tempScene->m_loopAnimationAtStartup)
						tempScene->BlendCycle(tempScene->GetCurrentClipIndex(), 1.0f, 0.0f);
					else
						tempScene->ExecuteAction(tempScene->GetCurrentClipIndex(), 0.0f, 0.0f, 1.0f, CFalse);
				}
				else
				{
					tempScene->SetAnimationStatus(eANIM_PAUSE);
				}
				//save functions/////////////////////////////////
				g_currentScene = tempScene; //mark the current scene. Save functions

				//save functions/////////////////////////////////
				g_render.SetScene(tempScene);
				tempScene->Update(0.0f, CTrue);

				if (CmpIn(tempScene->GetName(), "_LOD1"))
				{
					m_newPrefab->GetCurrentInstance()->SetScene(0, tempScene);
					m_newPrefab->SetHasLod(0);
					tempScene->SetDocURI(m_newPrefab->GetCurrentInstance()->GetName());
				}
				else if (CmpIn(tempScene->GetName(), "_LOD2"))
				{
					m_newPrefab->GetCurrentInstance()->SetScene(1, tempScene);
					m_newPrefab->SetHasLod(1);
					tempScene->SetDocURI(m_newPrefab->GetCurrentInstance()->GetName());
				}
				else if (CmpIn(tempScene->GetName(), "_LOD3"))
				{
					m_newPrefab->GetCurrentInstance()->SetScene(2, tempScene);
					m_newPrefab->SetHasLod(2);
					tempScene->SetDocURI(m_newPrefab->GetCurrentInstance()->GetName());
				}
				else if (CmpIn(tempScene->GetName(), "_COL"))
				{
					m_newPrefab->GetCurrentInstance()->SetScene(3, tempScene);
					m_newPrefab->GetCurrentInstance()->SetHasCollider(CTrue);
					tempScene->SetDocURI(m_newPrefab->GetCurrentInstance()->GetName());
				}

				sceneLoaded = CTrue;

			}
			else
			{
				delete tempScene;
				tempScene = NULL;
				g_scene.erase(g_scene.end());
				fclose(filePtr);
				MessageBox(NULL, _T("Couldn't load the scene"), _T("VandaEngine Error"), MB_OK);
				sceneLoaded = CFalse;
			}
			if (!sceneLoaded)
				return CFalse;
			if (CmpIn(tempScene->GetName(), "trigger")) //triggers
			{
				if (!g_nx->m_hasScene)
				{
					tempScene->m_isTrigger = CTrue;
					tempScene->Update();
					tempScene->CreateTrigger(g_nx);
				}
				else
				{
					MessageBoxA(NULL, "\nCouldn't create the triggers. In order to create triggers from COLLADA files, you should remove current external PhysX scene.", "Error", MB_OK);
				}

			}

			for (CUInt j = 0; j < (CUInt)tempGeoSize; j++)
			{
				CChar m_strNormalMap[MAX_NAME_SIZE];
				CChar m_strDirtMap[MAX_NAME_SIZE];
				CChar m_strGlossMap[MAX_NAME_SIZE];
				CChar m_strHeightMap[MAX_NAME_SIZE];
				CChar m_strDuDvMap[MAX_NAME_SIZE];
				CChar m_strDiffuse[MAX_NAME_SIZE];
				CBool m_hasNormalMap, m_hasGlossMap, m_hasHeightMap, m_hasDuDvMap, m_hasDirtMap, m_hasDiffuse;
				CBool m_cullFaces;
				CUInt m_groupSize;
				CFloat m_bias, m_scale;

				fread(&m_hasDirtMap, sizeof(CBool), 1, filePtr);
				fread(m_strDirtMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);

				fread(&m_hasNormalMap, sizeof(CBool), 1, filePtr);
				fread(m_strNormalMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);
				fread(&m_bias, sizeof(CFloat), 1, filePtr);
				fread(&m_scale, sizeof(CFloat), 1, filePtr);

				fread(&m_hasGlossMap, sizeof(CBool), 1, filePtr);
				fread(m_strGlossMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);

				fread(&m_hasHeightMap, sizeof(CBool), 1, filePtr);
				fread(m_strHeightMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);

				fread(&m_hasDuDvMap, sizeof(CBool), 1, filePtr);
				fread(m_strDuDvMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);

				fread(&m_hasDiffuse, sizeof(CBool), 1, filePtr);
				fread(m_strDiffuse, sizeof(CChar), MAX_NAME_SIZE, filePtr);

				//load cull face( enabled or disabled data) for the current geo
				fread(&m_cullFaces, sizeof(CBool), 1, filePtr);

				//read PhysX data
				CUInt physx_point_size;
				fread(&physx_point_size, sizeof(CUInt), 1, filePtr);
				for (CUInt m = 0; m < physx_point_size; m++)
				{
					CVec3f* point = CNew(CVec3f);
					fread(point, sizeof(CVec3f), 1, filePtr);
					if( tempScene->m_geometries.size() >= j + 1 )
						tempScene->m_geometries[j]->m_physx_points.push_back(point);
				}
				CUInt physx_triangles_size;
				fread(&physx_triangles_size, sizeof(CUInt), 1, filePtr);

				for (CUInt m = 0; m < physx_triangles_size; m++)
				{
					CTriangles* tri = CNew(CTriangles);
					CUInt count;
					fread(&count, sizeof(CUInt), 1, filePtr);
					tri->m_count = count;
					tri->m_indexes = CNewData(CUInt, tri->m_count * 3);

					for (CUInt n = 0; n < tri->m_count * 3; n++)
					{
						CUInt index;
						fread(&index, sizeof(CUInt), 1, filePtr);
						tri->m_indexes[n] = index;
					}
					if (tempScene->m_geometries.size() >= j + 1)
						tempScene->m_geometries[j]->m_physx_triangles.push_back(tri);
				}

				CInt cm_size = 0;
				fread(&cm_size, sizeof(CInt), 1, filePtr);
				if(tempScene->m_geometries.size() >= j + 1)
					tempScene->m_geometries[j]->m_collapseMap.SetSize(cm_size);
				for (CInt m = 0; m < cm_size; m++)
				{
					CInt cm_value;
					fread(&cm_value, sizeof(CInt), 1, filePtr);
					if (tempScene->m_geometries.size() >= j + 1)
						tempScene->m_geometries[j]->m_collapseMap[m] = cm_value;
				}

				///////////////////

				fread(&m_groupSize, sizeof(CUInt), 1, filePtr);
				//store group info

				if (sceneLoaded)
				{
					if (m_hasDirtMap && tempScene->m_geometries.size() >= j + 1)
					{
						tempScene->m_geometries[j]->m_hasDirtMap = CTrue;
						Cpy(tempScene->m_geometries[j]->m_strDirtMap, m_strDirtMap);
					}
					if (m_hasNormalMap && tempScene->m_geometries.size() >= j + 1)
					{
						tempScene->m_geometries[j]->m_hasNormalMap = CTrue;
						Cpy(tempScene->m_geometries[j]->m_strNormalMap, m_strNormalMap);
					}
					if ( tempScene->m_geometries.size() >= j + 1)
					{
						tempScene->m_geometries[j]->m_parallaxMapBias = m_bias;
						tempScene->m_geometries[j]->m_parallaxMapScale = m_scale;
					}
					if (m_hasGlossMap && tempScene->m_geometries.size() >= j + 1)
					{
						tempScene->m_geometries[j]->m_hasGlossMap = CTrue;
						Cpy(tempScene->m_geometries[j]->m_strGlossMap, m_strGlossMap);
					}
					if (m_hasDiffuse && tempScene->m_geometries.size() >= j + 1)
					{
						tempScene->m_geometries[j]->m_hasDiffuse = CTrue;
						Cpy(tempScene->m_geometries[j]->m_strDiffuse, m_strDiffuse);
					}
					//if( m_hasHeightMap )
					//	tempScene->m_geometries[j]->SetHeightMap( m_strHeightMap );
					//if( m_hasDuDvMap )
					//	tempScene->m_geometries[j]->SetDuDvMap(); //under construction!
					if (m_cullFaces && tempScene->m_geometries.size() >= j + 1)
						tempScene->m_geometries[j]->SetCullFace(CTrue);
					else if ( tempScene->m_geometries.size() >= j + 1)
						tempScene->m_geometries[j]->SetCullFace(CFalse);

					for (CUInt k = 0; k < m_groupSize; k++)
					{

						CChar m_strGroupGlossMap[MAX_NAME_SIZE];
						CBool m_hasGroupGlossMap;
						fread(&m_hasGroupGlossMap, sizeof(CBool), 1, filePtr);
						fread(m_strGroupGlossMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);
						if (m_hasGroupGlossMap && tempScene->m_geometries.size() >= j + 1)
							tempScene->m_geometries[j]->m_groups[k]->SetGlossMap(m_strGroupGlossMap);

						CChar m_strGroupDirtMap[MAX_NAME_SIZE];
						CBool m_hasGroupDirtMap;
						fread(&m_hasGroupDirtMap, sizeof(CBool), 1, filePtr);
						fread(m_strGroupDirtMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);
						if (m_hasGroupDirtMap && tempScene->m_geometries.size() >= j + 1)
							tempScene->m_geometries[j]->m_groups[k]->SetDirtMap(m_strGroupDirtMap);

						CChar m_strGroupNormalMap[MAX_NAME_SIZE];
						CFloat m_bias, m_scale;
						CBool m_hasGroupNormalMap;
						fread(&m_hasGroupNormalMap, sizeof(CBool), 1, filePtr);
						fread(m_strGroupNormalMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);
						fread(&m_bias, sizeof(CFloat), 1, filePtr);
						fread(&m_scale, sizeof(CFloat), 1, filePtr);
						if (m_hasGroupNormalMap && tempScene->m_geometries.size() >= j + 1)
							tempScene->m_geometries[j]->m_groups[k]->SetNormalMap(m_strGroupNormalMap, m_bias, m_scale);

						CChar m_strGroupDiffuseMap[MAX_NAME_SIZE];
						CBool m_hasGroupDiffuse;
						fread(&m_hasGroupDiffuse, sizeof(CBool), 1, filePtr);
						fread(m_strGroupDiffuseMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);
						if (m_hasGroupDiffuse && tempScene->m_geometries.size() >= j + 1)
							tempScene->m_geometries[j]->m_groups[k]->SetDiffuse(m_strGroupDiffuseMap);

						//Material Colors
						CFloat ambient[4]; CFloat diffuse[4]; CFloat specular[4]; CFloat emission[4];
						CFloat shininess, transparency;

						fread(ambient, sizeof(CFloat), 4, filePtr);
						fread(diffuse, sizeof(CFloat), 4, filePtr);
						fread(specular, sizeof(CFloat), 4, filePtr);
						fread(emission, sizeof(CFloat), 4, filePtr);
						fread(&shininess, sizeof(CFloat), 1, filePtr);
						fread(&transparency, sizeof(CFloat), 1, filePtr);

						if (tempScene->m_geometries.size() >= j + 1)
						{
							tempScene->m_geometries[j]->m_groups[k]->SetReadMaterialColorFromGameEngine();
							tempScene->m_geometries[j]->m_groups[k]->SetAmbient(ambient);
							tempScene->m_geometries[j]->m_groups[k]->SetDiffuse(diffuse);
							tempScene->m_geometries[j]->m_groups[k]->SetSpecular(specular);
							tempScene->m_geometries[j]->m_groups[k]->SetEmission(emission);
							tempScene->m_geometries[j]->m_groups[k]->SetShininess(shininess);
							tempScene->m_geometries[j]->m_groups[k]->SetTransparency(transparency);
						}
						//////////////////

					}
				}
			} //for all of the geos

			CUInt instanceGeoSize;
			fread(&instanceGeoSize, 1, sizeof(CUInt), filePtr);
			for (CUInt j = 0; j < instanceGeoSize; j++)
			{
				CChar geoName[MAX_NAME_SIZE];
				CMatrix instanceLocalToWorldMatrix;
				CBool hasPhysX;
				CChar PhysXName[MAX_NAME_SIZE];
				CPhysXAlgorithm physXAlgorithm;
				CFloat physXDensity;
				CInt physXPercentage;
				CBool isTrigger;
				CBool isInvisible;
				fread(geoName, 1, sizeof(CChar) * MAX_NAME_SIZE, filePtr);
				fread(instanceLocalToWorldMatrix, 1, sizeof(CMatrix), filePtr);
				fread(&hasPhysX, 1, sizeof(CBool), filePtr);
				fread(PhysXName, 1, sizeof(CChar) * MAX_NAME_SIZE, filePtr);
				fread(&physXAlgorithm, 1, sizeof(CPhysXAlgorithm), filePtr);
				fread(&physXDensity, 1, sizeof(CFloat), filePtr);
				fread(&physXPercentage, 1, sizeof(CInt), filePtr);
				fread(&isTrigger, 1, sizeof(CBool), filePtr);
				fread(&isInvisible, 1, sizeof(CBool), filePtr);

				if (hasPhysX)
				{
					for (CUInt k = 0; k < tempScene->m_instanceGeometries.size(); k++)
					{
						if (Cmp(geoName, tempScene->m_instanceGeometries[k]->m_abstractGeometry->GetName()))
						{
							CBool equal = CTrue;
							for (CUInt l = 0; l < 16; l++)
							{
								if (instanceLocalToWorldMatrix[l] != tempScene->m_instanceGeometries[k]->m_firstLocalToWorldMatrix[l])
								{
									equal = CFalse;
									break;
								}
							}
							if (equal)
							{
								//if (tempScene->GeneratePhysX(physXAlgorithm, physXDensity, physXPercentage, isTrigger, isInvisible, tempScene->m_instanceGeometries[k], CTrue))
								//{
									tempScene->m_instanceGeometries[k]->m_lodAlgorithm = physXAlgorithm;
									tempScene->m_instanceGeometries[k]->m_hasPhysX = CTrue;
									tempScene->m_instanceGeometries[k]->m_physXDensity = physXDensity;
									tempScene->m_instanceGeometries[k]->m_physXPercentage = physXPercentage;
									tempScene->m_instanceGeometries[k]->m_isTrigger = isTrigger;
									tempScene->m_instanceGeometries[k]->m_isInvisible = isInvisible;

									//if (physXDensity > 0 || tempScene->m_instanceGeometries[k]->m_abstractGeometry->m_hasAnimation)
									//	InsertItemToPhysXList(tempScene->m_instanceGeometries[k]->m_physXName, ePHYSXELEMENTLIST_DYNAMIC_RIGIDBODY);
									//else if (isTrigger)
									//	InsertItemToPhysXList(tempScene->m_instanceGeometries[k]->m_physXName, ePHYSXELEMENTLIST_TRIGGER);
									//else
									//	InsertItemToPhysXList(tempScene->m_instanceGeometries[k]->m_physXName, ePHYSXELEMENTLIST_STATIC_RIGIDBODY);
								//}
							}
						}
					} //for
				} //if has PhysX
			}
		} // for all of the scenes

		//if (g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX)
		//{
			//g_nx->gControllers->reportSceneChanged();
			//gPhysXscene->simulate(1.0f / 60.0f/*elapsedTime*/);
			//gPhysXscene->flushStream();
			//gPhysXscene->fetchResults(NX_ALL_FINISHED, true);
		//}

		fclose(filePtr);
	}
	return CTrue;
}

CBool CMain::Load(CChar* pathName)
{
	g_clickedOpen = CTrue;
	Cpy(g_currentVSceneName, GetAfterPath(pathName));

	FILE *filePtr;
	filePtr = fopen(pathName, "rb");
	if (!filePtr)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "Couldn't open the file: '", pathName, "' to load data");
		//fclose( filePtr );
		MessageBoxA(NULL, temp, "Error", MB_OK);
		return CFalse;
	}

	CChar engineName[MAX_NAME_SIZE];
	fread(&engineName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
	if (!CmpIn(engineName, "VandaEngine"))
	{
		fclose(filePtr);
		MessageBoxA(NULL, "Invalid Vin file!", "Vanda Engine Error", MB_OK | MB_ICONERROR);
		return CFalse;
	}
	fread(&g_edition, sizeof(CChar), MAX_NAME_SIZE, filePtr);
	fread(&g_maxVersion, 1, sizeof(CInt), filePtr);
	fread(&g_minVersion, 1, sizeof(CInt), filePtr);
	fread(&g_bugFixesVersion, 1, sizeof(CInt), filePtr);
	fread(&g_currentPassword, sizeof(CChar), MAX_NAME_SIZE, filePtr);

	//read engine options
	fread(&g_shadowProperties, sizeof(CShadowProperties), 1, filePtr);

	switch (g_shadowProperties.m_shadowResolution)
	{
	case eSHADOW_1024:
		g_dynamicShadowMap->depth_size = 1024;
		break;
	case eSHADOW_2048:
		g_dynamicShadowMap->depth_size = 2048;
		break;
	case eSHADOW_4096:
		g_dynamicShadowMap->depth_size = 4096;
		break;
	default:
		break;
	}
	g_dynamicShadowMap->RegenerateDepthTex(g_dynamicShadowMap->depth_size);

	switch (g_shadowProperties.m_shadowSplits)
	{
	case eSHADOW_1_SPLIT:
		g_dynamicShadowMap->cur_num_splits = 1;
		break;
	case eSHADOW_2_SPLITS:
		g_dynamicShadowMap->cur_num_splits = 2;
		break;
	case eSHADOW_3_SPLITS:
		g_dynamicShadowMap->cur_num_splits = 3;
		break;
	case eSHADOW_4_SPLITS:
		g_dynamicShadowMap->cur_num_splits = 4;
		break;
	default:
		break;
	}

	g_dynamicShadowMap->split_weight = g_shadowProperties.m_shadowSplitWeight;

	fread(&g_physXProperties, sizeof(CPhysXProperties), 1, filePtr);
	ResetPhysX(); //reset the physX based on the g_physXProperties information
	//temprary disable gravity to avoid falling objects while laoding the scene:
	gPhysXscene->setGravity(NxVec3(0.0f, 0.0f, 0.0f));

	fread(&g_dofProperties, sizeof(CDOFProperties), 1, filePtr);
	fread(&g_fogProperties, sizeof(CFogProperties), 1, filePtr);
	fread(&g_bloomProperties, sizeof(CBloomProperties), 1, filePtr);
	fread(&g_lightProperties, sizeof(CLightProperties), 1, filePtr);
	fread(&g_instancePrefabLODPercent, sizeof(CLODProperties), 1, filePtr);
	fread(&g_cameraProperties, sizeof(CCameraProperties), 1, filePtr);
	fread(&g_currentVSceneProperties, sizeof(CCurrentVSceneProperties), 1, filePtr);

	//fread(&g_characterBlendingProperties, sizeof(CCharacterBlendingProperties), 1, filePtr);
	fread(&g_pathProperties, sizeof(CPathProperties), 1, filePtr);
	CBool demo;
	fread(&demo, sizeof(CBool), 1, filePtr);
	if (demo)
	{
		MessageBoxA(NULL, "Thank you for your interest in Vanda Engine RTI.\nUnfortunatelly this feature is not supported in demo version. Please order Vanda Engine to run the final output.", "Warning", MB_OK | MB_ICONERROR);
		return CFalse;
	}

	CChar bannerName[MAX_NAME_SIZE];
	fread(&bannerName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

	CChar cursorName[MAX_NAME_SIZE];
	fread(&cursorName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

	fread(&g_extraTexturesNamingConventions, sizeof(CExtraTexturesNamingConventions), 1, filePtr);
	fread(&g_useGlobalAmbientColor, sizeof(CBool), 1, filePtr);
	fread(&g_globalAmbientColor, sizeof(CColor4f), 1, filePtr);
	fread(&g_sceneManagerObjectsPerSplit, sizeof(CInt), 1, filePtr);
	CFog fog;
	fog.SetColor(g_fogProperties.m_fogColor);
	fog.SetDensity(g_fogProperties.m_fogDensity);

	//read physX 
	CBool insertPhysXScene = CFalse;
	CChar strPhysXSceneName[MAX_NAME_SIZE];
	NxExtendedVec3 characterPos;
	CVec3f cameraInstancePos;
	CVec2f cameraInstancePanTilt;
	CFloat cameraInstanceZoom;
	fread(&insertPhysXScene, sizeof(CBool), 1, filePtr);
	fread(strPhysXSceneName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
	fread(&characterPos, sizeof(NxExtendedVec3), 1, filePtr);

	g_nx->gCharacterPos.x = characterPos.x;
	g_nx->gCharacterPos.y = characterPos.y;
	g_nx->gCharacterPos.z = characterPos.z;

	fread(&cameraInstancePos, sizeof(CVec3f), 1, filePtr);
	fread(&cameraInstancePanTilt, sizeof(CVec2f), 1, filePtr);
	fread(&cameraInstanceZoom, sizeof(CFloat), 1, filePtr);

	g_render.GetDefaultInstanceCamera()->MoveTransform2(cameraInstancePos.x, cameraInstancePos.y, cameraInstancePos.z);
	g_render.GetDefaultInstanceCamera()->SetPanAndTilt2(cameraInstancePanTilt.x, cameraInstancePanTilt.y);
	g_render.GetDefaultInstanceCamera()->ZoomTransform2(0.0f);
	g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetAngle(cameraInstanceZoom);
	g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetMinAngle(MIN_CAMERA_ANGLE);
	g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetMaxAngle(MAX_CAMERA_ANGLE);

	///////////////////////////////////

	if (insertPhysXScene)
	{
		//Copy this part to Win32 Project. Save functions
		//CChar temp[MAX_NAME_SIZE];
		CChar* PhysXName = GetAfterPath(strPhysXSceneName);
		CChar PhysXPath[MAX_NAME_SIZE];
		CChar g_currentVSceneNameWithoutDot[MAX_NAME_SIZE];
		Cpy(g_currentVSceneNameWithoutDot, g_currentVSceneName);
		GetWithoutDot(g_currentVSceneNameWithoutDot);
		sprintf(PhysXPath, "%s%s%s%s", "Assets/VScenes/", g_currentVSceneNameWithoutDot, "/External Physics/", PhysXName);

		if (g_nx->LoadScene(PhysXPath, NXU::FT_XML))
		{
			g_nx->SetSceneName(PhysXPath);
			//sprintf( temp, "PhysX scene '%s' imported successufully\n", strPhysXSceneName );
			//PrintInfo2( temp );
		}
		else
		{
			//sprintf( temp, "couldn't load the PhysX scene '%s'\n", strPhysXSceneName );
			//PrintInfo2( temp );
			MessageBox(NULL, _T("couldn't load the PhysX scene"), _T("VandaEngine Error"), MB_OK);

		}
	}

	g_nx->ResetCharacterPos(characterPos);

	//read prefabs here
	CUInt prefabSize;
	fread(&prefabSize, sizeof(CUInt), 1, filePtr);
	for (CUInt i = 0; i < prefabSize; i++)
	{
		//read prefab data
		CChar name[MAX_NAME_SIZE];
		CChar package_name[MAX_NAME_SIZE];
		CChar prefab_name[MAX_NAME_SIZE];
		fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(package_name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(prefab_name, sizeof(CChar), MAX_NAME_SIZE, filePtr);

		CPrefab* new_prefab = CNew(CPrefab);
		new_prefab->SetName(name);
		new_prefab->SetPackageName(package_name);
		new_prefab->SetPrefabName(prefab_name);

		g_prefab.push_back(new_prefab);

		CChar message[MAX_NAME_SIZE];
		sprintf(message, "%s%s", "Loading Prefab: ", name);
		MSG msg;
		HWND m_hwnd = NULL;
		while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate and dispatch to event queue
			DispatchMessage(&msg);
		}
		ShowLoadingScene(message);

		CUInt prefabInstanceSize;
		fread(&prefabInstanceSize, sizeof(CUInt), 1, filePtr);

		for (CUInt j = 0; j < prefabInstanceSize; j++)
		{
			//read instance data
			CInstancePrefab* new_instance_prefab = CNew(CInstancePrefab);
			g_currentInstancePrefab = new_instance_prefab;
			//fread(new_instance_prefab, sizeof(CInstancePrefab), 1, filePtr);

			CChar name[MAX_NAME_SIZE];
			CVec3f translate;
			CVec4f rotate;
			CVec3f scale;
			CBool isVisible;
			CChar script[MAX_NAME_SIZE];
			CBool isTrigger;
			CBool isControlledByPhysx;
			CBool isAnimated;
			CBool isStatic;
			CBool totalLights;
			CBool castShadow;
			CBool transformable;
			CBool selectable;

			fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
			new_instance_prefab->SetName(name);

			fread(&translate, sizeof(CVec3f), 1, filePtr);
			new_instance_prefab->SetTranslate(translate);

			fread(&rotate, sizeof(CVec4f), 1, filePtr);
			new_instance_prefab->SetRotate(rotate);

			fread(&scale, sizeof(CVec3f), 1, filePtr);
			new_instance_prefab->SetScale(scale);

			//Materials
			CBool enableMaterial;
			fread(&enableMaterial, sizeof(CBool), 1, filePtr);
			if (enableMaterial)
				new_instance_prefab->EnableMaterial();
			else
				new_instance_prefab->DisableMaterial();

			CFloat ambient[4];
			fread(ambient, sizeof(CFloat), 4, filePtr);
			new_instance_prefab->SetAmbient(ambient);

			CFloat diffuse[4];
			fread(diffuse, sizeof(CFloat), 4, filePtr);
			new_instance_prefab->SetDiffuse(diffuse);

			CFloat specular[4];
			fread(specular, sizeof(CFloat), 4, filePtr);
			new_instance_prefab->SetSpecular(specular);

			CFloat emission[4];
			fread(emission, sizeof(CFloat), 4, filePtr);
			new_instance_prefab->SetEmission(emission);

			CFloat shininess;
			fread(&shininess, sizeof(CFloat), 1, filePtr);
			new_instance_prefab->SetShininess(shininess);

			CFloat transparency;
			fread(&transparency, sizeof(CFloat), 1, filePtr);
			new_instance_prefab->SetTransparency(transparency);
			//////////

			fread(&isVisible, sizeof(CBool), 1, filePtr);

			fread(script, sizeof(CChar), MAX_NAME_SIZE, filePtr);
			new_instance_prefab->SetScript(script);
			new_instance_prefab->LoadLuaFile();

			fread(&isTrigger, sizeof(CBool), 1, filePtr);
			new_instance_prefab->SetIsTrigger(isTrigger);

			fread(&isControlledByPhysx, sizeof(CBool), 1, filePtr);
			new_instance_prefab->SetIsControlledByPhysX(isControlledByPhysx);

			fread(&isAnimated, sizeof(CBool), 1, filePtr);
			new_instance_prefab->SetIsAnimated(isAnimated);

			fread(&isStatic, sizeof(CBool), 1, filePtr);
			new_instance_prefab->SetIsStatic(isStatic);

			fread(&totalLights, sizeof(CBool), 1, filePtr);
			new_instance_prefab->SetTotalLights(totalLights);

			fread(&castShadow, sizeof(CBool), 1, filePtr);
			new_instance_prefab->SetCastShadow(castShadow);

			fread(&transformable, sizeof(CBool), 1, filePtr);
			new_instance_prefab->SetTransformable(transformable);

			fread(&selectable, sizeof(CBool), 1, filePtr);
			new_instance_prefab->SetSelectable(selectable);

			new_prefab->SetInstance(new_instance_prefab);
			new_prefab->SetCurrentInstance(new_instance_prefab);
			new_instance_prefab->SetPrefab(new_prefab);
			new_instance_prefab->SetNameIndex(); //for selection only
			new_instance_prefab->GenQueryIndex();
			new_instance_prefab->SetWater(NULL);
			g_instancePrefab.push_back(new_instance_prefab);
			Cpy(g_currentInstancePrefabName, new_instance_prefab->GetName());
			InsertPrefab(new_prefab);
			//new_instance_prefab->UpdateBoundingBox();
			//new_instance_prefab->CalculateDistance();
			//new_instance_prefab->UpdateIsStaticOrAnimated();
			new_instance_prefab->SetVisible(isVisible);
			new_instance_prefab->SetHasScript(new_prefab->GetHasScript());
			new_instance_prefab->SetScript(new_prefab->GetScript());
			new_instance_prefab->LoadLuaFile();

			CChar tempInstanceName[MAX_NAME_SIZE];
			sprintf(tempInstanceName, "%s%s%s", "\nPrefab Instance ' ", new_instance_prefab->GetName(), " ' created successfully");
		}
	}

	g_updateOctree = CTrue;

	CChar g_currentVSceneNameWithoutDot[MAX_NAME_SIZE];
	Cpy(g_currentVSceneNameWithoutDot, g_currentVSceneName);
	GetWithoutDot(g_currentVSceneNameWithoutDot);

	//load GUIs//////////////////
	CUInt guiSize;
	fread(&guiSize, sizeof(CUInt), 1, filePtr);

	for (CUInt i = 0; i < guiSize; i++)
	{
		CGUI* new_gui = CNew(CGUI);
		//save gui name
		CChar name[MAX_NAME_SIZE];
		fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);

		new_gui->SetName(name);

		CChar message[MAX_NAME_SIZE];
		sprintf(message, "%s%s", "Loading GUI: ", name);
		MSG msg;
		HWND m_hwnd = NULL;
		while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate and dispatch to event queue
			DispatchMessage(&msg);
		}
		ShowLoadingScene(message);

		CChar packageName[MAX_NAME_SIZE];
		fread(packageName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

		new_gui->SetPackageName(packageName);

		CChar guiName[MAX_NAME_SIZE];
		fread(guiName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

		new_gui->SetGUIName(guiName);

		//is gui visible?
		CBool isVisible;
		fread(&isVisible, sizeof(CBool), 1, filePtr);

		new_gui->SetVisible(isVisible);

		CChar guiPath[MAX_NAME_SIZE];
		CChar guiAndPackageName[MAX_NAME_SIZE];

		Cpy(guiAndPackageName, packageName);
		Append(guiAndPackageName, "_");
		Append(guiAndPackageName, guiName);

		Cpy(guiPath, "Assets/GUIs/");
		Append(guiPath, packageName);
		Append(guiPath, "/");
		Append(guiPath, guiName);
		Append(guiPath, "/");
		Append(guiPath, guiAndPackageName);
		Append(guiPath, ".gui");

		FILE *filePtr;
		filePtr = fopen(guiPath, "rb");
		if (!filePtr)
		{
			MessageBoxA(NULL, "Couldn't open the GUI file to load data", "Vanda Engine Error", MB_OK | MB_ICONERROR);
			return CFalse;
		}
		CUInt numButtons;
		fread(&numButtons, sizeof(CUInt), 1, filePtr);

		for (CUInt j = 0; j < numButtons; j++)
		{
			//load button information

			CChar name[MAX_NAME_SIZE];
			fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CChar packageName[MAX_NAME_SIZE];
			fread(packageName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CChar guiName[MAX_NAME_SIZE];
			fread(guiName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CVec2f pos;
			fread(&pos, sizeof(CVec2f), 1, filePtr);
			pos.x *= (CFloat)g_width;
			pos.y *= (CFloat)g_height;

			CInt size;
			fread(&size, sizeof(CInt), 1, filePtr);

			CChar mainImagePath[MAX_NAME_SIZE];
			fread(mainImagePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			//disable image
			CBool hasDisableImage;
			fread(&hasDisableImage, sizeof(CBool), 1, filePtr);

			CChar disableImagePath[MAX_NAME_SIZE];
			if (hasDisableImage)
				fread(disableImagePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			//hover image
			CBool hasHoverImage;
			fread(&hasHoverImage, sizeof(CBool), 1, filePtr);

			CChar hoverImagePath[MAX_NAME_SIZE];
			if (hasHoverImage)
				fread(hoverImagePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			//left click image
			CBool hasLeftClickImage;
			fread(&hasLeftClickImage, sizeof(CBool), 1, filePtr);

			CChar leftClickImagePath[MAX_NAME_SIZE];
			if (hasLeftClickImage)
				fread(leftClickImagePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			//right click image
			CBool hasRightClickImage;
			fread(&hasRightClickImage, sizeof(CBool), 1, filePtr);

			CChar rightClickImagePath[MAX_NAME_SIZE];
			if (hasRightClickImage)
				fread(rightClickImagePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			//left click script
			CBool hasScript;
			fread(&hasScript, sizeof(CBool), 1, filePtr);

			CChar ScriptPath[MAX_NAME_SIZE];
			if (hasScript)
				fread(ScriptPath, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CChar finalMainImagePath[MAX_NAME_SIZE];
			CChar* tempMainImagePath = GetAfterPath(mainImagePath);
			//Copy this to Win32 Project as well
			sprintf(finalMainImagePath, "%s%s%s%s%s%s", "assets/GUIs/",packageName, "/", guiName, "/Textures/Buttons/", tempMainImagePath);

			CGUIButton* guiButton = CNew(CGUIButton);

			guiButton->SetName(name);
			guiButton->SetPackageName(packageName);
			guiButton->SetGUIName(guiName);
			guiButton->SetPosition(pos);
			guiButton->SetSize(size);
			guiButton->SetMainImagePath(finalMainImagePath);
			guiButton->LoadMainImage();

			if (hasDisableImage)
			{
				CChar finalImagePath[MAX_NAME_SIZE];
				CChar* tempImagePath = GetAfterPath(disableImagePath);
				sprintf(finalImagePath, "%s%s%s%s%s%s", "assets/GUIs/",packageName, "/", guiName, "/Textures/Buttons/", tempImagePath);

				guiButton->SetDisableImagePath(finalImagePath);
				guiButton->SetHasDisableImage(CTrue);
				guiButton->LoadDisableImage();
			}
			else
			{
				guiButton->SetHasDisableImage(CFalse);
			}
			if (hasHoverImage)
			{
				CChar finalImagePath[MAX_NAME_SIZE];
				CChar* tempImagePath = GetAfterPath(hoverImagePath);
				sprintf(finalImagePath, "%s%s%s%s%s%s", "assets/GUIs/",packageName, "/", guiName, "/Textures/Buttons/", tempImagePath);

				guiButton->SetHoverImagePath(finalImagePath);
				guiButton->SetHasHoverImage(CTrue);
				guiButton->LoadHoverImage();
			}
			else
			{
				guiButton->SetHasHoverImage(CFalse);
			}
			if (hasLeftClickImage)
			{
				CChar finalImagePath[MAX_NAME_SIZE];
				CChar* tempImagePath = GetAfterPath(leftClickImagePath);
				sprintf(finalImagePath, "%s%s%s%s%s%s", "assets/GUIs/",packageName, "/", guiName, "/Textures/Buttons/", tempImagePath);

				guiButton->SetLeftClickImagePath(finalImagePath);
				guiButton->SetHasLeftClickImage(CTrue);
				guiButton->LoadLeftClickImage();
			}
			else
			{
				guiButton->SetHasLeftClickImage(CFalse);
			}
			if (hasRightClickImage)
			{
				CChar finalImagePath[MAX_NAME_SIZE];
				CChar* tempImagePath = GetAfterPath(rightClickImagePath);
				sprintf(finalImagePath, "%s%s%s%s%s%s", "assets/GUIs/",packageName, "/", guiName, "/Textures/Buttons/", tempImagePath);

				guiButton->SetRightClickImagePath(finalImagePath);
				guiButton->SetHasRightClickImage(CTrue);
				guiButton->LoadRightClickImage();
			}
			else
			{
				guiButton->SetHasRightClickImage(CFalse);
			}
			if (hasScript)
			{
				CChar finalScriptPath[MAX_NAME_SIZE];
				CChar* tempScriptPath = GetAfterPath(ScriptPath);
				sprintf(finalScriptPath, "%s%s%s%s%s%s", "assets/GUIs/",packageName, "/", guiName, "/Scripts/", tempScriptPath);

				guiButton->SetScriptPath(finalScriptPath);
				guiButton->SetHasScript(CTrue);
				guiButton->LoadLuaFile();
			}
			else
			{
				guiButton->SetHasScript(CFalse);
			}
			new_gui->AddGUIButton(guiButton);

		}

		CUInt numBackgrounds;
		fread(&numBackgrounds, sizeof(CUInt), 1, filePtr);

		for (CUInt j = 0; j < numBackgrounds; j++)
		{
			//load background information

			CChar name[MAX_NAME_SIZE];
			fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CChar packageName[MAX_NAME_SIZE];
			fread(packageName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CChar guiName[MAX_NAME_SIZE];
			fread(guiName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CVec2f pos;
			fread(&pos, sizeof(CVec2f), 1, filePtr);
			pos.x *= (CFloat)g_width;
			pos.y *= (CFloat)g_height;

			CInt size;
			fread(&size, sizeof(CInt), 1, filePtr);

			CChar imagePath[MAX_NAME_SIZE];
			fread(imagePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CChar finalImagePath[MAX_NAME_SIZE];
			CChar* tempPath = GetAfterPath(imagePath);
			//Copy this to Win32 Project as well
			sprintf(finalImagePath, "%s%s%s%s%s%s", "assets/GUIs/",packageName, "/", guiName, "/Textures/Backgrounds/", tempPath);

			CGUIBackground* guiBackground = CNew(CGUIBackground);
			guiBackground->SetName(name);
			guiBackground->SetPackageName(packageName);
			guiBackground->SetGUIName(guiName);
			guiBackground->SetPosition(pos);
			guiBackground->SetSize(size);
			guiBackground->SetImagePath(finalImagePath);
			guiBackground->LoadBackgroundImage();

			new_gui->AddGUIBackground(guiBackground);

		}

		CUInt numTexts;
		fread(&numTexts, sizeof(CUInt), 1, filePtr);

		for (CUInt j = 0; j < numTexts; j++)
		{
			//load text information

			CChar name[MAX_NAME_SIZE];
			fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CChar packageName[MAX_NAME_SIZE];
			fread(packageName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CChar guiName[MAX_NAME_SIZE];
			fread(guiName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			CVec2f pos;
			fread(&pos, sizeof(CVec2f), 1, filePtr);
			pos.x *= (CFloat)g_width;
			pos.y *= (CFloat)g_height;

			CInt size;
			fread(&size, sizeof(CInt), 1, filePtr);

			CChar text[MAX_URI_SIZE];
			fread(text, sizeof(CChar), MAX_URI_SIZE, filePtr);

			CVec3f color;
			fread(&color, sizeof(CVec3f), 1, filePtr);

			CFontType font;
			fread(&font, sizeof(CFontType), 1, filePtr);

			CGUIText* guiText = CNew(CGUIText);
			guiText->SetName(name);
			guiText->SetPackageName(packageName);
			guiText->SetGUIName(guiName);
			guiText->SetPosition(pos);
			guiText->SetSize(size);
			guiText->SetColor(color);
			guiText->SetText(text);
			guiText->SetType(font);
			guiText->SetFont();

			new_gui->AddGUIText(guiText);

		}
		fclose(filePtr);
		g_guis.push_back(new_gui);

	}
	//End of GUIs///////////////////

	//sky object

	CBool showSky;
	fread(&showSky, sizeof(CBool), 1, filePtr);

	if (showSky)
	{
		CChar message[MAX_NAME_SIZE];
		sprintf(message, "%s", "Loading sky dome");
		MSG msg;
		HWND m_hwnd = NULL;
		while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate and dispatch to event queue
			DispatchMessage(&msg);
		}
		ShowLoadingScene(message);

		CChar name[MAX_NAME_SIZE];
		CChar path[MAX_NAME_SIZE];
		CInt slices, sides;
		CFloat dampening, radius, position[3];
		CBool exponential;
		fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(path, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&slices, sizeof(CInt), 1, filePtr);
		fread(&sides, sizeof(CInt), 1, filePtr);
		fread(&radius, sizeof(CFloat), 1, filePtr);
		fread(position, sizeof(CFloat), 3, filePtr);
		fread(&dampening, sizeof(CFloat), 1, filePtr);
		fread(&exponential, sizeof(CBool), 1, filePtr);

		g_skyDome = CNew(CSkyDome);
		CChar skyPath[MAX_NAME_SIZE];
		CChar* tempPath = GetAfterPath(path);
		//Copy this to Win32 Project as well
		sprintf(skyPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Sky/", tempPath);

		g_skyDome->SetName(name);
		g_skyDome->SetPath(skyPath);
		g_skyDome->SetSlices(slices);
		g_skyDome->SetSides(sides);
		g_skyDome->SetRadius(radius);
		g_skyDome->SetPosition(position);
		g_skyDome->SetDampening(dampening);
		g_skyDome->SetExponential(exponential);
		g_skyDome->Initialize();
		g_databaseVariables.m_insertAndShowSky = CTrue;
	}
	else
		g_databaseVariables.m_insertAndShowSky = CFalse;

	//Load terrain here
	CBool showTerrain;

	fread(&showTerrain, sizeof(CBool), 1, filePtr);
	if (showTerrain)
	{
		CChar message[MAX_NAME_SIZE];
		sprintf(message, "%s", "Loading terrain");
		MSG msg;
		HWND m_hwnd = NULL;
		while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate and dispatch to event queue
			DispatchMessage(&msg);
		}
		ShowLoadingScene(message);

		CChar name[MAX_NAME_SIZE], heightMapPath[MAX_NAME_SIZE], bottomTexturePath[MAX_NAME_SIZE], bottomNormalMapPath[MAX_NAME_SIZE],
			slopeTexturePath[MAX_NAME_SIZE], slopeNormalMapPath[MAX_NAME_SIZE], topTexturePath[MAX_NAME_SIZE], topNormalPath[MAX_NAME_SIZE];

		CChar* temp_heightMapPath; CChar* temp_bottomTexturePath; CChar* temp_bottomNormalMapPath;
		CChar* temp_slopeTexturePath; CChar* temp_slopeNormalMapPath; CChar* temp_topTexturePath; CChar* temp_topNormalPath;

		CChar final_heightMapPath[MAX_NAME_SIZE], final_bottomTexturePath[MAX_NAME_SIZE], final_bottomNormalMapPath[MAX_NAME_SIZE],
			final_slopeTexturePath[MAX_NAME_SIZE], final_slopeNormalMapPath[MAX_NAME_SIZE], final_topTexturePath[MAX_NAME_SIZE], final_topNormalPath[MAX_NAME_SIZE];

		CFloat shininess, scaleHeight, scaleWidth, slopeFactor, startHeight;
		CInt smooth;
		CBool flatten;
		CFloat ambientColor[4], diffuseColor[4], specularColor[4];

		fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(heightMapPath, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(bottomTexturePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(bottomNormalMapPath, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(slopeTexturePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(slopeNormalMapPath, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(topTexturePath, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(topNormalPath, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&shininess, sizeof(CFloat), 1, filePtr);
		fread(&smooth, sizeof(CInt), 1, filePtr);
		fread(&scaleHeight, sizeof(CFloat), 1, filePtr);
		fread(&scaleWidth, sizeof(CFloat), 1, filePtr);
		fread(&slopeFactor, sizeof(CFloat), 1, filePtr);
		fread(&startHeight, sizeof(CFloat), 1, filePtr);
		fread(&flatten, sizeof(CBool), 1, filePtr);
		fread(ambientColor, sizeof(CFloat), 4, filePtr);
		fread(diffuseColor, sizeof(CFloat), 4, filePtr);
		fread(specularColor, sizeof(CFloat), 4, filePtr);

		temp_heightMapPath = GetAfterPath(heightMapPath);
		sprintf(final_heightMapPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Terrain/", temp_heightMapPath);

		temp_bottomTexturePath = GetAfterPath(bottomTexturePath);
		sprintf(final_bottomTexturePath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Terrain/", temp_bottomTexturePath);

		temp_bottomNormalMapPath = GetAfterPath(bottomNormalMapPath);
		sprintf(final_bottomNormalMapPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Terrain/", temp_bottomNormalMapPath);

		temp_slopeTexturePath = GetAfterPath(slopeTexturePath);
		sprintf(final_slopeTexturePath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Terrain/", temp_slopeTexturePath);

		temp_slopeNormalMapPath = GetAfterPath(slopeNormalMapPath);
		sprintf(final_slopeNormalMapPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Terrain/", temp_slopeNormalMapPath);

		temp_topTexturePath = GetAfterPath(topTexturePath);
		sprintf(final_topTexturePath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Terrain/", temp_topTexturePath);

		temp_topNormalPath = GetAfterPath(topNormalPath);
		sprintf(final_topNormalPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Terrain/", temp_topNormalPath);

		CChar physicsPath[MAX_NAME_SIZE];
		sprintf(physicsPath, "%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Terrain/");

		g_terrain = CNew(CTerrain);
		g_terrain->SetCookPhysicsTriangles(CFalse);
		g_terrain->SetPhysicsPath(physicsPath);
		g_terrain->SetName(name);

		g_terrain->SetHeightMapPath(final_heightMapPath);

		g_terrain->SetBottomTexturePath(final_bottomTexturePath);
		g_terrain->SetBottomNormalMapPath(final_bottomNormalMapPath);

		g_terrain->SetSlopeTexturePath(final_slopeTexturePath);
		g_terrain->SetSlopeNormalMapPath(final_slopeNormalMapPath);

		g_terrain->SetTopTexturePath(final_topTexturePath);
		g_terrain->SetTopNormalMapPath(final_topNormalPath);

		g_terrain->SetShininess(shininess);
		g_terrain->SetSmooth(smooth);
		g_terrain->SetScaleHeight(scaleHeight);
		g_terrain->SetScaleWidth(scaleWidth);
		g_terrain->SetSlopeFactor(slopeFactor);
		g_terrain->SetTopStartHeight(startHeight);
		g_terrain->SetFlatten(flatten);

		g_terrain->SetAmbientColor(ambientColor);
		g_terrain->SetDiffuseColor(diffuseColor);
		g_terrain->SetSpecularColor(specularColor);

		g_terrain->Initialize();
		g_databaseVariables.m_insertAndShowTerrain = CTrue;
	}
	else
		g_databaseVariables.m_insertAndShowTerrain = CFalse;

	//Load water here
	CInt tempWaterCount, tempInstancePrefabWaterCount;
	CChar strNormalMap[MAX_NAME_SIZE];
	CChar strDuDvMap[MAX_NAME_SIZE];
	CChar strWaterName[MAX_NAME_SIZE];
	CFloat waterPos[3];
	CFloat waterLightPos[3];
	CFloat waterHeight, waterSpeed, waterScale, waterUV;
	CBool waterVisible;

	fread(&tempWaterCount, sizeof(CInt), 1, filePtr);
	for (CInt i = 0; i < tempWaterCount; i++)
	{
		CWater* water = new CWater;

		fread(strWaterName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(strNormalMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(strDuDvMap, sizeof(CChar), MAX_NAME_SIZE, filePtr);

		CChar message[MAX_NAME_SIZE];
		sprintf(message, "%s%s", "Loading water: ", strWaterName);
		MSG msg;
		HWND m_hwnd = NULL;
		while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate and dispatch to event queue
			DispatchMessage(&msg);
		}
		ShowLoadingScene(message);

		fread(waterPos, sizeof(CFloat), 3, filePtr);
		fread(waterLightPos, sizeof(CFloat), 3, filePtr);
		fread(&waterHeight, sizeof(CFloat), 1, filePtr);
		fread(&waterSpeed, sizeof(CFloat), 1, filePtr);
		fread(&waterScale, sizeof(CFloat), 1, filePtr);
		fread(&waterUV, sizeof(CFloat), 1, filePtr);
		fread(&waterVisible, sizeof(CBool), 1, filePtr);

		fread(&tempInstancePrefabWaterCount, sizeof(CInt), 1, filePtr);
		for (CInt j = 0; j < tempInstancePrefabWaterCount; j++)
		{
			CChar instanceName[MAX_NAME_SIZE];
			fread(instanceName, sizeof(CChar), MAX_NAME_SIZE, filePtr);

			for (CUInt k = 0; k < g_instancePrefab.size(); k++)
			{
				if (Cmp(g_instancePrefab[k]->GetName(), instanceName))
				{
					water->m_instancePrefab.push_back(g_instancePrefab[k]);
					g_instancePrefab[k]->SetWater(water);
					g_instancePrefab[k]->UpdateBoundingBoxForWater(waterHeight);
				}
			}
		}
		CChar g_currentVSceneNameWithoutDot[MAX_NAME_SIZE];
		Cpy(g_currentVSceneNameWithoutDot, g_currentVSceneName);
		GetWithoutDot(g_currentVSceneNameWithoutDot);

		CChar dudvPath[MAX_NAME_SIZE];
		CChar* DuDvAfterPath = GetAfterPath(strDuDvMap);

		//Copy this to Win32 Project as well
		sprintf(dudvPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Waters/", DuDvAfterPath);

		CChar normalPath[MAX_NAME_SIZE];
		CChar* normalAfterPath = GetAfterPath(strNormalMap);

		//Copy this to Win32 Project as well
		sprintf(normalPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Waters/", normalAfterPath);

		water->SetName(strWaterName);
		water->SetDuDvMap(dudvPath);
		water->SetNormalMap(normalPath);
		water->SetHeight(waterHeight);
		water->SetSpeed(waterSpeed);
		water->SetScale(waterScale);
		water->SetUV(waterUV);
		water->SetPos(waterPos);
		water->SetVisible(waterVisible);
		water->SetLightPos(waterLightPos);
		water->CreateRenderTexture(g_waterTextureSize, 3, GL_RGB, WATER_REFLECTION_ID);
		water->CreateRenderTexture(g_waterTextureSize, 3, GL_RGB, WATER_REFRACTION_ID);
		water->CreateRenderTexture(g_waterTextureSize, 1, GL_DEPTH_COMPONENT, WATER_DEPTH_ID);
		water->SetSideVertexPositions();
		g_engineWaters.push_back(water);

	}
	//Engine Lights
	CInt tempLightCount;
	fread(&tempLightCount, sizeof(CInt), 1, filePtr);
	CFloat m_fLightPos[4]; 	CFloat m_fConstantAttenuation; CFloat m_fLinearAttenuation; CFloat m_fQuadAttenuation; CFloat m_fSpotCuttoff;
	CFloat m_fSpotDirection[4]; CFloat m_fSpotExponent; CFloat m_fShininess; CFloat m_fAmbientColor[4]; CFloat m_fDiffuseColor[4];
	CFloat m_fSpecularColor[4]; CLightType m_lightType;
	CChar lightName[MAX_NAME_SIZE];
	for (CInt i = 0; i < tempLightCount; i++)
	{
		fread(lightName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(m_fLightPos, sizeof(CFloat), 4, filePtr);
		fread(&m_fConstantAttenuation, sizeof(CFloat), 1, filePtr);
		fread(&m_fLinearAttenuation, sizeof(CFloat), 1, filePtr);
		fread(&m_fQuadAttenuation, sizeof(CFloat), 1, filePtr);
		fread(&m_fShininess, sizeof(CFloat), 1, filePtr);
		fread(m_fAmbientColor, sizeof(CFloat), 4, filePtr);
		fread(m_fDiffuseColor, sizeof(CFloat), 4, filePtr);
		fread(m_fSpecularColor, sizeof(CFloat), 4, filePtr);
		fread(&m_lightType, sizeof(CLightType), 1, filePtr);
		if (m_lightType == eLIGHTTYPE_SPOT)
		{
			fread(&m_fSpotCuttoff, sizeof(CFloat), 1, filePtr);
			fread(m_fSpotDirection, sizeof(CFloat), 4, filePtr);
			fread(&m_fSpotExponent, sizeof(CFloat), 1, filePtr);
		}
		CInstanceLight* instance_light = new CInstanceLight();
		CLight* abstract_light = new CLight();

		instance_light->m_parent = NULL; //Not using COLLADA parent
		instance_light->m_abstractLight = abstract_light;
		abstract_light->SetName(lightName);
		abstract_light->SetType(m_lightType);
		switch (m_lightType)
		{
		case eLIGHTTYPE_SPOT:
			instance_light->m_abstractLight->SetSpotCutoff(m_fSpotCuttoff);
			instance_light->m_abstractLight->SetSpotExponent(m_fSpotExponent);
			instance_light->m_abstractLight->SetSpotDirection(m_fSpotDirection);
			break;
		}

		abstract_light->SetAmbient(m_fAmbientColor);
		abstract_light->SetDiffuse(m_fDiffuseColor);
		abstract_light->SetSpecular(m_fSpecularColor);
		abstract_light->SetPosition(m_fLightPos);
		abstract_light->SetShininess(m_fShininess);
		abstract_light->SetConstantAttenuation(m_fConstantAttenuation);
		abstract_light->SetLinearAttenuation(m_fLinearAttenuation);
		abstract_light->SetQuadraticAttenuation(m_fQuadAttenuation);

		instance_light->SetIndex();

		g_engineLights.push_back(instance_light);

	}

	//Engine Cameras
	CInt tempCameraCount;
	fread(&tempCameraCount, sizeof(CInt), 1, filePtr);

	for (CInt i = 0; i < tempCameraCount; i++)
	{
		CChar cameraName[MAX_NAME_SIZE];
		CFloat pos[3], pan, tilt, fov, ncp, fcp;

		fread(cameraName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(pos, sizeof(CFloat), 3, filePtr);
		fread(&pan, sizeof(CFloat), 1, filePtr);
		fread(&tilt, sizeof(CFloat), 1, filePtr);
		fread(&fov, sizeof(CFloat), 1, filePtr);
		fread(&ncp, sizeof(CFloat), 1, filePtr);
		fread(&fcp, sizeof(CFloat), 1, filePtr);

		CInstanceCamera* instance_camera = new CInstanceCamera();
		CCamera* abstract_camera = new CCamera();
		CNode* parent = CNew(CNode);
		instance_camera->m_parent = parent;
		instance_camera->m_abstractCamera = abstract_camera;
		abstract_camera->SetName(cameraName);

		CVec3f vec_pos(pos[0], pos[1], pos[2]);
		instance_camera->SetPos(vec_pos);
		instance_camera->SetPan(pan);
		instance_camera->SetTilt(tilt);
		instance_camera->m_abstractCamera->SetAngle(fov);
		instance_camera->m_abstractCamera->SetMinAngle(MIN_CAMERA_ANGLE);
		instance_camera->m_abstractCamera->SetMaxAngle(MAX_CAMERA_ANGLE);
		instance_camera->SetNCP(ncp);
		instance_camera->SetFCP(fcp);

		instance_camera->SetIndex();

		instance_camera->MoveTransform2(vec_pos.x, vec_pos.y, vec_pos.z);
		instance_camera->SetPanAndTilt2(pan, tilt);
		instance_camera->ZoomTransform2(0.0f);

		g_engineCameraInstances.push_back(instance_camera);
	}

	//static sounds
	CInt tempStaticSoundCount;
	fread(&tempStaticSoundCount, sizeof(CInt), 1, filePtr);
	CFloat staticSoundMaxDistance, staticSoundPitch, staticSoundReferenceDistance, staticSoundRolloff, staticSoundVolume;
	CFloat staticSoundPos[3];
	CBool play, loop;

	CChar name[MAX_NAME_SIZE], path[MAX_NAME_SIZE];
	for (CInt i = 0; i < tempStaticSoundCount; i++)
	{
		fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);

		CChar message[MAX_NAME_SIZE];
		sprintf(message, "%s%s", "Loading sound: ", name);
		MSG msg;
		HWND m_hwnd = NULL;
		while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate and dispatch to event queue
			DispatchMessage(&msg);
		}
		ShowLoadingScene(message);

		fread(path, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&loop, sizeof(CBool), 1, filePtr);
		fread(&staticSoundMaxDistance, sizeof(CFloat), 1, filePtr);
		fread(&staticSoundPitch, sizeof(CFloat), 1, filePtr);
		fread(&play, sizeof(CBool), 1, filePtr);
		fread(staticSoundPos, sizeof(CFloat), 3, filePtr);
		fread(&staticSoundReferenceDistance, sizeof(CFloat), 1, filePtr);
		fread(&staticSoundRolloff, sizeof(CFloat), 1, filePtr);
		fread(&staticSoundVolume, sizeof(CFloat), 1, filePtr);

		CChar StaticSoundPath[MAX_NAME_SIZE];
		CChar* StaticSoundName = GetAfterPath(path);
		//Copy this to Win32 Project as well
		sprintf(StaticSoundPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Sounds/Static/", StaticSoundName);

		COpenALSoundBuffer* m_staticSoundBuffer = GetSoundBuffer(GetAfterPath(StaticSoundPath));

		if (m_staticSoundBuffer == NULL)
		{
			m_staticSoundBuffer = CNew(COpenALSoundBuffer);

			if (!m_staticSoundBuffer->LoadOggVorbisFromFile(StaticSoundPath))
				MessageBoxA(NULL, "Couldn't load the static sound", "Report", MB_OK);

			// all of the buffers are loaded from one location and may be shared.
			//however, I continue to create the sound if it fails to load

			m_staticSoundBuffer->SetName(StaticSoundPath);
			g_soundBuffers.push_back(m_staticSoundBuffer);
		}
		COpenALSoundSource* m_staticSoundSource = CNew(COpenALSoundSource);
		CStaticSound* m_staticSound = CNew(CStaticSound);

		m_staticSoundSource->BindSoundBuffer(*m_staticSoundBuffer);
		m_staticSoundSource->SetLooping(loop);
		m_staticSoundSource->SetPitch(staticSoundPitch);

		//m_staticSoundSource->SetVolume( staticSoundVolume );
		m_staticSoundSource->SetReferenceDistance(staticSoundReferenceDistance);
		m_staticSoundSource->SetMaxDistance(staticSoundMaxDistance);
		m_staticSoundSource->SetRolloff(staticSoundRolloff);
		m_staticSoundSource->SetSoundPosition(staticSoundPos);

		m_staticSound->SetName(name);
		m_staticSound->SetPath(StaticSoundPath);
		m_staticSound->SetPosition(staticSoundPos);
		m_staticSound->SetLoop(loop);
		m_staticSound->SetMaxDistance(staticSoundMaxDistance);
		m_staticSound->SetPitch(staticSoundPitch);
		m_staticSound->SetPlay(play);
		m_staticSound->SetRefrenceDistance(staticSoundReferenceDistance);
		m_staticSound->SetRolloff(staticSoundRolloff);
		m_staticSound->SetSoundSource(m_staticSoundSource);
		m_staticSound->SetSoundBuffer(m_staticSoundBuffer);
		m_staticSound->SetIndex();
		g_engineStaticSounds.push_back(m_staticSound);
		if (play)
			m_soundSystem->PlayALSound(*m_staticSoundSource);

	}

	//Ambient Sound
	CBool insertAmbientSound = CFalse;
	CChar strAmbientSoundName[MAX_NAME_SIZE];
	CChar strAmbientSoundPath[MAX_NAME_SIZE];
	CFloat volume, pitch;
	fread(&insertAmbientSound, sizeof(CBool), 1, filePtr);

	if (insertAmbientSound)
	{
		fread(strAmbientSoundName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(strAmbientSoundPath, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&volume, sizeof(CFloat), 1, filePtr);
		fread(&pitch, sizeof(CFloat), 1, filePtr);
	}
	CChar AmbientSoundPath[MAX_NAME_SIZE];
	CChar* AmbientSoundName = GetAfterPath(strAmbientSoundPath);
	//Copy this to Win32 Project as well
	sprintf(AmbientSoundPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Sounds/Ambient/", AmbientSoundName);

	CPrefab* box = CNew(CPrefab);
	CChar pr_name[MAX_NAME_SIZE];
	CChar package_name[MAX_NAME_SIZE];
	CChar prefab_name[MAX_NAME_SIZE];
	Cpy(package_name, "Vanda_Basics");
	Cpy(prefab_name, "Box_Trigger");
	sprintf(pr_name, "%s%s%s", package_name, "_", prefab_name);

	box->SetName(pr_name);
	box->SetPackageName(package_name);
	box->SetPrefabName(prefab_name);

	g_prefab.push_back(box);

	CPrefab* sphere = CNew(CPrefab);
	CChar pr_name2[MAX_NAME_SIZE];
	CChar package_name2[MAX_NAME_SIZE];
	CChar prefab_name2[MAX_NAME_SIZE];
	Cpy(package_name2, "Vanda_Basics");
	Cpy(prefab_name2, "Sphere_Trigger");
	sprintf(pr_name2, "%s%s%s", package_name2, "_", prefab_name2);

	sphere->SetName(pr_name2);
	sphere->SetPackageName(package_name2);
	sphere->SetPrefabName(prefab_name2);

	g_prefab.push_back(sphere);

	CInt triggerSize;
	fread(&triggerSize, sizeof(CInt), 1, filePtr);
	for (CInt i = 0; i < triggerSize; i++)
	{
		CChar trigger_name[MAX_NAME_SIZE];
		CChar instance_name[MAX_NAME_SIZE];
		CTriggerType type;
		CVec3f translation;
		CVec4f rotation;
		CVec3f scaling;
		CChar m_script[MAX_NAME_SIZE];
		CBool m_hasScript;
		fread(trigger_name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&type, sizeof(CTriggerType), 1, filePtr);

		fread(instance_name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&translation, sizeof(CVec3f), 1, filePtr);
		fread(&rotation, sizeof(CVec4f), 1, filePtr);
		fread(&scaling, sizeof(CVec3f), 1, filePtr);
		fread(m_script, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&m_hasScript, sizeof(CBool), 1, filePtr);

		//Copy this to Win32 Project as well
		CChar trimmed_script[MAX_NAME_SIZE];

		//read prefab data

		CTrigger* new_trigger = CNew(CTrigger);
		CPrefab* new_prefab;
		if (type == eTRIGGER_BOX)
		{
			new_trigger->SetTriggerType(eTRIGGER_BOX);
			new_prefab = box;
		}
		else
		{
			new_trigger->SetTriggerType(eTRIGGER_SPHERE);
			new_prefab = sphere;
		}

		new_trigger->SetName(trigger_name);

		CInstancePrefab* new_instance_prefab = CNew(CInstancePrefab);
		g_currentInstancePrefab = new_instance_prefab;
		new_prefab->SetInstance(new_instance_prefab);
		new_prefab->SetCurrentInstance(new_instance_prefab);
		new_instance_prefab->SetPrefab(new_prefab);
		new_instance_prefab->SetName(instance_name);
		new_instance_prefab->SetNameIndex(); //for selection only
		new_instance_prefab->GenQueryIndex();
		new_instance_prefab->SetWater(NULL);
		g_instancePrefab.push_back(new_instance_prefab);
		Cpy(g_currentInstancePrefabName, new_instance_prefab->GetName());
		InsertPrefab(new_prefab);
		new_instance_prefab->SetHasScript(new_prefab->GetHasScript());
		new_instance_prefab->SetScript(new_prefab->GetScript());
		new_instance_prefab->LoadLuaFile();

		new_trigger->SetInstancePrefab(g_instancePrefab[g_instancePrefab.size() - 1]); //last element
		new_trigger->GetInstancePrefab()->SetIsTrigger(CTrue);

		new_trigger->GetInstancePrefab()->SetTranslate(translation);
		new_trigger->GetInstancePrefab()->SetRotate(rotation);
		new_trigger->GetInstancePrefab()->SetScale(scaling);
		new_trigger->GetInstancePrefab()->UpdateBoundingBox();
		new_trigger->GetInstancePrefab()->CalculateDistance();
		//new_trigger->GetInstancePrefab()->UpdateIsStaticOrAnimated();

		CChar* scriptAfterPath = GetAfterPath(m_script);
		sprintf(trimmed_script, "%s%s%s%s/%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Script/Triggers/", new_trigger->GetInstancePrefab()->GetName(), scriptAfterPath);

		new_trigger->SetScript(trimmed_script);
		new_trigger->SetHasScript(m_hasScript);
		new_trigger->LoadLuaFile();

		g_triggers.push_back(new_trigger);
	}
	CBool hasCharacter;
	fread(&hasCharacter, sizeof(CBool), 1, filePtr);
	g_databaseVariables.m_insertCharacter = hasCharacter;

	if (hasCharacter)
	{
		CChar Message[MAX_NAME_SIZE];
		sprintf(Message, "%s", "Loading main character");
		MSG lpMsg;
		HWND hwnd = NULL;
		while (PeekMessage(&lpMsg, hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&lpMsg);		// translate and dispatch to event queue
			DispatchMessage(&lpMsg);
		}
		ShowLoadingScene(Message);

		if (!g_mainCharacter)
			g_mainCharacter = CNew(CMainCharacter);

		CPhysXCameraType type;
		CChar name[MAX_NAME_SIZE];
		CChar packageName[MAX_NAME_SIZE];
		CChar prefabName[MAX_NAME_SIZE];
		fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(packageName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(prefabName, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&type, sizeof(CTriggerType), 1, filePtr);

		g_mainCharacter->SetName(name);
		g_mainCharacter->SetPackageName(packageName);
		g_mainCharacter->SetPrefabName(prefabName);
		g_mainCharacter->SetCameraType(type);

		CChar instance_name[MAX_NAME_SIZE];
		CVec3f translation;
		CVec4f rotation;
		CVec3f scaling;
		CFloat delayIdle;
		//save instance data
		fread(instance_name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(&translation, sizeof(CVec3f), 1, filePtr);
		fread(&rotation, sizeof(CVec4f), 1, filePtr);
		fread(&scaling, sizeof(CVec3f), 1, filePtr);
		fread(&delayIdle, sizeof(CFloat), 1, filePtr);
		g_mainCharacter->SetIdleDelayForRandomPlay(delayIdle);

		CChar strWalkSoundPath[MAX_URI_SIZE];
		CChar strRunSoundPath[MAX_URI_SIZE];
		CChar strJumpSoundPath[MAX_URI_SIZE];

		fread(strWalkSoundPath, sizeof(CChar), MAX_URI_SIZE, filePtr);
		fread(strRunSoundPath, sizeof(CChar), MAX_URI_SIZE, filePtr);
		fread(strJumpSoundPath, sizeof(CChar), MAX_URI_SIZE, filePtr);

		//walk sound
		CChar walkSoundPath[MAX_NAME_SIZE];
		CChar* walkSoundName = GetAfterPath(strWalkSoundPath);
		//Copy this to Win32 Project as well
		sprintf(walkSoundPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Character/", walkSoundName);
		g_mainCharacter->SetWalkSoundPath(walkSoundPath);

		CChar runSoundPath[MAX_NAME_SIZE];
		CChar* runSoundName = GetAfterPath(strRunSoundPath);
		//Copy this to Win32 Project as well
		sprintf(runSoundPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Character/", runSoundName);
		g_mainCharacter->SetRunSoundPath(runSoundPath);

		CChar jumpSoundPath[MAX_NAME_SIZE];
		CChar* jumpSoundName = GetAfterPath(strJumpSoundPath);
		//Copy this to Win32 Project as well
		sprintf(jumpSoundPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Character/", jumpSoundName);
		g_mainCharacter->SetJumpSoundPath(jumpSoundPath);

		g_mainCharacter->SetWalkSound();
		g_mainCharacter->SetRunSound();
		g_mainCharacter->SetJumpSound();

		CPrefab* new_prefab = CNew(CPrefab);

		CChar pr_name[MAX_NAME_SIZE];
		sprintf(pr_name, "%s%s%s", packageName, "_", prefabName);

		new_prefab->SetName(pr_name);
		new_prefab->SetPackageName(packageName);
		new_prefab->SetPrefabName(prefabName);

		g_prefab.push_back(new_prefab);

		CInstancePrefab* new_instance_prefab = CNew(CInstancePrefab);
		g_currentInstancePrefab = new_instance_prefab;
		new_prefab->SetInstance(new_instance_prefab);
		new_prefab->SetCurrentInstance(new_instance_prefab);
		new_instance_prefab->SetPrefab(new_prefab);
		new_instance_prefab->SetName(instance_name);
		new_instance_prefab->SetNameIndex(); //for selection only
		new_instance_prefab->GenQueryIndex();
		new_instance_prefab->SetWater(NULL);
		g_instancePrefab.push_back(new_instance_prefab);
		Cpy(g_currentInstancePrefabName, new_instance_prefab->GetName());
		InsertPrefab(new_prefab);
		new_instance_prefab->SetHasScript(new_prefab->GetHasScript());
		new_instance_prefab->SetScript(new_prefab->GetScript());
		new_instance_prefab->LoadLuaFile();

		g_instancePrefab[g_instancePrefab.size() - 1]->SetName("VANDA_MAIN_CHARACTER");

		g_mainCharacter->SetInstancePrefab(g_instancePrefab[g_instancePrefab.size() - 1]); //last element

		g_mainCharacter->GetInstancePrefab()->SetTranslate(translation);
		g_mainCharacter->GetInstancePrefab()->SetRotate(rotation);
		g_mainCharacter->GetInstancePrefab()->SetScale(scaling);
		g_mainCharacter->GetInstancePrefab()->UpdateBoundingBox();
		g_mainCharacter->GetInstancePrefab()->CalculateDistance();
		//g_mainCharacter->GetInstancePrefab()->UpdateIsStaticOrAnimated();
		g_mainCharacter->SetCurrentRotation(rotation.y);
		g_mainCharacter->SetPosition(translation);
		g_camera->m_perspectiveCameraYaw = NxMath::degToRad(rotation.y) + NxMath::degToRad(180.f);


		g_main->RenderCharacter(CFalse);
		//save main actions
		CUInt idleSize;
		CUInt walkSize;
		CUInt runSize;
		CUInt jumpSize;
		std::vector<std::string> idleName;
		std::vector<std::string> walkName;
		std::vector<std::string> jumpName;
		std::vector<std::string> runName;

		fread(&idleSize, sizeof(CUInt), 1, filePtr);
		for (CUInt i = 0; i < idleSize; i++)
		{
			CChar name[MAX_NAME_SIZE];
			fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
			idleName.push_back(name);
		}
		g_mainCharacter->SetIdleName(idleName);

		fread(&walkSize, sizeof(CUInt), 1, filePtr);
		for (CUInt i = 0; i < walkSize; i++)
		{
			CChar name[MAX_NAME_SIZE];
			fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
			walkName.push_back(name);
		}
		g_mainCharacter->SetWalkName(walkName);

		fread(&jumpSize, sizeof(CUInt), 1, filePtr);
		for (CUInt i = 0; i < jumpSize; i++)
		{
			CChar name[MAX_NAME_SIZE];
			fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
			jumpName.push_back(name);
		}
		g_mainCharacter->SetJumpName(jumpName);

		fread(&runSize, sizeof(CUInt), 1, filePtr);
		for (CUInt i = 0; i < runSize; i++)
		{
			CChar name[MAX_NAME_SIZE];
			fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
			runName.push_back(name);
		}
		g_mainCharacter->SetRunName(runName);
		std::vector<std::string> idleName1 = g_mainCharacter->GetIdleName();
		std::vector<std::string> walkName1 = g_mainCharacter->GetWalkName();
		std::vector<std::string> jumpName1 = g_mainCharacter->GetJumpName();
		std::vector<std::string> runName1 = g_mainCharacter->GetRunName();
		g_main->ManageCharacterBlends("idle", (CChar*)idleName1[0].c_str());

		g_main->m_characterRotationTransition = CTrue;
		g_mainCharacter->SetCurrentRotation(g_mainCharacter->GetInstancePrefab()->GetRotate().y);

	}
	GLdouble angle;
	GLdouble minAngle;
	GLdouble maxAngle;

	fread(&angle, sizeof(GLdouble), 1, filePtr);
	fread(&minAngle, sizeof(GLdouble), 1, filePtr);
	fread(&maxAngle, sizeof(GLdouble), 1, filePtr);

	g_camera->m_cameraManager->SetAngle(angle);
	g_camera->m_cameraManager->SetDefaultAngle(angle);
	g_camera->m_cameraManager->SetMinAngle(minAngle);
	g_camera->m_cameraManager->SetMaxAngle(maxAngle);

	fread(&g_camera->m_perspectiveCameraTilt, sizeof(CFloat), 1, filePtr);
	fread(&g_camera->m_perspectiveCameraMinTilt, sizeof(CFloat), 1, filePtr);
	fread(&g_camera->m_perspectiveCameraMaxTilt, sizeof(CFloat), 1, filePtr);

	g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraTilt;

	//startup object
	CBool insertStartup;
	fread(&insertStartup, sizeof(CBool), 1, filePtr);
	if (insertStartup)
	{
		CChar message[MAX_NAME_SIZE];
		sprintf(message, "%s", "Loading startup scripts");
		MSG msg;
		HWND m_hwnd = NULL;
		while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate and dispatch to event queue
			DispatchMessage(&msg);
		}
		ShowLoadingScene(message);

		CChar name[MAX_NAME_SIZE];
		CChar path[MAX_URI_SIZE];

		fread(name, sizeof(CChar), MAX_NAME_SIZE, filePtr);
		fread(path, sizeof(CChar), MAX_URI_SIZE, filePtr);

		CChar* tempPath = GetAfterPath(path);
		//Copy this to Win32 Project as well
		CChar newPath[MAX_URI_SIZE];
		sprintf(newPath, "%s%s%s%s", "Assets/VScenes/", g_currentVSceneNameWithoutDot, "/Startup/", tempPath);
		if (!g_startup)
			g_startup = CNew(CStartUp);
		g_startup->SetName(name);
		g_startup->SetScriptPath(newPath);
		g_startup->SetUpdateScript(CFalse);
		g_startup->LoadLuaFile();

		g_databaseVariables.m_insertStartup = CTrue;

		g_startup->InitScript();
	}
	else
	{
		g_databaseVariables.m_insertStartup = CFalse;
	}

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		CScene* scene = NULL;
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j))
			{
				scene = g_instancePrefab[i]->GetScene(j);
				if (scene)
				{
					g_render.SetScene(scene);
					g_render.GetScene()->Update(0.001f, CTrue);
					g_instancePrefab[i]->SetSceneVisible(j, CTrue);
				}
			}
		}
		g_instancePrefab[i]->UpdateBoundingBox(CTrue);//init
		g_instancePrefab[i]->UpdateIsStaticOrAnimated();//init
	}

	CChar message[MAX_NAME_SIZE];
	sprintf(message, "%s", "Generating physics");
	MSG msg;
	HWND m_hwnd = NULL;
	while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);		// translate and dispatch to event queue
		DispatchMessage(&msg);
	}
	ShowLoadingScene(message);

	//generate physX colliders
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		g_currentInstancePrefab = g_instancePrefab[i];

		CScene* scene = NULL;

		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j))
			{
				scene = g_instancePrefab[i]->GetScene(j);
				if (scene)
				{
					for (CUInt k = 0; k < scene->m_instanceGeometries.size(); k++)
					{
						if (scene->m_instanceGeometries[k]->m_hasPhysX && scene->m_controllers.size())
						{
							scene->GeneratePhysX(scene->m_instanceGeometries[k]->m_lodAlgorithm, scene->m_instanceGeometries[k]->m_physXDensity, scene->m_instanceGeometries[k]->m_physXPercentage, scene->m_instanceGeometries[k]->m_isTrigger, scene->m_instanceGeometries[k]->m_isInvisible, scene->m_instanceGeometries[k], CFalse, g_currentInstancePrefab);
						}
						else if (scene->m_instanceGeometries[k]->m_hasPhysX)
						{
							scene->GeneratePhysX(scene->m_instanceGeometries[k]->m_lodAlgorithm, scene->m_instanceGeometries[k]->m_physXDensity, scene->m_instanceGeometries[k]->m_physXPercentage, scene->m_instanceGeometries[k]->m_isTrigger, scene->m_instanceGeometries[k]->m_isInvisible, scene->m_instanceGeometries[k], CFalse, NULL);
						}
					}
					scene->m_update = CFalse;
				}
			}
		}
		if (g_currentInstancePrefab->GetHasCollider())
		{
			scene = g_instancePrefab[i]->GetScene(3);
			if (scene)
			{
				for (CUInt k = 0; k < scene->m_instanceGeometries.size(); k++)
				{
					if (scene->m_instanceGeometries[k]->m_hasPhysX && scene->m_controllers.size())
					{
						scene->GeneratePhysX(scene->m_instanceGeometries[k]->m_lodAlgorithm, scene->m_instanceGeometries[k]->m_physXDensity, scene->m_instanceGeometries[k]->m_physXPercentage, scene->m_instanceGeometries[k]->m_isTrigger, scene->m_instanceGeometries[k]->m_isInvisible, scene->m_instanceGeometries[k], CFalse, g_currentInstancePrefab);
					}
					else if (scene->m_instanceGeometries[k]->m_hasPhysX)
					{
						scene->GeneratePhysX(scene->m_instanceGeometries[k]->m_lodAlgorithm, scene->m_instanceGeometries[k]->m_physXDensity, scene->m_instanceGeometries[k]->m_physXPercentage, scene->m_instanceGeometries[k]->m_isTrigger, scene->m_instanceGeometries[k]->m_isInvisible, scene->m_instanceGeometries[k], CFalse, NULL);
					}
				}
				scene->m_update = CFalse;
			}
		}
		g_instancePrefab[i]->InitScript();
	}

	if (g_physXProperties.m_bGroundPlane)
	{
		NxVec3 rot0(0, 0, 0);
		NxVec3 rot1(0, 0, 0);
		NxVec3 rot2(0, 0, 0);
		NxMat33 rot(rot0, rot1, rot2);
		g_nx->m_groundBox = g_nx->CreateBox(NxVec3(0.0f, g_physXProperties.m_fGroundHeight, 0.0f), NxVec3(2000.0f, 0.1f, 2000.0f), 0, rot, NULL, CFalse, CFalse);
	}

	//load the ambient sound if it exists
	if (insertAmbientSound)
	{
		CChar message[MAX_NAME_SIZE];
		sprintf(message, "%s%s", "Loading ambient sound: ", strAmbientSoundName);
		MSG msg;
		HWND m_hwnd = NULL;
		while (PeekMessage(&msg, m_hwnd, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate and dispatch to event queue
			DispatchMessage(&msg);
		}
		ShowLoadingScene(message);

		CDelete(m_ambientSound);
		COpenALSoundSource* m_ambientSoundSource = CNew(COpenALSoundSource);
		COpenALSoundBuffer* m_ambientSoundBuffer = CNew(COpenALSoundBuffer);

		//Initialize ambient sound here
		// Velocity of the source sound.
		m_ambientSoundBuffer->LoadOggVorbisFromFile(AmbientSoundPath);
		m_ambientSoundSource->BindSoundBuffer(*m_ambientSoundBuffer);

		m_ambientSoundSource->SetLooping(true);
		m_ambientSoundSource->SetPitch(pitch);
		m_ambientSoundSource->SetVolume(volume);
		m_soundSystem->PlayALSound(*m_ambientSoundSource);

		m_ambientSound = CNew(CAmbientSound);
		m_ambientSound->SetSoundSource(m_ambientSoundSource);
		m_ambientSound->SetSoundBuffer(m_ambientSoundBuffer);
		m_ambientSound->SetName(strAmbientSoundName);
		m_ambientSound->SetPath(AmbientSoundPath);
		m_ambientSound->SetVolume(volume);
		m_ambientSound->SetPitch(pitch);

		//CChar temp[MAX_NAME_SIZE];
		//sprintf( temp, "ambient sound '%s' loaded successfully\n", strAmbientSoundPath );
		//PrintInfo2( temp );

		g_databaseVariables.m_playAmbientSound = CTrue;
		g_databaseVariables.m_insertAmbientSound = CTrue;

	}

	//if( g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX )
	//{

	//g_nx->gControllers->reportSceneChanged();
	//gPhysXscene->simulate(1.0f / 60.0f/*elapsedTime*/);
	//gPhysXscene->flushStream();
	//gPhysXscene->fetchResults(NX_ALL_FINISHED, true);

	//}
	g_selectedName = -1;

	fclose( filePtr );

	//if (g_currentVSceneProperties.m_isMenu)
	//{
		//Load Cursor Icon
		CChar cursorPath[MAX_NAME_SIZE];
		CChar* tempCursorPath = GetAfterPath(cursorName);
		//Copy this to Win32 Project as well
		sprintf(cursorPath, "%s%s%s%s", "assets/vscenes/", g_currentVSceneNameWithoutDot, "/Cursor/", tempCursorPath);
		GenerateMenuCursorTexture(cursorPath);
	//}

	//m_particleSystem = CNew( CParticleSystem( 1000, "p1" ) );
	m_dof.InitFBOs( g_width, g_height );

	RenderQueries(CTrue);
	CalculateDistnces();
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		g_instancePrefab[i]->SetLightCooked(CFalse);
	}
	ResetTimer();

	g_clickedOpen = CFalse;
	return CTrue;

}

COpenALSoundBuffer *CMain::GetSoundBuffer( const CChar * name )
{
	if (name == NULL) return NULL;
	for(CUInt i=0; i<g_soundBuffers.size(); i++)
	{
		if ( ICmp(GetAfterPath(g_soundBuffers[i]->GetName()), name ) )
			return g_soundBuffers[i];
	}
	return NULL;
}

CVoid CMain::InitPrefabSelection(CDouble mouseXPos, CDouble mouseYPos, CDouble selectionWidth, CDouble selectionHeight)
{
	GLint viewport[4];
	glSelectBuffer(MAX_NAME_SIZE, Buffer);
	(void)glRenderMode(GL_SELECT);
	g_render.ProjectionMatrix();
	g_render.PushMatrix();
	g_render.IdentityMatrix();
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluPickMatrix(mouseXPos - (selectionWidth / 2.0), mouseYPos - (selectionHeight / 2.0), selectionWidth, selectionHeight, viewport);

	if (g_currentCameraType == eCAMERA_PHYSX)
	{
		gluPerspective(g_camera->m_cameraManager->GetAngle(), CDouble(g_width) / CDouble(g_height), g_cameraProperties.m_playModePerspectiveNCP, g_cameraProperties.m_playModePerspectiveFCP);
	}
	g_render.ModelViewMatrix();
	glInitNames();
}

CVoid CMain::FinishPrefabSelection()
{
	g_render.ProjectionMatrix();
	g_render.PopMatrix();
	g_render.ModelViewMatrix();
	glFlush();
	GLint hits = glRenderMode(GL_RENDER);

	if (hits > 0)
	{
		ptr = Buffer;
		for (CInt j = 0; j < hits; j++)
		{
			ptr++; //skip the first info ( number of names which is one in my engine )and move to the min depth info

			if (*ptr < minZ)
			{
				minZ = *ptr;

				m_selectedPrefabName = *(ptr + 2); //name of the object
			}
			ptr += 3; //move to the next hit
		}
	}
}

CChar* CMain::SelectPrefabInstances(CDouble mouseXPos, CDouble mouseYPos, CDouble selectionWidth, CDouble selectionHeight)
{
	minZ = 0xffffffff;
	m_selectedPrefabName = -1;
	glPushAttrib(GL_VIEWPORT_BIT);
	
	glViewport(0, 0, g_width, g_height);// resets the viewport to new dimensions.

	g_render.m_useShader = CFalse;
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		if (!g_instancePrefab[i]->GetVisible()) continue;

		g_currentInstancePrefab = g_instancePrefab[i];

		CScene* scene = NULL;

		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		scene = g_instancePrefab[i]->GetScene(0);

		if (!scene) continue;

		if (!scene->m_isSelectable) continue;

		if (m_bSelectionDistance)
		{
			if (g_currentInstancePrefab->GetDistanceFromCamera() > m_fSelectionDistance)
				continue;
		}

		InitPrefabSelection(mouseXPos, mouseYPos, selectionWidth, selectionHeight);
		glPushName(g_instancePrefab[i]->GetNameIndex());
		//g_scene[i]->RenderAABBWithQuads(); //Here's the code to "draw" the objects in selection mode
		g_render.SetScene(scene);
		//g_render.GetScene()->Update();
		if (!g_render.GetScene()->m_isTrigger /*&& g_render.GetScene()->m_hasAnimation*/)
		{
			g_render.GetScene()->Render(CTrue, NULL, CFalse);
			if (g_currentInstancePrefab)
			{
				g_render.ModelViewMatrix();
				g_render.PushMatrix();
				g_render.MultMatrix(*(g_currentInstancePrefab->GetInstanceMatrix()));
			}
			g_render.GetScene()->RenderAnimatedModels(CTrue, CTrue);
			if (g_currentInstancePrefab)
			{
				g_render.PopMatrix();
			}
			g_render.GetScene()->RenderAnimatedModels(CTrue, CFalse);

			g_render.GetScene()->RenderModelsControlledByPhysX(CTrue);
		}

		glPopName();
		FinishPrefabSelection();
	}
	if (g_render.UsingShader())
		g_render.m_useShader = CTrue;


	glPopAttrib();

	Cpy(m_selectedPrefabInstanceName, ""); //no name by default

	//run OnSelect function
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		if (m_selectedPrefabName != -1 && g_instancePrefab[i]->GetNameIndex() == m_selectedPrefabName)
		{
			g_instancePrefab[i]->OnSelectScript();
			Cpy(m_selectedPrefabInstanceName, g_instancePrefab[i]->GetName());
			break;
		}
	}
	return m_selectedPrefabInstanceName;
}


CBool CMain::InitFBOs( CInt channels, CInt type )
{
	//glGetIntegerv( GL_MAX_DRAW_BUFFERS, &g_numBuffers );

	CUInt numSamples;
	GLenum status;
	if( g_window.m_windowGL.multiSampling ) //initialize 2 separate FBOs
	{
		switch( g_options.m_numSamples  )
		{
		case 2:
			numSamples = 2;
			break;
		case 4:
			numSamples = 4;
			break;
		case 8:
			numSamples = 8;
			break;
		case 16:
			numSamples = 16;
			break;
		default:
			numSamples = 0;
		}
		GLint samples;
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
		if( numSamples > (CUInt)samples )
			numSamples = samples;
		m_mFboID = g_render.GenerateFBO();
		g_render.BindFBO( m_mFboID );
		//g_render.BindForWriting( m_mFboID );
		//g_render.Attach2DTextureToFBOColor( m_bloomTexture );
		m_rbDepthID = g_render.GenerateRenderBuffer();
		g_render.BindRenderBuffer( m_rbDepthID );
		g_render.RenderbufferDepthStorageMultisample( numSamples, g_width, g_height );
		g_render.AttachDepthToFBO( m_rbDepthID );

		for( CInt i = 0; i < eGBUFFER_NUM_TEXTURES; i++ )
		{
			m_rbColorID[i] = g_render.GenerateRenderBuffer();
			g_render.BindRenderBuffer( m_rbColorID[i] );
			g_render.RenderbufferColorStorageMultisample( numSamples, type, g_width, g_height );
			g_render.AttachRenderBufferToFBOColor( m_rbColorID[i], i );
		}
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
		GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT};
		glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);

		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status)
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			default:
				MessageBoxA( NULL, "CMain::InitFBo(): An error occured while creating the FBO", "Vanda Engine 1 Error",  MB_OK | MB_ICONERROR);
				break;
		}

		g_render.BindFBO(0);
		g_render.BindRenderBuffer(0);

		//second multisample FBO for multi light passes
		m_mFboID2 = g_render.GenerateFBO();
		g_render.BindFBO( m_mFboID2 );
		m_rbDepthMID2 = g_render.GenerateRenderBuffer();
		g_render.BindRenderBuffer( m_rbDepthMID2 );
		g_render.RenderbufferDepthStorageMultisample( numSamples, g_width, g_height );
		g_render.AttachDepthToFBO( m_rbDepthMID2 );

		m_rbColorID2 = g_render.GenerateRenderBuffer();
		g_render.BindRenderBuffer( m_rbColorID2 );
		g_render.RenderbufferColorStorageMultisample( numSamples, type, g_width, g_height );
		g_render.AttachRenderBufferToFBOColor( m_rbColorID2, 0 );

		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status)
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			default:
				MessageBoxA( NULL, "CMain::InitFBo(): An error occured while creating the FBO", "Vanda Engine 1 Error",  MB_OK | MB_ICONERROR);
				break;
		}

		g_render.BindFBO(0);
		g_render.BindRenderBuffer(0);

	}

	for( CInt i = 0; i < eGBUFFER_NUM_TEXTURES; i++ )
	{
		glGenTextures(1, &m_textureTarget[i] );								
		glBindTexture(GL_TEXTURE_2D, m_textureTarget[i] );					
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if( i == eGBUFFER_TEXTURE_TYPE_POSITION || i == eGBUFFER_TEXTURE_TYPE_NORMAL ) //vertex or normal
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);	
		else
    		glTexImage2D(GL_TEXTURE_2D, 0, channels, g_width, g_height, 0, type, GL_UNSIGNED_BYTE, NULL );
	}

	m_fboID = g_render.GenerateFBO();
	g_render.BindFBO( m_fboID );
	//g_render.BindForWriting( m_fboID );


	for( CInt i = 0; i < eGBUFFER_NUM_TEXTURES; i++ )
	{
		g_render.Attach2DTextureToFBOColor( m_textureTarget[i], i );
	}

	m_rbDepthID2 = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbDepthID2 );
	g_render.RenderbufferStorage( g_width, g_height );
	g_render.AttachDepthToFBO( m_rbDepthID2 );

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
	GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT};
	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			MessageBoxA( NULL, "CMain::InitFBo(): An error occured while creating the FBO", "Vanda Engine 1 Error",  MB_OK | MB_ICONERROR);
			break;
	}

	//second FBO for multi light passes
	glGenTextures(1, &m_textureTargetSwapLights );								
	glBindTexture(GL_TEXTURE_2D, m_textureTargetSwapLights );					
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);	
  	glTexImage2D(GL_TEXTURE_2D, 0, channels, g_width, g_height, 0, type, GL_UNSIGNED_BYTE, NULL );

	m_fboID2 = g_render.GenerateFBO();
	g_render.BindFBO( m_fboID2 );

	g_render.Attach2DTextureToFBOColor( m_textureTargetSwapLights, 0 );

	m_rbDepthIDSwapLights = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbDepthIDSwapLights );
	g_render.RenderbufferStorage( g_width, g_height );
	g_render.AttachDepthToFBO( m_rbDepthIDSwapLights );

	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			MessageBoxA( NULL, "CMain::InitFBo(): An error occured while creating the FBO", "Vanda Engine 1 Error",  MB_OK | MB_ICONERROR);
			break;
	}

	//Final FBO for multi light passes
	glGenTextures(1, &m_textureTargetSumLighting );								
	glBindTexture(GL_TEXTURE_2D, m_textureTargetSumLighting );					
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);	
   	glTexImage2D(GL_TEXTURE_2D, 0, channels, g_width, g_height, 0, type, GL_UNSIGNED_BYTE, NULL );

	m_fboIDSum = g_render.GenerateFBO();
	g_render.BindFBO( m_fboIDSum );
	g_render.Attach2DTextureToFBOColor( m_textureTargetSumLighting, 0 );

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			MessageBoxA( NULL, "CMain::InitFBo(): An error occured while creating the FBO", "Vanda Engine 1 Error",  MB_OK | MB_ICONERROR);
			break;
	}

	g_render.BindFBO( 0 );

	glBindTexture(GL_TEXTURE_2D, 0);



	// FBO for fog and depth of field
	glGenTextures(1, &m_textureFogDof );								
	glBindTexture(GL_TEXTURE_2D, m_textureFogDof );					
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);	
   	glTexImage2D(GL_TEXTURE_2D, 0, channels, g_width, g_height, 0, type, GL_UNSIGNED_BYTE, NULL );

	m_fboIDFogDof = g_render.GenerateFBO();
	g_render.BindFBO( m_fboIDFogDof );
	g_render.Attach2DTextureToFBOColor( m_textureFogDof, 0 );
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	m_rbDepthIDFogDof = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbDepthIDFogDof );
	g_render.RenderbufferStorage( g_width, g_height );
	g_render.AttachDepthToFBO( m_rbDepthIDFogDof );
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			MessageBoxA( NULL, "CMain::InitFBo(): An error occured while creating the FBO", "Vanda Engine 1 Error",  MB_OK | MB_ICONERROR);
			break;
	}

	//Multisample FBO for depth and DOF
	m_mFboIDFogDof = g_render.GenerateFBO();
	g_render.BindFBO( m_mFboIDFogDof );

	m_rbColorIDFogDof = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbColorIDFogDof );
	g_render.RenderbufferColorStorageMultisample( numSamples, type, g_width, g_height );
	g_render.AttachRenderBufferToFBOColor( m_rbColorIDFogDof, 0 );

	m_rbMDepthIDFogDof = g_render.GenerateRenderBuffer();
	g_render.BindRenderBuffer( m_rbMDepthIDFogDof );
	g_render.RenderbufferDepthStorageMultisample( numSamples, g_width, g_height );
	g_render.AttachDepthToFBO( m_rbMDepthIDFogDof );

	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			MessageBoxA( NULL, "CMain::InitFBo(): An error occured while creating the FBO", "Vanda Engine 1 Error",  MB_OK | MB_ICONERROR);
			break;
	}

	//g_render.BindFBO(0);
	g_render.BindFBO( 0 );
	g_render.BindRenderBuffer(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	return CTrue;

}


CVoid CMain::SetInstanceCamera(CInstanceCamera * inst, CFloat sWidth, CFloat sHeight, CFloat fov, CFloat zNear, CFloat zFar)
{
	//previous position of the perspective camera (used while instanced camera is enabled)
	g_camera->m_perspectiveCameraPosOfPreviousFrame = g_camera->m_perspectiveCameraPos;

	// Get the camera from the instance and set the projection matrix from it
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,
					( sWidth / sHeight ),
					zNear,
					zFar);

	if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	{
		//shadow
		glGetFloatv(GL_PROJECTION_MATRIX, cam_proj);
	}

	// Now get the node from the instance, and set the modelview matrix to the inverse of 
	// the node's matrix.
	glMatrixMode(GL_MODELVIEW);
	CMatrix mat;
	const CMatrix *ltow;
	ltow = inst->m_parent->GetLocalToWorldMatrix();
	CMatrixCopy(*ltow, mat); 
	//CMatrix3x4Invert( mat, mat);
	//LoadMatrix( mat );
	CVec3f at(0,0,-1),pos( 0,0,0);
	CMatrixTransform( mat, at, free_dae_cam_at );
	CMatrixTransform( mat, pos, free_dae_cam_pos );
	g_camera->m_perspectiveCameraPos.x = free_dae_cam_pos.x;
	g_camera->m_perspectiveCameraPos.y = free_dae_cam_pos.y;
	g_camera->m_perspectiveCameraPos.z = free_dae_cam_pos.z;

	//it's used for OpenAL
	g_camera->m_perspectiveCameraDir.x = -(free_dae_cam_at.x - free_dae_cam_pos.x);
	g_camera->m_perspectiveCameraDir.y = free_dae_cam_at.y - free_dae_cam_pos.y;
	g_camera->m_perspectiveCameraDir.z = -(free_dae_cam_at.z - free_dae_cam_pos.z);

	//shadow 
	cam_pos[0] = g_camera->m_perspectiveCameraPos.x;
	cam_pos[1] = g_camera->m_perspectiveCameraPos.y;
	cam_pos[2] = g_camera->m_perspectiveCameraPos.z;

	cam_dir[0] = -g_camera->m_perspectiveCameraDir.x;
	cam_dir[1] = g_camera->m_perspectiveCameraDir.y;
	cam_dir[2] = -g_camera->m_perspectiveCameraDir.z;

	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt(free_dae_cam_pos.x, free_dae_cam_pos.y, free_dae_cam_pos.z, free_dae_cam_at.x, free_dae_cam_at.y, free_dae_cam_at.z, 0.0, 1.0, 0.0);
	//if( g_shadowProperties.m_enable && g_render.UsingShadowShader()  && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
	//{
		// store the inverse of the resulting modelview matrix for the shadow computation
		glGetFloatv(GL_MODELVIEW_MATRIX, cam_modelview);
		g_camera->m_cameraManager->GetInverseMatrix( cam_inverse_modelview );
	//}
}

CVoid CMain::UpdateDynamicPhysicsObjects()
{
	//3D Model data
	CBool foundDynamicPhysX = CFalse;
	if (gPhysXscene)
	{
		for (CUInt j = 0; j < gPhysXscene->getNbActors(); j++)
		{
			if (!gPhysXscene->getActors()[j]->isSleeping())
				foundDynamicPhysX = CTrue;
		}
	}

	if (foundDynamicPhysX)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (!g_instancePrefab[i]->GetIsControlledByPhysX()) continue;

			g_currentInstancePrefab = g_instancePrefab[i];

			CScene* scene = NULL;

			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j))
				{
					scene = g_instancePrefab[i]->GetScene(j);
					if (!scene) continue;
					break;
				}
			}
			g_render.SetScene(scene);

			if (g_render.GetScene()->m_update)
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			g_render.GetScene()->UpdateDynamicPhysicsObjects();
		}
	}
}

CVoid CMain::UpdateAnimations(CBool init)
{
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		if (!g_instancePrefab[i]->GetVisible()) continue;
		if (!g_instancePrefab[i]->GetIsAnimated() && !g_instancePrefab[i]->GetIsControlledByPhysX()) continue;
		g_currentInstancePrefab = g_instancePrefab[i];

		CScene* scene = NULL;

		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j) /*&& (g_instancePrefab[i]->GetSceneVisible(j) || init)*/)
			{
				scene = g_instancePrefab[i]->GetScene(j);
				if (!scene) continue;
				g_render.SetScene(scene);
				if (!g_render.GetScene()->m_isTrigger)
				{
					if (g_render.GetScene()->m_hasAnimation && g_render.GetScene()->m_updateAnimation)
					{
						if ((g_render.GetScene()->GetAnimationStatus() == eANIM_PLAY && g_render.GetScene()->UpdateAnimationLists()) || g_render.GetScene()->IsTransformable())
						{
							g_render.GetScene()->Update(elapsedTime);
							g_render.GetScene()->m_updateAnimation = CFalse;
							g_render.GetScene()->m_update = CFalse;
							g_render.GetScene()->SetUpdateBB(CTrue);
						} //if
						else if (init)
						{
							g_render.GetScene()->Update(0.0f);
							g_render.GetScene()->m_updateAnimation = CFalse;
							g_render.GetScene()->m_update = CFalse;
						}
						else if (g_render.GetScene()->m_update)
						{
							g_render.GetScene()->Update();
							g_render.GetScene()->m_update = CFalse;
						} //else

					} //if
					else if (g_render.GetScene()->m_update)
					{
						g_render.GetScene()->Update();
						g_render.GetScene()->m_update = CFalse;
					} //else
				} //if
			}
		}
	} //for
}
CVoid CMain::Render3DModelsControlledByPhysX(CBool sceneManager)
{
	//3D Model data
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		if (!g_instancePrefab[i]->GetVisible()) continue;
		if (!g_instancePrefab[i]->GetIsControlledByPhysX()) continue;

		g_currentInstancePrefab = g_instancePrefab[i];

		//if (m_bQuery)
		//{
		//	if (g_instancePrefab[i]->GetResult() == 0)
		//	{
		//		if (g_instancePrefab[i]->GetDistanceFromCamera() - g_instancePrefab[i]->GetRadius() >(g_instancePrefab[i]->GetRadius() + 20.0f))
		//			continue;
		//	}
		//}
		CBool foundPhysX = CFalse;
		if (gPhysXscene)
		{
			for (CUInt j = 0; j < gPhysXscene->getNbActors(); j++)
			{
				if (!gPhysXscene->getActors()[j]->isSleeping())
					foundPhysX = CTrue;
			}
		}

		CScene* scene = NULL;

		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j) && (g_instancePrefab[i]->GetSceneVisible(j) || foundPhysX))
			{
				scene = g_instancePrefab[i]->GetScene(j);
				break;
			}
		}
		if (!scene) continue;
		g_render.SetScene(scene);

		if (!g_render.GetScene()->m_isTrigger)
		{
			if (g_render.GetScene()->m_update)
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			g_currentInstancePrefab->SetLight();
			g_render.GetScene()->RenderModelsControlledByPhysX(sceneManager);

			if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
			{
				glUseProgram(0);
			}

		}
	}

}

CVoid CMain::Render3DModelsControlledByPhysXForWater(CWater* water, CBool sceneManager)
{
	//3D Model data
	for (CUInt i = 0; i < water->m_instancePrefab.size(); i++)
	{
		if (!water->m_instancePrefab[i]->GetVisible()) continue;
		if (!water->m_instancePrefab[i]->GetIsControlledByPhysX()) continue;
		g_currentInstancePrefab = water->m_instancePrefab[i];

		CVec3f src[8];
		CVec3f maxAABB = water->m_instancePrefab[i]->GetInverseMaxAABB();
		CVec3f minAABB = water->m_instancePrefab[i]->GetInverseMinAABB();
		src[0].x = minAABB.x; src[0].y = minAABB.y; src[0].z = minAABB.z;
		src[1].x = maxAABB.x; src[1].y = minAABB.y; src[1].z = maxAABB.z;
		src[2].x = maxAABB.x; src[2].y = minAABB.y; src[2].z = minAABB.z;
		src[3].x = minAABB.x; src[3].y = minAABB.y; src[3].z = maxAABB.z;
		src[4].x = maxAABB.x; src[4].y = maxAABB.y; src[4].z = minAABB.z;
		src[5].x = minAABB.x; src[5].y = maxAABB.y; src[5].z = minAABB.z;
		src[6].x = minAABB.x; src[6].y = maxAABB.y; src[6].z = maxAABB.z;
		src[7].x = maxAABB.x; src[7].y = maxAABB.y; src[7].z = maxAABB.z;

		if (!g_camera->m_cameraManager->IsBoxInFrustum(&src[0], 8))
		{
			g_currentInstancePrefab->SetRenderForWaterQuery(CFalse);
			continue;
		}
		else
		{
			g_currentInstancePrefab->SetRenderForWaterQuery(CTrue);
		}

		g_currentInstancePrefab = water->m_instancePrefab[i];

		CScene* scene = NULL;

		CPrefab* prefab = water->m_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j) && water->m_instancePrefab[i]->GetSceneVisible(j))
			{
				scene = water->m_instancePrefab[i]->GetScene(j);
				break;
			}
		}
		if (!scene) continue;
		g_render.SetScene(scene);

		if (!g_render.GetScene()->m_isTrigger)
		{
			if (g_render.GetScene()->m_update)
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			g_currentInstancePrefab->SetLight();
			g_render.GetScene()->RenderModelsControlledByPhysX(sceneManager);

			if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
			{
				glUseProgram(0);
			}

		}
	}

}

CVoid CMain::RenderCharacter(CBool sceneManager)
{
	if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON) return;

	if (!g_databaseVariables.m_insertCharacter)
		return;
	//3D Model data
	if (!g_mainCharacter)
		return;
	g_nx->gCharacterPos = g_nx->GetFilteredCharacterPos();

	g_currentInstancePrefab = g_mainCharacter->GetInstancePrefab();

	if (!g_currentInstancePrefab->GetVisible()) return;
	g_currentInstancePrefab->SetLight();

	static CFloat lerpFactor = 0.0f;

	CFloat y = (CFloat)g_nx->gCharacterPos.y - (g_physXProperties.m_fCapsuleHeight * 0.5f) - g_physXProperties.m_fCapsuleRadius - g_physXProperties.m_fCharacterSkinWidth;

	CVec3f trans(g_nx->gCharacterPos.x, y, g_nx->gCharacterPos.z);

	g_mainCharacter->GetInstancePrefab()->SetTranslate(trans);
	if (g_nx->m_pushCharacter)
	{

		CFloat y = NxMath::radToDeg(atan2(g_nx->m_horizontalDisp.x, g_nx->m_horizontalDisp.z));
		if (y < 0.0f)
			y = 360.0f + y;
		if (y == 360.0f)
			y = 0.0f;
		if (g_nx->m_previousMoveDirection != g_nx->m_currentMoveDirection)
		{
			m_characterRotationTransition = CTrue;
			lerpFactor = 0.0f;
		}
		else if (!m_characterRotationTransition)
		{
			g_mainCharacter->SetCurrentRotation(y);
		}
		if (m_characterRotationTransition)
		{
			//find the shortest path to rotate
			if (g_mainCharacter->GetCurrentRotation() > y)
			{
				if (g_mainCharacter->GetCurrentRotation() - y > 360 + y - g_mainCharacter->GetCurrentRotation())
					y = y + 360.0f;
			}
			else if (g_mainCharacter->GetCurrentRotation() < y)
			{
				if (y - g_mainCharacter->GetCurrentRotation() > 360 + g_mainCharacter->GetCurrentRotation() - y)
					g_mainCharacter->SetCurrentRotation(g_mainCharacter->GetCurrentRotation() + 360.0f);
			}

			g_mainCharacter->SetCurrentRotation(Lerp(g_mainCharacter->GetCurrentRotation(), y, lerpFactor));
			lerpFactor += 2.0f * elapsedTime;
			//if it's equal to y, stop the transition
			if (fabs(g_mainCharacter->GetCurrentRotation() - y) <= elapsedTime)
			{
				m_characterRotationTransition = CFalse;
				lerpFactor = 0.0f;
			}
		}

		CVec4f rot(0.0f, g_mainCharacter->GetCurrentRotation(), 0.0f, 0.0f);

		m_previousCharacterRotation = y;
		g_mainCharacter->GetInstancePrefab()->SetRotate(rot);
	}

	//if (m_bQuery)
	//{
	//	if (g_instancePrefab[i]->GetResult() == 0)
	//	{
	//		if (g_instancePrefab[i]->GetDistanceFromCamera() - g_instancePrefab[i]->GetRadius() >(g_instancePrefab[i]->GetRadius() + 20.0f))
	//			continue;
	//	}
	//}

	CScene* scene = NULL;

	CPrefab* prefab = g_currentInstancePrefab->GetPrefab();
	for (CUInt j = 0; j < 3; j++)
	{
		if (prefab && prefab->GetHasLod(j) )
		{
			scene = g_currentInstancePrefab->GetScene(j);
			break;
		}
	}
	if (!scene) return;

	g_render.SetScene(scene);
	if (!g_render.GetScene()->m_isTrigger)
	{
		//if (g_render.GetScene()->m_hasAnimation && g_render.GetScene()->m_updateAnimation)
		//{
		//	if (g_render.GetScene()->GetAnimationStatus() == eANIM_PLAY  && g_render.GetScene()->UpdateAnimationLists())
		//	{
		//		g_render.GetScene()->Update(elapsedTime);
		//		g_render.GetScene()->m_updateAnimation = CFalse;
		//		g_render.GetScene()->m_update = CFalse;
		//		g_render.GetScene()->SetUpdateBB(CTrue);
		//	}
		//	else if (g_render.GetScene()->m_update)
		//	{
		//		g_render.GetScene()->Update();
		//		g_render.GetScene()->m_update = CFalse;
		//	}

		//}
		//else if (g_render.GetScene()->m_update)
		//{
		//	g_render.GetScene()->Update();
		//	g_render.GetScene()->m_update = CFalse;
		//}
		if (g_currentInstancePrefab)
		{
			g_render.ModelViewMatrix();
			g_render.PushMatrix();
			g_render.MultMatrix(*(g_currentInstancePrefab->GetInstanceMatrix()));
		}
		//disable self shadow to prevent artifacts
		CBool useShadow = g_render.m_useDynamicShadowMap;
		//g_render.m_useDynamicShadowMap = CFalse;
		g_render.GetScene()->RenderAnimatedModels(sceneManager, CTrue); //render controller
		g_render.m_useDynamicShadowMap = useShadow;

		if (g_currentInstancePrefab)
		{
			g_render.PopMatrix();
		}
		if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
		{
			glUseProgram(0);
		}

	}
}


CVoid CMain::Render3DAnimatedModels(CBool sceneManager)
{
	//3D Model data

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		if (!g_instancePrefab[i]->GetVisible()) continue;
		if (!g_instancePrefab[i]->GetIsAnimated()) continue;

		g_currentInstancePrefab = g_instancePrefab[i];
		if (Cmp(g_currentInstancePrefab->GetName(), "VANDA_MAIN_CHARACTER"))
			continue;
		if (!g_currentInstancePrefab->GetRenderForQuery())
			continue;

		//if (m_bQuery)
		//{
		//	if (g_instancePrefab[i]->GetResult() == 0)
		//	{
		//		if (g_instancePrefab[i]->GetDistanceFromCamera() - g_instancePrefab[i]->GetRadius() >(g_instancePrefab[i]->GetRadius() + 20.0f))
		//			continue;
		//	}
		//}

		CScene* scene = NULL;

		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j) && g_instancePrefab[i]->GetSceneVisible(j))
			{
				scene = g_instancePrefab[i]->GetScene(j);
				break;
			}
		}
		if (!scene) continue;

		g_render.SetScene(scene);
		if (!g_render.GetScene()->m_isTrigger)
		{
			g_currentInstancePrefab->SetLight();

			if (g_currentInstancePrefab)
			{
				g_render.ModelViewMatrix();
				g_render.PushMatrix();
				g_render.MultMatrix(*(g_instancePrefab[i]->GetInstanceMatrix()));
			}
			g_render.GetScene()->RenderAnimatedModels(sceneManager, CTrue); //render controller
			if (g_currentInstancePrefab)
			{
				g_render.PopMatrix();
			}
			g_render.GetScene()->RenderAnimatedModels(sceneManager, CFalse); //render animated models
			if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
			{
				glUseProgram(0);
			}

		}
	}
}

CVoid CMain::Render3DAnimatedModelsForWater(CWater* water, CBool sceneManager)
{
	//3D Model data
	for (CUInt i = 0; i < water->m_instancePrefab.size(); i++)
	{
		if (!water->m_instancePrefab[i]->GetVisible()) continue;
		if (!water->m_instancePrefab[i]->GetIsAnimated()) continue;
		g_currentInstancePrefab = water->m_instancePrefab[i];

		CVec3f src[8];
		CVec3f maxAABB = water->m_instancePrefab[i]->GetInverseMaxAABB();
		CVec3f minAABB = water->m_instancePrefab[i]->GetInverseMinAABB();
		src[0].x = minAABB.x; src[0].y = minAABB.y; src[0].z = minAABB.z;
		src[1].x = maxAABB.x; src[1].y = minAABB.y; src[1].z = maxAABB.z;
		src[2].x = maxAABB.x; src[2].y = minAABB.y; src[2].z = minAABB.z;
		src[3].x = minAABB.x; src[3].y = minAABB.y; src[3].z = maxAABB.z;
		src[4].x = maxAABB.x; src[4].y = maxAABB.y; src[4].z = minAABB.z;
		src[5].x = minAABB.x; src[5].y = maxAABB.y; src[5].z = minAABB.z;
		src[6].x = minAABB.x; src[6].y = maxAABB.y; src[6].z = maxAABB.z;
		src[7].x = maxAABB.x; src[7].y = maxAABB.y; src[7].z = maxAABB.z;

		if (!g_camera->m_cameraManager->IsBoxInFrustum(&src[0], 8))
		{
			g_currentInstancePrefab->SetRenderForWaterQuery(CFalse);
			continue;
		}
		else
		{
			g_currentInstancePrefab->SetRenderForWaterQuery(CTrue);
		}


		CScene* scene = NULL;

		CPrefab* prefab = water->m_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j) && water->m_instancePrefab[i]->GetSceneVisible(j))
			{
				scene = water->m_instancePrefab[i]->GetScene(j);
				break;
			}
		}
		if (!scene) continue;

		g_render.SetScene(scene);
		if (!g_render.GetScene()->m_isTrigger)
		{
			CBool update = CTrue;
			if (g_currentVSceneProperties.m_isMenu && g_currentVSceneProperties.m_isPause)
				update = CFalse;
			if (update)
			{
				if (g_render.GetScene()->m_hasAnimation && g_render.GetScene()->m_updateAnimation)
				{
					if (g_render.GetScene()->GetAnimationStatus() == eANIM_PLAY  && g_render.GetScene()->UpdateAnimationLists())
					{
						g_render.GetScene()->Update(elapsedTime);
						g_render.GetScene()->m_updateAnimation = CFalse;
						g_render.GetScene()->m_update = CFalse;
						g_render.GetScene()->SetUpdateBB(CTrue);
					}
					else if (g_render.GetScene()->m_update)
					{
						g_render.GetScene()->Update();
						g_render.GetScene()->m_update = CFalse;
					}

				}
				else if (g_render.GetScene()->m_update)
				{
					g_render.GetScene()->Update();
					g_render.GetScene()->m_update = CFalse;
				}
			}
			g_currentInstancePrefab->SetLight();
			if (g_currentInstancePrefab)
			{
				g_render.ModelViewMatrix();
				g_render.PushMatrix();
				g_render.MultMatrix(*(water->m_instancePrefab[i]->GetInstanceMatrix()));
			}
			g_render.GetScene()->RenderAnimatedModels(sceneManager, CTrue); //render controller
			if (g_currentInstancePrefab)
			{
				g_render.PopMatrix();
			}
			g_render.GetScene()->RenderAnimatedModels(sceneManager, CFalse); //render animated models
			if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
			{
				glUseProgram(0);
			}

		}
	}
}

CVoid CMain::RenderTerrain()
{
	if (!g_terrain) return;
	if (g_engineLights.size() == 0)
		SetDefaultLight();

	// Display the terrain mesh.
	if (g_terrain->GetTerrainTexture())
		g_terrain->GetTerrainTexture()->enableTextures();

	if (g_options.m_enableShader && g_render.UsingShader())
	{
		if (g_fogBlurPass)
			g_shaderType = g_render.m_fogBlurProgram;
		else if (g_shadowProperties.m_enable && g_render.UsingShadowShader() && !Cmp(g_shadowProperties.m_directionalLightName, "\n"))
		{
			if (false/*m_hasNormalMap*/) //currently no normal map
			{
				//switch (g_shadowProperties.m_shadowType)
				//{
				//case eSHADOW_SINGLE_HL:
				//	g_shaderType = g_render.m_terrain_shad_single_hl_normal_prog;
				//	break;
				//case eSHADOW_SINGLE:
				//	g_shaderType = g_render.m_terrain_shad_single_normal_prog;
				//	break;
				//case eSHADOW_MULTI_LEAK:
				//	g_shaderType = g_render.m_terrain_shad_multi_normal_prog;
				//	break;
				//case eSHADOW_MULTI_NOLEAK:
				//	g_shaderType = g_render.m_terrain_shad_multi_noleak_normal_prog;
				//	break;
				//case eSHADOW_PCF:
				//	g_shaderType = g_render.m_terrain_shad_pcf_normal_prog;
				//	break;
				//case eSHADOW_PCF_TRILIN:
				//	g_shaderType = g_render.m_terrain_shad_pcf_trilin_normal_prog;
				//	break;
				//case eSHADOW_PCF_4TAP:
				//	g_shaderType = g_render.m_terrain_shad_pcf_4tap_normal_prog;
				//	break;
				//case eSHADOW_PCF_8TAP:
				//	g_shaderType = g_render.m_terrain_shad_pcf_8tap_normal_prog;
				//	break;
				//case eSHADOW_PCF_GAUSSIAN:
				//	g_shaderType = g_render.m_terrain_shad_pcf_gaussian_normal_prog;
				//	break;
				//}
			}
			else
			{
				switch (g_shadowProperties.m_shadowType)
				{
				case eSHADOW_SINGLE_HL:
					g_shaderType = g_render.m_shad_single_hl_prog;
					break;
				case eSHADOW_SINGLE:
					g_shaderType = g_render.m_terrain_shad_single_prog;
					break;
				case eSHADOW_MULTI_LEAK:
					g_shaderType = g_render.m_terrain_shad_multi_prog;
					break;
				case eSHADOW_MULTI_NOLEAK:
					g_shaderType = g_render.m_terrain_shad_multi_noleak_prog;
					break;
				case eSHADOW_PCF:
					g_shaderType = g_render.m_terrain_shad_pcf_prog;
					break;
				case eSHADOW_PCF_TRILIN:
					g_shaderType = g_render.m_terrain_shad_pcf_trilin_prog;
					break;
				case eSHADOW_PCF_4TAP:
					g_shaderType = g_render.m_terrain_shad_pcf_4tap_prog;
					break;
				case eSHADOW_PCF_8TAP:
					g_shaderType = g_render.m_terrain_shad_pcf_8tap_prog;
					break;
				case eSHADOW_PCF_GAUSSIAN:
					g_shaderType = g_render.m_terrain_shad_pcf_gaussian_prog;
					break;
				}
			}
		}
		else
			g_shaderType = g_render.m_terrainProgram;

		if (g_fogBlurPass)
		{
			glUseProgram(g_shaderType);
			glUniform1f(glGetUniformLocation(g_shaderType, "focalDistance"), g_main->m_dof.m_focalDistance);
			glUniform1f(glGetUniformLocation(g_shaderType, "focalRange"), g_main->m_dof.m_focalRange);
			CBool useFog;
			if ((g_dofProperties.m_enable && g_dofProperties.m_debug) || (g_shadowProperties.m_shadowType == eSHADOW_SINGLE_HL && g_shadowProperties.m_enable && g_render.UsingShadowShader()))
				useFog = CFalse;
			else
				useFog = CTrue;

			if (g_fogProperties.m_enable && useFog)
				glUniform1i(glGetUniformLocation(g_shaderType, "enableFog"), CTrue);
			else
				glUniform1i(glGetUniformLocation(g_shaderType, "enableFog"), CFalse);
		}
		else
		{
			glUseProgram(g_shaderType);
			glUniform1i(glGetUniformLocation(g_shaderType, "image1"), 0);
			glUniform1i(glGetUniformLocation(g_shaderType, "image2"), 1);
			glUniform1i(glGetUniformLocation(g_shaderType, "image3"), 2);
			glUniform1i(glGetUniformLocation(g_shaderType, "shadowMap"), 3);
			glUniform1i(glGetUniformLocation(g_shaderType, "image1Normal"), 4);
			glUniform1i(glGetUniformLocation(g_shaderType, "image2Normal"), 5);
			glUniform1i(glGetUniformLocation(g_shaderType, "image3Normal"), 6);

			glUniform1i(glGetUniformLocation(g_shaderType, "stex"), 7); // depth-maps
			glUniform4fv(glGetUniformLocation(g_shaderType, "far_d"), 1, g_main->far_bound);
			glUniform2f(glGetUniformLocation(g_shaderType, "texSize"), (float)g_main->m_dynamicShadowMap->depth_size, 1.0f / (float)g_main->m_dynamicShadowMap->depth_size);
			glUniform1f(glGetUniformLocation(g_shaderType, "shadow_intensity"), g_shadowProperties.m_intensity);
		}
	}
	else
	{
		glUseProgram(0);
	}

	if (g_terrain->GetTerrain())
		g_terrain->GetTerrain()->draw(CFalse);
	if (g_terrain->GetTerrainTexture())
		g_terrain->GetTerrainTexture()->disableTextures();
}


CVoid CMain::RenderQueries(CBool init)
{
	if (m_bQuery || init)
	{
		COpenGLUtility m_glUtil;
		m_renderQuery = CTrue;
		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		if (!g_useOldRenderingStyle && g_window.m_windowGL.multiSampling && g_options.m_enableFBO)
			g_render.BindFBO(m_mFboID);
		else if (!g_useOldRenderingStyle && !g_window.m_windowGL.multiSampling && g_options.m_enableFBO)
			g_render.BindFBO(m_fboID);
		glClear(GL_DEPTH_BUFFER_BIT);

		g_render.m_useShader = CFalse;
		glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		for (CUInt i = 0; i < 8; i++)
		{
			glActiveTexture(GL_TEXTURE0 + (GLenum)i);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}

		g_render.ModelViewMatrix();

		//render Bounding Boxes for 3d objects here
		//Results are used in ManageLOD() functions to determine the LOD level
		g_render.ModelViewMatrix();
		glDisable(GL_CULL_FACE);
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (g_instancePrefab[i]->GetIsTrigger() || Cmp(g_instancePrefab[i]->GetName(), "VANDA_MAIN_CHARACTER") || !g_instancePrefab[i]->GetRenderForQuery() || !g_instancePrefab[i]->GetVisible())
				continue;

			g_currentInstancePrefab = g_instancePrefab[i];
			glBeginQuery(GL_SAMPLES_PASSED, g_instancePrefab[i]->GetQueryIndex());

			CVec3f ObjectCenter = (g_currentInstancePrefab->GetMinAABB() + g_currentInstancePrefab->GetMaxAABB()) * 0.5f;
			g_render.ModelViewMatrix();
			g_render.PushMatrix();
			g_render.IdentityMatrix();
			if (g_currentCameraType == eCAMERA_PHYSX)
			{
				gluLookAt(g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z,
				ObjectCenter.x, ObjectCenter.y, ObjectCenter.z, 0.0f, 1.0f, 0.0f);
			}
			else
			{
				gluLookAt(free_dae_cam_pos.x, free_dae_cam_pos.y, free_dae_cam_pos.z,
					ObjectCenter.x, ObjectCenter.y, ObjectCenter.z, 0.0f, 1.0f, 0.0f);
			}
			m_glUtil.DrawSimpleBox(g_currentInstancePrefab->GetMinAABB(), g_currentInstancePrefab->GetMaxAABB());
			g_render.PopMatrix();
			glEndQuery(GL_SAMPLES_PASSED);
		}

		g_render.m_useShader = CTrue;
		//end of rendering Object's bounding boxes 
		if (g_engineWaters.size() > 0)
		{
			//set all the lights here
			for (CUInt j = 0; j < g_engineWaters.size(); j++)
			{
				CVec3f waterPoints[4];
				waterPoints[0].x = g_engineWaters[j]->m_sidePoint[0].x; waterPoints[0].y = g_engineWaters[j]->m_sidePoint[0].y; waterPoints[0].z = g_engineWaters[j]->m_sidePoint[0].z;
				waterPoints[1].x = g_engineWaters[j]->m_sidePoint[1].x; waterPoints[1].y = g_engineWaters[j]->m_sidePoint[1].y; waterPoints[1].z = g_engineWaters[j]->m_sidePoint[1].z;
				waterPoints[2].x = g_engineWaters[j]->m_sidePoint[2].x; waterPoints[2].y = g_engineWaters[j]->m_sidePoint[2].y; waterPoints[2].z = g_engineWaters[j]->m_sidePoint[2].z;
				waterPoints[3].x = g_engineWaters[j]->m_sidePoint[3].x; waterPoints[3].y = g_engineWaters[j]->m_sidePoint[3].y; waterPoints[3].z = g_engineWaters[j]->m_sidePoint[3].z;

				if (g_camera->m_cameraManager->IsBoxInFrustum(waterPoints, 4))
				{
					g_engineWaters[j]->SetOutsideFrustom(CFalse);
					glBeginQuery(GL_SAMPLES_PASSED, g_engineWaters[j]->GetQueryIndex());

					CVec3f WaterCenter(g_engineWaters[j]->m_fWaterCPos[0], g_engineWaters[j]->m_fWaterCPos[1], g_engineWaters[j]->m_fWaterCPos[2]);
					g_render.ModelViewMatrix();
					g_render.PushMatrix();
					g_render.IdentityMatrix();
					if (g_currentCameraType == eCAMERA_PHYSX)
					{
						gluLookAt(g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z,
						WaterCenter.x, WaterCenter.y, WaterCenter.z, 0.0f, 1.0f, 0.0f);
					}
					else
					{
						gluLookAt(free_dae_cam_pos.x, free_dae_cam_pos.y, free_dae_cam_pos.z,
							WaterCenter.x, WaterCenter.y, WaterCenter.z, 0.0f, 1.0f, 0.0f);
					}
					COpenGLUtility glUtil;
					glUtil.DrawSquare(g_engineWaters[j]->m_sidePoint[0], g_engineWaters[j]->m_sidePoint[1], g_engineWaters[j]->m_sidePoint[2], g_engineWaters[j]->m_sidePoint[3]);
					g_render.PopMatrix();
					glEndQuery(GL_SAMPLES_PASSED);

					for (CUInt k = 0; k < g_engineWaters[j]->m_instancePrefab.size(); k++)
					{
						g_currentInstancePrefab = g_engineWaters[j]->m_instancePrefab[k];

						//to determine the LOD level of water, look at object, even if it's not inside the camera.
						//if we have already rendered the object query, skip it
						if (!g_currentInstancePrefab->GetRenderForWaterQuery() || g_currentInstancePrefab->GetRenderForQuery() || !g_currentInstancePrefab->GetVisible() || g_currentInstancePrefab->GetIsTrigger() || Cmp(g_currentInstancePrefab->GetName(), "VANDA_MAIN_CHARACTER"))
							continue;
						glBeginQuery(GL_SAMPLES_PASSED, g_currentInstancePrefab->GetQueryIndex());

						CVec3f ObjectCenter = (g_currentInstancePrefab->GetMinAABB() + g_currentInstancePrefab->GetMaxAABB()) * 0.5f;
						g_render.ModelViewMatrix();
						g_render.PushMatrix();
						g_render.IdentityMatrix();
						if (g_currentCameraType == eCAMERA_PHYSX)
						{
							gluLookAt(g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z,
							ObjectCenter.x, ObjectCenter.y, ObjectCenter.z, 0.0f, 1.0f, 0.0f);
						}
						else
						{
							gluLookAt(free_dae_cam_pos.x, free_dae_cam_pos.y, free_dae_cam_pos.z,
								ObjectCenter.x, ObjectCenter.y, ObjectCenter.z, 0.0f, 1.0f, 0.0f);

						}
						m_glUtil.DrawSimpleBox(g_currentInstancePrefab->GetMinAABB(), g_currentInstancePrefab->GetMaxAABB());
						g_render.PopMatrix();
						glEndQuery(GL_SAMPLES_PASSED);
					}

				}
				else
				{
					g_engineWaters[j]->SetOutsideFrustom(CTrue);
				}

			}
		}

		glDepthMask(GL_FALSE);

		if ( g_engineLights.size() > 0)
		{
			//set all the lights here
			for (CUInt j = 0; j < g_engineLights.size(); j++)
			{
				CInstanceLight *instanceLight = g_engineLights[j];

				if (g_engineLights[j]->m_abstractLight->GetType() == eLIGHTTYPE_DIRECTIONAL)
				{
					//Cpy(g_shadowProperties.m_directionalLightName, instanceLight->m_abstractLight->GetName());
					g_engineLights[j]->SetRunTimeVisible(CTrue);
				}

				CVec3f  Position;
				if (instanceLight->m_parent)
				{
					float *matrix = (float *)instanceLight->m_parent->GetLocalToWorldMatrix();
					Position.x = matrix[12]; Position.y = matrix[13]; Position.z = matrix[14];
				}
				else
				{
					Position.x = instanceLight->m_abstractLight->GetPosition()[0];
					Position.y = instanceLight->m_abstractLight->GetPosition()[1];
					Position.z = instanceLight->m_abstractLight->GetPosition()[2];
				}

				if (g_camera->m_cameraManager->IsSphereInFrustum(Position.x, Position.y, Position.z, (CFloat)instanceLight->GetRadius()))
				{
					g_engineLights[j]->SetRunTimeVisible(CTrue);
				}
				else
				{
					g_engineLights[j]->SetRunTimeVisible(CFalse);
				}
				//glBeginQuery(GL_SAMPLES_PASSED, instanceLight->GetQueryIndex());
				//CFloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				//CFloat pos[3]; pos[0] = Position.x; pos[1] = Position.y; pos[2] = Position.z;
				//g_render.DrawSolidSphere(instanceLight->GetRadius(), 10, 10, pos, color);
				//glEndQuery(GL_SAMPLES_PASSED);
			}
		}
		glDepthMask(CTrue);
		glEnable(GL_CULL_FACE);
		glPopAttrib();
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		//glDepthMask(GL_TRUE);
		m_renderQuery = CFalse;
		g_render.BindFBO(0);
	}
}

CVoid CMain::RenderBakedOctree3DModels()
{
	//3D Model data
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		//if it's not visible, skip
		if (!g_instancePrefab[i]->GetVisible()) continue;
		g_currentInstancePrefab = g_instancePrefab[i];

		//if it's not in view frustom, skip
		if (!g_instancePrefab[i]->GetRenderForQuery())
			continue;

		//if it's 3rd person character, skip
		if (Cmp(g_currentInstancePrefab->GetName(), "VANDA_MAIN_CHARACTER"))
			continue;


		CScene* scene = NULL;

		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j))
			{
				if (g_instancePrefab[i]->GetSceneVisible(j))
				{
					scene = g_instancePrefab[i]->GetScene(j);
					break;
				}
			}
		}
		if (!scene) continue;
		g_render.SetScene(scene);

		if (!g_render.GetScene()->m_isTrigger)
		{
			if (g_render.GetScene()->m_update)
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			g_currentInstancePrefab->SetLight();
			g_render.GetScene()->Render(g_currentInstancePrefab->GetRealTimeSceneCheckIsInFrustom());

			if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
			{
				glUseProgram(0);
			}

		}

	}
}

CVoid CMain::Render3DModels(CBool sceneManager, CChar* parentTreeNameOfGeometries, CBool checkVisibility, CBool drawGeometry)
{
	//3D Model data

	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		if (!g_instancePrefab[i]->GetVisible()) continue;
		g_currentInstancePrefab = g_instancePrefab[i];

		if (!checkVisibility)
			if (!g_instancePrefab[i]->GetRenderForQuery())
				continue;

		if (Cmp(g_currentInstancePrefab->GetName(), "VANDA_MAIN_CHARACTER"))
			continue;

		//if (m_bQuery)
		//{
		//	if (g_instancePrefab[i]->GetResult() == 0)
		//	{
		//		if (g_instancePrefab[i]->GetDistanceFromCamera() - g_instancePrefab[i]->GetRadius() > (g_instancePrefab[i]->GetRadius() + 20.0f))
		//			continue;
		//	}
		//}

		CScene* scene = NULL;

		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j) )
			{
				if (checkVisibility)
				{
					scene = g_instancePrefab[i]->GetScene(j);
					break;
				}
				else
				{
					if (g_instancePrefab[i]->GetSceneVisible(j))
					{
						scene = g_instancePrefab[i]->GetScene(j);
						break;
					}
				}
			}
		}
		if (!scene) continue;
		g_render.SetScene(scene);
		if (checkVisibility && g_currentInstancePrefab)
		{
			g_currentInstancePrefab->SetRealTimeSceneCheckIsInFrustom(sceneManager);
		}

		if (!g_render.GetScene()->m_isTrigger)
		{
			if (g_render.GetScene()->m_update)
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			g_currentInstancePrefab->SetLight();
			g_render.GetScene()->Render(sceneManager, parentTreeNameOfGeometries, checkVisibility, drawGeometry);

			if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
			{
				glUseProgram(0);
			}
		}
	}

}
CVoid CMain::Render3DModelsForWater(CWater* water, CBool sceneManager, CChar* parentTreeNameOfGeometries)
{
	//3D Model data
	for (CUInt i = 0; i < water->m_instancePrefab.size(); i++)
	{
		if (!water->m_instancePrefab[i]->GetVisible()) continue;
		g_currentInstancePrefab = water->m_instancePrefab[i];

		CVec3f src[8];
		CVec3f maxAABB = water->m_instancePrefab[i]->GetInverseMaxAABB();
		CVec3f minAABB = water->m_instancePrefab[i]->GetInverseMinAABB();
		src[0].x = minAABB.x; src[0].y = minAABB.y; src[0].z = minAABB.z;
		src[1].x = maxAABB.x; src[1].y = minAABB.y; src[1].z = maxAABB.z;
		src[2].x = maxAABB.x; src[2].y = minAABB.y; src[2].z = minAABB.z;
		src[3].x = minAABB.x; src[3].y = minAABB.y; src[3].z = maxAABB.z;
		src[4].x = maxAABB.x; src[4].y = maxAABB.y; src[4].z = minAABB.z;
		src[5].x = minAABB.x; src[5].y = maxAABB.y; src[5].z = minAABB.z;
		src[6].x = minAABB.x; src[6].y = maxAABB.y; src[6].z = maxAABB.z;
		src[7].x = maxAABB.x; src[7].y = maxAABB.y; src[7].z = maxAABB.z;

		if (!g_camera->m_cameraManager->IsBoxInFrustum(&src[0], 8))
		{
			g_currentInstancePrefab->SetRenderForWaterQuery(CFalse);
			continue;
		}
		else
		{
			g_currentInstancePrefab->SetRenderForWaterQuery(CTrue);
		}

		CScene* scene = NULL;

		CPrefab* prefab = water->m_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j) && water->m_instancePrefab[i]->GetSceneVisible(j))
			{
				scene = water->m_instancePrefab[i]->GetScene(j);
				break;
			}
		}
		if (!scene) continue;
		g_render.SetScene(scene);

		if (!g_render.GetScene()->m_isTrigger)
		{
			if (g_render.GetScene()->m_update)
			{
				g_render.GetScene()->Update();
				g_render.GetScene()->m_update = CFalse;
			}
			g_currentInstancePrefab->SetLight();
			g_render.GetScene()->Render(sceneManager, parentTreeNameOfGeometries);

			if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
			{
				glUseProgram(0);
			}
			//if (g_databaseVariables.m_showBoundingBox)
			//{
			//	g_render.GetScene()->RenderAABBWithLines();
			//}
		}
	}
}

CVoid CMain::DrawGUI()
{
	m_timerCounter++;

	g_font->StartRendering();
	//g_font->Print( "Vanda Engine 1", 10.0f, 980.0f, 0.0f, 1.0f, 1.0f, 1.0f );
	//g_font->Print( "Copyright (C) 2021 Ehsan Kamrani ", 10.0f, 950.0f, 0.0f, 1.0f, 1.0f, 1.0f );
	//g_font->Print( "http://www.vandaengine.org", 10.0f, 920.0f, 0.0f, 1.0f, 1.0f, 1.0f );
	//g_font->Print( "Press F4 for help...", 10.0f, 890.0f, 0.0f, 1.0f, 1.0f, 1.0f );

	if( m_showHelpInfo )
	{
		//g_font->Print( "F1 key: Enable / Disable help.\n", 10.0f, 830.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "Right Arrow key or D key: Move Right.\n", 10.0f, 800.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "Up Arrow key or W key: Move Front.\n", 10.0f, 770.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "Down Arrow key or S key: Move Back.\n", 10.0f, 740.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "Left Arrow key or A key: Move Left.\n", 10.0f, 710.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "E key: Move Down.\n", 10.0f, 680.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "Q key: Move Up (If gravity is disabled).\n", 10.0f, 650.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "G key: Enable / Disable gravity.\n", 10.0f, 620.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "Spacebar key: Jump (If gravity is enabled).\n", 10.0f, 590.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "Mouse movement: Camera rotation.\n", 10.0f, 560.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "Right mouse button: Zoom in / Zoom out.\n", 10.0f, 530.0f, 0.0f, 1.0f, 1.0f, 0.5f );
		//g_font->Print( "P key: Enable / Disable physics debug mode.\n", 10.0f, 500.0f, 0.0f, 1.0f, 1.0f, 0.5f );

		if (/*g_databaseVariables.m_showStatistics*/true)
		{
			CInt m_numSamples;
			if (g_window.m_windowGL.multiSampling)
				m_numSamples = g_width * g_height *  g_options.m_numSamples;
			else
				m_numSamples = g_width * g_height;
			CFloat percentage;

			static int m_numVerts = 0;
			static int m_fps = 0;

			CChar temp[MAX_NAME_SIZE];
			CInt fps = (CInt)g_timer->GetFps(m_timerCounter);

			if( m_totalElapsedTime >= 0.5 ) //Every 0.5 seconds update the FPS
			{
				ResetTimer();
				m_numVerts = g_numVerts;
				m_fps = fps;
			}
			
			sprintf(temp, "FPS : %i", m_fps);
			g_font->Print("---Statistics---", 10.0f, 990.0, 0.0f, 0.85f, 0.67f, 0.0f);
			g_font->Print(temp, 10.0f, 960.0, 0.0f, 0.85f, 0.67f, 0.0f);
			sprintf(temp, "Rendered Vertexes : %i", m_numVerts);
			g_font->Print(temp, 10.0f, 930.0, 0.0f, 0.85f, 0.67f, 0.0f);
			sprintf(temp, "Rendered Triangles : %i", m_numVerts / 3);
			g_font->Print(temp, 10.0f, 900, 0.0f, 0.85f, 0.67f, 0.0f);
			g_font->Print("Object -------------------- Samples", 10.0f, 870, 0.0f, 0.85f, 0.67f, 0.0f);
			CInt index = 0;
			for (CUInt i = 0; i < g_instancePrefab.size(); i++)
			{
				if (g_instancePrefab[i]->GetIsTrigger() || Cmp(g_instancePrefab[i]->GetName(), "VANDA_MAIN_CHARACTER") || !g_instancePrefab[i]->GetVisible())
					continue;

				CBool renderForWater = CFalse;
				if (g_instancePrefab[i]->GetWater())
				{
					if (!g_instancePrefab[i]->GetWater()->GetOutsideFrustom())
					{
						if (g_instancePrefab[i]->GetRenderForWaterQuery())
						{
							renderForWater = CTrue;
						}
					}
				}
				if (!g_instancePrefab[i]->GetRenderForQuery() && !renderForWater)
					continue;

				percentage = ((CFloat)g_instancePrefab[i]->GetResult() / (CFloat)m_numSamples) * 100.0f;

				if (index <= 20)
				{
					char temp[MAX_NAME_SIZE];
					sprintf(temp, "%s LOD Percentage: %.2f", g_instancePrefab[i]->GetName(), percentage);
					g_font->Print(temp, 10.0f, 870.0f - (index + 1) * 30, 0.0f, 0.85f, 0.67f, 0.0f);
				}
				else
				{
					break;
				}
				index++;
			}
			for (CUInt i = 0; i < g_engineLights.size(); i++)
			{
				char temp[200];
				if (g_engineLights[i]->GetRunTimeVisible())
					sprintf(temp, "%s : %s", g_engineLights[i]->m_abstractLight->GetName(), "Visible"/*g_engineLights[i]->GetLODPercent()*/);
				else
					sprintf(temp, "%s : %s", g_engineLights[i]->m_abstractLight->GetName(), "Invisible"/*g_engineLights[i]->GetLODPercent()*/);
				g_font->Print(temp, 10.0f, 870.0f - (index + i + 1) * 30, 0.0f, g_engineLights[i]->m_abstractLight->GetDiffuse()[0], g_engineLights[i]->m_abstractLight->GetDiffuse()[1], g_engineLights[i]->m_abstractLight->GetDiffuse()[2]);
			}

			for (CUInt i = 0; i < g_engineWaters.size(); i++)
			{
				if (g_engineWaters[i]->GetOutsideFrustom()) continue;
				if (CTrue/*g_currentCameraType == eCAMERA_PHYSX*/)
					if (!g_engineWaters[i]->GetVisible())
						continue;
				percentage = ((CFloat)g_engineWaters[i]->GetResult() / (CFloat)m_numSamples) * 100.0f;

				char temp[200];
				sprintf(temp, "%s LOD Percentage: %.2f", g_engineWaters[i]->GetName(), percentage);
				g_font->Print(temp, 10.0f, 870.0f - (index + g_engineLights.size() + i + 1) * 30, 0.0f, 0.65f, 0.5f, 0.65f);
			}

			for (CUInt i = 0; i < g_instancePrefab.size(); i++)
			{
				if (i <= 25)
				{
					char temp[200];
					sprintf(temp, "%s Distance: %.2f", g_instancePrefab[i]->GetName(), g_instancePrefab[i]->GetDistanceFromCamera());
					g_font->Print(temp, 140.0, 1020.0 - (i + 1) * 30, 0.0f, 0.85f, 0.85f, 0.0f);
				}
				else
				{
					break;
				}
			}

		}
	}
	//if( g_databaseVariables.m_showStatistics )
	//{
	//}

	g_font->EndRendering();

	for (CUInt i = 0; i < g_guis.size(); i++)
	{
		if (g_guis[i]->GetVisible())
		{
			for (CUInt j = 0; j < g_guis[i]->m_guiBackgrounds.size(); j++)
			{
				g_guis[i]->m_guiBackgrounds[j]->Render();
			}
			for (CUInt j = 0; j < g_guis[i]->m_guiButtons.size(); j++)
			{
				g_guis[i]->m_guiButtons[j]->Render();
			}
			for (CUInt j = 0; j < g_guis[i]->m_guiTexts.size(); j++)
			{
				g_guis[i]->m_guiTexts[j]->Render();
			}
		}
	}

	//Draw Cursor
	if (g_currentVSceneProperties.m_isMenu && !g_main->GetCursorIcon()->GetVisible())
	{
		CFloat cursorSize = ((CFloat)g_currentVSceneProperties.m_cursorSize * (CFloat)g_width) / 100.0f;
		CFloat halfCursorSize = cursorSize / 2.0f;
		glUseProgram(0);
		glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5f);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, g_width, g_height);
		g_render.ProjectionMatrix();
		g_render.PushMatrix();
		g_render.IdentityMatrix();
		gluOrtho2D(0.0, g_width, 0.0, g_height);
		g_render.ModelViewMatrix();
		g_render.PushMatrix();
		g_render.IdentityMatrix();
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		if (m_menuCursorImg)
		{
			glBindTexture(GL_TEXTURE_2D, m_menuCursorImg->GetId());
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		}
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(g_main->m_mousePosition.x - halfCursorSize, g_height - g_main->m_mousePosition.y + halfCursorSize - cursorSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(g_main->m_mousePosition.x - halfCursorSize + cursorSize, g_height - g_main->m_mousePosition.y + halfCursorSize - cursorSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(g_main->m_mousePosition.x - halfCursorSize + cursorSize, g_height - g_main->m_mousePosition.y + halfCursorSize);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(g_main->m_mousePosition.x - halfCursorSize, g_height - g_main->m_mousePosition.y + halfCursorSize);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		g_render.ProjectionMatrix();
		g_render.PopMatrix();

		g_render.ModelViewMatrix();
		g_render.PopMatrix();
		glPopAttrib(); //viewport
	}

	CVec2f pos(g_main->m_mousePosition.x, g_height - g_main->m_mousePosition.y);
	m_cursorIcon->Render(pos);
}

void CMain::ResetPhysX(CBool releaseActors)
{
	if( releaseActors )
	{
		CUInt actor_counts = gPhysXscene->getNbActors();
		for( CUInt i = 0; i < actor_counts; i++ )
		{
			if( !gPhysXscene->getActors()[i]->getName() ) continue;
			gPhysXscene->releaseActor( *gPhysXscene->getActors()[i] );
		}
	}
	//always release ground plane
	if (!releaseActors)
		g_nx->ReleaseGroundPlane();

	g_nx->gControllers->reportSceneChanged();

	//release character controller
	for( CUInt i = 0; i < gPhysXscene->getNbActors(); i++ )
	{
		if( !gPhysXscene->getActors()[i]->getName() )
		{
			gPhysXscene->releaseActor( *gPhysXscene->getActors()[i] );
			break;
		}
	}

	g_nx->ReleaseCharacterControllers();

	g_nx->m_defaultGravity.x = g_physXProperties.m_fGravityX;
	g_nx->m_defaultGravity.y = g_physXProperties.m_fGravityY;
	g_nx->m_defaultGravity.z = g_physXProperties.m_fGravityZ;
	gPhysicsSDK->setParameter( NX_SKIN_WIDTH, g_physXProperties.m_fDefaultSkinWidth );

	NxMaterial* defaultMaterial = gPhysXscene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(g_physXProperties.m_fDefaultRestitution);
	defaultMaterial->setStaticFriction(g_physXProperties.m_fDefaultStaticFriction);
	defaultMaterial->setDynamicFriction(g_physXProperties.m_fDefaultDynamicFriction);
	if (g_physXProperties.m_bGroundPlane)
		g_nx->CreateGroundPlane(g_physXProperties.m_fGroundHeight);
	g_nx->InitCharacterControllers(g_nx->gCharacterPos.x, g_nx->gCharacterPos.y, g_nx->gCharacterPos.z, g_physXProperties.m_fCapsuleRadius, g_physXProperties.m_fCapsuleHeight, g_physXProperties.m_fCharacterSkinWidth, g_physXProperties.m_fCharacterSlopeLimit, g_physXProperties.m_fCharacterStepOffset);
	
	if( g_physXProperties.m_bApplyGravity )
	{
		g_nx->m_defaultGravity = NxVec3( g_physXProperties.m_fGravityX, g_physXProperties.m_fGravityY, g_physXProperties.m_fGravityZ );
		gPhysXscene->setGravity(NxVec3(g_nx->m_defaultGravity.x, g_nx->m_defaultGravity.y, g_nx->m_defaultGravity.z));
	}
	else
	{
		g_nx->m_defaultGravity = NxVec3(0.0f);
		gPhysXscene->setGravity(NxVec3(0.0f));
	}
	g_nx->gDesiredDistance = g_physXProperties.m_fCameraCharacterDistance;
	g_nx->debugMode = g_physXProperties.m_bDebugMode;
	g_nx->gCharacterWalkSpeed = g_physXProperties.m_fCharacterWalkSpeed;
	g_nx->gCharacterRunSpeed = g_physXProperties.m_fCharacterRunSpeed;

	if( g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX || g_currentCameraType == eCAMERA_COLLADA || g_currentCameraType == eCAMERA_ENGINE)
	{
		if( gPhysXscene )
		{
		  // Run collision and dynamics for delta time since the last frame
			g_nx->gControllers->reportSceneChanged();
			gPhysXscene->simulate(1.0f / 60.0f /*elapsedTime*/);
			gPhysXscene->flushStream();
			gPhysXscene->fetchResults(NX_ALL_FINISHED, true);
		}
	}


}

CVoid CMain::ManageLODs()
{
	CInt m_numSamples;
	if (g_window.m_windowGL.multiSampling)
		m_numSamples = g_width * g_height *  g_options.m_numSamples;
	else
		m_numSamples = g_width * g_height;

	//CInt index = 0;
	//m_bQuery = CFalse;
	//for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	//{
	//	GLint resultAvailable = GL_FALSE;
	//	glGetQueryObjectiv(g_instancePrefab[i]->GetQueryIndex(), GL_QUERY_RESULT_AVAILABLE, &resultAvailable);

	//	if (resultAvailable)
	//		index++;
	//}
	//CInt numLights = 0;
	//for (CUInt i = 0; i < g_engineLights.size(); i++)
	//{
	//	if (Cmp(g_shadowProperties.m_directionalLightName, g_engineLights[i]->m_abstractLight->GetName()))
	//		continue;
	//	numLights++;
	//	GLint resultAvailable = GL_FALSE;
	//	glGetQueryObjectiv(g_engineLights[i]->GetQueryIndex(), GL_QUERY_RESULT_AVAILABLE, &resultAvailable);

	//	if (resultAvailabrenle)
	//		index++;
	//}

	//if (index == g_instancePrefab.size() + numLights)
	//{
		//all results are available
		m_bQuery = CTrue;
	//}

	if (m_bQuery)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (g_instancePrefab[i]->GetIsTrigger() || Cmp(g_instancePrefab[i]->GetName(), "VANDA_MAIN_CHARACTER") || !g_instancePrefab[i]->GetVisible())
				continue;

			CBool renderForWater = CFalse;
			if (g_instancePrefab[i]->GetWater())
			{
				if (!g_instancePrefab[i]->GetWater()->GetOutsideFrustom())
				{
					if (g_instancePrefab[i]->GetRenderForWaterQuery())
					{
						renderForWater = CTrue;
					}
				}
			}

			if (!g_instancePrefab[i]->GetRenderForQuery() && !renderForWater)
				continue;

			GLint result;
			glGetQueryObjectiv(g_instancePrefab[i]->GetQueryIndex(), GL_QUERY_RESULT, &result);
			g_instancePrefab[i]->SetResult(result);
			CFloat percentage;
			percentage = ((CFloat)g_instancePrefab[i]->GetResult() / (CFloat)m_numSamples) * 100.0f;

			if (result == 0 && g_instancePrefab[i]->GetDistanceFromCamera() > g_instancePrefab[i]->GetRadius() + g_instancePrefabLODPercent.m_lod1)
			{
				g_instancePrefab[i]->SetSceneVisible(0, CFalse);
				g_instancePrefab[i]->SetSceneVisible(1, CFalse);
				g_instancePrefab[i]->SetSceneVisible(2, CFalse);

				continue;
			}

			if (percentage >= g_instancePrefabLODPercent.m_lod1 || (g_instancePrefab[i]->GetDistanceFromCamera() <= (g_instancePrefab[i]->GetRadius() + g_instancePrefabLODPercent.m_lod1MinObjectCameraDistance)))
			{
				if (g_instancePrefab[i]->GetElapsedTime() >= 0.01f)
				{
					if (g_instancePrefab[i]->GetSceneVisible(1) && g_instancePrefab[i]->GetScene(1) && g_instancePrefab[i]->GetScene(1)->m_hasAnimation)
					{
						for (CUInt j = 0; j < g_instancePrefab[i]->GetScene(1)->m_animationClips.size(); j++)
						{
							if (g_instancePrefab[i]->GetScene(0))
							{
								g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->SetCurrentTime(g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->GetCurrentAnimationTime());
								g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->SetCurrentDelayInTime(g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->GetCurrentDelayInTime());
								g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->SetCurrentDelayOutTime(g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->GetCurrentDelayOutTime());
								g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->SetCurrentWeight(g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->GetCurrentWeight());
							}
						}
					}
					else if (g_instancePrefab[i]->GetSceneVisible(2) && g_instancePrefab[i]->GetScene(2) && g_instancePrefab[i]->GetScene(2)->m_hasAnimation)
					{
						for (CUInt j = 0; j < g_instancePrefab[i]->GetScene(2)->m_animationClips.size(); j++)
						{
							if (g_instancePrefab[i]->GetScene(0))
							{
								g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->SetCurrentTime(g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->GetCurrentAnimationTime());
								g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->SetCurrentDelayInTime(g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->GetCurrentDelayInTime());
								g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->SetCurrentDelayOutTime(g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->GetCurrentDelayOutTime());
								g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->SetCurrentWeight(g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->GetCurrentWeight());
							}
						}
					}

					g_instancePrefab[i]->SetSceneVisible(0, CTrue);
					g_instancePrefab[i]->SetSceneVisible(1, CFalse);
					g_instancePrefab[i]->SetSceneVisible(2, CFalse);
					g_instancePrefab[i]->ResetElapsedTime();
				}
			}
			else if (percentage >= g_instancePrefabLODPercent.m_lod2.x && percentage < g_instancePrefabLODPercent.m_lod2.y)
			{
				if (g_instancePrefab[i]->GetElapsedTime() >= 0.01f)
				{
					if (g_instancePrefab[i]->GetSceneVisible(0) && g_instancePrefab[i]->GetScene(0) && g_instancePrefab[i]->GetScene(0)->m_hasAnimation)
					{
						for (CUInt j = 0; j < g_instancePrefab[i]->GetScene(0)->m_animationClips.size(); j++)
						{
							if (g_instancePrefab[i]->GetScene(1))
							{
								g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentTime(g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->GetCurrentAnimationTime());
								g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentDelayInTime(g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->GetCurrentDelayInTime());
								g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentDelayOutTime(g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->GetCurrentDelayOutTime());
								g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentWeight(g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->GetCurrentWeight());
							}
						}
					}
					else if (g_instancePrefab[i]->GetSceneVisible(2) && g_instancePrefab[i]->GetScene(2) && g_instancePrefab[i]->GetScene(2)->m_hasAnimation)
					{
						for (CUInt j = 0; j < g_instancePrefab[i]->GetScene(2)->m_animationClips.size(); j++)
						{
							if (g_instancePrefab[i]->GetScene(1))
							{
								g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentTime(g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->GetCurrentAnimationTime());
								g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentDelayInTime(g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->GetCurrentDelayInTime());
								g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentDelayOutTime(g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->GetCurrentDelayOutTime());
								g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentWeight(g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->GetCurrentWeight());
							}
						}
					}

					if (g_instancePrefab[i]->GetPrefab()->GetHasLod(1))
					{
						g_instancePrefab[i]->SetSceneVisible(0, CFalse);
						g_instancePrefab[i]->SetSceneVisible(1, CTrue);
						g_instancePrefab[i]->SetSceneVisible(2, CFalse);
						g_instancePrefab[i]->ResetElapsedTime();
					}
				}
			}
			else if (percentage > g_instancePrefabLODPercent.m_lod3.x && percentage < g_instancePrefabLODPercent.m_lod3.y)
			{
				if (g_instancePrefab[i]->GetElapsedTime() >= 0.01f)
				{
					if (!g_instancePrefab[i]->GetSceneVisible(0) && !g_instancePrefab[i]->GetSceneVisible(1) && !g_instancePrefab[i]->GetSceneVisible(2))
					{
						if (g_instancePrefab[i]->GetPrefab()->GetHasLod(2))
						{
							g_instancePrefab[i]->SetSceneVisible(0, CFalse);
							g_instancePrefab[i]->SetSceneVisible(1, CFalse);
							g_instancePrefab[i]->SetSceneVisible(2, CTrue);
							g_instancePrefab[i]->ResetElapsedTime();
						}
						else if (g_instancePrefab[i]->GetPrefab()->GetHasLod(1))
						{
							g_instancePrefab[i]->SetSceneVisible(0, CFalse);
							g_instancePrefab[i]->SetSceneVisible(1, CTrue);
							g_instancePrefab[i]->SetSceneVisible(2, CFalse);
							g_instancePrefab[i]->ResetElapsedTime();
						}
						else if (g_instancePrefab[i]->GetPrefab()->GetHasLod(0))
						{
							g_instancePrefab[i]->SetSceneVisible(0, CTrue);
							g_instancePrefab[i]->SetSceneVisible(1, CFalse);
							g_instancePrefab[i]->SetSceneVisible(2, CFalse);
							g_instancePrefab[i]->ResetElapsedTime();
						}
					}
					if (g_instancePrefab[i]->GetPrefab()->GetHasLod(2))
					{
						if (g_instancePrefab[i]->GetSceneVisible(1))
						{
							if (g_instancePrefab[i]->GetScene(1) && g_instancePrefab[i]->GetScene(1)->m_hasAnimation)
							{
								for (CUInt j = 0; j < g_instancePrefab[i]->GetScene(1)->m_animationClips.size(); j++)
								{
									if (g_instancePrefab[i]->GetScene(2))
									{
										g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->SetCurrentTime(g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->GetCurrentAnimationTime());
										g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->SetCurrentDelayInTime(g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->GetCurrentDelayInTime());
										g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->SetCurrentDelayOutTime(g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->GetCurrentDelayOutTime());
										g_instancePrefab[i]->GetScene(2)->m_animationClips[j]->SetCurrentWeight(g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->GetCurrentWeight());
									}
								}
							}

							g_instancePrefab[i]->SetSceneVisible(0, CFalse);
							g_instancePrefab[i]->SetSceneVisible(1, CFalse);
							g_instancePrefab[i]->SetSceneVisible(2, CTrue);
							g_instancePrefab[i]->ResetElapsedTime();
						}
					}
					else if (g_instancePrefab[i]->GetSceneVisible(0))
					{
						if (g_instancePrefab[i]->GetPrefab()->GetHasLod(1))
						{
							if (g_instancePrefab[i]->GetScene(0) && g_instancePrefab[i]->GetScene(0)->m_hasAnimation)
							{
								for (CUInt j = 0; j < g_instancePrefab[i]->GetScene(0)->m_animationClips.size(); j++)
								{
									if (g_instancePrefab[i]->GetScene(1))
									{
										g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentTime(g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->GetCurrentAnimationTime());
										g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentDelayInTime(g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->GetCurrentDelayInTime());
										g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentDelayOutTime(g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->GetCurrentDelayOutTime());
										g_instancePrefab[i]->GetScene(1)->m_animationClips[j]->SetCurrentWeight(g_instancePrefab[i]->GetScene(0)->m_animationClips[j]->GetCurrentWeight());
									}
								}
							}

							g_instancePrefab[i]->SetSceneVisible(0, CFalse);
							g_instancePrefab[i]->SetSceneVisible(1, CTrue);
							g_instancePrefab[i]->SetSceneVisible(2, CFalse);
							g_instancePrefab[i]->ResetElapsedTime();
						}
					}

				}
			}
			//Make sure objects are always visible in free camera

			g_instancePrefab[i]->SetElapsedTime(g_elapsedTime);

			//Make sure objects are always visible in free camera
			if (g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX)
			{
				if (!g_instancePrefab[i]->GetSceneVisible(0) && !g_instancePrefab[i]->GetSceneVisible(1) && !g_instancePrefab[i]->GetSceneVisible(2))
				{
					for (CUInt j = 2; j >= 0; j--)
					{
						if (g_instancePrefab[i]->GetPrefab() && g_instancePrefab[i]->GetPrefab()->GetHasLod(j))
						{
							CScene* scene = g_instancePrefab[i]->GetScene(j);
							if (!scene) continue;

							g_instancePrefab[i]->SetSceneVisible(j, CTrue);
							break;
						}
					}
				}
			}

		}

		//for (CUInt i = 0; i < g_engineLights.size(); i++)
		//{
		//	if (Cmp(g_shadowProperties.m_directionalLightName, g_engineLights[i]->m_abstractLight->GetName()))
		//		continue;

		//	if (g_engineLights[i]->GetRunTimeVisible())
		//	{
		//		GLint result;
		//		glGetQueryObjectiv(g_engineLights[i]->GetQueryIndex(), GL_QUERY_RESULT, &result);
		//		CFloat percentage;
		//		percentage = ((CFloat)result / (CFloat)m_numSamples) * 100.0f;

		//		g_engineLights[i]->SetLODPercent(percentage);
		//	}
		//	else
		//	{
		//		g_engineLights[i]->SetLODPercent(0.0f);
		//	}
		//}

		for (CUInt i = 0; i < g_engineWaters.size(); i++)
		{
			if (g_engineWaters[i]->GetOutsideFrustom()) continue;
			GLint result;
			glGetQueryObjectiv(g_engineWaters[i]->GetQueryIndex(), GL_QUERY_RESULT, &result);
			g_engineWaters[i]->SetResult(result);
			if (result == 0)
				g_engineWaters[i]->SetQueryVisible(CFalse);
			else
				g_engineWaters[i]->SetQueryVisible(CTrue);
		}

	}
}

CVoid CMain::Draw3DObjects()
{
	if (!g_useOldRenderingStyle && g_options.m_enableFBO && g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
	{
		if (!g_useOldRenderingStyle && g_window.m_windowGL.multiSampling && g_options.m_numSamples && g_options.m_enableFBO)
			g_render.BindForWriting(m_mFboID);
		else if (!g_useOldRenderingStyle && g_options.m_enableFBO)
			g_render.BindForWriting(m_fboID);

		if (g_engineLights.size() == 0)
			SetDefaultLight();

		g_main->RenderBakedOctree3DModels();
		g_main->Render3DAnimatedModels(CTrue);
		g_main->Render3DModelsControlledByPhysX();
		g_main->RenderCharacter(CFalse);

		++g_numLights;
		//g_octree->ResetOctreeGeoCount();
	} //if !g_useOldRenderingStyle
}

CVoid CMain::SetDefaultLight()
{
	glEnable(GL_LIGHT0);	 //just for per vertex lighting 	

	//This is the properties of the camera light
	GLfloat light_pos0[4] = { g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z, 1.0f };

	GLfloat light_ambient0[4] = { 0.5f, 0.5f, 0.5f, 0.0f };
	GLfloat light_diffuse0[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat light_specular0[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light_shininess0 = 100.0f;

	glLightfv( GL_LIGHT0, GL_POSITION, light_pos0  );
	glLightfv( GL_LIGHT0, GL_AMBIENT , light_ambient0 );
	glLightfv( GL_LIGHT0, GL_DIFFUSE , light_diffuse0 );
	glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular0 );
	glLightf ( GL_LIGHT0, GL_SHININESS, light_shininess0 );
	glLightf ( GL_LIGHT0, GL_SPOT_CUTOFF,(GLfloat)180.0f );
	glLightf ( GL_LIGHT0, GL_CONSTANT_ATTENUATION , (GLfloat)1.0f );
	glLightf ( GL_LIGHT0, GL_LINEAR_ATTENUATION,	(GLfloat)0.0f );
	glLightf ( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, (GLfloat)0.0f );
		
	g_numLights++;
}

CVoid CMain::BlendFogWithScene()
{
	if( !g_useOldRenderingStyle && g_window.m_windowGL.multiSampling && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForReading( m_mFboID );
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting( m_fboID );
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		g_render.BindForReading( m_mFboIDFogDof );
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting( m_fboIDFogDof );
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, m_textureFogDof);

	g_render.BindForWriting( m_fboIDSum );

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(g_render.m_blendTexturesProgram);
	glUniform1i( glGetUniformLocation(g_render.m_blendTexturesProgram, "tex_unit_0"), 0 );
	glUniform1i( glGetUniformLocation(g_render.m_blendTexturesProgram, "tex_unit_1"), 1 );

	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
	gluOrtho2D(0, g_width, 0, g_height);
	glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity();
	glBegin(GL_QUADS);
	glTexCoord2i(0,	0);	glVertex2i(0, 0); 
	glTexCoord2i(1, 0);	glVertex2i(g_width, 0);
	glTexCoord2i(1, 1);	glVertex2i(g_width, g_height);
	glTexCoord2i(0, 1);	glVertex2i(0, g_height);
	glEnd();
	glFlush();

	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	//glActiveTextureARB(GL_TEXTURE0_ARB);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTextureARB(GL_TEXTURE1_ARB);
	//glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	g_render.BindFBO(0);

	//copy  sum to the default FBO
	g_render.BindForReading( m_fboIDSum );
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	if( !g_useOldRenderingStyle && g_window.m_windowGL.multiSampling && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForWriting( m_mFboID );
	}
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
	{
		g_render.BindForWriting( m_fboID );
	}
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	if ( g_render.UsingFBOs() && g_options.m_enableFBO )
		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

CVoid CMain::ResetPhysXCounts()
{
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			g_scene[i]->m_instanceGeometries[j]->m_physXCount = 0;
	}
}

CImage *CWater::GetWaterImage( const CChar * name )
{
	if (name == NULL) return NULL;
	for(CUInt i=0; i<g_waterImages.size(); i++)
	{
		if ( ICmp(GetAfterPath(g_waterImages[i]->GetName()), name ) )
			return g_waterImages[i];
	}
	return NULL;
}

CVoid CMain::UpdatePrefabInstanceBB()
{
	for (CUInt i = 0; i < g_instancePrefab.size(); i++)
	{
		CBool foundDynamicActorTarget = CFalse;

		if (!g_instancePrefab[i]->GetVisible())
		{
			if (g_instancePrefab[i]->GetIsControlledByPhysX())
			{
				for (CUInt s = 0; s < 3; s++)
				{
					if (g_instancePrefab[i]->GetPrefab()->GetHasLod(s))
					{
						CScene* scene = g_instancePrefab[i]->GetScene(s);
						if (!scene) continue;
						if (!scene->m_isTrigger)
						{
							for (CUInt j = 0; j < scene->m_instanceGeometries.size(); j++)
							{
								if (scene->m_instanceGeometries[j]->GetHasPhysXActor() && scene->m_instanceGeometries[j]->GetPhysXActorDensity() > 0.0f)
								{
									for (CUInt k = 0; k < gPhysXscene->getNbActors(); k++)
									{
										//Start Of Load/Unload PhysX Actors
										if (gPhysXscene->getActors()[k]->isDynamic() && !gPhysXscene->getActors()[k]->isSleeping())
										{
											CChar actorName[MAX_NAME_SIZE];
											if (!gPhysXscene->getActors()[k]->getName()) continue; //main character
											Cpy(actorName, gPhysXscene->getActors()[k]->getName());
											if (Cmp(scene->m_instanceGeometries[j]->GetPhysXActorName(), actorName))
											{
												foundDynamicActorTarget = CTrue;
											}
										}
										if (foundDynamicActorTarget)
											break;
									}
								}
								if (foundDynamicActorTarget)
									break;
							}
						}
					}
					if (foundDynamicActorTarget)
						break;
				}
			}
		}

		if (!foundDynamicActorTarget && !g_instancePrefab[i]->GetVisible()) continue;

		if (g_instancePrefab[i]->GetIsAnimated() || g_instancePrefab[i]->GetIsControlledByPhysX())
		{
			if (g_camera->m_cameraManager->IsBoxInFrustum(&g_instancePrefab[i]->m_boundingBox[0], 8))
			{
				g_instancePrefab[i]->SetRenderForQuery(CTrue);
			}
			else
			{
				g_instancePrefab[i]->SetRenderForQuery(CFalse);
			}
		}

		g_currentInstancePrefab = g_instancePrefab[i];
		CBool foundTarget = CFalse;
		CScene* scene = NULL;

		CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
		for (CUInt j = 0; j < 3; j++)
		{
			if (prefab && prefab->GetHasLod(j))
			{
				scene = g_instancePrefab[i]->GetScene(j);

				if (!scene) continue;
				if (scene->GetUpdateBB())
				{
					g_instancePrefab[i]->UpdateBoundingBox(CFalse);
					scene->SetUpdateBB(CFalse);
					foundTarget = CTrue;
					break;
				}
			}
		}

		if (foundTarget)
			continue;
	}
}

CVoid CMain::InitGUISelection()
{
	GLint viewport[4];
	glSelectBuffer(MAX_NAME_SIZE, m_guiBuffer);
	(void)glRenderMode(GL_SELECT);
	g_render.ProjectionMatrix();
	g_render.PushMatrix();
	g_render.IdentityMatrix();
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluPickMatrix((CDouble)m_mousePosition.x - 3.0, (CDouble)g_height - (CDouble)m_mousePosition.y - 3.0, 6.0, 6.0, viewport);

	glOrtho(0.0, g_width, 0.0, g_height, -100.0, 100.0);

	g_render.ModelViewMatrix();
	g_render.PushMatrix();
	g_render.IdentityMatrix();

	glInitNames();
}

CVoid CMain::FinishGUISelection()
{
	g_render.ProjectionMatrix();
	g_render.PopMatrix();
	g_render.ModelViewMatrix();
	g_render.PopMatrix();

	glFlush();
	GLint hits = glRenderMode(GL_RENDER);

	if (hits > 0)
	{
		m_guiPtr = m_guiBuffer;
		for (CInt j = 0; j < hits; j++)
		{
			m_guiPtr++; //skip the first info ( number of names which is one in my engine )and move to the min depth info

			if (*m_guiPtr < m_guiMinZ)
			{
				m_guiMinZ = *m_guiPtr;

				m_guiSelectedName = *(m_guiPtr + 2); //name of the object
			}
			m_guiPtr += 3; //move to the next hit
		}
	}
}
//Returns -1 if no selection occurs
CUInt CMain::GetSelectedGUI()
{
	m_guiMinZ = 0xffffffff;
	m_guiSelectedName = -1;
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, g_width, g_height);// resets the viewport to new dimensions.

	InitGUISelection();
	//render here
	for (CUInt i = 0; i < g_guis.size(); i++)
	{
		if (g_guis[i]->GetVisible())
		{
			for (CUInt j = 0; j < g_guis[i]->m_guiBackgrounds.size(); j++)
			{
				g_guis[i]->m_guiBackgrounds[j]->Render(CTrue);
			}
			for (CUInt j = 0; j < g_guis[i]->m_guiButtons.size(); j++)
			{
				g_guis[i]->m_guiButtons[j]->Render(CTrue);
			}
			for (CUInt j = 0; j < g_guis[i]->m_guiTexts.size(); j++)
			{
				g_guis[i]->m_guiTexts[j]->Render(CTrue);
			}
		}
	}
	FinishGUISelection();

	glPopAttrib();

	return m_guiSelectedName;
}

void CMain::GenerateMenuCursorTexture(char* fileName)
{
	DeleteMenuCursorTexture();

	m_menuCursorImg = CNew(CImage);

	Cpy(m_strMenuCursorImg, fileName);

	if (!CTexture::LoadDDSTexture(m_menuCursorImg, m_strMenuCursorImg, NULL))
		MessageBox(NULL, _T("GenerateMenuCursorTexture>Couldn't load the cursor image"), _T("VandaEngine Error"), MB_OK);

	m_menuCursorImg->SetFileName(GetAfterPath(m_strMenuCursorImg));
}

void CMain::DeleteMenuCursorTexture()
{
	CDelete(m_menuCursorImg);
}

CFloat CMain::GetCursorX()
{
	CFloat posX = (CFloat)g_main->m_mousePosition.x;
	return posX;
}

CFloat CMain::GetCursorY()
{
	CFloat posY = CFloat(g_height - g_main->m_mousePosition.y);
	return posY;
}