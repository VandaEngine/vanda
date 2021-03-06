//Copyright (C) 2021 Ehsan Kamrani 
//This file is licensed and distributed under MIT license

// PerspectiveWindow.cpp : implementation file
//

#include "stdafx.h"
#include "PerspectiveWindow.h"
#include "opengl.h"
#include "render.h"
#include "scene.h"
#include "fog.h"
#include "..\VandaEngine1.h"
#include "..\VandaEngine1Dlg.h"
#include "Animation.h"
#include <thread>
#include "imageLib.h"

// CMultipleWindows
CInt g_numLights = 0;
CInt g_totalLights = 0;
CBool g_fogBlurPass = CFalse;
IMPLEMENT_DYNAMIC(CMultipleWindows, CWnd)

//Vanda C Functions to be used in Lua scripts
CInt PlaySoundLoop(lua_State *L)
{
	if( g_testScript )
		return 0;
	int argc = lua_gettop(L);

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (int n = 1; n <= argc; ++n)
		{
			CBool foundTarget = CFalse;

			CChar luaToString[MAX_NAME_SIZE];
			Cpy(luaToString, lua_tostring(L, n));
			StringToUpper(luaToString);

			for (CUInt pr = 0; pr < g_projects.size(); pr++)
			{
				for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
				{
					for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames.size(); j++)
					{
						CChar staticSoundName[MAX_NAME_SIZE];
						Cpy(staticSoundName, g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames[j].c_str());
						StringToUpper(staticSoundName);

						if (Cmp(staticSoundName, luaToString))
						{
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\nProject '%s', VScene '%s' : Sound '%s' will be played continiously", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames[j].c_str());
							PrintInfo(temp, COLOR_GREEN);
							foundTarget = CTrue;
							break;
						}
					}
				}
			}
			if (!foundTarget)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nPlaySoundLoop() Error: Couldn't find Sound '", lua_tostring(L, n), "' to be played.");
				PrintInfo(temp, COLOR_RED);
			}
		}
		return 0;
	}

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
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nSound '", g_engineStaticSounds[i]->GetName(), "' is playing continiously.");
				PrintInfo(temp, COLOR_GREEN);
				foundTarget = CTrue;
				break;
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nPlaySoundLoop() Error: Couldn't find Sound '", lua_tostring(L, n), "' to be played.");
			PrintInfo(temp, COLOR_RED);
		}
	}

	return 0; // number of return values
}

CInt PlaySoundOnce(lua_State *L)
{
	if (g_testScript)
		return 0;
	int argc = lua_gettop(L);

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (int n = 1; n <= argc; ++n)
		{
			CBool foundTarget = CFalse;

			CChar luaToString[MAX_NAME_SIZE];
			Cpy(luaToString, lua_tostring(L, n));
			StringToUpper(luaToString);

			for (CUInt pr = 0; pr < g_projects.size(); pr++)
			{
				for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
				{
					for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames.size(); j++)
					{
						CChar staticSoundName[MAX_NAME_SIZE];
						Cpy(staticSoundName, g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames[j].c_str());
						StringToUpper(staticSoundName);

						if (Cmp(staticSoundName, luaToString))
						{
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\nProject '%s', VScene '%s' : Sound '%s' will be played once", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames[j].c_str());
							PrintInfo(temp, COLOR_GREEN);
							foundTarget = CTrue;
							break;
						}
					}
				}
			}
			if (!foundTarget)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nPlaySoundOnce() Error: Couldn't find Sound '", lua_tostring(L, n), "' to be played.");
				PrintInfo(temp, COLOR_RED);
			}
		}
		return 0;
	}

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
				g_engineStaticSounds[i]->SetLoop( CFalse );
				g_engineStaticSounds[i]->SetPlay(CTrue);
				g_soundSystem->PlayALSound(*(g_engineStaticSounds[i]->GetSoundSource()));
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nSound '", g_engineStaticSounds[i]->GetName(), "' is playing once.");
				PrintInfo(temp, COLOR_GREEN);
				foundTarget = CTrue;
				break;
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nPlaySoundOnce() Error: Couldn't find Sound '", lua_tostring(L, n), "' to be played.");
			PrintInfo(temp, COLOR_RED);
		}
	}

	return 0; // number of return values
}


CInt PauseSound(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (int n = 1; n <= argc; ++n)
		{
			CBool foundTarget = CFalse;

			CChar luaToString[MAX_NAME_SIZE];
			Cpy(luaToString, lua_tostring(L, n));
			StringToUpper(luaToString);

			for (CUInt pr = 0; pr < g_projects.size(); pr++)
			{
				for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
				{
					for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames.size(); j++)
					{
						CChar staticSoundName[MAX_NAME_SIZE];
						Cpy(staticSoundName, g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames[j].c_str());
						StringToUpper(staticSoundName);

						if (Cmp(staticSoundName, luaToString))
						{
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\nProject '%s', VScene '%s' : Sound '%s' will pause", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames[j].c_str());
							PrintInfo(temp, COLOR_GREEN);
							foundTarget = CTrue;
							break;
						}
					}
				}
			}
			if (!foundTarget)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nPauseSound() Error: Couldn't find Sound '", lua_tostring(L, n), "' to be paused.");
				PrintInfo(temp, COLOR_RED);
			}
		}
		return 0;
	}

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
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nSound '", g_engineStaticSounds[i]->GetName(), "' was paused.");
				PrintInfo(temp, COLOR_GREEN);
				foundTarget = CTrue;
				break;
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nPauseSound() Error: Couldn't find Sound '", lua_tostring(L, n), "' to be paused.");
			PrintInfo(temp, COLOR_RED);
		}

	}
	return 0; // number of return values
}

CInt StopSound(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (int n = 1; n <= argc; ++n)
		{
			CBool foundTarget = CFalse;

			CChar luaToString[MAX_NAME_SIZE];
			Cpy(luaToString, lua_tostring(L, n));
			StringToUpper(luaToString);

			for (CUInt pr = 0; pr < g_projects.size(); pr++)
			{
				for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
				{
					for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames.size(); j++)
					{
						CChar staticSoundName[MAX_NAME_SIZE];
						Cpy(staticSoundName, g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames[j].c_str());
						StringToUpper(staticSoundName);

						if (Cmp(staticSoundName, luaToString))
						{
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\nProject '%s', VScene '%s' : Sound '%s' will stop", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_staticSoundsNames[j].c_str());
							PrintInfo(temp, COLOR_GREEN);
							foundTarget = CTrue;
							break;
						}
					}
				}
			}
			if (!foundTarget)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nStopSound() Error: Couldn't find Sound '", lua_tostring(L, n), "' to be stopped.");
				PrintInfo(temp, COLOR_RED);
			}
		}
		return 0;
	}

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
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nSound '", g_engineStaticSounds[i]->GetName(), "' was stopped.");
				PrintInfo(temp, COLOR_GREEN);
				foundTarget = CTrue;
				break;
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nStopSound() Error: Couldn't find Sound '", lua_tostring(L, n), "' to be stopped.");
			PrintInfo(temp, COLOR_RED);
		}

	}
	return 0; // number of return values
}

CInt BlendCycle(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);
	if( argc < 4 )
	{
		PrintInfo( "\nPlease specify 4 arguments for BlendCycle()", COLOR_RED );
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
		if (g_editorMode == eMODE_VSCENE)
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
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
							PrintInfo(temp, COLOR_RED);
							return 0;
						}
						scene->BlendCycle(index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber(L, 4));
					}
				}
				if (!scene)
				{
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
			}
			else
			{
				PrintInfo("\nBlendCycle() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
				scene->BlendCycle(index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber(L, 4));
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						CBool foundAnimation = CFalse;
						foundPrefabInstance = CTrue;

						//animation clip name
						CChar luaToString2[MAX_NAME_SIZE];
						Cpy(luaToString2, lua_tostring(L, 2));
						StringToUpper(luaToString2);

						for (CUInt k = 0; k < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames.size(); k++)
						{
							CChar animationName[MAX_NAME_SIZE];
							Cpy(animationName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
							StringToUpper(animationName);

							if (Cmp(animationName, luaToString2))
							{
								foundAnimation = CTrue;

								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nBlendCycle() will execute for project '%s', VScene '%s', prefab Instance '%s', animation '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}

						if (!foundAnimation)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nBlendCycle() Error: project '%s', VScene '%s', prefab Instance '%s' has no animation called '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, lua_tostring(L, 2));
							PrintInfo(message, COLOR_RED);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "BlendCycle() Error : Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						PrintInfo(temp, COLOR_RED);
						return 0;
					}
					scene->BlendCycle(index, (CFloat)lua_tonumber(L, 3), (CFloat)lua_tonumber(L, 4));
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt ClearCycle(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		PrintInfo("\nPlease specify 3 arguments for ClearCycle()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
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
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
							PrintInfo(temp, COLOR_RED);
							return 0;
						}

						scene->ClearCycle(index, (CFloat)lua_tonumber(L, 3));
					}
				}
				if (!scene)
				{
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
			}
			else
			{
				PrintInfo("\nClearCycle() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}

				scene->ClearCycle(index, (CFloat)lua_tonumber(L, 3));
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						CBool foundAnimation = CFalse;
						foundPrefabInstance = CTrue;

						//animation clip name
						CChar luaToString2[MAX_NAME_SIZE];
						Cpy(luaToString2, lua_tostring(L, 2));
						StringToUpper(luaToString2);

						for (CUInt k = 0; k < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames.size(); k++)
						{
							CChar animationName[MAX_NAME_SIZE];
							Cpy(animationName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
							StringToUpper(animationName);

							if (Cmp(animationName, luaToString2))
							{
								foundAnimation = CTrue;

								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nClearCycle() will execute for project '%s', VScene '%s', prefab Instance '%s', animation '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}

						if (!foundAnimation)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nClearCycle() Error: project '%s', VScene '%s', prefab Instance '%s' has no animation called '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, lua_tostring(L, 2));
							PrintInfo(message, COLOR_RED);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "ClearCycle() Error: Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->ClearCycle(index, (CFloat)lua_tonumber(L, 3));
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt ExecuteAction(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for ExecuteAction()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
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
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
							PrintInfo(temp, COLOR_RED);
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
			}
			else
			{
				PrintInfo("\nExecuteAction() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
					PrintInfo(temp, COLOR_RED);
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
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						CBool foundAnimation = CFalse;
						foundPrefabInstance = CTrue;

						//animation clip name
						CChar luaToString2[MAX_NAME_SIZE];
						Cpy(luaToString2, lua_tostring(L, 2));
						StringToUpper(luaToString2);

						for (CUInt k = 0; k < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames.size(); k++)
						{
							CChar animationName[MAX_NAME_SIZE];
							Cpy(animationName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
							StringToUpper(animationName);

							if (Cmp(animationName, luaToString2))
							{
								foundAnimation = CTrue;

								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nExecuteAction() will execute for project '%s', VScene '%s', prefab Instance '%s', animation '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}

						if (!foundAnimation)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nExecuteAction() Error: project '%s', VScene '%s', prefab Instance '%s' has no animation called '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, lua_tostring(L, 2));
							PrintInfo(message, COLOR_RED);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "ExecuteAction() Error: Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt ReverseExecuteAction(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for ReverseExecuteAction()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
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
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
							PrintInfo(temp, COLOR_RED);
							return 0;
						}

						scene->ReverseExecuteAction(index);
					}
				}
				if (!scene)
				{
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
			}
			else
			{
				PrintInfo("\nReverseExecuteAction() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}

				scene->ReverseExecuteAction(index);
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						CBool foundAnimation = CFalse;
						foundPrefabInstance = CTrue;

						//animation clip name
						CChar luaToString2[MAX_NAME_SIZE];
						Cpy(luaToString2, lua_tostring(L, 2));
						StringToUpper(luaToString2);

						for (CUInt k = 0; k < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames.size(); k++)
						{
							CChar animationName[MAX_NAME_SIZE];
							Cpy(animationName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
							StringToUpper(animationName);

							if (Cmp(animationName, luaToString2))
							{
								foundAnimation = CTrue;

								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nReverseExecuteAction() will execute for project '%s', VScene '%s', prefab Instance '%s', animation '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}

						if (!foundAnimation)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nReverseExecuteAction() Error: project '%s', VScene '%s', prefab Instance '%s' has no animation called '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, lua_tostring(L, 2));
							PrintInfo(message, COLOR_RED);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "ReverseExecuteAction() Error: Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->ReverseExecuteAction(index);
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First argument: prefab instance name
//Second argument: animation clip name
CInt RemoveAction(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for RemoveAction()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
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
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
							PrintInfo(temp, COLOR_RED);
							return 0;
						}

						scene->RemoveAction(index);
					}
				}
				if (!scene)
				{
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
			}
			else
			{
				PrintInfo("\nRemoveAction() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}

				scene->RemoveAction(index);
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						CBool foundAnimation = CFalse;
						foundPrefabInstance = CTrue;

						//animation clip name
						CChar luaToString2[MAX_NAME_SIZE];
						Cpy(luaToString2, lua_tostring(L, 2));
						StringToUpper(luaToString2);

						for (CUInt k = 0; k < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames.size(); k++)
						{
							CChar animationName[MAX_NAME_SIZE];
							Cpy(animationName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
							StringToUpper(animationName);

							if (Cmp(animationName, luaToString2))
							{
								foundAnimation = CTrue;

								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nRemoveAction() will execute for project '%s', VScene '%s', prefab Instance '%s', animation '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}

						if (!foundAnimation)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nRemoveAction() Error: project '%s', VScene '%s', prefab Instance '%s' has no animation called '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, lua_tostring(L, 2));
							PrintInfo(message, COLOR_RED);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "RemoveAction() Error: Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
						PrintInfo(temp, COLOR_RED);
						return 0;
					}

					scene->RemoveAction(index);
				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First Argument: Prefab Instance Name
//Second Argument: Animation Clip Name
CInt GetAnimationClipDuration(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for GetAnimationClipDuration()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
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
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", luaToString2, "'");
							PrintInfo(temp, COLOR_RED);
							return 0;
						}

						lua_pushnumber(L, scene->m_animationClips[index]->GetDuration());
						return 1;

					}
				}
			}
			else
			{
				PrintInfo("\nGetAnimationClipDuration() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;
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
					PrintInfo(temp, COLOR_RED);
					return 0;
				}

				lua_pushnumber(L, scene->m_animationClips[index]->GetDuration());
				return 1;
			}
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						CBool foundAnimation = CFalse;
						foundPrefabInstance = CTrue;

						//animation clip name
						CChar luaToString2[MAX_NAME_SIZE];
						Cpy(luaToString2, lua_tostring(L, 2));
						StringToUpper(luaToString2);

						for (CUInt k = 0; k < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames.size(); k++)
						{
							CChar animationName[MAX_NAME_SIZE];
							Cpy(animationName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
							StringToUpper(animationName);

							if (Cmp(animationName, luaToString2))
							{
								foundAnimation = CTrue;

								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nGetAnimationClipDuration() will execute for project '%s', VScene '%s', prefab Instance '%s', animation '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_animationNames[k].c_str());
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}

						if (!foundAnimation)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nGetAnimationClipDuration() Error: project '%s', VScene '%s', prefab Instance '%s' has no animation called '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, lua_tostring(L, 2));
							PrintInfo(message, COLOR_RED);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "GetAnimationClipDuration() Error: Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
						PrintInfo(temp, COLOR_RED);
						return 0;
					}

					lua_pushnumber( L, scene->m_animationClips[index]->GetDuration());
					return 1;

				}
			}
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt ShowPrefabInstance(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 arguments for ShowPrefabInstance()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
				g_currentInstancePrefab->SetVisible(CTrue);
			else
				PrintInfo("\nShowPrefabInstance() Error: Couldn't find current prefab instance", COLOR_RED);
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			PrintInfo("\nShowPrefabInstance() : All LODs will become visible", COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : Prefab Instance '%s' will become visible", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nShowPrefabInstance() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
			g_instancePrefab[i]->SetVisible(CTrue);
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nShowPrefabInstance() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt HidePrefabInstance(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 arguments for HidePrefabInstance()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
				g_currentInstancePrefab->SetVisible(CFalse);
			else
				PrintInfo("\nHidePrefabInstance() Error: Couldn't find current prefab instance", COLOR_RED);
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			PrintInfo("\nAll LODs will become invisible", COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : Prefab Instance '%s' will become invisible", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nHidePrefabInstance() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
			g_instancePrefab[i]->SetVisible(CFalse);
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nHidePrefabInstance() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt PauseAnimations(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 arguments for PauseAnimations()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
			}
			else
			{
				PrintInfo("\nPauseAnimations() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;
				scene->SetAnimationStatus(eANIM_PAUSE);
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nPauseAnimations() will execute for Project '%s', VScene '%s', Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "PauseAnimations() Error: Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt LoadVScene(lua_State *L)
{
	if( g_testScript )
		return 0;

	int argc = lua_gettop(L);
	if( argc < 1 )
	{
		PrintInfo( "\nPlease specify 1 argument for LoadVScene()", COLOR_RED );
		return 0;
	}
	//g_clickedNew = g_clickedOpen = CTrue;
	//g_multipleView->m_loadScene = CTrue;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy( luaToString, lua_tostring(L, 1) );

	CChar VSceneName[MAX_NAME_SIZE];
	sprintf(VSceneName, "%s%s", luaToString, ".vin");
	StringToUpper(VSceneName);

	//GetWithoutDot(luaToString);
	//CChar RTIPath[MAX_NAME_SIZE];
	//SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, RTIPath);
	//sprintf( g_fileNameInCommandLine, "%s%s%s%s%s%s", RTIPath, "/Vanda/Projects/", luaToString, "/", luaToString, ".vin" );
	//ex_pVandaEngine1Dlg->OnMenuClickedOpenVScene(CFalse);
	//g_multipleView->m_loadScene = CFalse;
	//g_clickedNew = g_clickedOpen = CFalse;
	CBool foundTarget = CFalse;

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt i = 0; i < g_projects.size(); i++)
		{
			for (CUInt j = 0; j < g_projects[i]->m_sceneNames.size(); j++)
			{
				CChar currentVSceneName[MAX_NAME_SIZE];
				Cpy(currentVSceneName, g_projects[i]->m_sceneNames[j].c_str());
				StringToUpper(currentVSceneName);
				if (Cmp(currentVSceneName, VSceneName))
				{
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "%s%s%s%s%s", "\nVScene '", g_projects[i]->m_sceneNames[j].c_str(), "' in project '", g_projects[i]->m_name, "' will be loaded");
					PrintInfo(temp, COLOR_GREEN);
					foundTarget = CTrue;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nLoadVScene() Error: Couldn't find VScene '", luaToString, "' to be loaded.");
			PrintInfo(temp, COLOR_RED);
		}
		return 0;
	}

	for (CUInt i = 0; i < g_VSceneNamesOfCurrentProject.size(); i++)
	{
		CChar currentVSceneName[MAX_NAME_SIZE];
		Cpy(currentVSceneName, g_VSceneNamesOfCurrentProject[i].c_str());
		StringToUpper(currentVSceneName);

		if (Cmp(VSceneName, currentVSceneName))
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nVScene '", luaToString, "' will be loaded.");
			PrintInfo(temp, COLOR_GREEN);
			foundTarget = CTrue;
			break;
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nLoadVScene() Error: Couldn't find VScene '", luaToString, "' to be loaded.");
		PrintInfo(temp, COLOR_RED);
	}

	return 0;
}

//first argument: true or false
//second argument: pause or resume VScene (true: pause, false: resume)
//third argument: size of cursor- If first argument if false, this argument is ignored

CInt SetCurrentVSceneAsMenu(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		PrintInfo("\nPlease specify 3 arguments for SetCurrentVSceneAsMenu()", COLOR_RED);
		return 0;
	}
	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		PrintInfo("\nSetCurrentVSceneAsMenu() will be executed for current VScene", COLOR_GREEN);
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
	g_multipleView->m_isMenu = menu;

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
		g_multipleView->timer.GetElapsedSeconds(CTrue);

		if (g_multipleView->m_tempAllPlayingSoundSources.size() != 0)
		{
			for (CUInt i = 0; i < g_multipleView->m_tempAllPlayingSoundSources.size(); i++)
			{
				//resume matched resource sounds
				for (CUInt j = 0; j < g_resourceFiles.size(); j++)
				{
					if (g_resourceFiles[j]->GetSoundSource())
					{
						if (Cmp(g_multipleView->m_tempAllPlayingSoundSources[i].c_str(), g_resourceFiles[j]->GetSoundSource()->GetName()))
							g_soundSystem->PlayALSound(*(g_resourceFiles[j]->GetSoundSource()->GetSoundSource()));
					}
				}

				//resume matched static sounds
				for (CUInt j = 0; j < g_engineStaticSounds.size(); j++)
				{
					if (Cmp(g_multipleView->m_tempAllPlayingSoundSources[i].c_str(), g_engineStaticSounds[j]->GetName()))
						g_soundSystem->PlayALSound(*(g_engineStaticSounds[j]->GetSoundSource()));
				}

				//resume matched main character sounds
				if (g_mainCharacter)
				{
					if (Cmp(g_multipleView->m_tempAllPlayingSoundSources[i].c_str(), g_mainCharacter->m_walkSound->GetName()))
						g_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));

					if (Cmp(g_multipleView->m_tempAllPlayingSoundSources[i].c_str(), g_mainCharacter->m_runSound->GetName()))
						g_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));

					if (Cmp(g_multipleView->m_tempAllPlayingSoundSources[i].c_str(), g_mainCharacter->m_jumpSound->GetName()))
						g_soundSystem->PlayALSound(*(g_mainCharacter->m_jumpSound->GetSoundSource()));
				}

				//resume matched ambient sound
				if (g_menu.m_insertAmbientSound)
				{
					if (Cmp(g_multipleView->m_tempAllPlayingSoundSources[i].c_str(), g_multipleView->m_ambientSound->GetName()))
						g_soundSystem->PlayALSound(*(g_multipleView->m_ambientSound->GetSoundSource()));
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
					g_multipleView->m_tempAllPlayingSoundSources.push_back(g_resourceFiles[i]->GetSoundSource()->GetName());
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
				g_multipleView->m_tempAllPlayingSoundSources.push_back(g_engineStaticSounds[i]->GetName());
			}
		}

		//ambient sound
		if (g_menu.m_insertAmbientSound)
		{
			ALint state;
			alGetSourcei(g_multipleView->m_ambientSound->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING)
			{
				g_multipleView->m_tempAllPlayingSoundSources.push_back(g_multipleView->m_ambientSound->GetName());
			}
		}

		//main character sounds
		if (g_mainCharacter)
		{
			ALint state1;
			alGetSourcei(g_mainCharacter->m_jumpSound->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state1);
			if (state1 == AL_PLAYING)
			{
				g_multipleView->m_tempAllPlayingSoundSources.push_back(g_mainCharacter->m_jumpSound->GetName());
			}
			ALint state2;
			alGetSourcei(g_mainCharacter->m_walkSound->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state2);
			if (state2 == AL_PLAYING)
			{
				g_multipleView->m_tempAllPlayingSoundSources.push_back(g_mainCharacter->m_walkSound->GetName());
			}
			ALint state3;
			alGetSourcei(g_mainCharacter->m_runSound->GetSoundSource()->GetSource(), AL_SOURCE_STATE, &state3);
			if (state3 == AL_PLAYING)
			{
				g_multipleView->m_tempAllPlayingSoundSources.push_back(g_mainCharacter->m_runSound->GetName());
			}
		}
	}

	if (g_currentVSceneProperties.m_isMenu)
	{
		CInt size;
		size = lua_tointeger(L, 3);
		g_currentVSceneProperties.m_cursorSize = size;

		g_multipleView->GenerateMenuCursorTexture(g_vsceneMenuCursor.GetCursorPath());
	}

	return 0;
}

CInt ExitGame(lua_State *L)
{
	PrintInfo("\nGame Will Exit", COLOR_YELLOW);
	return 0;
}

CInt ActivateThirdPersonCamera(lua_State *L)
{
	if (g_testScript)
		return 0;

	if (g_mainCharacter)
	{
		g_render.SetActiveInstanceCamera(NULL);
		g_currentCameraType = eCAMERA_PHYSX;
		g_mainCharacter->SetCameraType(ePHYSX_CAMERA_THIRD_PERSON);
		PrintInfo("\nDefault third person PhysX camera was activated.", COLOR_YELLOW);
		return 0;
	}
	else
	{
		if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
		{
			PrintInfo("\nActivateThirdPersonCamera() will activate third person camera in VScene mode", COLOR_GREEN);
		}
		else
		{
			PrintInfo("\nActivateThirdPersonCamera() Error: Couldn't find main character", COLOR_RED);
		}
		return 0;
	}
}

CInt ActivateFirstPersonCamera(lua_State *L)
{
	if (g_testScript)
		return 0;

	if (g_mainCharacter)
	{
		g_render.SetActiveInstanceCamera(NULL);
		g_currentCameraType = eCAMERA_PHYSX;
		g_mainCharacter->SetCameraType(ePHYSX_CAMERA_FIRST_PERSON);
		PrintInfo("\nDefault first person PhysX camera was activated.", COLOR_YELLOW);
		return 0;
	}
	else
	{
		if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
		{
			PrintInfo("\nActivateFirstPersonCamera() will activate first person camera in VScene mode", COLOR_GREEN);
		}
		else
		{
			PrintInfo("\nActivateFirstPersonCamera() Error: Couldn't find main character", COLOR_RED);
		}
		return 0;
	}
}

//First Argument: Imported Camera Name
//Second Argument: End Time. Should Be Positive Value. Arbitrary Argument
CInt ActivateImportedCamera( lua_State* L )
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify at least 1 argument for ActivateImportedCamera()", COLOR_RED);
		return 0;
	}

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy( luaToString, lua_tostring(L, 1) );
	StringToUpper( luaToString );

	CBool foundTarget = CFalse;

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_importedCameraNames.size(); j++)
				{
					 CChar camera[MAX_NAME_SIZE];
					 Cpy(camera, g_projects[pr]->m_vsceneObjectNames[i].m_importedCameraNames[j].c_str());
					 StringToUpper(camera);

					 if (Cmp(camera, luaToString))
					 {
						 foundTarget = CTrue;

						 CChar message[MAX_NAME_SIZE];
						 sprintf(message, "\nActivateImportedCamera() will execute for Project '%s', VScene '%s', Imported Camera '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_importedCameraNames[j].c_str());
						 PrintInfo(message, COLOR_GREEN);

						 break;
					 }
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nActivateImportedCamera() Error: Couldn't find camera '", luaToString, "' to be activated.");
			PrintInfo(temp, COLOR_RED);
			return 0;
		}
		return 0;
	}

	for( CUInt i = 0; i < g_importedCameraInstances.size(); i++ )
	{
		CChar camera[MAX_NAME_SIZE];
		Cpy( camera, g_importedCameraInstances[i]->m_abstractCamera->GetName() );
		StringToUpper( camera );

		if( Cmp( camera, luaToString ) )
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

			g_render.SetActiveInstanceCamera(  g_importedCameraInstances[i] );
			g_importedCameraInstances[i]->SetIsTimerEnabled(enableTimer);
			g_importedCameraInstances[i]->SetEndTime(end_time);
			g_currentCameraType = eCAMERA_COLLADA;
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nCamera '", g_importedCameraInstances[i]->m_abstractCamera->GetName(), "' was activated.");
			PrintInfo(temp, COLOR_GREEN);

			foundTarget = CTrue;
			break;
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nActivateImportedCamera() Error: Couldn't find camera '", luaToString, "' to be activated.");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

//First Argument: Prefab Instance Name. Use "this" to refer to current prefab instance
//Second Argument: Imported camera name of prefab
//Third Argument: End Time. Should Be Positive Value. Arbitrary Argument
CInt ActivateImportedCameraOfPrefabInstance(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify at least 2 arguments for ActivateImportedCameraOfPrefabInstance()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
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
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "%s%s%s", "\nCamera '", scene->m_cameraInstances[index]->m_abstractCamera->GetPureName(), "' was activated.");
							PrintInfo(temp, COLOR_GREEN);
						}
					}
					if (foundCameraTarget)
						break;
				}
				if (!scene)
				{
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Prefab Instance '", g_currentInstancePrefab->GetName(), "' Is Invisible");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
				if (!foundCameraTarget)
				{
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "\n%s%s%s", "Couldn't find the camera '", luaToString2, "'");
					PrintInfo(temp, COLOR_RED);
					return 0;
				}
			}
			else
			{
				PrintInfo("\nActivateImportedCameraOfPrefabInstance() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;

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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "%s%s%s", "\nCamera '", scene->m_cameraInstances[index]->m_abstractCamera->GetPureName(), "' was activated.");
					PrintInfo(temp, COLOR_GREEN);
				}
				if (foundCameraTarget)
					break;
			}
			if (!foundCameraTarget)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "\n%s%s%s", "Couldn't find the camera '", luaToString2, "'");
				PrintInfo(temp, COLOR_RED);
				return 0;
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						CBool foundCamera = CFalse;
						foundPrefabInstance = CTrue;

						for (CUInt k = 0; k < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_cameraNames.size(); k++)
						{
							CChar cameraName[MAX_NAME_SIZE];
							Cpy(cameraName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_cameraNames[k].c_str());
							StringToUpper(cameraName);

							if (Cmp(cameraName, luaToString2))
							{
								foundCamera = CTrue;

								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nActivateImportedCameraOfPrefabInstance() will execute for project '%s', VScene '%s', prefab Instance '%s', camera '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_cameraNames[k].c_str());
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}

						if (!foundCamera)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nActivateImportedCameraOfPrefabInstance() Error: project '%s', VScene '%s', prefab Instance '%s' has no camera called '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, lua_tostring(L, 2));
							PrintInfo(message, COLOR_RED);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "ActivateImportedCameraOfPrefabInstance() Error: Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "%s%s%s", "\nCamera '", scene->m_cameraInstances[index]->m_abstractCamera->GetPureName(), "' was activated.");
						PrintInfo(temp, COLOR_GREEN);
					}
				}
				if (foundCameraTarget)
					break;
			}
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!scene)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Prefab Instance '", luaToString, "' Is Invisible");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	if (!foundCameraTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find the camera '", luaToString2, "'");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First Argument: Engine Camera Name
//Second Argument: End Time. Should Be Positive Value. Arbitrary Argument
CInt ActivateEngineCamera(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify at least 1 argument for ActivateEngineCamera()", COLOR_RED);
		return 0;
	}

	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundTarget = CFalse;

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_engineCameraNames.size(); j++)
				{
					CChar camera[MAX_NAME_SIZE];
					Cpy(camera, g_projects[pr]->m_vsceneObjectNames[i].m_engineCameraNames[j].c_str());
					StringToUpper(camera);

					if (Cmp(camera, luaToString))
					{
						foundTarget = CTrue;

						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nActivateEngineCamera() will execute for Project '%s', VScene '%s', Engine Camera '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_engineCameraNames[j].c_str());
						PrintInfo(message, COLOR_GREEN);

						break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nActivateEngineCamera() Error: Couldn't find camera '", luaToString, "' to be activated.");
			PrintInfo(temp, COLOR_RED);
			return 0;
		}
		return 0;
	}

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
			PrintInfo(temp, COLOR_GREEN);

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
		sprintf(temp, "%s%s%s", "\nActivateEngineCamera() Error: Couldn't find camera '", luaToString, "' to be activated.");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

//Argument: Angle in degrees
CInt SetPhysicsCameraAngle(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraAngle()", COLOR_RED);
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
	if (g_testScript)
		return 0;
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraTilt()", COLOR_RED);
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraMaxTilt()", COLOR_RED);
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraMinTilt()", COLOR_RED);
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsCameraYaw()", COLOR_RED);
		return 0;
	}
	if (g_camera)
	{
		CFloat value = (CFloat)lua_tonumber(L, 1);
		g_camera->m_perspectiveCameraYaw = NxMath::degToRad(value) /*+ NxMath::degToRad(180.f)*/;
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for LoadResource()", COLOR_RED);
		return 0;
	}

	CChar luaToString1[MAX_NAME_SIZE];
	Cpy(luaToString1, lua_tostring(L, 1));
	StringToUpper(luaToString1); //package name

	CChar luaToString2[MAX_NAME_SIZE];
	Cpy(luaToString2, lua_tostring(L, 2));
	StringToUpper(luaToString2);

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		CBool foundTarget = CFalse;
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			CBool foundTargetInCurrentProject = CFalse;
			for (CUInt i = 0; i < g_projects[pr]->m_resourceNames.size(); i++)
			{
				CChar resourceFolder[MAX_NAME_SIZE];
				Cpy(resourceFolder, g_projects[pr]->m_resourceNames[i].front().c_str());
				StringToUpper(resourceFolder);

				if (Cmp(luaToString1, resourceFolder))
				{
					for (CUInt j = 0; j < g_projects[pr]->m_resourceNames[i].size(); j++)
					{
						if (j == 0)
							continue; //it's folder name

						CChar resourceFile[MAX_NAME_SIZE];
						Cpy(resourceFile, g_projects[pr]->m_resourceNames[i][j].c_str());
						StringToUpper(resourceFile);

						if (Cmp(luaToString2, resourceFile))
						{
							foundTarget = CTrue;
							foundTargetInCurrentProject = CTrue;
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nLoadResource() will load '%s/%s' resource in Project '%s'", lua_tostring(L, 1), lua_tostring(L, 2), g_projects[pr]->m_name );
							PrintInfo(message, COLOR_GREEN);
							break;
						}
					}
				}
				if (foundTargetInCurrentProject)
					break;
			}
		}
		if (!foundTarget)
		{
			CChar errorMessage[MAX_NAME_SIZE];
			sprintf(errorMessage, "\nLoadResource() Error: Couldn't find '%s/%s' resource in all projects", lua_tostring(L, 1), lua_tostring(L, 2));
			PrintInfo(errorMessage, COLOR_RED);
		}
		return 0;
	}

	CChar fileName[MAX_NAME_SIZE];
	Cpy(fileName, g_currentProjectPath);

	Append(fileName, "Resources/");
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for DeleteResource()", COLOR_RED);
		return 0;
	}

	CChar luaToString1[MAX_NAME_SIZE];
	Cpy(luaToString1, lua_tostring(L, 1));
	StringToUpper(luaToString1); //directory name

	CChar luaToString2[MAX_NAME_SIZE];
	Cpy(luaToString2, lua_tostring(L, 2));
	StringToUpper(luaToString2); //file name

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		CBool foundTarget = CFalse;
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			CBool foundTargetInCurrentProject = CFalse;
			for (CUInt i = 0; i < g_projects[pr]->m_resourceNames.size(); i++)
			{
				CChar resourceFolder[MAX_NAME_SIZE];
				Cpy(resourceFolder, g_projects[pr]->m_resourceNames[i].front().c_str());
				StringToUpper(resourceFolder);

				if (Cmp(luaToString1, resourceFolder))
				{
					for (CUInt j = 0; j < g_projects[pr]->m_resourceNames[i].size(); j++)
					{
						if (j == 0)
							continue; //it's folder name

						CChar resourceFile[MAX_NAME_SIZE];
						Cpy(resourceFile, g_projects[pr]->m_resourceNames[i][j].c_str());
						StringToUpper(resourceFile);

						if (Cmp(luaToString2, resourceFile))
						{
							foundTarget = CTrue;
							foundTargetInCurrentProject = CTrue;
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nDeleteResource() will delete '%s/%s' resource in Project '%s'", lua_tostring(L, 1), lua_tostring(L, 2), g_projects[pr]->m_name);
							PrintInfo(message, COLOR_GREEN);
							break;
						}
					}
				}
				if (foundTargetInCurrentProject)
					break;
			}
		}
		if (!foundTarget)
		{
			CChar errorMessage[MAX_NAME_SIZE];
			sprintf(errorMessage, "\nDeleteResource() Error: Couldn't find '%s/%s' resource in all projects", lua_tostring(L, 1), lua_tostring(L, 2));
			PrintInfo(errorMessage, COLOR_RED);
		}
		return 0;
	}

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
	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		PrintInfo("\nDeleteAllResources() - All loaded resources will be deleted.");
		return 0;
	}

	for (CUInt j = 0; j < g_resourceFiles.size(); j++)
		CDelete(g_resourceFiles[j]);
	g_resourceFiles.clear();
	
	if (g_multipleView && g_multipleView->GetCursorIcon())
		g_multipleView->GetCursorIcon()->SetVisible(CFalse);

	return 0;
}

CInt PlayResourceSoundLoop(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify at least 1 argument for PlayResourceSoundLoop()", COLOR_RED);
		return 0;
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (int n = 1; n <= argc; ++n)
		{
			CBool foundTarget = CFalse;

			CChar luaToString[MAX_NAME_SIZE];
			Cpy(luaToString, lua_tostring(L, n));
			StringToUpper(luaToString); //package name

			CChar extension[MAX_NAME_SIZE];
			if (GetFileExtension(luaToString))
			{
				Cpy(extension, GetFileExtension(luaToString));
				if (Cmp(extension, ".OGG"))
				{
					for (CUInt pr = 0; pr < g_projects.size(); pr++)
					{
						CBool foundTargetInCurrentProject = CFalse;
						for (CUInt i = 0; i < g_projects[pr]->m_resourceNames.size(); i++)
						{
							for (CUInt j = 0; j < g_projects[pr]->m_resourceNames[i].size(); j++)
							{
								if (j == 0)
									continue; //it's folder name

								CChar resourceFile[MAX_NAME_SIZE];
								sprintf(resourceFile, "%s_%s", g_projects[pr]->m_resourceNames[i].front().c_str(), g_projects[pr]->m_resourceNames[i][j].c_str());
								StringToUpper(resourceFile);

								if (Cmp(luaToString, resourceFile))
								{
									foundTarget = CTrue;
									foundTargetInCurrentProject = CTrue;
									CChar message[MAX_NAME_SIZE];
									sprintf(message, "\nProject '%s' - PlayResourceSoundLoop() will play sound resource '%s' continiously", g_projects[pr]->m_name, lua_tostring(L, n));
									PrintInfo(message, COLOR_GREEN);
									break;
								}
							}
							if (foundTargetInCurrentProject)
								break;
						}
					}
				}
			}
			if (!foundTarget)
			{
				CChar errorMessage[MAX_NAME_SIZE];
				sprintf(errorMessage, "\nPlayResourceSoundLoop() Error: Couldn't find '%s' sound resource in all projects", lua_tostring(L, n));
				PrintInfo(errorMessage, COLOR_RED);
			}
		}
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "%s%s%s", "\nSound '", g_resourceFiles[i]->GetSoundSource()->GetName(), "' was played continiously.");
					PrintInfo(temp, COLOR_YELLOW);
					foundTarget = CTrue;
					break;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nPlayResourceSoundLoop() Error: Couldn't find the sound '", lua_tostring(L, n), "' to be played.");
			PrintInfo(temp, COLOR_RED);
		}
	}

	return 0;
}

CInt PlayResourceSoundOnce(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify at least 1 argument for PlayResourceSoundOnce()", COLOR_RED);
		return 0;
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (int n = 1; n <= argc; ++n)
		{
			CBool foundTarget = CFalse;

			CChar luaToString[MAX_NAME_SIZE];
			Cpy(luaToString, lua_tostring(L, n));
			StringToUpper(luaToString); //package name

			CChar extension[MAX_NAME_SIZE];
			if (GetFileExtension(luaToString))
			{
				Cpy(extension, GetFileExtension(luaToString));
				if (Cmp(extension, ".OGG"))
				{
					for (CUInt pr = 0; pr < g_projects.size(); pr++)
					{
						CBool foundTargetInCurrentProject = CFalse;
						for (CUInt i = 0; i < g_projects[pr]->m_resourceNames.size(); i++)
						{
							for (CUInt j = 0; j < g_projects[pr]->m_resourceNames[i].size(); j++)
							{
								if (j == 0)
									continue; //it's folder name

								CChar resourceFile[MAX_NAME_SIZE];
								sprintf(resourceFile, "%s_%s", g_projects[pr]->m_resourceNames[i].front().c_str(), g_projects[pr]->m_resourceNames[i][j].c_str());
								StringToUpper(resourceFile);

								if (Cmp(luaToString, resourceFile))
								{
									foundTarget = CTrue;
									foundTargetInCurrentProject = CTrue;
									CChar message[MAX_NAME_SIZE];
									sprintf(message, "\nProject '%s' - PlayResourceSoundOnce() will play sound resource '%s' once", g_projects[pr]->m_name, lua_tostring(L, n));
									PrintInfo(message, COLOR_GREEN);
									break;
								}
							}
							if (foundTargetInCurrentProject)
								break;
						}
					}
				}
			}
			if (!foundTarget)
			{
				CChar errorMessage[MAX_NAME_SIZE];
				sprintf(errorMessage, "\nPlayResourceSoundOnce() Error: Couldn't find '%s' sound resource in all projects", lua_tostring(L, n));
				PrintInfo(errorMessage, COLOR_RED);
			}
		}
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "%s%s%s", "\nSound '", g_resourceFiles[i]->GetSoundSource()->GetName(), "' was played once.");
					PrintInfo(temp, COLOR_YELLOW);
					foundTarget = CTrue;
					break;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nPlayResourceSoundOnce() Error: Couldn't find the sound '", lua_tostring(L, n), "' to be played once.");
			PrintInfo(temp, COLOR_RED);
		}
	}

	return 0;
}

CInt StopResourceSound(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify at least 1 argument for StopResourceSound()", COLOR_RED);
		return 0;
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (int n = 1; n <= argc; ++n)
		{
			CBool foundTarget = CFalse;

			CChar luaToString[MAX_NAME_SIZE];
			Cpy(luaToString, lua_tostring(L, n));
			StringToUpper(luaToString); //package name

			CChar extension[MAX_NAME_SIZE];
			if (GetFileExtension(luaToString))
			{
				Cpy(extension, GetFileExtension(luaToString));
				if (Cmp(extension, ".OGG"))
				{
					for (CUInt pr = 0; pr < g_projects.size(); pr++)
					{
						CBool foundTargetInCurrentProject = CFalse;
						for (CUInt i = 0; i < g_projects[pr]->m_resourceNames.size(); i++)
						{
							for (CUInt j = 0; j < g_projects[pr]->m_resourceNames[i].size(); j++)
							{
								if (j == 0)
									continue; //it's folder name

								CChar resourceFile[MAX_NAME_SIZE];
								sprintf(resourceFile, "%s_%s", g_projects[pr]->m_resourceNames[i].front().c_str(), g_projects[pr]->m_resourceNames[i][j].c_str());
								StringToUpper(resourceFile);

								if (Cmp(luaToString, resourceFile))
								{
									foundTarget = CTrue;
									foundTargetInCurrentProject = CTrue;
									CChar message[MAX_NAME_SIZE];
									sprintf(message, "\nProject '%s' - StopResourceSound() will stop sound resource '%s'", g_projects[pr]->m_name, lua_tostring(L, n));
									PrintInfo(message, COLOR_GREEN);
									break;
								}
							}
							if (foundTargetInCurrentProject)
								break;
						}
					}
				}
			}
			if (!foundTarget)
			{
				CChar errorMessage[MAX_NAME_SIZE];
				sprintf(errorMessage, "\nStopResourceSound() Error: Couldn't find '%s' sound resource in all projects", lua_tostring(L, n));
				PrintInfo(errorMessage, COLOR_RED);
			}
		}
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "%s%s%s", "\nSound '", g_resourceFiles[i]->GetSoundSource()->GetName(), "' was stopped.");
					PrintInfo(temp, COLOR_YELLOW);
					foundTarget = CTrue;
					break;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nStopResourceSound() Error: Couldn't find the sound '", lua_tostring(L, n), "' to be stopped.");
			PrintInfo(temp, COLOR_RED);
		}
	}

	return 0;
}

CInt StopAllResourceSounds(lua_State *L)
{
	if (g_testScript)
		return 0;

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		PrintInfo("\nStopAllResourceSounds() - All resource sounds will be stopped.");
		return 0;
	}

	for (CUInt i = 0; i < g_resourceFiles.size(); i++)
	{
		if (g_resourceFiles[i]->GetSoundSource())
		{
			g_soundSystem->StopALSound(*(g_resourceFiles[i]->GetSoundSource()->GetSoundSource()));
		}
	}

	return 0;
}

CInt PauseResourceSound(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify at least 1 argument for PauseResourceSound()", COLOR_RED);
		return 0;
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (int n = 1; n <= argc; ++n)
		{
			CBool foundTarget = CFalse;

			CChar luaToString[MAX_NAME_SIZE];
			Cpy(luaToString, lua_tostring(L, n));
			StringToUpper(luaToString); //package name

			CChar extension[MAX_NAME_SIZE];
			if (GetFileExtension(luaToString))
			{
				Cpy(extension, GetFileExtension(luaToString));
				if (Cmp(extension, ".OGG"))
				{
					for (CUInt pr = 0; pr < g_projects.size(); pr++)
					{
						CBool foundTargetInCurrentProject = CFalse;
						for (CUInt i = 0; i < g_projects[pr]->m_resourceNames.size(); i++)
						{
							for (CUInt j = 0; j < g_projects[pr]->m_resourceNames[i].size(); j++)
							{
								if (j == 0)
									continue; //it's folder name

								CChar resourceFile[MAX_NAME_SIZE];
								sprintf(resourceFile, "%s_%s", g_projects[pr]->m_resourceNames[i].front().c_str(), g_projects[pr]->m_resourceNames[i][j].c_str());
								StringToUpper(resourceFile);

								if (Cmp(luaToString, resourceFile))
								{
									foundTarget = CTrue;
									foundTargetInCurrentProject = CTrue;
									CChar message[MAX_NAME_SIZE];
									sprintf(message, "\nProject '%s' - PauseResourceSound() will pause sound resource '%s'", g_projects[pr]->m_name, lua_tostring(L, n));
									PrintInfo(message, COLOR_GREEN);
									break;
								}
							}
							if (foundTargetInCurrentProject)
								break;
						}
					}
				}
			}
			if (!foundTarget)
			{
				CChar errorMessage[MAX_NAME_SIZE];
				sprintf(errorMessage, "\nPauseResourceSound() Error: Couldn't find '%s' sound resource in all projects", lua_tostring(L, n));
				PrintInfo(errorMessage, COLOR_RED);
			}
		}
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
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "%s%s%s", "\nSound '", g_resourceFiles[i]->GetSoundSource()->GetName(), "' was paused.");
					PrintInfo(temp, COLOR_YELLOW);
					foundTarget = CTrue;
					break;
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nPauseResourceSound() Error: Couldn't find the sound '", lua_tostring(L, n), "' to be paused.");
			PrintInfo(temp, COLOR_RED);
		}
	}

	return 0;
}

CInt ShowGUI(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for ShowGUI()", COLOR_RED);
		return 0;
	}
	CBool foundTarget = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_guiNames.size(); j++)
				{
					CChar camera[MAX_NAME_SIZE];
					Cpy(camera, g_projects[pr]->m_vsceneObjectNames[i].m_guiNames[j].c_str());
					StringToUpper(camera);

					if (Cmp(camera, luaToString))
					{
						foundTarget = CTrue;

						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nShowGUI() - Project '%s', VScene '%s', GUI '%s' will become visible", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_guiNames[j].c_str());
						PrintInfo(message, COLOR_GREEN);

						break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nShowGUI() Error: Couldn't find GUI '", luaToString, "'");
			PrintInfo(temp, COLOR_RED);
			return 0;
		}
		return 0;
	}

	for (CUInt i = 0; i < g_guis.size(); i++)
	{
		CChar gui[MAX_NAME_SIZE];
		Cpy(gui, g_guis[i]->GetName());
		StringToUpper(gui);

		if (Cmp(gui, luaToString))
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nGUI '", g_guis[i]->GetName(), "' was activated.");
			PrintInfo(temp, COLOR_GREEN);

			g_guis[i]->SetVisible(CTrue);

			foundTarget = CTrue;
			break;
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nShowGUI() Error: Couldn't find the GUI '", luaToString, "' to be activated.");
		PrintInfo(temp, COLOR_RED);
	}

	return 0;
}

CInt HideGUI(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for HideGUI()", COLOR_RED);
		return 0;
	}
	CBool foundTarget = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_guiNames.size(); j++)
				{
					CChar camera[MAX_NAME_SIZE];
					Cpy(camera, g_projects[pr]->m_vsceneObjectNames[i].m_guiNames[j].c_str());
					StringToUpper(camera);

					if (Cmp(camera, luaToString))
					{
						foundTarget = CTrue;

						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nHideGUI() - Project '%s', VScene '%s', GUI '%s' will become invisible", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_guiNames[j].c_str());
						PrintInfo(message, COLOR_GREEN);

						break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nHideGUI() Error: Couldn't find GUI '", luaToString, "'");
			PrintInfo(temp, COLOR_RED);
			return 0;
		}
		return 0;
	}

	for (CUInt i = 0; i < g_guis.size(); i++)
	{
		CChar gui[MAX_NAME_SIZE];
		Cpy(gui, g_guis[i]->GetName());
		StringToUpper(gui);

		if (Cmp(gui, luaToString))
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nGUI '", g_guis[i]->GetName(), "' was deactivated.");
			PrintInfo(temp, COLOR_GREEN);

			g_guis[i]->SetVisible(CFalse);

			foundTarget = CTrue;
			break;
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nHideGUI() Error: Couldn't find the GUI '", luaToString, "' to be deactivated.");
		PrintInfo(temp, COLOR_RED);
	}

	return 0;
}

CInt ShowCursorIcon(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for ShowCursorIcon()", COLOR_RED);
		return 0;
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		CBool foundTarget = CFalse;

		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, 1));
		StringToUpper(luaToString); //package name

		CChar extension[MAX_NAME_SIZE];
		if (GetFileExtension(luaToString))
		{
			Cpy(extension, GetFileExtension(luaToString));
			if (Cmp(extension, ".DDS"))
			{
				for (CUInt pr = 0; pr < g_projects.size(); pr++)
				{
					CBool foundTargetInCurrentProject = CFalse;
					for (CUInt i = 0; i < g_projects[pr]->m_resourceNames.size(); i++)
					{
						for (CUInt j = 0; j < g_projects[pr]->m_resourceNames[i].size(); j++)
						{
							if (j == 0)
								continue; //it's folder name

							CChar resourceFile[MAX_NAME_SIZE];
							sprintf(resourceFile, "%s_%s", g_projects[pr]->m_resourceNames[i].front().c_str(), g_projects[pr]->m_resourceNames[i][j].c_str());
							StringToUpper(resourceFile);

							if (Cmp(luaToString, resourceFile))
							{
								foundTarget = CTrue;
								foundTargetInCurrentProject = CTrue;
								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nProject '%s' - ShowCursorIcon() will show image resource '%s'", g_projects[pr]->m_name, lua_tostring(L, 1));
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}
						if (foundTargetInCurrentProject)
							break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar errorMessage[MAX_NAME_SIZE];
			sprintf(errorMessage, "\nShowCursorIcon() Error: Couldn't find '%s' image resource in all projects", lua_tostring(L, 1));
			PrintInfo(errorMessage, COLOR_RED);
		}

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
				g_multipleView->GetCursorIcon()->SetVisible(CTrue);
				g_multipleView->GetCursorIcon()->SetName(string);
				g_multipleView->GetCursorIcon()->SetImage(g_resourceFiles[i]->GetImage());
				g_multipleView->GetCursorIcon()->SetSize(lua_tonumber(L, 2));

				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nMouse icon '", g_resourceFiles[i]->GetSoundSource()->GetName(), "' was shown.");
				PrintInfo(temp, COLOR_YELLOW);
				foundTarget = CTrue;
				break;
			}
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nShowCursorIcon() Error: Couldn't find the image '", lua_tostring(L, 1), "' to be shown.");
		PrintInfo(temp, COLOR_RED);
	}

	return 0;
}

CInt HideCursorIcon(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for HideCursorIcon()", COLOR_RED);
		return 0;
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		CBool foundTarget = CFalse;

		CChar luaToString[MAX_NAME_SIZE];
		Cpy(luaToString, lua_tostring(L, 1));
		StringToUpper(luaToString); //package name

		CChar extension[MAX_NAME_SIZE];
		if (GetFileExtension(luaToString))
		{
			Cpy(extension, GetFileExtension(luaToString));
			if (Cmp(extension, ".DDS"))
			{
				for (CUInt pr = 0; pr < g_projects.size(); pr++)
				{
					CBool foundTargetInCurrentProject = CFalse;
					for (CUInt i = 0; i < g_projects[pr]->m_resourceNames.size(); i++)
					{
						for (CUInt j = 0; j < g_projects[pr]->m_resourceNames[i].size(); j++)
						{
							if (j == 0)
								continue; //it's folder name

							CChar resourceFile[MAX_NAME_SIZE];
							sprintf(resourceFile, "%s_%s", g_projects[pr]->m_resourceNames[i].front().c_str(), g_projects[pr]->m_resourceNames[i][j].c_str());
							StringToUpper(resourceFile);

							if (Cmp(luaToString, resourceFile))
							{
								foundTarget = CTrue;
								foundTargetInCurrentProject = CTrue;
								CChar message[MAX_NAME_SIZE];
								sprintf(message, "\nProject '%s' - HideCursorIcon() will hide image resource '%s'", g_projects[pr]->m_name, lua_tostring(L, 1));
								PrintInfo(message, COLOR_GREEN);
								break;
							}
						}
						if (foundTargetInCurrentProject)
							break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar errorMessage[MAX_NAME_SIZE];
			sprintf(errorMessage, "\nHideCursorIcon() Error: Couldn't find '%s' image resource in all projects", lua_tostring(L, 1));
			PrintInfo(errorMessage, COLOR_RED);
		}

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
				g_multipleView->GetCursorIcon()->SetVisible(CFalse);
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "%s%s%s", "\nMouse icon '", g_resourceFiles[i]->GetSoundSource()->GetName(), "' is now hidden.");
				PrintInfo(temp, COLOR_YELLOW);
				foundTarget = CTrue;
				break;
			}
		}
	}
	if (!foundTarget)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "%s%s%s", "\nHideCursorIcon() Error: Couldn't find the image '", lua_tostring(L, 1), "' to be hidden.");
		PrintInfo(temp, COLOR_RED);
	}

	return 0;
}

CInt PrintConsole(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for PrintConsole()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));

	PrintInfo(luaToString);

	return 0;

}

CInt IsKeyDown(lua_State *L)
{
	if (g_testScript)
		return 0;

	if (g_camera)
		if (!g_camera->m_activatePerspectiveCamera)
			return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for IsKeyDown()", COLOR_RED);
		lua_pushboolean(L, 0);
		return 0;
	}

	CChar keyCode[MAX_NAME_SIZE];
	Cpy(keyCode, lua_tostring(L, 1)); //DirecInput Key Code
	StringToUpper(keyCode);

	g_multipleView->m_inputSystem->Update();

	if (Cmp(keyCode, "0"))
	{
		if (g_multipleView->m_inputSystem->ButtonDown(0)) //left click
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "1"))
	{
		if (g_multipleView->m_inputSystem->ButtonDown(1)) //right click
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "2"))
	{
		if (g_multipleView->m_inputSystem->ButtonDown(2)) //middle button click
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_ESCAPE"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_ESCAPE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_1"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_1))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_2"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_2))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_3"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_3))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_4"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_4))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_5"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_5))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_6"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_6))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_7"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_7))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_8"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_8))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_9"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_9))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_0"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_0))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_MINUS"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_MINUS))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_EQUALS"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_EQUALS))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_BACK"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_BACK))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_TAB"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_TAB))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_Q"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_Q))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_W"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_W))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_E"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_E))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_R"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_R))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_T"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_T))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_Y"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_Y))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_U"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_U))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_I"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_I))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_O"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_O))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_P"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_P))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LBRACKET"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_LBRACKET))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RBRACKET"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_RBRACKET))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RETURN"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_RETURN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LCONTROL"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_LCONTROL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_A"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_A))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_S"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_S))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_D"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_D))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_G"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_G))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_H"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_H))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_J"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_J))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_K"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_K))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_L"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_L))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SEMICOLON"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_SEMICOLON))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_APOSTROPHE"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_APOSTROPHE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_GRAVE"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_GRAVE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LSHIFT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_LSHIFT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_BACKSLASH"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_BACKSLASH))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_Z"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_Z))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_X"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_X))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_C"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_C))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_V"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_V))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_B"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_B))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_N"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_N))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_M"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_M))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_COMMA"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_COMMA))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_PERIOD"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_PERIOD))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SLASH"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_SLASH))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RSHIFT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_RSHIFT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_MULTIPLY"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_MULTIPLY))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LMENU"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_LMENU))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SPACE"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_SPACE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_CAPITAL"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_CAPITAL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F1"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F1))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F2"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F2))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F3"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F3))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	if (Cmp(keyCode, "DIK_F4"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F4))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F5"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F5))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F6"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F6))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F7"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F7))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F8"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F8))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F9"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F9))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F10"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F10))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMLOCK"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMLOCK))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SCROLL"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_SCROLL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD7"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD7))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD8"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD8))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD9"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD9))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SUBTRACT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_SUBTRACT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD4"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD4))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD5"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD5))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD6"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD6))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_ADD"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_ADD))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD1"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD1))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD2"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD2))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD3"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD3))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPAD0"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPAD0))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_DECIMAL"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_DECIMAL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F11"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F11))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F12"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F12))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F13"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F13))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F14"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F14))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_F15"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_F15))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_KANA"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_KANA))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_CONVERT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_CONVERT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NOCONVERT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NOCONVERT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_YEN"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_YEN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPADEQUALS"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPADEQUALS))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_CIRCUMFLEX"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_CIRCUMFLEX))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_AT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_AT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_COLON"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_COLON))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_UNDERLINE"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_UNDERLINE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_KANJI"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_KANJI))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_STOP"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_STOP))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_AX"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_AX))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_UNLABELED"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_UNLABELED))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPADENTER"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPADENTER))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RCONTROL"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_RCONTROL))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NUMPADCOMMA"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NUMPADCOMMA))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_DIVIDE"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_DIVIDE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SYSRQ"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_SYSRQ))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RMENU"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_RMENU))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_PAUSE"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_PAUSE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_HOME"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_HOME))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_UP"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_UP))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_PRIOR"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_PRIOR))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LEFT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_LEFT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RIGHT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_RIGHT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_END"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_END))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_DOWN"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_DOWN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_NEXT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_NEXT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_INSERT"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_INSERT))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_DELETE"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_DELETE))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_LWIN"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_LWIN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_RWIN"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_RWIN))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_APPS"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_APPS))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_POWER"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_POWER))
		{
			lua_pushboolean(L, 1); //true
			return 1;
		}
	}
	else if (Cmp(keyCode, "DIK_SLEEP"))
	{
		if (g_multipleView->m_inputSystem->KeyDown(DIK_SLEEP))
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetSelectionDistance()", COLOR_RED);
		return 0;
	}
	g_multipleView->SetSelectionDistance(CFloat(lua_tonumber(L, 1)));
	return 0;
}

CInt GetSelectionDistance(lua_State *L)
{
	if (g_testScript)
		return 0;

	CFloat selectionDistance = g_multipleView->GetSelectionDistance();
	lua_pushnumber(L, selectionDistance);
	return 1;
}

CInt SelectPrefabInstances(lua_State *L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for SelectPrefabInstances()", COLOR_RED);
		return 0;
	}

	CDouble mouseXPos = CDouble(lua_tonumber(L, 1));
	CDouble mouseYPos = CDouble(lua_tonumber(L, 2));
	CDouble selectionWidth = CDouble(lua_tonumber(L, 3));
	CDouble selectionHeight = CDouble(lua_tonumber(L, 4));

	CChar* selectedName = g_multipleView->SelectPrefabInstances(mouseXPos, mouseYPos, selectionWidth, selectionHeight);

	lua_pushstring(L, selectedName);
	return 1;
}

CInt GetScreenWidth(lua_State* L)
{
	if (g_testScript)
		return 0;

	CFloat width = (CFloat)g_width;
	lua_pushnumber(L, width);
	return 1;
}

CInt GetScreenHeight(lua_State* L)
{
	if (g_testScript)
		return 0;

	CFloat height = (CFloat)g_height;
	lua_pushnumber(L, height);
	return 1;
}

CInt GetCursorX(lua_State* L)
{
	if (g_testScript)
		return 0;

	CFloat posX = g_multipleView->GetCursorX();
	lua_pushnumber(L, posX);
	return 1;

}

CInt GetCursorY(lua_State* L)
{
	if (g_testScript)
		return 0;

	CFloat posY = g_multipleView->GetCursorY();
	lua_pushnumber(L, posY);
	return 1;
}

CInt GetElapsedTime(lua_State* L)
{
	if (g_testScript)
		return 0;

	lua_pushnumber(L, g_elapsedTime);
	return 1;
}

CInt IsMenuEnabled(lua_State* L)
{
	if (g_testScript)
		return 0;

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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for GetPrefabInstanceNameFromActor()", COLOR_RED);
		return 0;
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		PrintInfo("\nVanda Engine will try to find the prefab instance name that contains specifed physics actor in VScene Mode");
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for TranslatePrefabInstance()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				if (g_currentInstancePrefab->GetScene(0) && g_currentInstancePrefab->GetScene(0)->IsTransformable())
					g_currentInstancePrefab->SetTranslate(pos);
			}
			else
				PrintInfo("\nTranslatePrefabInstance() Error: Couldn't find current prefab instance", COLOR_RED);
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			if (g_prefabProperties.m_isTransformable)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "\nTranslatePrefabInstance(%.2f, %.2f, %.2f)", xPos, yPos, zPos);
				PrintInfo(temp, COLOR_GREEN);
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						if (g_prefabProperties.m_isTransformable)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nTranslatePrefabInstance() will be executed for Project '%s', VScene '%s' : Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
							PrintInfo(message, COLOR_GREEN);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First argument: prefab instance name
//2nd, 3rd, and 4th arguments: x rotation, y rotation, z rotation of prefab instance
CInt RotatePrefabInstance(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for RotatePrefabInstance()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
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
				PrintInfo("\nRotatePrefabInstance() Error: Couldn't find current prefab instance", COLOR_RED);
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			if (g_prefabProperties.m_isTransformable)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "\nRotatePrefabInstance(%.2f, %.2f, %.2f)", xRot, yRot, zRot);
				PrintInfo(temp, COLOR_GREEN);
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						if (g_prefabProperties.m_isTransformable)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nRotatePrefabInstance() will be executed for Project '%s', VScene '%s' : Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
							PrintInfo(message, COLOR_GREEN);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//First argument: prefab instance name
//2nd, 3rd, and 4th arguments: x scale, y scale, z scale of prefab instance
CInt ScalePrefabInstance(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for ScalePrefabInstance()", COLOR_RED);
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
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				if (g_currentInstancePrefab->GetScene(0) && g_currentInstancePrefab->GetScene(0)->IsTransformable())
				{
					g_currentInstancePrefab->SetScale(scale);
				}
			}
			else
				PrintInfo("\nScalePrefabInstance() Error: Couldn't find current prefab instance", COLOR_RED);
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			if (g_prefabProperties.m_isTransformable)
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "\nScalePrefabInstance(%.2f, %.2f, %.2f)", xScale, yScale, zScale);
				PrintInfo(temp, COLOR_GREEN);
			}
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						if (g_prefabProperties.m_isTransformable)
						{
							CChar message[MAX_NAME_SIZE];
							sprintf(message, "\nScalePrefabInstance() will be executed for Project '%s', VScene '%s' : Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
							PrintInfo(message, COLOR_GREEN);
						}
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetPrefabInstanceTranslate(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 arguments for GetPrefabInstanceTranslate()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
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
				PrintInfo("\nGetPrefabInstanceTranslate() Error: Couldn't find current prefab instance", COLOR_RED);
				return 0;
			}
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			lua_pushnumber(L, 0.0);
			lua_pushnumber(L, 0.0);
			lua_pushnumber(L, 0.0);

			return 3;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nGetPrefabInstanceTranslate() will be executed for Project '%s', VScene '%s' : Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetPrefabInstanceRotate(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 arguments for GetPrefabInstanceRotate()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
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
				PrintInfo("\nGetPrefabInstanceRotate() Error: Couldn't find current prefab instance", COLOR_RED);
				return 0;
			}
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			lua_pushnumber(L, 0.0);
			lua_pushnumber(L, 0.0);
			lua_pushnumber(L, 0.0);

			return 3;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nGetPrefabInstanceRotate() will be executed for Project '%s', VScene '%s' : Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetPrefabInstanceScale(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 arguments for GetPrefabInstanceScale()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
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
				PrintInfo("\nGetPrefabInstanceScale() Error: Couldn't find current prefab instance", COLOR_RED);
				return 0;
			}
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			lua_pushnumber(L, 1.0);
			lua_pushnumber(L, 1.0);
			lua_pushnumber(L, 1.0);

			return 3;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nGetPrefabInstanceScale() will be executed for Project '%s', VScene '%s' : Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetPrefabInstanceRadius(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 arguments for GetPrefabInstanceRadius()", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				CFloat radius = g_currentInstancePrefab->GetRadius();

				lua_pushnumber(L, radius);

				return 1;
			}
			else
			{
				PrintInfo("\nGetPrefabInstanceRadius() Error: Couldn't find current prefab instance", COLOR_RED);
				return 0;
			}
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			PrintInfo("\nGetPrefabInstanceRadius() will get current prefab instance radius in VScene mode", COLOR_GREEN);

			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nGetPrefabInstanceRadius() will be executed for Project '%s', VScene '%s' : Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
			CFloat radius = g_instancePrefab[i]->GetRadius();

			lua_pushnumber(L, radius);

			return 1;
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt GetDistanceOfPrefabInstanceFromPhysicsCamera(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 arguments for GetDistanceOfPrefabInstanceFromPhysicsCamera()", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				CFloat distance = g_currentInstancePrefab->GetDistanceFromCamera();

				lua_pushnumber(L, distance);

				return 1;
			}
			else
			{
				PrintInfo("\nGetDistanceOfPrefabInstanceFromPhysicsCamera() Error: Couldn't find current prefab instance", COLOR_RED);
				return 0;
			}
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			PrintInfo("\nGetDistanceOfPrefabInstanceFromPhysicsCamera() will get the distance of current prefab instance from camera in VScene mode", COLOR_GREEN);

			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nGetDistanceOfPrefabInstanceFromPhysicsCamera() will be executed for Project '%s', VScene '%s' : Prefab Instance '%s'", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
			CFloat distance = g_instancePrefab[i]->GetDistanceFromCamera();

			lua_pushnumber(L, distance);

			return 1;
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

//depth of field
CInt EnableDepthOfField(lua_State* L)
{
	if (g_testScript)
		return 0;

	g_dofProperties.m_enable = CTrue;
	return 0;
}

CInt DisableDepthOfField(lua_State* L)
{
	if (g_testScript)
		return 0;

	g_dofProperties.m_enable = CFalse;
	return 0;
}

CInt SetDepthOfFieldFocalDistance(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDepthOfFieldFocalDistance()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);
	if (value <= 0.0f)
	{
		PrintInfo("\nPlease specify a positive value for SetDepthOfFieldFocalDistance()", COLOR_RED);
		return 0;
	}

	g_dofProperties.m_dofFocalDistance = value;

	return 0;
}

CInt SetDepthOfFieldFocalRange(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDepthOfFieldFocalRange()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);
	if (value <= 0.0f)
	{
		PrintInfo("\nPlease specify a positive value for SetDepthOfFieldFocalRange()", COLOR_RED);
		return 0;
	}

	g_dofProperties.m_dofFocalRange = value;

	return 0;
}

//fog
CInt EnableFog(lua_State* L)
{
	if (g_testScript)
		return 0;

	g_fogProperties.m_enable = CTrue;
	return 0;
}

CInt DisableFog(lua_State* L)
{
	if (g_testScript)
		return 0;

	g_fogProperties.m_enable = CFalse;
	return 0;
}

CInt SetFogColor(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		PrintInfo("\nPlease specify 3 arguments for SetFogColor()", COLOR_RED);
		return 0;
	}

	CFloat value[4];

	for (CUInt i = 1; i < 4; i++)
	{
		value[i - 1] = (CFloat)lua_tonumber(L, i);
		if (value[i - 1] < 0.0f || value[i - 1] > 1.0f)
		{
			PrintInfo("\nError: All arguments of SetFogColor() must be between 0.0 and 1.0", COLOR_RED);
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetFogDensity()", COLOR_RED);
		return 0;
	}

	CFloat density;

	density = (CFloat)lua_tonumber(L, 1);

	if (density < 0.0f || density > 1.0f)
	{
		PrintInfo("\nError: argument of SetFogDensity() must be between 0.0 and 1.0", COLOR_RED);
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
	if (g_testScript)
		return 0;

	g_bloomProperties.m_enable = CTrue;
	return 0;
}

CInt DisableBloom(lua_State* L)
{
	if (g_testScript)
		return 0;

	g_bloomProperties.m_enable = CFalse;
	return 0;
}

CInt SetBloomColor(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		PrintInfo("\nPlease specify 3 arguments for SetBloomColor()", COLOR_RED);
		return 0;
	}

	CFloat value[3];
	for (CUInt i = 1; i < 4; i++)
	{
		value[i - 1] = (CFloat)lua_tonumber(L, i);
		if (value[i - 1] < 0.0f || value[i - 1] > 1.0f)
		{
			PrintInfo("\nError: All arguments of SetBloomColor() must be between 0.0 and 1.0", COLOR_RED);
			return 0;
		}
		g_bloomProperties.m_bloomColor[i - 1] = value[i - 1];
	}

	return 0;
}

CInt SetBloomIntensity(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetBloomIntensity()", COLOR_RED);
		return 0;
	}

	CFloat intensity;

	intensity = (CFloat)lua_tonumber(L, 1);
	if (intensity < 0.0f || intensity > 1.0f)
	{
		PrintInfo("\nError: argument of SetBloomIntensity() must be between 0.0 and 1.0", COLOR_RED);
		return 0;
	}

	g_bloomProperties.m_bloomIntensity = intensity;

	return 0;
}

//shadow
CInt EnableDirectionalShadow(lua_State* L)
{
	if (g_testScript)
		return 0;

	g_shadowProperties.m_enable = CTrue;

	return 0;
}

CInt DisableDirectionalShadow(lua_State* L)
{
	if (g_testScript)
		return 0;

	g_shadowProperties.m_enable = CFalse;

	return 0;
}

CInt SetDirectionalShadowAlgorithm(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowAlgorithm()", COLOR_RED);
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
		PrintInfo("\nError: SetDirectionalShadowAlgorithm() argument is invalid", COLOR_RED);
	}

	return 0;
}

CInt SetDirectionalShadowNumberOfSplits(lua_State* L)
{
	if (g_testScript)
		return 0;

	if (g_dynamicShadowMap == NULL)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowNumberOfSplits()", COLOR_RED);
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
		PrintInfo("\nError:SetDirectionalShadowNumberOfSplits() argument must be 1, 2, 3 or 4", COLOR_RED);
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
	if (g_testScript)
		return 0;

	if (g_dynamicShadowMap == NULL)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowWeightOfSplits()", COLOR_RED);
		return 0;
	}

	CFloat weight = (CFloat)lua_tonumber(L, 1);

	g_shadowProperties.m_shadowSplitWeight = weight;

	g_dynamicShadowMap->split_weight = g_shadowProperties.m_shadowSplitWeight;

	return 0;
}

CInt SetDirectionalShadowNearClipPlane(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowNearClipPlane()", COLOR_RED);
		return 0;
	}

	CFloat ncp = (CFloat)lua_tonumber(L, 1);

	if (ncp <= 0.0f)
	{
		PrintInfo("\nError: SetDirectionalShadowNearClipPlane() argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_shadowProperties.m_shadowNearClipPlane = ncp;

	return 0;
}

CInt SetDirectionalShadowFarClipPlane(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowFarClipPlane()", COLOR_RED);
		return 0;
	}

	CFloat fcp = (CFloat)lua_tonumber(L, 1);

	if (fcp <= 0.0f)
	{
		PrintInfo("\nError: SetDirectionalShadowFarClipPlane() argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_shadowProperties.m_shadowFarClipPlane = fcp;

	return 0;
}

CInt SetDirectionalShadowResolution(lua_State* L)
{
	if (g_testScript)
		return 0;

	if (g_dynamicShadowMap == NULL)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowResolution()", COLOR_RED);
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
		PrintInfo("\nError:SetDirectionalShadowResolution() argument must be 1024, 2048 or 4096", COLOR_RED);
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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowIntensity()", COLOR_RED);
		return 0;
	}

	CFloat intensity = (CFloat)lua_tonumber(L, 1);

	if (intensity < 0.0f || intensity > 1.0f)
	{
		PrintInfo("\nError: SetDirectionalShadowIntensity() argument must be between 0.0 and 1.0", COLOR_RED);
		return 0;
	}

	g_shadowProperties.m_intensity = intensity;

	return 0;
}

CInt SetDirectionalShadowLight(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDirectionalShadowLight()", COLOR_RED);
		return 0;
	}
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));

	CBool foundTarget = CFalse;
	if (g_editorMode == eMODE_VSCENE)
	{
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
			PrintInfo("\nSetDirectionalShadowLight() Error:light name does not exist", COLOR_RED);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames.size(); j++)
				{
					CChar lightName[MAX_NAME_SIZE];
					Cpy(lightName, g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str());

					if (Cmp(lightName, luaToString))
					{
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\nProject '%s', VScene '%s' : Light name '%s' will be set as default light for directional shadow", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str());
						PrintInfo(temp, COLOR_GREEN);
						foundTarget = CTrue;
						break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nSetDirectionalShadowLight() Error:Couldn't find light name '", lua_tostring(L, 1), "'.");
			PrintInfo(temp, COLOR_RED);
			return 0;
		}
	}
	Cpy(g_shadowProperties.m_directionalLightName, luaToString);

	return 0;
}

CInt SetLightAmbient(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for SetLightAmbient()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		PrintInfo("\nSecond argument of SetLightAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		PrintInfo("\nThird argument of SetLightAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		PrintInfo("\nFourth argument of SetLightAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		CBool foundTarget = CFalse;

		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames.size(); j++)
				{
					CChar lightName[MAX_NAME_SIZE];
					Cpy(lightName, g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str());
					StringToUpper(lightName);

					if (Cmp(lightName, luaToString))
					{
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\nProject '%s', VScene '%s' : SetLightAmbient(%s, %.2f, %.2f, %.2f) will change ambient light", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str(), R, G, B);
						PrintInfo(temp, COLOR_GREEN);
						foundTarget = CTrue;
						break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nSetLightAmbient() Error: Couldn't find light '", lua_tostring(L, 1), "' in all projects.");
			PrintInfo(temp, COLOR_RED);
		}
		return 0;
	}

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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for SetLightDiffuse()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		PrintInfo("\nSecond argument of SetLightDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		PrintInfo("\nThird argument of SetLightDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		PrintInfo("\nFourth argument of SetLightDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		CBool foundTarget = CFalse;

		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames.size(); j++)
				{
					CChar lightName[MAX_NAME_SIZE];
					Cpy(lightName, g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str());
					StringToUpper(lightName);

					if (Cmp(lightName, luaToString))
					{
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\nProject '%s', VScene '%s' : SetLightDiffuse(%s, %.2f, %.2f, %.2f) will change diffuse light", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str(), R, G, B);
						PrintInfo(temp, COLOR_GREEN);
						foundTarget = CTrue;
						break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nSetLightDiffuse() Error: Couldn't find light '", lua_tostring(L, 1), "' in all projects.");
			PrintInfo(temp, COLOR_RED);
		}
		return 0;
	}

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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for SetLightSpecular()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		PrintInfo("\nSecond argument of SetLightSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		PrintInfo("\nThird argument of SetLightSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		PrintInfo("\nFourth argument of SetLightSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		CBool foundTarget = CFalse;

		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames.size(); j++)
				{
					CChar lightName[MAX_NAME_SIZE];
					Cpy(lightName, g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str());
					StringToUpper(lightName);

					if (Cmp(lightName, luaToString))
					{
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\nProject '%s', VScene '%s' : SetLightSpecular(%s, %.2f, %.2f, %.2f) will change specular light", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str(), R, G, B);
						PrintInfo(temp, COLOR_GREEN);
						foundTarget = CTrue;
						break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nSetLightSpecular() Error: Couldn't find light '", lua_tostring(L, 1), "' in all projects.");
			PrintInfo(temp, COLOR_RED);
		}
		return 0;
	}

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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for SetLightShininess()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CFloat shininess = (CFloat)lua_tonumber(L, 2);

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		CBool foundTarget = CFalse;

		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames.size(); j++)
				{
					CChar lightName[MAX_NAME_SIZE];
					Cpy(lightName, g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str());
					StringToUpper(lightName);

					if (Cmp(lightName, luaToString))
					{
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "\nProject '%s', VScene '%s' : SetLightShininess(%s, %.2f) will change shininess", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_engineLightNames[j].c_str(), shininess);
						PrintInfo(temp, COLOR_GREEN);
						foundTarget = CTrue;
						break;
					}
				}
			}
		}
		if (!foundTarget)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "%s%s%s", "\nSetLightShininess() Error: Couldn't find light '", lua_tostring(L, 1), "' in all projects.");
			PrintInfo(temp, COLOR_RED);
		}
		return 0;
	}

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
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for SetPrefabInstanceAmbient()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		PrintInfo("\nSecond argument of SetPrefabInstanceAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		PrintInfo("\nThird argument of SetPrefabInstanceAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		PrintInfo("\nFourth argument of SetPrefabInstanceAmbient() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				g_currentInstancePrefab->SetAmbient(Color);
			}
			else
			{
				PrintInfo("\nSetPrefabInstanceAmbient() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceAmbient(%.2f, %.2f, %.2f) will execute for current prefab instance", R, G, B);
			PrintInfo(temp, COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : SetPrefabInstanceAmbient(%s, %.2f, %.2f, %.2f) will execute", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, R, G, B);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceAmbient() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nSetPrefabInstanceAmbient() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceDiffuse(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for SetPrefabInstanceDiffuse()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		PrintInfo("\nSecond argument of SetPrefabInstanceDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		PrintInfo("\nThird argument of SetPrefabInstanceDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		PrintInfo("\nFourth argument of SetPrefabInstanceDiffuse() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				g_currentInstancePrefab->SetDiffuse(Color);
			}
			else
			{
				PrintInfo("\nSetPrefabInstanceDiffuse() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceDiffuse(%.2f, %.2f, %.2f) will execute for current prefab instance", R, G, B);
			PrintInfo(temp, COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : SetPrefabInstanceDiffuse(%s, %.2f, %.2f, %.2f) will execute", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, R, G, B);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceDiffuse() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nSetPrefabInstanceDiffuse() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceSpecular(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for SetPrefabInstanceSpecular()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		PrintInfo("\nSecond argument of SetPrefabInstanceSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		PrintInfo("\nThird argument of SetPrefabInstanceSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		PrintInfo("\nFourth argument of SetPrefabInstanceSpecular() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				g_currentInstancePrefab->SetSpecular(Color);
			}
			else
			{
				PrintInfo("\nSetPrefabInstanceSpecular() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceSpecular(%.2f, %.2f, %.2f) will execute for current prefab instance", R, G, B);
			PrintInfo(temp, COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : SetPrefabInstanceSpecular(%s, %.2f, %.2f, %.2f) will execute", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, R, G, B);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceSpecular() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nSetPrefabInstanceSpecular() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceEmission(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 4)
	{
		PrintInfo("\nPlease specify 4 arguments for SetPrefabInstanceEmission()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CBool foundColorError = CFalse;
	CFloat R = (CFloat)lua_tonumber(L, 2);
	if (R < 0.0f || R > 1.0f)
	{
		PrintInfo("\nSecond argument of SetPrefabInstanceEmission() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat G = (CFloat)lua_tonumber(L, 3);
	if (G < 0.0f || G > 1.0f)
	{
		PrintInfo("\nThird argument of SetPrefabInstanceEmission() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	CFloat B = (CFloat)lua_tonumber(L, 4);
	if (B < 0.0f || B > 1.0f)
	{
		PrintInfo("\nFourth argument of SetPrefabInstanceEmission() must be between 0.0 and 1.0", COLOR_RED);
		foundColorError = CTrue;
	}

	if (foundColorError)
		return 0;

	CFloat Color[4] = { R, G, B, 1.0f };

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				g_currentInstancePrefab->SetEmission(Color);
			}
			else
			{
				PrintInfo("\nSetPrefabInstanceEmission() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceEmission(%.2f, %.2f, %.2f) will execute for current prefab instance", R, G, B);
			PrintInfo(temp, COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : SetPrefabInstanceEmission(%s, %.2f, %.2f, %.2f) will execute", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, R, G, B);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceEmission() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nSetPrefabInstanceEmission() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceShininess(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for SetPrefabInstanceShininess()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CFloat shininess = (CFloat)lua_tonumber(L, 2);
	if (shininess < 0.0f)
	{
		PrintInfo("\nSecond argument of SetPrefabInstanceShininess() must be 0.0 or higher", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				g_currentInstancePrefab->SetShininess(shininess);
			}
			else
			{
				PrintInfo("\nSetPrefabInstanceShininess() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceShininess(%.2f) will execute for current prefab instance", shininess);
			PrintInfo(temp, COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : SetPrefabInstanceShininess(%s, %.2f) will execute", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, shininess);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceShininess() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nSetPrefabInstanceShininess() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt SetPrefabInstanceTransparency(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 2)
	{
		PrintInfo("\nPlease specify 2 arguments for SetPrefabInstanceTransparency()", COLOR_RED);
		return 0;
	}

	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1));
	StringToUpper(luaToString);

	CFloat transparency = (CFloat)lua_tonumber(L, 2);
	if (transparency < 0.0f)
	{
		PrintInfo("\nSecond argument of SetPrefabInstanceTransparency() must be 0.0 or higher", COLOR_RED);
		return 0;
	}

	CBool foundPrefabInstance = CFalse;

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
			{
				g_currentInstancePrefab->SetTransparency(transparency);
			}
			else
			{
				PrintInfo("\nSetPrefabInstanceTransparency() Error: Couldn't find current prefab instance", COLOR_RED);
			}
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceTransparency(%.2f) will execute for current prefab instance", transparency);
			PrintInfo(temp, COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : SetPrefabInstanceTransparency(%s, %.2f) will execute", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name, transparency);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nSetPrefabInstanceTransparency() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nSetPrefabInstanceTransparency() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}

	return 0;
}

CInt EnablePrefabInstanceMaterial(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for EnablePrefabInstanceMaterial()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
				g_currentInstancePrefab->EnableMaterial();
			else
				PrintInfo("\nEnablePrefabInstanceMaterial() Error: Couldn't find current prefab instance", COLOR_RED);
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			PrintInfo("\nMaterial of current prefab instance will become enabled", COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : Material of prefab Instance '%s' will become enabled", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nEnablePrefabInstanceMaterial() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
			g_instancePrefab[i]->EnableMaterial();
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nEnablePrefabInstanceMaterial() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
		return 0;
	}
	return 0;
}

CInt DisablePrefabInstanceMaterial(lua_State* L)
{
	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for DisablePrefabInstanceMaterial()", COLOR_RED);
		return 0;
	}
	CBool foundPrefabInstance = CFalse;
	//find the scene
	CChar luaToString[MAX_NAME_SIZE];
	Cpy(luaToString, lua_tostring(L, 1)); //Prefab Instance Name- First Argument
	StringToUpper(luaToString);

	if (Cmp("THIS", luaToString))
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			if (g_currentInstancePrefab)
				g_currentInstancePrefab->DisableMaterial();
			else
				PrintInfo("\nDisablePrefabInstanceMaterial Error: Couldn't find current prefab instance", COLOR_RED);
			return 0;
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			PrintInfo("\nMaterial of current prefab instance will become disabled", COLOR_GREEN);
			return 0;
		}
	}

	if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_GUI)
	{
		for (CUInt pr = 0; pr < g_projects.size(); pr++)
		{
			for (CUInt i = 0; i < g_projects[pr]->m_vsceneObjectNames.size(); i++)
			{
				for (CUInt j = 0; j < g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames.size(); j++)
				{
					CChar prefabInstanceName[MAX_NAME_SIZE];
					Cpy(prefabInstanceName, g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
					StringToUpper(prefabInstanceName);

					if (Cmp(prefabInstanceName, luaToString))
					{
						foundPrefabInstance = CTrue;
						CChar message[MAX_NAME_SIZE];
						sprintf(message, "\nProject '%s', VScene '%s' : Material of prefab Instance '%s' will become disabled", g_projects[pr]->m_name, g_projects[pr]->m_sceneNames[i].c_str(), g_projects[pr]->m_vsceneObjectNames[i].m_instancePrefabNames[j].m_name);
						PrintInfo(message, COLOR_GREEN);
						break;
					}
				}
			}
		}
		if (!foundPrefabInstance)
		{
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\nDisablePrefabInstanceMaterial() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
			PrintInfo(temp, COLOR_RED);
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
			g_instancePrefab[i]->DisableMaterial();
		}
	}
	if (!foundPrefabInstance)
	{
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\nDisablePrefabInstanceMaterial() Error: %s%s%s", "Couldn't find '", luaToString, "' Prefab Instance");
		PrintInfo(temp, COLOR_RED);
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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsRestitution()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetPhysicsRestitution() Error: argument must be greater than 0", COLOR_RED);
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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsSkinWidth()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetPhysicsSkinWidth() Error: argument must be greater than 0", COLOR_RED);
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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsStaticFriction()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetPhysicsStaticFriction() Error: argument must be greater than 0", COLOR_RED);
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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicsDynamicFriction()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetPhysicsDynamicFriction() Error: argument must be greater than 0", COLOR_RED);
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
	if (g_testScript)
		return 0;

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
	g_multipleView->m_nx->m_defaultGravity = NxVec3(g_physXProperties.m_fGravityX, g_physXProperties.m_fGravityY, g_physXProperties.m_fGravityZ);
	gPhysXscene->setGravity(NxVec3(g_multipleView->m_nx->m_defaultGravity.x, g_multipleView->m_nx->m_defaultGravity.y, g_multipleView->m_nx->m_defaultGravity.z));

	return 0;
}

CInt DisablePhysicsGravity(lua_State* L)
{
	if (g_testScript)
		return 0;

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
	g_multipleView->m_nx->m_defaultGravity = NxVec3(0.0f, 0.0f, 0.0f);
	gPhysXscene->setGravity(NxVec3(0.0f));

	return 0;
}

CInt SetPhysicsGravity(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		PrintInfo("\nPlease specify 3 arguments for SetPhysicsGravity()", COLOR_RED);
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
		g_multipleView->m_nx->m_defaultGravity = NxVec3(g_physXProperties.m_fGravityX, g_physXProperties.m_fGravityY, g_physXProperties.m_fGravityZ);
		gPhysXscene->setGravity(NxVec3(g_multipleView->m_nx->m_defaultGravity.x, g_multipleView->m_nx->m_defaultGravity.y, g_multipleView->m_nx->m_defaultGravity.z));
	}

	return 0;
}

CInt EnablePhysicsGroundPlane(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	g_physXProperties.m_bGroundPlane = CTrue;

	g_multipleView->m_nx->ReleaseGroundPlane();

	g_multipleView->m_nx->CreateGroundPlane(g_physXProperties.m_fGroundHeight);
	NxVec3 rot0(0, 0, 0);
	NxVec3 rot1(0, 0, 0);
	NxVec3 rot2(0, 0, 0);
	NxMat33 rot(rot0, rot1, rot2);
	g_multipleView->m_nx->m_groundBox = g_multipleView->m_nx->CreateBox(NxVec3(0.0f, g_physXProperties.m_fGroundHeight, 0.0f), NxVec3(2000.0f, 0.1, 2000.0f), 0, rot, NULL, CFalse, CFalse);

	g_multipleView->m_nx->gControllers->reportSceneChanged();
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

	if (g_testScript)
		return 0;

	g_physXProperties.m_bGroundPlane = CFalse;

	g_multipleView->m_nx->ReleaseGroundPlane();

	g_multipleView->m_nx->gControllers->reportSceneChanged();
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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetPhysicGroundHeight()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);
	g_physXProperties.m_fGroundHeight = value;
	
	if (g_physXProperties.m_bGroundPlane)
	{
		g_multipleView->m_nx->ReleaseGroundPlane();

		g_multipleView->m_nx->CreateGroundPlane(g_physXProperties.m_fGroundHeight);
		NxVec3 rot0(0, 0, 0);
		NxVec3 rot1(0, 0, 0);
		NxVec3 rot2(0, 0, 0);
		NxMat33 rot(rot0, rot1, rot2);
		g_multipleView->m_nx->m_groundBox = g_multipleView->m_nx->CreateBox(NxVec3(0.0f, g_physXProperties.m_fGroundHeight, 0.0f), NxVec3(2000.0f, 0.1, 2000.0f), 0, rot, NULL, CFalse, CFalse);
	}

	g_multipleView->m_nx->gControllers->reportSceneChanged();
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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetDistanceBetweenPhysicsCameraAndCharacterController()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetDistanceBetweenPhysicsCameraAndCharacterController() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCameraCharacterDistance = value;
	g_multipleView->m_nx->gDesiredDistance = value;

	return 0;
}

CInt SetCharacterControllerCapsuleRadius(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetCharacterControllerCapsuleRadius()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetCharacterControllerCapsuleRadius() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCapsuleRadius = value;
	g_multipleView->m_nx->gControllers->setRadius(g_physXProperties.m_fCapsuleRadius);

	return 0;
}

CInt SetCharacterControllerCapsuleHeight(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetCharacterControllerCapsuleHeight()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetCharacterControllerCapsuleHeight() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCapsuleHeight = value;
	g_multipleView->m_nx->gControllers->setHeight(g_physXProperties.m_fCapsuleHeight);

	return 0;
}

CInt SetCharacterControllerForcePower(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetCharacterControllerForcePower()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetCharacterControllerForcePower() Error: argument must be greater than 0", COLOR_RED);
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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetCharacterControllerWalkSpeed()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetCharacterControllerWalkSpeed() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCharacterWalkSpeed = value;
	g_multipleView->m_nx->gCharacterWalkSpeed = g_physXProperties.m_fCharacterWalkSpeed;

	return 0;
}

CInt SetCharacterControllerRunSpeed(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetCharacterControllerRunSpeed()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetCharacterControllerRunSpeed() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCharacterRunSpeed = value;
	g_multipleView->m_nx->gCharacterRunSpeed = g_physXProperties.m_fCharacterRunSpeed;

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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetCharacterControllerStepOffset()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetCharacterControllerStepOffset() Error: argument must be greater than 0", COLOR_RED);
		return 0;
	}

	g_physXProperties.m_fCharacterStepOffset = value;
	g_multipleView->m_nx->gControllers->setStepOffset(g_physXProperties.m_fCharacterStepOffset);

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

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 1)
	{
		PrintInfo("\nPlease specify 1 argument for SetCharacterControllerJumpPower()", COLOR_RED);
		return 0;
	}

	CFloat value = (CFloat)lua_tonumber(L, 1);

	if (value < 0.0f)
	{
		PrintInfo("\nSetCharacterControllerJumpPower() Error: argument must be greater than 0", COLOR_RED);
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

	if (g_testScript)
		return 0;

	g_physXProperties.m_bJumping = CTrue;

	return 0;
}

CInt DisableCharacterControllerJump(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	g_physXProperties.m_bJumping = CFalse;

	return 0;
}

CInt EnablePhysicsDebugMode(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	g_physXProperties.m_bDebugMode = CTrue;
	g_multipleView->m_nx->debugMode = g_physXProperties.m_bDebugMode;

	return 0;
}

CInt DisablePhysicsDebugMode(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	g_physXProperties.m_bDebugMode = CFalse;
	g_multipleView->m_nx->debugMode = g_physXProperties.m_bDebugMode;

	return 0;
}

CInt SetCharacterControllerPosition(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	int argc = lua_gettop(L);
	if (argc < 3)
	{
		PrintInfo("\nPlease specify 3 arguments for SetCharacterControllerPosition()", COLOR_RED);
		return 0;
	}

	CFloat value1 = (CFloat)lua_tonumber(L, 1);
	CFloat value2 = (CFloat)lua_tonumber(L, 2);
	CFloat value3 = (CFloat)lua_tonumber(L, 3);

	NxExtendedVec3 nxPos(value1, value2, value3);
	g_multipleView->m_nx->SetCharacterPos(nxPos);

	return 0;
}

CInt GetCharacterControllerPosition(lua_State* L)
{
	if (!gPhysXscene)
	{
		return 0;
	}

	if (g_testScript)
		return 0;

	CFloat x = g_multipleView->m_nx->GetFilteredCharacterPos().x;
	CFloat y = g_multipleView->m_nx->GetFilteredCharacterPos().y;
	CFloat z = g_multipleView->m_nx->GetFilteredCharacterPos().z;

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);

	return 3;
}

CBool CMultipleWindows::firstIdle = CTrue;
CChar CMultipleWindows::currentIdleName[MAX_NAME_SIZE];

CMultipleWindows::CMultipleWindows()
: m_multiSample(CFalse)
, m_grid( CTrue )
, m_numSamples( 8 )
, m_lMouseDown(CFalse)
,m_rMouseDown(CFalse)
,m_mMouseDown(CFalse)
,m_selectObject(CTrue)
,m_tempMovement(CFalse)
,m_tempSelectedName( -1 )
,m_lastSelectedName( -1 )
,m_loadData( CTrue )
,m_dynamicShadowMap(NULL)
,m_timerCounter(0)
,m_lockInput(CFalse)
,m_loadScene(CFalse)
,m_isPlayingGame(CFalse)
, m_renderQuery(CFalse)
, m_characterRotationTransition(CFalse)
, m_previousCharacterRotation(0.0f)
, m_initError(CFalse)
, m_idleCounter(0.0f)
{
	m_selectedGUIIndex = -1;
	m_cursorIcon = CNew(CIcon);
	Cpy(m_previousCharacterAnimationType, "\n");
	m_menuCursorImg = NULL;
	m_playGameMode = CFalse;
	m_saveScreenshot = CFalse;
	m_swapBuffers = CTrue;
	m_notFocus = CFalse; //by default we focus on main dialog

	m_fSelectionDistance = -1.f;
	m_bSelectionDistance = CFalse;
}

CMultipleWindows::~CMultipleWindows()
{
	CDelete(m_cursorIcon);
	DeleteMenuCursorTexture();

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
	
	CDelete( g_translateArrow );
	CDelete( g_negativeZArrow );
	CDelete (g_scaleArrow);
	CDelete(g_rotateArrow);
	CDelete( g_centerArrowScene );
	CDelete( g_soundImg );
	CDelete( g_pointLightImg );
	CDelete( g_directionalLightImg );
	CDelete( g_spotLightImg );
	CDelete( g_pointLightCOLLADAImg );
	CDelete( g_directionalLightCOLLADAImg );
	CDelete( g_spotLightCOLLADAImg );
	CDelete( g_waterImg );
	CDelete( g_skyImg );
	CDelete( g_cameraImg );

	CDelete( m_translationController );
	CDelete(g_mainCharacter);
	//Release physX here
	if( m_nx )
	{
		m_nx->releaseNx();
		delete m_nx;
		m_nx = NULL;
	}
	//Release Audio
	CDelete( m_ambientSound );
	CDelete(m_soundSystem );

	//Release input here
	m_inputSystem->UnacquireAll();
	m_inputSystem->Shutdown();
	CDelete( m_inputSystem );

	CDelete( g_camera );
	CDelete( m_dynamicShadowMap );
	g_octree->ResetState();
	m_dof.DeleteFBOs();
	m_simpleFont.Release();
	m_simpleFont2.Release();
	distance_vector.clear();
	sorted_prefabs.clear();
	KillTimer( 1 );
}


BEGIN_MESSAGE_MAP(CMultipleWindows, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()

	ON_WM_MOUSEHOVER()
END_MESSAGE_MAP()

// CMultipleWindows message handlers

CInt CMultipleWindows::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  Add your specialized creation code here
	// Can we put this in the constructor?
  
	CInt PixelFormat;
	PIXELFORMATDESCRIPTOR pfd;
	memset (&pfd, 0, sizeof(pfd));
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits	= 24;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits	= 32;
	pfd.cStencilBits = 0;

	//Get a DC for the Client Area
    m_pDC = new CClientDC(this);

    //Failure to Get DC
    if(m_pDC == NULL) {
        MessageBox(_T("Failure to Get DC"));
        return -1;
	}
	if( m_multiSample )
	{
		switch( g_options.m_numSamples )
		{
			case 2:
				m_numSamples = 2;
				break;
			case 4:
				m_numSamples = 4;
				break;
			case 8:
				m_numSamples = 8;
				break;
			case 16:
				m_numSamples = 16;
				break;
			default:
				m_numSamples = 0;
		}
		GLint samples;
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
		if( m_numSamples > (CInt)samples )
			m_numSamples = samples;
		if( WGLEW_ARB_pixel_format )
		{
			CFloat fAttributes[] = {0,0};
			UINT numFormats;
			CInt attributes[] = {
				WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
				WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB,24,
				WGL_ALPHA_BITS_ARB,8,
				WGL_DEPTH_BITS_ARB,24,
				WGL_STENCIL_BITS_ARB,0,
				WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
				WGL_SAMPLE_BUFFERS_ARB,1,
				WGL_SAMPLES_ARB, m_numSamples ,						// Check For m_numSamplesX Multisampling
				0,0
			};

			wglChoosePixelFormatARB( m_pDC->GetSafeHdc(), attributes, fAttributes, 1, &PixelFormat, &numFormats );
			if( numFormats == 0 )
			{
				AfxMessageBox( "Couldn't initialize your window with your desired samples");
				CInt temp = m_numSamples / 2;
				//Test for 2 samples
				while( numFormats == 0 )
				{
					if( temp < 2 )
					{
						m_multiSample = CFalse;
						break;
					}
					attributes[19] = temp;
					wglChoosePixelFormatARB( m_pDC->GetSafeHdc(), attributes, fAttributes, 1, &PixelFormat, &numFormats );
					temp = m_numSamples / 2 ;
				}
				if( numFormats == 0 )
					AfxMessageBox( "Couldn't support multisampling" );
				else
					AfxMessageBox( "I could enable multisampling with lower samples" );
			}
		}
		else
		{
			AfxMessageBox( "Couldn't support multisampling" );
			m_multiSample = CFalse;
		}

	}//if( multisampling )

	if( !m_multiSample )
	{
		PixelFormat = ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfd);
		if (PixelFormat == 0)
		{	
			AfxMessageBox("Couldn't choose the pixel format.");
			delete m_pDC;
			m_pDC = NULL;
			return (-1);							
		}
	}
	if (SetPixelFormat(m_pDC->GetSafeHdc(), PixelFormat, &pfd ) == FALSE)	
	{
		AfxMessageBox("Couldn't set the pixel format.");
		delete m_pDC;
		m_pDC = NULL;
		return (-1);							
	}
	m_hRC = wglCreateContext( m_pDC->GetSafeHdc() );
	if ( m_hRC == NULL)
	{
		AfxMessageBox("Couldn't create the OpenGL context.");
		delete m_pDC;
		m_pDC = NULL;
		return (-1);								
	}

	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))					
	{
		AfxMessageBox("Couldn't make a stream between HDC and HRC.");
		wglDeleteContext(m_hRC);
		delete m_pDC;
		m_pDC = NULL;
		m_hRC = NULL;
		return (-1);								
	}
	//if( !InitAll() )
	//	return (-1);
	return (1);
}

CBool CMultipleWindows::InitAll()
{
	for( CInt i = 0; i < 8; i++ )
	{
		glEnable(GL_LIGHT0 + i);	 //just for per vertex lighting 	

		//This is the properties of the camera light
		GLfloat light_pos0[4] = {0.0f, 0.0f, 0.0f, 0.0f };
		GLfloat light_ambient0[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		GLfloat light_diffuse0[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat light_specular0[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		glLightfv( GL_LIGHT0 + i, GL_POSITION, light_pos0  );
		glLightfv( GL_LIGHT0 + i, GL_AMBIENT , light_ambient0 );
		glLightfv( GL_LIGHT0 + i, GL_DIFFUSE , light_diffuse0 );
		glLightfv( GL_LIGHT0 + i, GL_SPECULAR, light_specular0 );
		glLightf ( GL_LIGHT0 + i, GL_SPOT_CUTOFF,(GLfloat)180.0f );
		glLightf ( GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION , (GLfloat)0.0f );
		glLightf ( GL_LIGHT0 + i, GL_LINEAR_ATTENUATION,	(GLfloat)0.0f );
		glLightf ( GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, (GLfloat)0.0f );
	}
	glShadeModel(GL_SMOOTH);										// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);							// Black Background
	glClearDepth( 1.0 );

	//glEnable(GL_TEXTURE_2D);							
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

	if( m_multiSample )
		glDisable( GL_MULTISAMPLE );
	//glDrawBuffer( GL_BACK );

	if( !m_dynamicShadowMap )
	{
		m_dynamicShadowMap = CNew( CShadowMap );
		g_dynamicShadowMap = m_dynamicShadowMap;
		if( m_dynamicShadowMap->Init() )
			g_render.m_useDynamicShadowMap = CTrue;
		else
		{
			g_render.m_useDynamicShadowMap = CFalse;
		}
	}

	// I use this variable to load the data once. We may destroy and create the window multiple times,
	// But we don't need to load and unload some data for multiple times
	if( m_loadData ) 
	{
		m_dof.InitFBOs( g_width, g_height );
		g_octree = CNew( COctree );
		g_updateOctree = CTrue;
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

		//initialize input
		m_inputSystem = new CInputSystem();
		m_inputSystem->AcquireAll();

		if( !m_inputSystem->Initialize( CWnd::m_hWnd, AfxGetInstanceHandle(), true, IS_USEKEYBOARD | IS_USEMOUSE ) )
		{
			PrintInfo( "\nCouldn't initialize Input Engine", COLOR_RED );
			return false;
		}
		else
			PrintInfo( "\nKeyboard and Mouse manager initialized successfully", COLOR_WHITE );

		//Initialize physX here

		m_nx = new CNovodex();

		if (!m_nx->initNx(-1000.0, g_physXProperties.m_fCapsuleHeight * 0.5f, 1000.0, g_physXProperties.m_fCapsuleRadius, g_physXProperties.m_fCapsuleHeight, g_physXProperties.m_fCharacterSlopeLimit, g_physXProperties.m_fCharacterSkinWidth, g_physXProperties.m_fCharacterStepOffset))
		{
			PrintInfo("\nCouldn't initialize physX", COLOR_RED);
			return false;
		}
		else
		{
			char temp[MAX_NAME_SIZE];
			sprintf( temp, "\nPhysX version %i.%i.%i initialized successfully", NX_SDK_VERSION_MAJOR, NX_SDK_VERSION_MINOR, NX_SDK_VERSION_BUGFIX );
			PrintInfo( temp, COLOR_WHITE );
		}

		if( !timer.Init() )
		{
			PrintInfo("\nCouldn't initialize timer.", COLOR_RED);
			return false; 
		}
		else
			PrintInfo( "\nTimer initialized successfully\n", COLOR_WHITE );

		if (!m_simpleFont.Init("Arial", 14))
		{
			PrintInfo("\nCouldn't initialize the font", COLOR_RED);
			return false;
		}
		if (!m_simpleFont2.Init("Arial", 13))
		{
			PrintInfo("Couldn't initialize the font", COLOR_RED);
			return false;
		}
		g_camera = new CUpdateCamera();

		m_loadData = CFalse;
	}
	SetTimer( 1, 1, 0 );
	m_enableTimer = CTrue;
	SetElapsedTimeFromBeginning(); //reset the timer
	g_translateArrow = CNew( CScene );
	//Load Default Arrows
	g_reportInfo = CFalse;

	g_shareGeometriesBetweenScenes = CFalse;
	if( !g_translateArrow->Load( "Assets/engine/Arrows/ArrowTranslate.dae", CFalse ) )
	{
		PrintInfo( "\nCouldn't load the arrows", COLOR_RED );
		//return false;
	}
	g_render.SetScene( g_translateArrow );
	g_translateArrow->Update();

	g_scaleArrow = CNew( CScene );
	if( !g_scaleArrow->Load( "Assets/engine/Arrows/ArrowScale.dae", CFalse ) )
	{
		PrintInfo("\nCouldn't load the Scale arrow", COLOR_RED);
		//return false;
	}
	g_render.SetScene(g_scaleArrow);
	g_scaleArrow->Update();

	g_rotateArrow = CNew(CScene);
	if (!g_rotateArrow->Load("Assets/engine/Arrows/ArrowRotate.dae", CFalse))
	{
		PrintInfo("\nCouldn't load the Rotate arrow", COLOR_RED);
		//return false;
	}
	g_render.SetScene(g_rotateArrow);
	g_rotateArrow->Update();

	g_negativeZArrow = CNew( CScene );
	if( !g_negativeZArrow->Load( "Assets/engine/Arrows/NegativeZArrow.dae", CFalse ) )
	{
		PrintInfo( "\nCouldn't load the negative Z arrow", COLOR_RED );
		return false;
	}
	g_render.SetScene( g_negativeZArrow );
	g_negativeZArrow->Update();

	g_centerArrowScene = CNew( CScene );
	//Load Default Arrows
	if( !g_centerArrowScene->Load( "Assets/engine/Arrows/ArrowCenter.dae", CFalse ) )
	{
		PrintInfo( "\nCouldn't load the arrows", COLOR_RED );
		return false;
	}
	g_render.SetScene( g_centerArrowScene );
	g_shareGeometriesBetweenScenes = CTrue;
	g_reportInfo = CTrue;

	g_centerArrowScene->Update();
	m_translationController = CNew( CMouseTranslationController );
	//Load Default Textures
	GenerateDefaultTextures();
	return true;

}

CBool CMultipleWindows::GenerateDefaultTextures()
{
	g_soundImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_soundImg, "Assets/Engine/Icons/sound.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the sound texture");
		return CFalse;
	}
	g_soundImg->SetFileName( "sound.dds" );

	//point light
	g_pointLightImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_pointLightImg, "Assets/Engine/Icons/point_light.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the point light texture");
		return CFalse;
	}
	g_pointLightImg->SetFileName( "point_light.dds" );

	//directional light
	g_directionalLightImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_directionalLightImg, "Assets/Engine/Icons/directional_light.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the directional light texture");
		return CFalse;
	}
	g_directionalLightImg->SetFileName( "directional_light.dds" );

	//spot light
	g_spotLightImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_spotLightImg, "Assets/Engine/Icons/spot_light.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the spot light texture");
		return CFalse;
	}
	g_spotLightImg->SetFileName( "spot_light.dds" );

	//COLLADA icons
	//point light
	g_pointLightCOLLADAImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_pointLightCOLLADAImg, "Assets/Engine/Icons/point_light_collada.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the point_light_collada texture");
		return CFalse;
	}
	g_pointLightCOLLADAImg->SetFileName( "point_light_collada.dds" );

	//directional light
	g_directionalLightCOLLADAImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_directionalLightCOLLADAImg, "Assets/Engine/Icons/directional_light_collada.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the directional_light_collada texture");
		return CFalse;
	}
	g_directionalLightCOLLADAImg->SetFileName( "directional_light_collada.dds" );

	//spot light
	g_spotLightCOLLADAImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_spotLightCOLLADAImg, "Assets/Engine/Icons/spot_light_collada.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the spot_light_collada texture");
		return CFalse;
	}
	g_spotLightCOLLADAImg->SetFileName( "spot_light_collada.dds" );
	//////////////////
	g_skyImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_skyImg, "Assets/Engine/Icons/sky.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the sky texture");
		return CFalse;
	}
	g_skyImg->SetFileName( "sky.dds" );

	g_waterImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_waterImg, "Assets/Engine/Icons/water.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the water texture");
		return CFalse;
	}
	g_waterImg->SetFileName( "water.dds" );
	//camera
	g_cameraImg = CNew( CImage );
	if(!CTexture::LoadDDSTexture( g_cameraImg, "Assets/Engine/Icons/camera.dds", NULL ) )
	{
		PrintInfo( "\nGenerateDefaultTextures>Couldn't load the camera texture");
		return CFalse;
	}
	g_cameraImg->SetFileName( "camera.dds" );

	return CTrue;
}

CVoid CMultipleWindows::OnSize(UINT nType, CInt cx, CInt cy)
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_width = cx;
	m_height = cy;
}

CVoid CMultipleWindows::OnDestroy()
{
	wglMakeCurrent( NULL, NULL );
	if( m_hRC )
		wglDeleteContext( m_hRC );
	if( m_pDC )
		delete m_pDC;
	CWnd::OnDestroy();

}

CVoid CMultipleWindows::OnPaint()
{
	CPaintDC dc(this); // device context for paCInting
	// TODO: Add your message handler code here
	RenderWindow();
}

CVoid CMultipleWindows::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnTimer(nIDEvent);
	OnPaint();
}

CVoid CMultipleWindows::OnLButtonDown(UINT nFlags, CPoint point )
{
	if (g_editorMode == eMODE_GUI && g_multipleView->IsPlayGameMode())
	{
		m_selectedGUIIndex = GetSelectedGUI();
		for (CUInt k = 0; k < g_guiButtons.size(); k++)
		{
			if (g_guiButtons[k]->GetIndex() == m_selectedGUIIndex)
			{
				if (g_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_DISABLE)
				{
					if (g_guiButtons[k]->GetHasLeftClickImage())
					{
						g_multipleView->timer.GetElapsedSeconds2(CTrue);
						g_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_LEFT_CLICK);
					}
				}
			}
		}
	}
	if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode() && g_currentVSceneProperties.m_isMenu)
	{
		m_selectedGUIIndex = GetSelectedGUI();
		for (CUInt i = 0; i < g_guis.size(); i++)
		{
			for (CUInt k = 0; k < g_guis[i]->m_guiButtons.size(); k++)
			{
				if (g_guis[i]->m_guiButtons[k]->GetIndex() == m_selectedGUIIndex)
				{
					if (g_guis[i]->m_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_DISABLE)
					{
						if (g_guis[i]->m_guiButtons[k]->GetHasLeftClickImage())
						{
							g_multipleView->timer.GetElapsedSeconds2(CTrue);
							g_guis[i]->m_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_LEFT_CLICK);
						}
					}
				}
			}
		}
	}

	if (g_editorMode == eMODE_GUI && !g_multipleView->IsPlayGameMode())
	{
		m_selectedGUIIndex = GetSelectedGUI();
		m_lMouseDown = CTrue;
		m_mousePosition = point;  //It's used for selection
		m_mouseOldPosition = point;
		SetCapture();
		SetFocus();
		m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_MOVE));
		SetCursor(m_currentCursor);
		if (m_selectedGUIIndex != -1)
		{
			ex_pVandaEngine1Dlg->m_editX.EnableWindow(TRUE);
			ex_pVandaEngine1Dlg->m_editY.EnableWindow(TRUE);

			CChar name[MAX_NAME_SIZE];

			CBool foundTarget = CFalse;
			for (CUInt k = 0; k < g_guiBackgrounds.size(); k++)
			{
				if (g_guiBackgrounds[k]->GetIndex() == m_selectedGUIIndex)
				{
					COLORREF color = COLOR_WHITE;
					CHARFORMAT cf;
					cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
					cf.dwEffects = NULL;
					cf.crTextColor = color;

					ex_pStaticSelectedObject->SetWindowTextA("\n");
					CInt nSel = ex_pStaticSelectedObject->GetWindowTextLength();
					ex_pStaticSelectedObject->SetSel(nSel, nSel);

					ex_pStaticSelectedObject->SetSelectionCharFormat(cf);

					ex_pVandaEngine1Dlg->m_staticSelectedObject.ReplaceSel(g_guiBackgrounds[k]->GetName());

					CChar posX[MAX_NAME_SIZE];
					CChar posY[MAX_NAME_SIZE];
					CVec2f position = g_guiBackgrounds[k]->GetPosition();
					sprintf(posX, "%.0f", position.x);
					sprintf(posY, "%.0f", position.y);

					ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(posX);
					ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(posY);

					foundTarget = CTrue;
					Cpy(name, g_guiBackgrounds[k]->GetName());
				}
			}
			if (!foundTarget)
			{
				for (CUInt k = 0; k < g_guiButtons.size(); k++)
				{
					if (g_guiButtons[k]->GetIndex() == m_selectedGUIIndex)
					{
						COLORREF color = COLOR_WHITE;
						CHARFORMAT cf;
						cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
						cf.dwEffects = NULL;
						cf.crTextColor = color;

						ex_pStaticSelectedObject->SetWindowTextA("\n");
						CInt nSel = ex_pStaticSelectedObject->GetWindowTextLength();
						ex_pStaticSelectedObject->SetSel(nSel, nSel);

						ex_pStaticSelectedObject->SetSelectionCharFormat(cf);

						ex_pVandaEngine1Dlg->m_staticSelectedObject.ReplaceSel(g_guiButtons[k]->GetName());

						CChar posX[MAX_NAME_SIZE];
						CChar posY[MAX_NAME_SIZE];
						CVec2f position = g_guiButtons[k]->GetPosition();
						sprintf(posX, "%.0f", position.x);
						sprintf(posY, "%.0f", position.y);

						ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(posX);
						ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(posY);

						foundTarget = CTrue;
						Cpy(name, g_guiButtons[k]->GetName());
					}
				}
			}
			if (!foundTarget)
			{
				for (CUInt k = 0; k < g_guiTexts.size(); k++)
				{
					if (g_guiTexts[k]->GetIndex() == m_selectedGUIIndex)
					{
						COLORREF color = COLOR_WHITE;
						CHARFORMAT cf;
						cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
						cf.dwEffects = NULL;
						cf.crTextColor = color;

						ex_pStaticSelectedObject->SetWindowTextA("\n");
						CInt nSel = ex_pStaticSelectedObject->GetWindowTextLength();
						ex_pStaticSelectedObject->SetSel(nSel, nSel);

						ex_pStaticSelectedObject->SetSelectionCharFormat(cf);

						ex_pVandaEngine1Dlg->m_staticSelectedObject.ReplaceSel(g_guiTexts[k]->GetName());

						CChar posX[MAX_NAME_SIZE];
						CChar posY[MAX_NAME_SIZE];
						CVec2f position = g_guiTexts[k]->GetPosition();
						sprintf(posX, "%.0f", position.x);
						sprintf(posY, "%.0f", position.y);

						ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(posX);
						ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(posY);

						foundTarget = CTrue;
						Cpy(name, g_guiTexts[k]->GetName());
					}
				}
			}

			for (int k = 0; k < ex_pVandaEngine1Dlg->m_listBoxGUIElements.GetItemCount(); k++)
			{
				if (Cmp(name, ex_pVandaEngine1Dlg->m_listBoxGUIElements.GetItemText(k, 0)))
				{
					ex_pVandaEngine1Dlg->m_listBoxGUIElements.SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
					ex_pVandaEngine1Dlg->m_listBoxGUIElements.SetSelectionMark(k);
					ex_pVandaEngine1Dlg->m_listBoxGUIElements.Update(k);
				}
				else
				{
					ex_pVandaEngine1Dlg->m_listBoxGUIElements.SetItemState(k, ~LVIS_SELECTED, LVIS_SELECTED);
					ex_pVandaEngine1Dlg->m_listBoxGUIElements.Update(k);
				}
			}

		}
		else
		{
			COLORREF color = COLOR_WHITE;
			CHARFORMAT cf;
			cf.dwMask = CFM_COLOR/* | CFM_SIZE*/;
			cf.dwEffects = NULL;
			cf.crTextColor = color;

			ex_pStaticSelectedObject->SetWindowTextA("\n");
			CInt nSel = ex_pStaticSelectedObject->GetWindowTextLength();
			ex_pStaticSelectedObject->SetSel(nSel, nSel);

			ex_pStaticSelectedObject->SetSelectionCharFormat(cf);

			ex_pVandaEngine1Dlg->m_staticSelectedObject.ReplaceSel("\n");

			ex_pVandaEngine1Dlg->m_editX.SetWindowTextA("\n");
			ex_pVandaEngine1Dlg->m_editY.SetWindowTextA("\n");

			//ex_pVandaEngine1Dlg->m_editX.EnableWindow(FALSE);
			//ex_pVandaEngine1Dlg->m_editY.EnableWindow(FALSE);


		}
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}
	if (g_editorMode == eMODE_GUI)
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	ActivateCamera( point );
	m_isPlayingGame = CTrue;
	SetCapture();
	SetFocus();
	if( !m_enableTimer )
	{
		SetElapsedTimeFromBeginning();
		SetTimer( 1, 1, 0 );
	}
	if( !g_menu.m_justPerspective )
	{
		m_translateMousePosition.x = point.x * 2;
		m_translateMousePosition.y =   ( 2 * point.y )  -  g_height;
	}
	else
		m_translateMousePosition = point;

	if (g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX )
	{
		CUInt m_name;
		m_name = GetSelectedObject(CTrue);
		RenderWindow();
		//check arrows
		CScene* scene = NULL;
		if (g_currentTransformType == eCTranslate)
		{
			scene = g_translateArrow;
		}
		else if (g_currentTransformType == eCRotate)
		{
			scene = g_rotateArrow;
		}
		else if (g_currentTransformType == eCScale)
		{
			scene = g_scaleArrow;
		}
		for (CUInt j = 0; j < scene->m_geometries.size(); j++)
		{
			CGeometry* m_geometry = scene->m_geometries[j];
			if (g_menu.m_geometryBasedSelection)
			{
				CBool selected = CFalse;
				for (CUInt k = 0; k < scene->m_geometries[j]->m_instanceGeometries.size(); k++)
				{
					if (scene->m_geometries[j]->m_instanceGeometries[k]->m_nameIndex == m_name)
					{
						selected = CTrue;
						break;
					}
				}
				if (selected)
				{
					g_render.SetSelectedScene(scene);
					g_lastEngineObjectSelectedName = g_tempLastEngineObjectSelectedName;

					g_transformObject = CTrue; //we are in transformation mode ( not in selection mode )
					if (Cmp(scene->m_geometries[j]->GetName(), "geom-XAXIS"))
					{
						if (g_currentTransformType == eCTranslate || g_currentTransformType == eCScale)
						{
							g_transformDirection = XTRANS/*XYTRANS*/;
						}
						else
						{
							g_transformDirection = ZXTRANS;
						}
					}
					else if (Cmp(scene->m_geometries[j]->GetName(), "geom-YAXIS"))
					{
						if (g_currentTransformType == eCTranslate || g_currentTransformType == eCScale)
						{
							g_transformDirection = ZTRANS/*ZXTRANS*/;
						}
						else
						{
							g_transformDirection = XYTRANS;
						}
					}
					else if (Cmp(scene->m_geometries[j]->GetName(), "geom-ZAXIS"))
					{
						if (g_currentTransformType == eCTranslate || g_currentTransformType == eCScale)
						{
							g_transformDirection = YTRANS/*YZTRANS*/;
						}
						else
						{
							g_transformDirection = YZTRANS;
						}
					}
					else if (Cmp(scene->m_geometries[j]->GetName(), "geom-XYZ"))
					{
						g_transformDirection = FREETRANS;
					}

					SetDialogData(CTrue, NULL, scene->m_geometries[j], CTrue, CFalse);
				}
			}
			else //material based selection
			{
				for (CUInt k = 0; k < m_geometry->m_groups.size(); k++)
				{
					CPolyGroup* m_group = m_geometry->m_groups[k];
					if (m_group->m_nameIndex == m_name)
					{
						g_render.SetSelectedScene(scene);
						g_lastEngineObjectSelectedName = g_tempLastEngineObjectSelectedName;

						g_transformObject = CTrue; //we are in translation mode ( not in selection mode )
						if (Cmp(m_group->m_materialName, "XAXIS"))
						{
							g_transformDirection = XTRANS;
						}
						else if (Cmp(m_group->m_materialName, "YAXIS"))
						{
							g_transformDirection = ZTRANS;
						}
						else if (Cmp(m_group->m_materialName, "ZAXIS"))
						{
							g_transformDirection = YTRANS;
						}
						else if (Cmp(m_group->m_materialName, "XYZ"))
						{
							g_transformDirection = FREETRANS;
						}

						SetDialogData2(CTrue, scene->m_geometries[j]->m_groups[k], CTrue, CFalse);
					}
				}
			}
		}

		if (g_camera->m_activatePerspectiveCamera && g_transformObject && !m_translationController->Initialized())
		{
			m_translationController->Initialize((int)m_translateMousePosition.x, (int)m_translateMousePosition.y, g_transformDirection, g_arrowPosition);
		}
		m_selectObject = CTrue;
	}
	m_lMouseDown = CTrue;
	m_mousePosition = point;  //It's used for selection
	m_tempMovement = CFalse; //It's used for selection
	
	if( g_camera->m_activatePerspectiveCamera )
	{
		//if( g_camera->m_activatePerspectiveCamera && g_transformObject && !m_translationController->Initialized() )
		//	m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_UNAVAILABLE));
		//else
			m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_MOVE));
	}
	else
		m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CUSTOM_HAND));

	SetCursor( m_currentCursor );

	CWnd::OnLButtonDown(nFlags, point );
}

CVoid CMultipleWindows::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (g_editorMode == eMODE_GUI && !g_multipleView->IsPlayGameMode())
	{
		if (GetCapture() == this)
			ReleaseCapture();
		m_lMouseDown = CFalse;

		CWnd::OnLButtonUp(nFlags, point);
	}
	m_isPlayingGame = CFalse;
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() == this)
		ReleaseCapture();

	if (g_editorMode == eMODE_VSCENE)
	{
		if (g_transformObject && g_selectedName != -1)
		{
			for (CUInt i = 0; i < g_instancePrefab.size(); i++)
			{
				if (g_instancePrefab[i]->GetNameIndex() == g_selectedName)
				{
					g_instancePrefab[i]->CalculateDistance();
					g_multipleView->RenderQueries(CTrue);
				}
			}
			g_updateOctree = CTrue;
		}
	}

	if (g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX )
	{
		if (!g_transformObject && !m_tempMovement && g_camera->m_activatePerspectiveCamera)
		{
			g_lastEngineObjectSelectedName = -1;
			g_selectedName = m_tempSelectedName;
			if (g_selectedName == m_lastSelectedName)
			{
				m_lastSelectedName = g_selectedName = -1;
			}
			else
			{
				m_lastSelectedName = g_selectedName;
			}

			if (g_selectedName != -1)
			{
				CBool foundTarget = CFalse;
				if (g_editorMode == eMODE_PREFAB)
				{
					for (CUInt i = 0; i < g_scene.size(); i++)
					{
						for (CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++)
						{
							CGeometry* m_geometry = g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry;
							if (g_menu.m_geometryBasedSelection)
							{
								if (g_scene[i]->m_instanceGeometries[j]->m_nameIndex == g_selectedName)
								{
									foundTarget = CTrue;
									g_render.SetSelectedScene(g_scene[i]);
									SetDialogData(CTrue, g_scene[i]->m_instanceGeometries[j], m_geometry);
								}
							}
							else //material based selection
							{
								for (CUInt k = 0; k < m_geometry->m_groups.size(); k++)
								{
									CPolyGroup* m_group = m_geometry->m_groups[k];
									if (m_group->m_nameIndex == g_selectedName)
									{
										foundTarget = CTrue;
										g_render.SetSelectedScene(g_scene[i]);
										SetDialogData2(CTrue, g_scene[i]->m_instanceGeometries[j]->m_abstractGeometry->m_groups[k]);
									}
								}
							}
						}

					}
				}
				else
				{
					for (CUInt i = 0; i < g_instancePrefab.size(); i++)
					{
						if (g_instancePrefab[i]->GetNameIndex() == g_selectedName)
						{
							if (g_mainCharacter)
								if (Cmp(g_mainCharacter->GetInstancePrefab()->GetName(), g_instancePrefab[i]->GetName())) continue;

							g_render.SetSelectedInstancePrefab(g_instancePrefab[i]);
							SetDialogData3(CTrue, g_instancePrefab[i]);
						}
					}

				}

				if (!foundTarget) //something like light, sound, sky or water has been selected
					SetDialogData(CFalse, NULL, NULL, CTrue);
			}
			else
			{
				if (g_editorMode == eMODE_PREFAB)
				{
					g_render.SetSelectedScene(NULL);
					ex_pBtnPlayAnim->EnableWindow(FALSE);
					ex_pBtnPauseAnim->EnableWindow(FALSE);
					ex_pBtnPrevAnim->EnableWindow(FALSE);
					ex_pBtnNextAnim->EnableWindow(FALSE);

					if (g_menu.m_geometryBasedSelection)
					{
						SetDialogData(CFalse);
					}
					else
					{
						SetDialogData2(CFalse);
					}
				}
				else
				{
					g_render.SetSelectedInstancePrefab(NULL);
					SetDialogData3(CFalse);
				}
			}

			//Select items in List Controls as well
			if (g_editorMode == eMODE_VSCENE)
			{
				if (g_selectedName == -1)
				{
					//erase all items in list controls
					for (int i = 0; i < ex_pVandaEngine1Dlg->m_listBoxEngineObjects.GetItemCount(); i++)
					{
						ex_pVandaEngine1Dlg->m_listBoxEngineObjects.SetItemState(i, 0, LVIS_SELECTED);
						ex_pVandaEngine1Dlg->m_listBoxEngineObjects.Update(i);
					}

					for (int i = 0; i < ex_pVandaEngine1Dlg->m_listBoxScenes.GetItemCount(); i++)
					{
						ex_pVandaEngine1Dlg->m_listBoxScenes.SetItemState(i, 0, LVIS_SELECTED);
						ex_pVandaEngine1Dlg->m_listBoxScenes.Update(i);
					}

					for (int nItem = ex_pVandaEngine1Dlg->m_listBoxObjects.GetItemCount() - 1; nItem >= 0; nItem--)
					{
						ex_pVandaEngine1Dlg->m_listBoxObjects.DeleteItem(nItem);
						ex_pVandaEngine1Dlg->m_listBoxObjects.Update(nItem);
					}
					ex_pVandaEngine1Dlg->m_btnRemoveScene.EnableWindow(FALSE);
					ex_pVandaEngine1Dlg->m_btnRemoveEngineObject.EnableWindow(FALSE);
					ex_pVandaEngine1Dlg->m_btnEngineObjectProperties.EnableWindow(FALSE);

					g_showArrow = CFalse;
				}
				else
				{
					CBool foundTarget = CFalse;

					for (CUInt i = 0; i < g_instancePrefab.size(); i++)
					{
						if (g_instancePrefab[i]->GetNameIndex() == g_selectedName)
						{
							//Skip main character
							if (g_mainCharacter)
								if (Cmp(g_mainCharacter->GetInstancePrefab()->GetName(), g_instancePrefab[i]->GetName())) continue;

							//If it's trigger, skip
							CBool foundTrigger;
							for (CUInt j = 0; j < g_triggers.size(); j++)
							{
								if (Cmp(g_triggers[j]->GetInstancePrefab()->GetName(), g_instancePrefab[i]->GetName()))
								{
									foundTrigger = CTrue;
									break;
								}
							}
							if (foundTrigger)
								continue;

							foundTarget = CTrue;
							ex_pVandaEngine1Dlg->m_btnRemoveScene.EnableWindow(TRUE);

							for (int k = 0; k < ex_pVandaEngine1Dlg->m_listBoxScenes.GetItemCount(); k++)
							{
								if (Cmp(g_instancePrefab[i]->GetName(), ex_pVandaEngine1Dlg->m_listBoxScenes.GetItemText(k, 0)))
								{
									ex_pVandaEngine1Dlg->m_listBoxScenes.SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
									ex_pVandaEngine1Dlg->m_listBoxScenes.SetSelectionMark(k);
									ex_pVandaEngine1Dlg->m_listBoxScenes.Update(k);
								}
								else
								{
									ex_pVandaEngine1Dlg->m_listBoxScenes.SetItemState(k, ~LVIS_SELECTED, LVIS_SELECTED);
									ex_pVandaEngine1Dlg->m_listBoxScenes.Update(k);
								}
							}

							//Delete all items of m_listBoxObjects
							for (int nItem = ex_pVandaEngine1Dlg->m_listBoxObjects.GetItemCount() - 1; nItem >= 0; nItem--)
							{
								ex_pVandaEngine1Dlg->m_listBoxObjects.DeleteItem(nItem);
							}

							//fill m_listBoxObjects with new data
							for (CUInt j = 0; j < 3; j++)
							{
								if (g_instancePrefab[i]->GetPrefab()->GetHasLod(j))
									ex_pVandaEngine1Dlg->InsertItemToObjectList(g_instancePrefab[i]->GetScene(j)->GetName(), eOBJECTLIST_SCENE);
							}

							//Erase all items of m_listBoxEngineObjects
							for (int k = 0; k < ex_pVandaEngine1Dlg->m_listBoxEngineObjects.GetItemCount(); k++)
							{
								ex_pVandaEngine1Dlg->m_listBoxEngineObjects.SetItemState(k, ~LVIS_SELECTED, LVIS_SELECTED);
								ex_pVandaEngine1Dlg->m_listBoxEngineObjects.Update(k);
							}
							ex_pVandaEngine1Dlg->m_btnRemoveEngineObject.EnableWindow(FALSE);
							ex_pVandaEngine1Dlg->m_btnEngineObjectProperties.EnableWindow(FALSE);

							break;
						}
					}
					if (!foundTarget)
					{
						CChar name[MAX_NAME_SIZE];
						for (CUInt i = 0; i < g_engineLights.size(); i++)
						{
							if (g_engineLights[i]->GetIndex() == g_selectedName)
							{
								Cpy(name, g_engineLights[i]->m_abstractLight->GetName());
								foundTarget = CTrue;
								break;
							}
						}
						if (!foundTarget)
						{
							if (g_skyDome && g_skyDome->GetIndex() == g_selectedName)
							{
								Cpy(name, g_skyDome->GetName());
								foundTarget = CTrue;
							}
						}
						if (!foundTarget)
						{
							if (g_mainCharacter && g_mainCharacter->GetInstancePrefab()->GetNameIndex() == g_selectedName)
							{
								Cpy(name, g_mainCharacter->GetName());
								foundTarget = CTrue;
							}
						}

						if (!foundTarget)
						{
							for (CUInt i = 0; i < g_engineWaters.size(); i++)
							{
								if (g_engineWaters[i]->GetIndex() == g_selectedName)
								{
									Cpy(name, g_engineWaters[i]->GetName());

									foundTarget = CTrue;
									break;
								}
							}
						}
						if (!foundTarget)
						{
							for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
							{
								if (g_engineStaticSounds[i]->GetIndex() == g_selectedName)
								{
									Cpy(name, g_engineStaticSounds[i]->GetName());

									foundTarget = CTrue;
									break;
								}
							}
						}
						if (!foundTarget)
						{
							for (CUInt i = 0; i < g_triggers.size(); i++)
							{
								if (g_triggers[i]->GetInstancePrefab()->GetNameIndex() == g_selectedName)
								{
									Cpy(name, g_triggers[i]->GetName());

									foundTarget = CTrue;
									break;
								}
							}
						}
						if (!foundTarget)
						{
							for (CUInt i = 0; i < g_engineCameraInstances.size(); i++)
							{
								if (g_engineCameraInstances[i]->GetIndex() == g_selectedName)
								{
									Cpy(name, g_engineCameraInstances[i]->m_abstractCamera->GetName());

									foundTarget = CTrue;
									break;
								}
							}
						}
						if (foundTarget)
						{
							ex_pVandaEngine1Dlg->m_btnRemoveEngineObject.EnableWindow(TRUE);
							ex_pVandaEngine1Dlg->m_btnEngineObjectProperties.EnableWindow(TRUE);

							for (int k = 0; k < ex_pVandaEngine1Dlg->m_listBoxEngineObjects.GetItemCount(); k++)
							{
								if (Cmp(name, ex_pVandaEngine1Dlg->m_listBoxEngineObjects.GetItemText(k, 0)))
								{
									ex_pVandaEngine1Dlg->m_listBoxEngineObjects.SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
									ex_pVandaEngine1Dlg->m_listBoxEngineObjects.SetSelectionMark(k);
									ex_pVandaEngine1Dlg->m_listBoxEngineObjects.Update(k);
								}
								else
								{
									ex_pVandaEngine1Dlg->m_listBoxEngineObjects.SetItemState(k, ~LVIS_SELECTED, LVIS_SELECTED);
									ex_pVandaEngine1Dlg->m_listBoxEngineObjects.Update(k);
								}
							}


							//Erase all items of m_listBoxEngineObjects
							for (int k = 0; k < ex_pVandaEngine1Dlg->m_listBoxScenes.GetItemCount(); k++)
							{
								ex_pVandaEngine1Dlg->m_listBoxScenes.SetItemState(k, ~LVIS_SELECTED, LVIS_SELECTED);
								ex_pVandaEngine1Dlg->m_listBoxScenes.Update(k);
							}
							ex_pVandaEngine1Dlg->m_btnRemoveScene.EnableWindow(FALSE);
							ex_pVandaEngine1Dlg->m_btnSceneProperties.EnableWindow(FALSE);

						}
					}

				} //else

			}

		}
	}

	g_transformObject = CFalse;

	m_translationController->Finish();
	m_lMouseDown = CFalse;


	RenderWindow();
	if( !m_enableTimer )
		KillTimer(1);
	CWnd::OnLButtonUp(nFlags, point);
}

CVoid CMultipleWindows::EnableTimer( CBool enable )
{
	if( enable )
		SetTimer( 1, 1, 0 );
	else
		KillTimer( 1 );
}

CVoid CMultipleWindows::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (g_editorMode == eMODE_GUI && g_multipleView->IsPlayGameMode())
	{
		m_selectedGUIIndex = GetSelectedGUI();
		for (CUInt k = 0; k < g_guiButtons.size(); k++)
		{
			if (g_guiButtons[k]->GetIndex() == m_selectedGUIIndex)
			{
				if (g_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_DISABLE)
				{
					if (g_guiButtons[k]->GetHasRightClickImage())
					{
						g_multipleView->timer.GetElapsedSeconds2(CTrue);
						g_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_RIGHT_CLICK);
					}
				}
			}
		}
	}
	if (g_editorMode == eMODE_VSCENE &&  g_multipleView->IsPlayGameMode() && g_currentVSceneProperties.m_isMenu)
	{
		m_selectedGUIIndex = GetSelectedGUI();
		for (CUInt i = 0; i < g_guis.size(); i++)
		{
			for (CUInt k = 0; k < g_guis[i]->m_guiButtons.size(); k++)
			{
				if (g_guis[i]->m_guiButtons[k]->GetIndex() == m_selectedGUIIndex)
				{
					if (g_guis[i]->m_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_DISABLE)
					{
						if (g_guis[i]->m_guiButtons[k]->GetHasRightClickImage())
						{
							g_multipleView->timer.GetElapsedSeconds2(CTrue);
							g_guis[i]->m_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_RIGHT_CLICK);
						}
					}
				}
			}
		}
	}
	if (g_editorMode == eMODE_GUI)
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;
	}


	m_isPlayingGame	= CTrue;
	ActivateCamera( point );

	if( !m_enableTimer )
	{
		SetElapsedTimeFromBeginning();
		SetTimer( 1, 1, 0 );
	}

	SetCapture();
	SetFocus();

	m_rMouseDown = CTrue;
	m_selectObject = CTrue;

	if (!m_playGameMode)
	{
		m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ZOOMIN));

		SetCursor(m_currentCursor);
	}
	else if (!g_camera->m_activatePerspectiveCamera)
	{
		m_currentCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ZOOMIN));

		SetCursor(m_currentCursor);
	}
	CWnd::OnRButtonDown(nFlags, point);
}

CVoid CMultipleWindows::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_isPlayingGame = CFalse;
	// TODO: Add your message handler code here and/or call default
	if( GetCapture() == this )
		ReleaseCapture();
	m_rMouseDown = CFalse;

	if( !m_enableTimer )
		KillTimer(1);
	CWnd::OnRButtonUp(nFlags, point);
}

CVoid CMultipleWindows::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_mMouseDown = CTrue;
	if(!m_isPlayingGame)
		OnLButtonDown( nFlags, point );
	else
		OnLButtonUp( nFlags, point );
	//CWnd::OnMButtonDown(nFlags, point);
}

CVoid CMultipleWindows::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_mMouseDown = CFalse;

	// TODO: Add your message handler code here and/or call default
	//OnLButtonUp( nFlags, point );
	CWnd::OnMButtonUp(nFlags, point);
}

CVoid CMultipleWindows::OnMouseMove(UINT nFlags, CPoint point)
{
	if (g_editorMode == eMODE_GUI && g_multipleView->IsPlayGameMode())
	{
		m_mousePosition = point;

		m_selectedGUIIndex = GetSelectedGUI();
		for (CUInt k = 0; k < g_guiButtons.size(); k++)
		{
			if (g_guiButtons[k]->GetIndex() == m_selectedGUIIndex)
			{
				if (g_guiButtons[k]->GetCurrentImageType() == eBUTTON_IMAGE_MAIN &&  g_guiButtons[k]->GetHasHoverImage())
				{
					if (g_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_HOVER)
					{
						if (m_previousSelectedGUIIndex != m_selectedGUIIndex)
							g_guiButtons[k]->OnSelectMouseHoverScript();
					}
					g_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_HOVER);
				}
			}
			else
			{
				if (g_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_DISABLE && g_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_LEFT_CLICK && g_guiButtons[k]->GetCurrentImageType() != eBUTTON_IMAGE_RIGHT_CLICK)
					g_guiButtons[k]->SetCurrentImageType(eBUTTON_IMAGE_MAIN);
			}
		}
		m_mouseOldPosition = m_mousePosition;
		m_previousSelectedGUIIndex = m_selectedGUIIndex;
		CWnd::OnMouseMove(nFlags, point);
		return;

	}
	if (g_editorMode == eMODE_VSCENE &&  g_multipleView->IsPlayGameMode() && g_currentVSceneProperties.m_isMenu)
	{
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
							if (m_previousSelectedGUIIndex != m_selectedGUIIndex)
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
		m_previousSelectedGUIIndex = m_selectedGUIIndex;
	}
	else if (g_editorMode == eMODE_GUI && !g_multipleView->IsPlayGameMode())
	{
		if (m_selectedGUIIndex != -1 && m_lMouseDown)
		{ 
			m_mousePosition = point;

			CVec2f position;
			position.x = m_mousePosition.x - m_mouseOldPosition.x;
			position.y = m_mousePosition.y - m_mouseOldPosition.y;

			CBool foundTarget = CFalse;
			for (CUInt k = 0; k < g_guiBackgrounds.size(); k++)
			{
				if (g_guiBackgrounds[k]->GetIndex() == m_selectedGUIIndex)
				{
					g_guiBackgrounds[k]->SetPosition2(position);

					CChar posX[MAX_NAME_SIZE];
					CChar posY[MAX_NAME_SIZE];
					CVec2f position = g_guiBackgrounds[k]->GetPosition();
					sprintf(posX, "%.0f", position.x);
					sprintf(posY, "%.0f", position.y);

					ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(posX);
					ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(posY);

					foundTarget = CTrue;
				}
			}
			if (!foundTarget)
			{
				for (CUInt k = 0; k < g_guiButtons.size(); k++)
				{
					if (g_guiButtons[k]->GetIndex() == m_selectedGUIIndex)
					{
						g_guiButtons[k]->SetPosition2(position);

						CChar posX[MAX_NAME_SIZE];
						CChar posY[MAX_NAME_SIZE];
						CVec2f position = g_guiButtons[k]->GetPosition();
						sprintf(posX, "%.0f", position.x);
						sprintf(posY, "%.0f", position.y);

						ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(posX);
						ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(posY);

						foundTarget = CTrue;
					}
				}
			}
			if (!foundTarget)
			{
				for (CUInt k = 0; k < g_guiTexts.size(); k++)
				{
					if (g_guiTexts[k]->GetIndex() == m_selectedGUIIndex)
					{
						g_guiTexts[k]->SetPosition2(position);

						CChar posX[MAX_NAME_SIZE];
						CChar posY[MAX_NAME_SIZE];
						CVec2f position = g_guiTexts[k]->GetPosition();
						sprintf(posX, "%.0f", position.x);
						sprintf(posY, "%.0f", position.y);

						ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(posX);
						ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(posY);


						foundTarget = CTrue;
					}
				}
			}
			m_mouseOldPosition = m_mousePosition;
			CWnd::OnMouseMove(nFlags, point);
			return;

		}
	}
	m_mouseMove = CTrue;
	// TODO: Add your message handler code here and/or call default
	if (g_transformObject)
		m_tempMovement = CTrue;
	if (!g_menu.m_justPerspective)
	{
		m_translateMousePosition.x = point.x * 2;
		m_translateMousePosition.y = (2 * point.y) - g_height;
	}
	else
		m_translateMousePosition = point;

	if (g_editorMode == eMODE_VSCENE)
		g_multipleView->CalculateDistances(CTrue);

	m_mousePosition = point;


	CWnd::OnMouseMove(nFlags, point);

}

CVoid CMultipleWindows::ActivateCamera( CPoint point )
{
	if( g_menu.m_justPerspective || ( point.x < m_width / 2 && point.y > m_height / 2 ) ) //perspective window
	{
		g_camera->m_activatePerspectiveCamera = CTrue;
		g_camera->m_activateLowerRightCamera = CFalse;
		g_camera->m_activateUpperLeftCamera = CFalse;
		g_camera->m_activateUpperRightCamera = CFalse;
	}
	else if( ( point.x < m_width / 2 && point.y < m_height / 2 ) ) //upper left
	{
		g_camera->m_activatePerspectiveCamera = CFalse;
		g_camera->m_activateLowerRightCamera = CFalse;
		g_camera->m_activateUpperLeftCamera = CTrue;
		g_camera->m_activateUpperRightCamera = CFalse;
	}
	else if( ( point.x > m_width / 2 && point.y < m_height / 2 ) ) //upper right
	{
		g_camera->m_activatePerspectiveCamera = CFalse;
		g_camera->m_activateLowerRightCamera = CFalse;
		g_camera->m_activateUpperLeftCamera = CFalse;
		g_camera->m_activateUpperRightCamera = CTrue;
	}
	else if( ( point.x > m_width / 2 && point.y > m_height / 2 ) ) //lower right
	{
		g_camera->m_activatePerspectiveCamera = CFalse;
		g_camera->m_activateLowerRightCamera = CTrue;
		g_camera->m_activateUpperLeftCamera = CFalse;
		g_camera->m_activateUpperRightCamera = CFalse;
	}
}

CVoid CMultipleWindows::DrawGrid(CVoid)
{
	glUseProgram(0);
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_VIEWPORT_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	//Draw the grid here
	glLineWidth(1.0f);
	glColor3f(0.4f, 0.5f, 0.4f);
	glBegin(GL_LINES);
	for (CInt index = -50; index <= 50; index += 2)
	{
		glVertex3f(-50.0f, 0.0f, (CFloat)index);
		glVertex3f(50.0f, 0.0f, (CFloat)index);

		glVertex3f((CFloat)index, 0.0f, -50.0f);
		glVertex3f((CFloat)index, 0.0f, 50.0f);
	}
	glEnd();
	glLineWidth(3.0f);
	//Draw the axes here

	//glBegin( GL_LINES );

	//glColor3f( 1.0f, 0.0f, 0.0f );
	//glVertex3f( 0.0f, 0.0f, 0.0f );
	//glVertex3f( 1.0f, 0.0f, 0.0f );
	//
	//glColor3f( 0.0f, 1.0f, 0.0f );
	//glVertex3f( 0.0f, 0.0f, 0.0f );
	//glVertex3f( 0.0f, 1.0f, 0.0f );

	//glColor3f( 0.0f, 0.0f, 1.0f );
	//glVertex3f( 0.0f, 0.0f, 0.0f );
	//glVertex3f( 0.0f, 0.0f, 1.0f );

	//glEnd();

	glPopAttrib();
}

CVoid CMultipleWindows::DrawGUIMode()
{
	if (!g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
		g_render.BindForWriting(m_mFboID);
	else if (!g_useOldRenderingStyle && g_options.m_enableFBO)
		g_render.BindForWriting(m_fboID);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (g_useOldRenderingStyle || !g_options.m_enableFBO)
	{
		if (m_multiSample) glDisable(GL_MULTISAMPLE);
	}
	else
	{
		if (m_multiSample && g_options.m_numSamples) glEnable(GL_MULTISAMPLE);
		else glDisable(GL_MULTISAMPLE);
	}

	glPushAttrib(GL_VIEWPORT_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	////Draw grids of the GUI window 
	glViewport(0, 0, m_width ,m_height );// resets the viewport to new dimensions.

	g_camera->m_cameraManager->Set2DOrtho(-100.0f, 100.0f, -100.0f, 100.0f);
	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt(0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.0f);
	g_render.m_useShader = CFalse;

	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	for (CInt index = -100; index <= 100; index++)
	{
		CInt layer = -1;
		if (index == 0)
		{
			glColor3f(0.4f, 0.4f, 0.4f);
			layer = -49000;
		}
		else if (index % 20 == 0)
		{
			glColor3f(0.4f, 0.4f, 0.3f);
			layer = -49000;
		}
		else if (index % 5 == 0)
		{
			glColor3f(0.2f, 0.2f, 0.2f);
			layer = -49000;
		}

		if (index % 5 == 0)
		{
			glVertex3f(0.0f + (CFloat)index, -100.0f, layer);
			glVertex3f(0.0f + (CFloat)index, 100.0f, layer);

			glVertex3f(-100.0f, (CFloat)index, layer);
			glVertex3f(100.0f, (CFloat)index, layer);
		}
	}
	glEnd();

	glLineWidth(2.0f);
	g_camera->m_cameraManager->Set2DOrtho(0.0f, 1024.0f, 0.0, 768.0f);

	glColor4f(0.1f, 0.1f, 0.1f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(1, 1, 49999.0);
	glVertex3f(1024 - 1, 1, 49999.0);
	glVertex3f(1024 - 1, 768 - 1, 49999.0);
	glVertex3f(1, 768 - 1, 49999.0);
	glEnd();

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	for (CUInt k = 0; k < g_guiBackgrounds.size(); k++)
	{
		g_guiBackgrounds[k]->Render();
	}
	for (CUInt k = 0; k < g_guiButtons.size(); k++)
	{
		g_guiButtons[k]->Render();
	}
	for (CUInt k = 0; k < g_guiTexts.size(); k++)
	{
		g_guiTexts[k]->Render();
	}

	glPopAttrib();


	if (!g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForReading(m_mFboID);
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting(m_fboID);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

		g_render.SetReadBuffer(eGBUFFER_TEXTURE_TYPE_DEFAULT);

		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (!g_useOldRenderingStyle && g_options.m_enableFBO)
	{
		glViewport(0, 0, m_width, m_height);

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glUseProgram(0);
		glDrawBuffer(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT);
		glPushAttrib(GL_ENABLE_BIT);
		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		gluOrtho2D(0, g_width, 0, g_height);
		glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity();
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);

		glEnable(GL_TEXTURE_2D);
		glClearColor(0.37f, 0.37f, 0.37f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);	glVertex2d(0, 0);
		glTexCoord2d(1, 0);	glVertex2d(g_width, 0);
		glTexCoord2d(1, 1);	glVertex2d(g_width, g_height);
		glTexCoord2d(0, 1);	glVertex2d(0, g_height);
		glEnd();
		glUseProgram(0);

		glPopAttrib();
		glMatrixMode(GL_PROJECTION); glPopMatrix();
		glMatrixMode(GL_MODELVIEW); glPopMatrix();
	}

	if (GetSaveScreenshot())
	{
		CChar screenshotTGAPath[MAX_URI_SIZE];
		sprintf(screenshotTGAPath, "%s%s", GetScreenshotPath(), ".tga");

		g_glUtil.SaveScreenShot(screenshotTGAPath, g_width, g_height);
		ILboolean loaded = ilLoadImage(screenshotTGAPath);
		if (loaded != IL_FALSE)
		{
			iluImageParameter(ILU_FILTER, ILU_SCALE_LANCZOS3);

			iluScale(182, 101, ilGetInteger(IL_IMAGE_DEPTH));

			ilEnable(IL_FILE_OVERWRITE);

			CChar screenshotBMPPath[MAX_URI_SIZE];
			sprintf(screenshotBMPPath, "%s%s", GetScreenshotPath(), ".bmp");

			ilSaveImage(screenshotBMPPath);

			DeleteFile(screenshotTGAPath);

		}
		SetSaveScreenshot(CFalse);
	}

}

CVoid CMultipleWindows::DrawUpperLeft()
{
	if (m_initError)
		return;

	glPushAttrib( GL_VIEWPORT_BIT );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	////Draw grids of the upper left window 
	glViewport( 0, m_height / 2 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	g_camera->m_cameraManager->Set2DOrtho( -m_width * g_camera->m_upperLeftZoom , m_width * g_camera->m_upperLeftZoom, -m_height * g_camera->m_upperLeftZoom, m_height * g_camera->m_upperLeftZoom ); 
	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt( g_camera->m_upperLeftMoveHorizantal , g_camera->m_upperLeftMoveVertical, 1.0f,
		g_camera->m_upperLeftMoveHorizantal, g_camera->m_upperLeftMoveVertical,
		0.0f, 0.0f, 1.0f, 0.0f );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glLineWidth( 1.0f );
	g_render.m_useShader = CFalse;

	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glBegin( GL_LINES );
	for( CInt index = -100; index <= 100; index++ )
	{
		CInt layer = -1;
		if( index == 0 )
		{
			glColor3f( 0.0f, 0.0f, 0.0f );
			layer = -49000;
		}
		else if( index % 5 ==  0 ) 
		{
			glColor3f( 0.2f, 0.2f, 0.2f );
			layer = -49000;
		}
		else
		{
			glColor3f( 0.45f, 0.45f, 0.45f );
			layer = -49001;
		}

		if( g_camera->m_upperLeftZoom > 0.05 )
		{
			if( index % 5 == 0 )
			{
				glVertex3f( 0.0f + (CFloat)index, -100.0f, layer );
				glVertex3f( 0.0f + (CFloat)index, 100.0f, layer );

				glVertex3f( -100.0f ,(CFloat)index, layer );
				glVertex3f( 100.0f,(CFloat)index, layer );
			}
		}
		else
		{
			glVertex3f( 0.0f + (CFloat)index, -100.0f, layer );
			glVertex3f( 0.0f + (CFloat)index, 100.0f, layer );

			glVertex3f( -100.0f ,(CFloat)index, layer );
			glVertex3f( 100.0f,(CFloat)index, layer );
		}
	}
	glEnd();
	glPopAttrib();

	if (g_editorMode == eMODE_VSCENE)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (!g_instancePrefab[i]->GetVisible()) continue;
			g_currentInstancePrefab = g_instancePrefab[i];
			if (Cmp(g_currentInstancePrefab->GetName(), "VANDA_MAIN_CHARACTER"))
			{
				if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON && g_multipleView->IsPlayGameMode())
					continue;
			}

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
			if (!scene)
			{
				if (prefab && prefab->GetHasLod(2))
					scene = g_instancePrefab[i]->GetScene(2);
				else if (prefab && prefab->GetHasLod(1))
					scene = g_instancePrefab[i]->GetScene(1);
				else if (prefab && prefab->GetHasLod(0))
					scene = g_instancePrefab[i]->GetScene(0);
			}
			if (!scene)
				continue;
			g_render.SetScene(scene);

			if (!g_render.GetScene()->m_isTrigger)
			{
				g_render.GetScene()->Render(CFalse, NULL, CTrue);

				g_render.ModelViewMatrix();
				g_render.PushMatrix();
				g_render.MultMatrix(*(g_instancePrefab[i]->GetInstanceMatrix()));
				g_render.GetScene()->RenderAnimatedModels(CFalse, CTrue);
				g_render.PopMatrix();

				g_render.GetScene()->RenderAnimatedModels(CFalse, CFalse);

				g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
			}
		}
	}
	else
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			if (!g_scene[i]->m_isVisible) continue;
			g_render.SetScene(g_scene[i]);
			//g_render.GetScene()->Update();

			if (!g_scene[i]->m_isTrigger)
			{
				g_render.GetScene()->Render(CFalse, NULL, CTrue);
				g_render.GetScene()->RenderAnimatedModels(CFalse, CTrue);
				g_render.GetScene()->RenderAnimatedModels(CFalse, CFalse);
				g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
			}
		}
	}
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	m_simpleFont.StartRendering();
	m_simpleFont.Print("Front", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f);
	m_simpleFont.EndRendering();
	glPopAttrib();
}

CVoid CMultipleWindows::DrawUpperRight()
{
	if (m_initError)
		return;

	glPushAttrib( GL_VIEWPORT_BIT );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	//Draw grids of the upper right window 
	glViewport( m_width / 2, m_height / 2 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	g_camera->m_cameraManager->Set2DOrtho( -m_width * g_camera->m_upperRightZoom, m_width * g_camera->m_upperRightZoom, -m_height * g_camera->m_upperRightZoom, m_height * g_camera->m_upperRightZoom ); 

	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt( 1.0, g_camera->m_upperRightMoveVertical, g_camera->m_upperRightMoveHorizantal,
		0.0f, g_camera->m_upperRightMoveVertical, g_camera->m_upperRightMoveHorizantal,
		0.0f, 1.0f, 0.0f );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	g_render.m_useShader = CFalse;

	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glLineWidth( 1.0f );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glBegin( GL_LINES );
	for( CInt index = -100; index <= 100; index++ )
	{
		CInt layer = -1;
		if( index == 0 )
		{
			glColor3f( 0.0f, 0.0f, 0.0f );
			layer = -49000;
		}
		else if( index % 5 ==  0 ) 
		{
			glColor3f( 0.2f, 0.2f, 0.2f );
			layer = -49000;
		}
		else
		{
			glColor3f(0.45f, 0.45f, 0.45f);
			layer = -49001;
		}

		if( g_camera->m_upperRightZoom > 0.05f )
		{
			if( index % 5 == 0 )
			{
				glVertex3f( layer, -100.f, (CFloat)index );
				glVertex3f( layer, 100.f, (CFloat)index );

				glVertex3f( layer,(CFloat)index, -100.f );
				glVertex3f( layer,(CFloat)index, 100.f );
			}
		}
		else
		{
			glVertex3f( layer, -100.f, (CFloat)index );
			glVertex3f( layer, 100.f, (CFloat)index);

			glVertex3f(  layer,(CFloat)index, -100.f );
			glVertex3f( layer,(CFloat)index, 100.f );
		}
	}
	glEnd();
	glPopAttrib();

	if (g_editorMode == eMODE_VSCENE)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (!g_instancePrefab[i]->GetVisible()) continue;
			g_currentInstancePrefab = g_instancePrefab[i];
			if (Cmp(g_currentInstancePrefab->GetName(), "VANDA_MAIN_CHARACTER"))
			{
				if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON && g_multipleView->IsPlayGameMode())
					continue;
			}

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
			if (!scene)
			{
				if (prefab && prefab->GetHasLod(2))
					scene = g_instancePrefab[i]->GetScene(2);
				else if (prefab && prefab->GetHasLod(1))
					scene = g_instancePrefab[i]->GetScene(1);
				else if (prefab && prefab->GetHasLod(0))
					scene = g_instancePrefab[i]->GetScene(0);
			}
			if (!scene)
				continue;
			g_render.SetScene(scene);

			if (!g_render.GetScene()->m_isTrigger)
			{
				g_render.GetScene()->Render(CFalse, NULL, CTrue);

				g_render.ModelViewMatrix();
				g_render.PushMatrix();
				g_render.MultMatrix(*(g_instancePrefab[i]->GetInstanceMatrix()));
				g_render.GetScene()->RenderAnimatedModels(CFalse, CTrue);
				g_render.PopMatrix();

				g_render.GetScene()->RenderAnimatedModels(CFalse, CFalse);

				g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
			}
		}
	}
	else
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			if (!g_scene[i]->m_isVisible) continue;
			g_render.SetScene(g_scene[i]);
			//g_render.GetScene()->Update();
			if (!g_scene[i]->m_isTrigger)
			{
				g_render.GetScene()->Render(CFalse, NULL, CTrue);
				g_render.GetScene()->RenderAnimatedModels(CFalse, CTrue);
				g_render.GetScene()->RenderAnimatedModels(CFalse, CFalse);
				g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
			}
		}
	}
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	m_simpleFont.StartRendering();
	m_simpleFont.Print( "Side", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f );
	m_simpleFont.EndRendering();
	glPopAttrib();
}

CVoid CMultipleWindows::DrawLowerRight()
{
	if (m_initError)
		return;

	glPushAttrib( GL_VIEWPORT_BIT );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	//Draw grids of the lower right window 
	glViewport( m_width / 2, 0 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	g_camera->m_cameraManager->Set2DOrtho( -m_width * g_camera->m_lowerRightZoom, m_width * g_camera->m_lowerRightZoom, -m_height * g_camera->m_lowerRightZoom, m_height * g_camera->m_lowerRightZoom ); 

	g_render.ModelViewMatrix();
	g_render.IdentityMatrix();
	gluLookAt( g_camera->m_lowerRightMoveHorizantal, 1.0, g_camera->m_lowerRightMoveVertical,
		g_camera->m_lowerRightMoveHorizantal, 0.0f, g_camera->m_lowerRightMoveVertical,
		0.0f, 0.0f , 1.0f );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	g_render.m_useShader = CFalse;

	glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glLineWidth( 1.0f );
	glBegin( GL_LINES );
	for( CInt index = -100; index <= 100; index++ )
	{
		CInt layer = -1;
		if( index == 0 )
		{
			glColor3f( 0.0f, 0.0f, 0.0f );
			layer = -49000;
		}
		else if( index % 5 ==  0 ) 
		{
			glColor3f( 0.2f, 0.2f, 0.2f );
			layer = -49000;
		}
		else
		{
			glColor3f(0.45f, 0.45f, 0.45f);
			layer = -49001;
		}

		if( g_camera->m_lowerRightZoom > 0.05 )
		{
			if( index % 5 == 0 )
			{
				glVertex3f( -100.0f, layer, (CFloat)index );
				glVertex3f(  100.0f, layer, (CFloat)index );

				glVertex3f( (CFloat)index, layer, -100.0f );
				glVertex3f( (CFloat)index, layer, 100.0f );
			}
		}
		else
		{
			glVertex3f( -100.0f, layer, (CFloat)index );
			glVertex3f(  100.0f, layer, (CFloat)index );

			glVertex3f( (CFloat)index, layer, -100.0f );
			glVertex3f( (CFloat)index, layer, 100.0f );
		}
	}
	glEnd();
	glPopAttrib();

	if (g_editorMode == eMODE_VSCENE)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (!g_instancePrefab[i]->GetVisible()) continue;
			g_currentInstancePrefab = g_instancePrefab[i];
			if (Cmp(g_currentInstancePrefab->GetName(), "VANDA_MAIN_CHARACTER"))
			{
				if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON && g_multipleView->IsPlayGameMode())
					continue;
			}

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
			if (!scene)
			{
				if (prefab && prefab->GetHasLod(2))
					scene = g_instancePrefab[i]->GetScene(2);
				else if (prefab && prefab->GetHasLod(1))
					scene = g_instancePrefab[i]->GetScene(1);
				else if (prefab && prefab->GetHasLod(0))
					scene = g_instancePrefab[i]->GetScene(0);
			}
			if (!scene)
				continue;
			g_render.SetScene(scene);

			if (!g_render.GetScene()->m_isTrigger)
			{
				g_render.GetScene()->Render(CFalse, NULL, CTrue);

				g_render.ModelViewMatrix();
				g_render.PushMatrix();
				g_render.MultMatrix(*(g_instancePrefab[i]->GetInstanceMatrix()));
				g_render.GetScene()->RenderAnimatedModels(CFalse, CTrue);
				g_render.PopMatrix();

				g_render.GetScene()->RenderAnimatedModels(CFalse, CFalse);

				g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
			}
		}
	}
	else
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			if (!g_scene[i]->m_isVisible) continue;
			g_render.SetScene(g_scene[i]);
			//g_render.GetScene()->Update();

			if (!g_scene[i]->m_isTrigger)
			{
				g_render.GetScene()->Render(CFalse, NULL, CTrue);
				g_render.GetScene()->RenderAnimatedModels(CFalse, CTrue);
				g_render.GetScene()->RenderAnimatedModels(CFalse, CFalse);
				g_render.GetScene()->RenderModelsControlledByPhysX(CFalse);
			}

		}
	}
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	m_simpleFont.StartRendering();
	m_simpleFont.Print( "Top", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f );
	m_simpleFont.EndRendering();
	glPopAttrib();
}

CVoid CMultipleWindows::SetElapsedTimeFromBeginning()
{
	elapsedTime = timer.GetElapsedSeconds( CTrue ); //start from the beginning
	timer.GetFps( 1, CTrue );
	g_elapsedTime = elapsedTime;
	m_timerCounter  = 0;
	m_totalElapsedTime = 0;
}

CVoid CMultipleWindows::CalculateDistances(CBool force)
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
	//currently, engine lights and waters are static
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

		if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				CScene* scene = g_scene[i];
				if (CmpIn(scene->GetName(), "_COL"))
					continue;
				scene->CalculateDistances();
			}
		}
	}
}

CVoid CMultipleWindows::DrawPerspective()
{
	if (m_initError || !g_render.m_shaderAvailable)
		return;
	g_numVerts = 0; //debug info

	//Update timer
	if (!g_currentVSceneProperties.m_isPause)
	{
		if (!m_selectObject)
			elapsedTime = timer.GetElapsedSeconds();
		else
			elapsedTime = timer.GetElapsedSeconds(CTrue); //start from the beginning
		g_elapsedTime = elapsedTime;
		m_totalElapsedTime += elapsedTime;
	
		//use multithreading for animations
		/*std::thread t1(&CMain::*/UpdateAnimations();/*, this, false)*/;
		//t1.join();
		UpdateDynamicPhysicsObjects();
	}
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
		g_render.BindForWriting(m_mFboID);
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
		g_render.BindForWriting( m_fboID );
	//if( !g_useOldRenderingStyle )
	//{
	//	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
	//	GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT};
	//	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);
	//}
	//else
		glDrawBuffer( GL_BACK );

	m_cameraTypeOfPreviousFrame = m_cameraType;

	if( g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX )
	{
		CInstanceCamera *instanceCamera = g_render.GetDefaultInstanceCamera();
		if (instanceCamera)
		{
			m_cameraType = eCAMERA_DEFAULT_FREE_NO_PHYSX;
			SetInstanceCamera(instanceCamera, m_width / 2.f, m_height / 2.f, instanceCamera->m_abstractCamera->GetAngle(), g_cameraProperties.m_freePerspectiveNCP, g_cameraProperties.m_freePerspectiveFCP);
			m_lockInput = CFalse;
			if (g_selectedName != -1)
				g_showArrow = CTrue;
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


				SetInstanceCamera(instanceCamera, m_width / 2.f, m_height / 2.f, m_fov, m_zNear, m_zFar);
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
						SetInstanceCamera(g_engineCameraInstances[c], m_width / 2.f, m_height / 2.f, g_engineCameraInstances[c]->m_abstractCamera->GetAngle(), g_engineCameraInstances[c]->GetNCP(), g_engineCameraInstances[c]->GetFCP());
						g_render.SetActiveInstanceCamera(g_engineCameraInstances[c]);
						foundTarget = CTrue;
						g_showArrow = CFalse;
						break;
					}
				}
				else
				{
					SetInstanceCamera(g_engineCameraInstances[c], m_width / 2.f, m_height / 2.f, g_engineCameraInstances[c]->m_abstractCamera->GetAngle(), g_engineCameraInstances[c]->GetNCP(), g_engineCameraInstances[c]->GetFCP());
					g_render.SetActiveInstanceCamera(g_engineCameraInstances[c]);
					foundTarget = CTrue;
					g_showArrow = CFalse;
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

	if( g_currentCameraType == eCAMERA_PHYSX )
	{
		m_cameraType = eCAMERA_PHYSX;

		m_lockInput = CFalse;
		g_camera->m_cameraManager->SetPerspective(g_camera->m_cameraManager->GetAngle(), m_width / 2., m_height / 2., g_cameraProperties.m_playModePerspectiveNCP, g_cameraProperties.m_playModePerspectiveFCP);

		if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
		{
			//shadow
			glGetFloatv(GL_PROJECTION_MATRIX, cam_proj);
		}

		//previous position of character controller
		g_camera->m_perspectiveCharacterPosOfPreviousFrame = g_camera->m_perspectiveCharacterPos;
		m_nx->GetCameraAndCharacterPositions( g_camera->m_perspectiveCameraPitch,
			g_camera->m_perspectiveCameraYaw,
			g_camera->m_perspectiveCameraPos,
			g_camera->m_perspectiveCharacterPos,
			g_camera->m_perspectiveCameraDir );

		//Update camera position and orientation
		g_camera->m_cameraManager->UpdateCameraPosDir( g_camera->m_perspectiveCameraPos,
			g_camera->m_perspectiveCharacterPos,
			g_camera->m_perspectiveCameraDir,
			g_camera->m_perspectiveCurrentCameraTilt );

		//if( g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader )
		//{
			// store the inverse of the resulting modelview matrix for the shadow computation
			glGetFloatv(GL_MODELVIEW_MATRIX, cam_modelview);
			g_camera->m_cameraManager->GetInverseMatrix( cam_inverse_modelview );
		//}
			g_showArrow = CFalse;
	}

	g_camera->m_cameraManager->UpdateFrustum();
	if (g_editorMode == eMODE_VSCENE)
	{
		g_octree->ResetOctreeGeoCount();
		g_octree->Render(CTrue);
		g_octree->ResetOctreeGeoCount();
	}
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
	if (g_editorMode == eMODE_VSCENE/* && g_multipleView->IsPlayGameMode()*/)
		UpdatePrefabInstanceBB();

	CalculateDistances();

	const float bias[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f };
	if (!g_useOldRenderingStyle && g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_render.m_useDynamicShadowMap && g_options.m_enableShader)
	{
		//shadow
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
	if(	g_menu.m_insertAmbientSound )
		m_ambientSound->GetSoundSource()->SetSoundPosition( m_ambientSourcePos );

	if( g_polygonMode == ePOLYGON_LINE )
	{
		g_render.m_useShader = CFalse;
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glDisable(GL_LIGHTING );
		glDisable(GL_CULL_FACE );
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_MULTISAMPLE );
		glLineWidth(0.5f);
	}
	else if( g_polygonMode == ePOLYGON_POINT )
	{
		g_render.m_useShader = CFalse;
		glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
		glDisable(GL_LIGHTING );
		glDisable(GL_CULL_FACE );
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_MULTISAMPLE );
		glPointSize(1.0);
	}
	else if( g_polygonMode == ePOLYGON_FILL )
	{
		g_render.m_useShader = CTrue;
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	glPushAttrib(GL_LIGHTING_BIT);

	//create all the water textures here
	if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
	{
		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
		{
			if (g_engineWaters[i]->GetOutsideFrustom()) continue;

			if (g_multipleView->IsPlayGameMode())
			{
				if (g_engineWaters[i]->GetVisible() && g_engineWaters[i]->GetQueryVisible())
				{
					g_engineWaters[i]->CreateReflectionTexture(g_waterTextureSize);
					//g_engineWaters[i]->CreateRefractionDepthTexture(g_waterTextureSize );
				}
			}
			else if (g_engineWaters[i]->GetQueryVisible())
			{
				g_engineWaters[i]->CreateReflectionTexture(g_waterTextureSize);
				//g_engineWaters[i]->CreateRefractionDepthTexture(g_waterTextureSize );
			}
		}
	}
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
		g_render.BindForWriting(m_mFboID);
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
		g_render.BindForWriting( m_fboID );

	//glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	if( g_useOldRenderingStyle || !g_options.m_enableFBO)
		glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );
	else
		glClearColor( 0.0f, 0.0f, 0.f, 1.0f );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );



	//Draw grids of the lower left window (perspective)
	if( g_menu.m_justPerspective )
	{
		glViewport( 0, 0 , m_width , m_height );// resets the viewport to new dimensions.
		if( m_multiSample && g_options.m_numSamples ) glEnable( GL_MULTISAMPLE );
		else glDisable( GL_MULTISAMPLE );
	}
	else
	{
		if( g_useOldRenderingStyle || !g_options.m_enableFBO)
		{
			if( m_multiSample ) glDisable( GL_MULTISAMPLE );
			glViewport( 0, 0 , m_width /2 , m_height /2 );// resets the viewport to new dimensions.
		}
		else
		{
			glViewport( 0, 0 , m_width, m_height );// resets the viewport to new dimensions.
			if( m_multiSample && g_options.m_numSamples ) glEnable( GL_MULTISAMPLE );
			else glDisable( GL_MULTISAMPLE );
		}
	}
	if( g_camera->m_activatePerspectiveCamera && g_transformObject && m_translationController->Initialized() )
	{
		m_translationController->Update((int)m_translateMousePosition.x,(int)m_translateMousePosition.y );		
	}

	if (g_multipleView->IsPlayGameMode() && g_currentVSceneProperties.m_isMenu)
		m_isMenu = CTrue;
	else
		m_isMenu = CFalse;


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
		if (g_menu.m_insertAmbientSound)
		{
			g_soundSystem->PauseALSound(*(g_multipleView->m_ambientSound->GetSoundSource()));
		}

	}

	//Process  inputs
 	if( !m_translationController->Initialized()  )
		ProcessInputs();

	if( m_selectObject  )
	{
		if (g_camera->m_activatePerspectiveCamera)
		{
			CUInt selected = -1;
			selected = GetSelectedObject();

			//CChar temp[ MAX_NAME_SIZE];
			//sprintf( temp, "%d\n", selected );
			//PrintInfo2( temp );
		}

		m_selectObject = CFalse;
	}
	//render waters here
	CVec3f cameraPos( g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z );

	if ( g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader )
	{
		for (CUInt i = 0; i < g_engineWaters.size(); i++)
		{
			if (g_engineWaters[i]->GetOutsideFrustom()) continue;

			if (g_multipleView->IsPlayGameMode())
			{
				if (g_engineWaters[i]->GetVisible() && g_engineWaters[i]->GetQueryVisible())
				{
					glUseProgram(g_render.m_waterProgram);
					g_engineWaters[i]->RenderWater(cameraPos, elapsedTime);
					glUseProgram(0);
				}
			}
			else if (g_engineWaters[i]->GetQueryVisible())
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
	if (g_camera->m_activatePerspectiveCamera && g_transformObject && m_translationController->Initialized())
	{
		if (g_currentTransformType == eCTranslate)
		{
			g_arrowPosition += m_translationController->GetDisplacement();
		}
		else if (g_currentTransformType == eCRotate)
		{
			g_arrowRotate += m_translationController->GetDisplacement() * 2.0f;
		}
		else if (g_currentTransformType == eCScale)
		{
			g_arrowScale += m_translationController->GetDisplacement();
			g_arrowScale.x *= 0.3f;
			g_arrowScale.y *= 0.3f;
			g_arrowScale.z *= 0.3f;
		}
		m_translationController->DrawPlane();
	}

	//render the scene
	g_numLights = 0;
	if (g_polygonMode == ePOLYGON_FILL)
		glEnable(GL_LIGHTING);   //just for per vertex lighting

	if (g_multipleView->IsPlayGameMode())
	{
		if (g_editorMode == eMODE_VSCENE)
		{
			for (CUInt i = 0; i < g_instancePrefab.size(); i++)
			{
				if (!g_instancePrefab[i]->GetVisible()) continue;
				g_instancePrefab[i]->UpdateScript();
			}
			if (g_startup)
				g_startup->UpdateScript();
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			if (g_prefabProperties.m_hasScript)
			{
				lua_getglobal(g_lua, "Update");
				if (lua_isfunction(g_lua, -1))
				{
					lua_pcall(g_lua, 0, 0, 0);
				}
				lua_settop(g_lua, 0);
			}
		}
	}

	RenderQueries();

	ManageLODs();

	Draw3DObjects();

	if (g_menu.m_insertAndShowTerrain)
		RenderTerrain();
	
	g_totalLights = g_engineLights.size();

	if (g_menu.m_showBoundingBox && g_editorMode == eMODE_PREFAB)
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			if (!g_scene[i]->m_isVisible) continue;
			g_render.SetScene(g_scene[i]);
			g_render.GetScene()->RenderAABBWithLines(g_render.GetScene()->m_hasAnimation);
		}
	}
	else if (g_menu.m_showBoundingBox)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			CVector lineColor(1.0, 1.0, 1.0);
			CVector min(g_instancePrefab[i]->GetMinAABB().x, g_instancePrefab[i]->GetMinAABB().y, g_instancePrefab[i]->GetMinAABB().z);
			CVector max(g_instancePrefab[i]->GetMaxAABB().x, g_instancePrefab[i]->GetMaxAABB().y, g_instancePrefab[i]->GetMaxAABB().z);

			//CVector min2(g_instancePrefab[i]->GetInverseMinAABB().x, g_instancePrefab[i]->GetInverseMinAABB().y, g_instancePrefab[i]->GetInverseMinAABB().z);
			//CVector max2(g_instancePrefab[i]->GetInverseMaxAABB().x, g_instancePrefab[i]->GetInverseMaxAABB().y, g_instancePrefab[i]->GetInverseMaxAABB().z);

			g_glUtil.DrawCWBoxWithLines(min, max, lineColor);
			//g_glUtil.DrawCWBoxWithLines(min2, max2, lineColor);
		}
	}

	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
		g_render.BindForWriting( m_mFboID );
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
		g_render.BindForWriting( m_fboID );

	g_currentInstanceLight = NULL;

	CBool useFog;
	if (g_polygonMode != ePOLYGON_FILL || (g_dofProperties.m_enable && g_dofProperties.m_debug) || (g_shadowProperties.m_shadowType == eSHADOW_SINGLE_HL && g_shadowProperties.m_enable && g_render.UsingShadowShader()))
		useFog = CFalse;
	else 
		useFog = CTrue;

	if ((g_fogProperties.m_enable && useFog) || (g_dofProperties.m_enable && g_polygonMode == ePOLYGON_FILL))
	{
		if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
			g_render.BindForWriting(m_mFboIDFogDof);
		else if(!g_useOldRenderingStyle && g_options.m_enableFBO )
			g_render.BindForWriting( m_fboIDFogDof );
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		g_fogBlurPass = CTrue;
		if (g_editorMode == eMODE_PREFAB)
		{
			g_octree->Render();
		}
		else if (g_editorMode == eMODE_VSCENE)
		{
			g_multipleView->RenderBakedOctree3DModels();
			if (g_menu.m_showOctree)
				g_octree->Render(CFalse, CFalse); //just show octree for debug mode.
		}
		g_multipleView->Render3DAnimatedModels(CTrue);
		g_multipleView->Render3DModelsControlledByPhysX();
		g_multipleView->RenderCharacter(CFalse);
		if (g_menu.m_insertAndShowTerrain)
			g_multipleView->RenderTerrain();


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
		PrintInfo( "\nUpdating Octree...", COLOR_GREEN );
		//Force Update
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			g_render.SetScene( g_scene[i] );
			g_render.GetScene()->m_update = CTrue;
		}

		Render3DModels( CTrue, NULL );
		CDelete(g_octree);
		g_octree = CNew(COctree);
		//g_octree->ResetState();
		//g_octree->Init();
		g_octree->GetWorldDimensions();
		g_octree->SetName( "octree_root" );
		g_octree->SetLevel(0);
		g_octree->AttachGeometriesToNode();
		//g_octree->AttachLightsToGeometries();
		g_updateOctree = CFalse;
		PrintInfo( "\nOctree updated successfully", COLOR_RED_GREEN );
		PrintInfo( "\nReady", COLOR_GREEN );

		g_maxInstancePrefabRadius = -1.f;
		//update shadow max radius
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
		glMatrixMode( GL_MODELVIEW );
	}
	
	if( g_menu.m_insertAndShowSky )
	{
		g_skyDome->RenderDome();
		if( g_menu.m_showSkyIcon && !g_dofProperties.m_debug )
			g_skyDome->RenderIcon();
	}

	//render water icons after the scene rendering
	if ( g_menu.m_showWaterIcons && g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader && !g_dofProperties.m_debug)
	{
		for( CUInt i = 0 ; i < g_engineWaters.size(); i++ )
			g_engineWaters[i]->RenderIcon();
	}
	//Render light icons after the scene rendering
	//Engine lights
	if( !g_dofProperties.m_debug )
	{
		for( CUInt i = 0; i < g_engineLights.size(); i++ )
		{
			g_engineLights[i]->RenderIcon();
			g_engineLights[i]->RenderExtents();
		}
	}
	//render character's camera icon
	if (!g_multipleView->IsPlayGameMode() && g_menu.m_showCameraIcons && g_menu.m_insertCharacter)
	{
		CVec4f rotation(0.0f, 0.0f, 0.0f, 0.0f);
		if (g_mainCharacter)
			rotation = g_mainCharacter->GetInstancePrefab()->GetRotate();

		CFloat pitch = 0.0f;
		CFloat yaw = NxMath::degToRad(rotation.y) + NxMath::degToRad(180.f);
		NxVec3 cameraPos, characterPos, cameraDirection;
		g_multipleView->m_nx->GetCameraAndCharacterPositions(pitch, yaw, cameraPos, characterPos, cameraDirection);
		g_glUtil.Billboarding(cameraPos.x, cameraPos.y, cameraPos.z, g_cameraImg->GetId(), 1.0f, 1.0f);
	}

	if( !g_dofProperties.m_debug )
	{
		//COLLADA Lights. Not recommended
		for( CUInt i = 0 ; i < g_scene.size(); i++ )
		{
			g_render.SetScene( g_scene[i] );
			
			//set all the lights here
			for( CUInt j = 0; j < g_render.GetScene()->GetLightInstanceCount(); j++ )
			{
				CInstanceLight *instanceLight = g_render.GetScene()->GetLightInstances(j);
				if( instanceLight )
				{
					instanceLight->RenderIcon();
					instanceLight->RenderExtents();
				}
			}
		}
	}
	if( !g_dofProperties.m_debug )
	{
		for( CUInt i = 0; i < g_importedCameraInstances.size(); i++ )
		{
			CInstanceCamera * instanceCamera = g_importedCameraInstances[i];
			if( instanceCamera )
				instanceCamera->RenderIcon();
		}
	}

	if( g_physXProperties.m_bDebugMode )
		m_nx->debugMode = CTrue;
	else
		m_nx->debugMode = CFalse;

	if( m_nx->debugMode )
		m_nx->debugRenderer();

	if( g_menu.m_showPerspectiveGrids && !g_dofProperties.m_debug)
		DrawGrid();
	if( g_menu.m_showSoundIcons && !g_dofProperties.m_debug)
	{
		for( CUInt i = 0; i < g_engineStaticSounds.size(); i++ )
		{
			 g_engineStaticSounds[i]->RenderIcon();
		}
	}

	if (g_menu.m_showCameraIcons && !g_dofProperties.m_debug)
	{
		for (CUInt i = 0; i < g_engineCameraInstances.size(); i++)
		{
			g_engineCameraInstances[i]->RenderIcon();
		}
	}

	g_render.m_useShader = CFalse;
	glUseProgram(0);
	if (g_showArrow && !g_multipleView->IsPlayGameMode())
	{
		CFloat distance = sqrt( pow( g_camera->m_perspectiveCameraPos.x - g_arrowPosition.x, 2 ) + 
			pow( g_camera->m_perspectiveCameraPos.y - g_arrowPosition.y, 2 ) + 
			pow( g_camera->m_perspectiveCameraPos.z - g_arrowPosition.z, 2 ) );
		if( distance < 10.0f )
			distance = 10.0f;

		CUInt m_name = g_selectedName;
		static CUInt selectedName;
		if (!g_transformObject && !m_lMouseDown)
		{
			selectedName = g_selectedName = GetSelectedObject(CTrue);
		}
		else
		{
			g_selectedName = selectedName;
		}

		if (g_editorMode == eMODE_VSCENE)
		{
			glDisable(GL_DEPTH_TEST);
			glPushMatrix();
			glTranslatef( g_arrowPosition.x, g_arrowPosition.y, g_arrowPosition.z );
			glScalef( distance * 0.15f , distance * 0.15f, distance * 0.15f);
			if (g_currentTransformType == eCTranslate)
			{
				g_translateArrow->Render(CFalse);
			}
			else if (g_currentTransformType == eCRotate)
			{
				g_rotateArrow->Render(CFalse);
			}
			else if (g_currentTransformType == eCScale)
			{
				g_scaleArrow->Render(CFalse);
			}
			glPopMatrix();
			glEnable(GL_DEPTH_TEST);
		}

		g_selectedName = m_name;
	}

	if (g_camera->m_activatePerspectiveCamera && g_transformObject && m_translationController->Initialized())
	{
		m_translationController->DrawPlane();
	}

	if (g_menu.m_showPerspectiveGrids)
	{
		g_render.PushMatrix();
		glScalef(0.5f, 0.5f, 0.5f);
		g_centerArrowScene->Render(CTrue);
		g_render.PopMatrix();
	}
	g_render.m_useShader = CTrue;

	glDisable( GL_LIGHT0 );

	for( CInt i = 0; i <g_numLights; i++ )
	{
		glDisable( GL_LIGHT0 + i );
	}
	glDisable( GL_LIGHTING ); //just for per vertex lighting'
	glMatrixMode( GL_MODELVIEW );

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

	glPopAttrib(); //lighting

	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForReading( m_mFboID );
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		g_render.BindForWriting( m_fboID );
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

		g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_DEFAULT );

		glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_POSITION );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_NORMAL );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT3_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_DIFFUSE_SHADOW_DIRT );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT4_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT4_EXT);

		//g_render.SetReadBuffer( eGBUFFER_TEXTURE_TYPE_GLOSSMAP );
		//glBlitFramebufferEXT(0, 0, g_width, g_height, 0, 0, g_width, g_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//g_render.BindForReading( m_mFboID );
		//glReadBuffer(GL_COLOR_ATTACHMENT5_EXT);
		//g_render.BindForWriting( m_fboID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT5_EXT);


	}
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	if (!g_useOldRenderingStyle && g_options.m_enableFBO)
	{
		if (g_dofProperties.m_enable && g_polygonMode == ePOLYGON_FILL)
		{
			glUseProgram(g_render.m_dofProgram[1]);
			if (g_dofProperties.m_debug)
				glUniform1i(glGetUniformLocation(g_render.m_dofProgram[1], "debugMode"), CTrue);
			else
				glUniform1i(glGetUniformLocation(g_render.m_dofProgram[1], "debugMode"), CFalse);

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
			glUniform1i(glGetUniformLocation(g_render.m_dofProgram[0], "Tex0"), 0);
			glViewport(0, 0, g_width / 2, g_height / 2);
			m_dof.DrawQuad();

			/* Third pass: Gaussian filtering along the X axis */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[1]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, m_dof.m_texid[0]);
			glUseProgram(g_render.m_dofProgram[1]);
			glUniform1i(glGetUniformLocation(g_render.m_dofProgram[1], "Tex0"), 0);
			glUniform1i(glGetUniformLocation(g_render.m_dofProgram[1], "Width"), g_width);
			m_dof.DrawQuad();

			/* Fourth pass: Gaussian filtering along the Y axis */
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_dof.m_fbo[2]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(g_render.m_dofProgram[2]);
			glUniform1i(glGetUniformLocation(g_render.m_dofProgram[2], "Tex0"), 0);
			glUniform1i(glGetUniformLocation(g_render.m_dofProgram[2], "Height"), g_height);
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
			glUniform1i(glGetUniformLocation(g_render.m_dofProgram[3], "Tex0"), 0);
			glUniform1i(glGetUniformLocation(g_render.m_dofProgram[3], "Tex1"), 1);

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

		if (!g_menu.m_justPerspective)
			glViewport(0, 0, m_width / 2, m_height / 2);// resets the viewport to new dimensions.

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glUseProgram(0);
		glDrawBuffer(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT);
		glPushAttrib(GL_ENABLE_BIT);
		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		gluOrtho2D(0, g_width, 0, g_height);
		glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity();
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		CBool found = CFalse;
		glBindTexture(GL_TEXTURE_2D, m_textureTarget[0]);

		if (!found)
			if (g_dofProperties.m_enable && g_polygonMode == ePOLYGON_FILL)
			{
				glBindTexture(GL_TEXTURE_2D, m_dof.m_texid[3]);
				found = CTrue;
			}

		glEnable(GL_TEXTURE_2D);
		glClearColor(0.37f, 0.37f, 0.37f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//if( !found ) //render default texture with deferred shader
		//{
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_textureTarget[1]);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, m_textureTarget[2]);
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, m_textureTarget[3]);
		//glUseProgram(g_render.m_deferredProgram);
		//glUniform1i( glGetUniformLocation(g_render.m_deferredProgram, "tPosition"), 0 );
		//glUniform1i( glGetUniformLocation(g_render.m_deferredProgram, "tNormals"), 1 );
		//glUniform1i( glGetUniformLocation(g_render.m_deferredProgram, "tDiffuse"), 2 );
		//CFloat cameraPos[3] = { g_camera->m_perspectiveCameraPos.x, g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z };
		//glUniform3fv(glGetUniformLocation(g_render.m_deferredProgram, "cameraPosition"), 1, cameraPos);
		//}

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);	glVertex2d(0, 0);
		glTexCoord2d(1, 0);	glVertex2d(g_width, 0);
		glTexCoord2d(1, 1);	glVertex2d(g_width, g_height);
		glTexCoord2d(0, 1);	glVertex2d(0, g_height);
		glEnd();
		glUseProgram(0);

		CBool useBloom;
		if ((g_dofProperties.m_enable && g_dofProperties.m_debug) || g_materialChannels != eCHANNELS_ALL || g_polygonMode != ePOLYGON_FILL)
			useBloom = CFalse;
		else
			useBloom = CTrue;

		if (g_render.m_useBloom && useBloom && g_bloomProperties.m_enable)
		{
			g_bloom->CreateRuntimeTexture(g_width, g_height, m_textureTarget[0]);

			glPushAttrib(GL_CURRENT_BIT);

			glEnable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glColor4f(g_bloomProperties.m_bloomColor[0], g_bloomProperties.m_bloomColor[1], g_bloomProperties.m_bloomColor[2], g_bloomProperties.m_bloomIntensity);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, g_bloom->m_bloomTexture);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0);	glVertex2d(0, 0);
			glTexCoord2d(1, 0);	glVertex2d(g_width, 0);
			glTexCoord2d(1, 1);	glVertex2d(g_width, g_height);
			glTexCoord2d(0, 1);	glVertex2d(0, g_height);
			glEnd();
			glDisable(GL_BLEND);
			glPopAttrib();
		}

		if (g_shadowProperties.m_enable && g_render.UsingShadowShader() && g_menu.m_showDynamicShadowDepthTexture && g_render.m_useDynamicShadowMap && g_options.m_enableShader)
			m_dynamicShadowMap->ShowDepthTex();

		DrawGUI();
		glPopAttrib();
		glMatrixMode(GL_PROJECTION); glPopMatrix();
		glMatrixMode(GL_MODELVIEW); glPopMatrix();

		if (GetSaveScreenshot())
		{
			CUInt width, height;
			if (g_menu.m_justPerspective)
			{
				width = g_width;
				height = g_height;
			}
			else
			{
				width = g_width / 2;
				height = g_height / 2;
			}
			CChar screenshotTGAPath[MAX_URI_SIZE];
			sprintf(screenshotTGAPath, "%s%s", GetScreenshotPath(), ".tga");

			g_glUtil.SaveScreenShot(screenshotTGAPath, width, height);
			ILboolean loaded = ilLoadImage(screenshotTGAPath);
			if (loaded != IL_FALSE)
			{
				iluImageParameter(ILU_FILTER, ILU_SCALE_LANCZOS3);

				iluScale(182, 101, ilGetInteger(IL_IMAGE_DEPTH));

				ilEnable(IL_FILE_OVERWRITE);

				CChar screenshotBMPPath[MAX_URI_SIZE];
				sprintf(screenshotBMPPath, "%s%s", GetScreenshotPath(), ".bmp");

				ilSaveImage(screenshotBMPPath);

				DeleteFile(screenshotTGAPath);

			}
			SetSaveScreenshot(CFalse);
		}

	}
	else
	{
		DrawGUI();
	}


	ResetData();

	glFlush();

}

CVoid CMultipleWindows::ResetData()
{
	if (g_editorMode == eMODE_VSCENE)
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
	else if (g_editorMode == eMODE_PREFAB)
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			g_scene[i]->ResetSkinData();
		}
	}

}

CVoid CMultipleWindows::DrawJustPerspectiveBorders()
{
	//Draw black borders here
    glMatrixMode(GL_PROJECTION);    // Sets the projection matrix.
	glPushMatrix();
    glLoadIdentity();               // Reset the modelview matrix.

    // calculate the aspect ratio of the window.
	glOrtho( 0, 1024, 0, 768, -50000, 50000 );
	

	glPushAttrib( GL_VIEWPORT_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );

	//main borders
    glMatrixMode(GL_MODELVIEW);     // Sets the model view matrix.
	glPushMatrix();
    glLoadIdentity();               // Reset the modelview matrix.

	glColor4f( 0.4f, 0.4f, 0.4f, 0.0f );
	glLineWidth( 3.0f );

	glBegin( GL_LINE_LOOP );
	glVertex3f( 0 , 0, 49999.0 ); 
	glVertex3f( 1024 , 0, 49999.0 ); 
	glVertex3f( 1024 , 768, 49999.0 ); 
	glVertex3f( 0, 768, 49999.0 ); 
	glEnd();
	glPopAttrib();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

CVoid CMultipleWindows::DrawBordersAndUI()
{
	//Draw black borders here
    glMatrixMode(GL_PROJECTION);    // Sets the projection matrix.
	glPushMatrix();
    glLoadIdentity();               // Reset the modelview matrix.

    // calculate the aspect ratio of the window.
	glOrtho( 0, 1024, 0, 768, -50000, 50000 );
	
    glMatrixMode(GL_MODELVIEW);     // Sets the model view matrix.
	glPushMatrix();
    glLoadIdentity();               // Reset the modelview matrix.

	glPushAttrib( GL_VIEWPORT_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );

	//main borders

	glViewport( 0, 0, m_width, m_height );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0.0, 1024.0, 0.0, 768.0, -50000.0, 50000.0 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glColor4f( 0.157f, 0.157f, 0.157f, 0.0f );
	glLineWidth( 5.0f );

	glBegin( GL_LINES );
	glVertex3f( 1024 / 2 , 0, 49999.0 ); 
	glVertex3f( 1024 / 2 , 768, 49999.0 ); 
	glVertex3f( 0 , 768 / 2, 49999.0 ); 
	glVertex3f( 1024, 768 / 2, 49999.0 ); 
	glEnd();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glLineWidth( 2.0f );
	//perspective borders
	glViewport(0, 0, m_width / 2, m_height / 2);// resets the viewport to new dimensions.
	if( g_camera->m_activatePerspectiveCamera )
		glColor4f( 1.0f, 0.6f, 0.0f, 0.0f );
	else
		glColor4f(0.1f, 0.1f, 0.1f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f( 2, 2, 49999.0 ); 
	glVertex3f( 1024 - 6, 2, 49999.0 ); 
	glVertex3f( 1024 - 6, 768 - 8, 49999.0 ); 
	glVertex3f( 2, 768 - 8, 49999.0 ); 
	glEnd();

	//Lower right borders
	glLineWidth( 2.0f );

	glViewport(m_width / 2, 0, m_width / 2 , m_height / 2);// resets the viewport to new dimensions.
	if( g_camera->m_activateLowerRightCamera )
		glColor4f( 1.0f, 0.6f, 0.0f, 0.0f );
	else
		glColor4f(0.1f, 0.1f, 0.1f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f( 5, 2, 49999.0 ); 
	glVertex3f( 1024 - 2, 2, 49999.0 ); 
	glVertex3f( 1024 - 2, 768 - 8, 49999.0 ); 
	glVertex3f( 5, 768 - 8, 49999.0 ); 
	glEnd();

	//Axes of lower right window
	glLineWidth( 1.0f );
	glColor4f( 0.7f, 0.0f, 0.0f, 0.0f );
	glBegin( GL_LINES );
	glVertex2i( 31 , 39 ); 
	glVertex2i( 77 , 39 ); 
	glColor4f( 0.0f, 0.0f, 1.0f, 0.0f );
	glVertex2i( 77 , 39 ); 
	glVertex2i( 77 , 100 ); 
	glEnd();

	m_simpleFont.StartRendering();
	m_simpleFont.Print( "y", 60.0f, 40.0f, 0.0f, 0.00f, 0.7f, 0.0f );
	m_simpleFont.Print( "z", 55.0f, 140.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	m_simpleFont.Print( "x", 15.0f, 35.0f, 0.0f, 0.7f, 0.0f, 0.0f );
	m_simpleFont.EndRendering();

	//Upper left borders
	glLineWidth( 2.0f );

	glViewport(0, m_height / 2 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	if( g_camera->m_activateUpperLeftCamera )
		glColor4f( 1.0f, 0.6f, 0.0f, 0.0f );
	else
		glColor4f(0.1f, 0.1f, 0.1f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f( 2, 6, 49999.0 ); 
	glVertex3f( 1024 - 6, 6, 49999.0 ); 
	glVertex3f( 1024 - 6, 768 - 2, 49999.0 ); 
	glVertex3f( 2, 768 - 2, 49999.0 ); 
	glEnd();

	//Axes of upper left window
	glLineWidth( 1.0f );
	glColor4f( 0.7f, 0.0f, 0.0f, 0.0f );
	glBegin( GL_LINES );
	glVertex2i( 31 , 39 ); 
	glVertex2i( 77 , 39 ); 
	glColor4f( 0.0f, 0.7f, 0.0f, 0.0f );
	glVertex2i( 31 , 39 ); 
	glVertex2i( 31 , 100 ); 
	glEnd();

	m_simpleFont.StartRendering();
	m_simpleFont.Print( "x", 60.0f, 40.0f, 0.0f, 0.7f, 0.0f, 0.0f );
	m_simpleFont.Print( "y", 20.0f, 140.0f, 0.0f, 0.0f, 0.7f, 0.0f );
	m_simpleFont.Print( "z", 15.0f, 35.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	m_simpleFont.EndRendering();

	//Upper right borders
	glLineWidth( 2.0f );

	glViewport(m_width / 2, m_height / 2 , m_width / 2 , m_height / 2 );// resets the viewport to new dimensions.
	if( g_camera->m_activateUpperRightCamera )
		glColor4f( 1.0f, 0.6f, 0.0f, 0.0f );
	else
		glColor4f(0.1f, 0.1f, 0.1f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f( 5, 6, 49999.0 ); 
	glVertex3f( 1024 - 2, 6, 49999.0 ); 
	glVertex3f( 1024 - 2, 768 - 2, 49999.0 ); 
	glVertex3f( 5, 768 - 2, 49999.0 ); 
	glEnd();
	//Axes of upper right window
	glLineWidth( 1.0f );
	glColor4f( 0.0f, 0.0f, 1.0f, 0.0f );
	glBegin( GL_LINES );
	glVertex2i( 31 , 39 ); 
	glVertex2i( 77 , 39 ); 
	glColor4f( 0.0f, 0.7f, 0.0f, 0.0f );
	glVertex2i( 77 , 39 ); 
	glVertex2i( 77 , 100 ); 
	glEnd();

    glMatrixMode(GL_PROJECTION);    // Sets the projection matrix.
	glPopMatrix();
    glMatrixMode(GL_MODELVIEW);     // Sets the model view matrix.
	glPopMatrix();

	m_simpleFont.StartRendering();
	m_simpleFont.Print( "x", 60.0f, 40.0f, 0.0f, 0.7f, 0.0f, 0.0f );
	m_simpleFont.Print( "y", 55.0f, 140.0f, 0.0f, 0.0f, 0.7f, 0.0f );
	m_simpleFont.Print( "z", 15.0f, 35.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	m_simpleFont.EndRendering();


	glPopAttrib();
}

CVoid CMultipleWindows::RenderWindow()
{
	if (g_clickedOpen)
		return;
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	if (g_editorMode == eMODE_GUI)
	{
		DrawGUIMode();
	}
	else
	{
		DrawPerspective();
		if (!g_menu.m_justPerspective)
		{
			CPolygonMode tempMode = g_polygonMode;
			g_polygonMode = ePOLYGON_FILL; //to enable texturing. It's not a good way though!
			glDisable(GL_MULTISAMPLE);

			for (CUInt i = 0; i < g_instancePrefab.size(); i++)
			{
				g_instancePrefab[i]->SetRenderForQuery(CTrue);
			}

			DrawUpperLeft();
			DrawUpperRight();
			DrawLowerRight();

			for (CUInt i = 0; i < g_instancePrefab.size(); i++)
			{
				g_instancePrefab[i]->SetRenderForQuery(CFalse);
			}

			DrawBordersAndUI();
			g_polygonMode = tempMode;
		}
		else
		{
			DrawJustPerspectiveBorders();
		}
		ResetPhysXCounts();
	}
	if (m_swapBuffers)
		SwapBuffers(m_pDC->m_hDC);
}

CVoid CMultipleWindows::ResetPhysXCounts()
{
	for( CUInt i = 0 ; i < g_scene.size(); i++ )
	{
		for( CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++ )
			g_scene[i]->m_instanceGeometries[j]->m_physXCount = 0;
	}
}

BOOL CMultipleWindows::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

		if( pMsg->message == WM_KEYDOWN )
		{
			return TRUE;
		}

	return CWnd::PreTranslateMessage(pMsg);
}

//void CMultipleWindows::ApplyForce( NxVec3 forceDirection, CInt moveDirection, CFloat elapsedTime )
//{
//	if( m_physXVariables.m_applyDirectionalForce )
//	{
//		if( m_physXVariables.m_forceSpeedValue >= 0.0f )
//		{
//			m_nx->bPushCharacter = CTrue;
//			m_nx->runPhysics( forceDirection, m_physXVariables.m_forceSpeedValue, moveDirection, elapsedTime );
//		}
//		else 
//		{
//			m_nx->bPushCharacter = CFalse;
//			m_physXVariables.m_applyDirectionalForce = CFalse;
//		}
//		m_physXVariables.m_forceSpeedValue -= elapsedTime * m_physXVariables.m_forceDecreaseValue;
//	}
//	else
//		m_nx->runPhysics( NxVec3( 0.0f, 0.0f, 0.0f ), 0.0f, moveDirection, elapsedTime );
//}

CVoid CMultipleWindows::ApplyForce( CInt direction, CFloat elapsedTime )
{
	m_nx->runPhysics( NxVec3( 0.0f, 0.0f, 0.0f ), 0.0f, direction, elapsedTime );
}

CBool CMultipleWindows::ManageCharacterBlends(CChar* animationType, CChar* IdleAnimationName)
{
	if (!IsPlayGameMode()) return CFalse;

	if (Cmp(m_previousCharacterAnimationType, animationType))
		return CFalse;
	else 
		Cpy(m_previousCharacterAnimationType, animationType);

	if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON && g_multipleView->IsPlayGameMode()) return CFalse;

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
			CChar temp[MAX_NAME_SIZE];
			if (IdleAnimationName)
				sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", IdleAnimationName, "'");
			else
				sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", idleName[0].c_str(), "'");
			PrintInfo(temp, COLOR_RED);
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
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", walkName[0].c_str(), "'");
			PrintInfo(temp, COLOR_RED);
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
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", runName[0].c_str(), "'");
			PrintInfo(temp, COLOR_RED);
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
			CChar temp[MAX_NAME_SIZE];
			sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", jumpName[0].c_str(), "'");
			PrintInfo(temp, COLOR_RED);
			return CFalse;
		}

		if (index != -1)
		{
			scene->SetClipIndex(index);
			scene->ExecuteAction(index, g_characterBlendingProperties.m_jumpDelayIn, g_characterBlendingProperties.m_jumpDelayOut, 1.0f, CFalse);

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

CBool CMultipleWindows::IsJumping(CBool &isInList)
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

CBool CMultipleWindows::GetJumpCurrentEndDuration(CFloat& duration)
{
	if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON && g_multipleView->IsPlayGameMode()) return CFalse;

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
		CChar temp[MAX_NAME_SIZE];
		sprintf(temp, "\n%s%s%s", "Couldn't find the animation clip '", jumpName[0].c_str(), "'");
		PrintInfo(temp, COLOR_RED);
		return CFalse;
	}
	duration = scene->m_animationClips[index]->GetEnd() - scene->m_animationClips[index]->GetCurrentAnimationTime();
	return CTrue;
}

CVoid CMultipleWindows::ProcessInputs()
{

	m_inputSystem->Update();

	CInt dx, dy;
	if (m_lMouseDown || m_rMouseDown)
	{
		m_inputSystem->GetMouseMovement(dx, dy);
	}
	//lower left viewport
	if (g_camera->m_activateLowerRightCamera)
	{
		if (m_lMouseDown)
		{
			if (dx > 0)
				g_camera->m_lowerRightMoveHorizantal += (CFloat)dx * g_camera->m_lowerRightZoom * 2.0f;
			else if (dx < 0)
				g_camera->m_lowerRightMoveHorizantal += (CFloat)dx * g_camera->m_lowerRightZoom * 2.0f;

			if (dy > 0)
				g_camera->m_lowerRightMoveVertical += (CFloat)dy * g_camera->m_lowerRightZoom * 2.0f;
			else if (dy < 0)
				g_camera->m_lowerRightMoveVertical += (CFloat)dy * g_camera->m_lowerRightZoom * 2.0f;
		}
		else if (m_rMouseDown) //zoom in and out
		{
			if (dy > 0 && g_camera->m_lowerRightZoom <  0.2f)
				g_camera->m_lowerRightZoom += (CFloat)dy * g_camera->m_lowerRightZoom * 0.004f;
			else if (dy < 0 && g_camera->m_lowerRightZoom > 0.001f)
				g_camera->m_lowerRightZoom += (CFloat)dy * g_camera->m_lowerRightZoom * 0.004f;
		}
	}
	//upper left viewport
	else if (g_camera->m_activateUpperLeftCamera)
	{
		if (m_lMouseDown) //move horizontal or vertical
		{
			if (dx > 0)
				g_camera->m_upperLeftMoveHorizantal -= (CFloat)dx * g_camera->m_upperLeftZoom * 2.0f;
			else if (dx < 0)
				g_camera->m_upperLeftMoveHorizantal -= (CFloat)dx * g_camera->m_upperLeftZoom * 2.0f;

			if (dy > 0)
				g_camera->m_upperLeftMoveVertical += (CFloat)dy * g_camera->m_upperLeftZoom * 2.0f;
			else if (dy < 0)
				g_camera->m_upperLeftMoveVertical += (CFloat)dy * g_camera->m_upperLeftZoom * 2.0f;
		}
		else if (m_rMouseDown) //zoom in and out
		{
			if (dy > 0 && g_camera->m_upperLeftZoom < 0.2f)
				g_camera->m_upperLeftZoom += (CFloat)dy * g_camera->m_upperLeftZoom * 0.004f;
			else if (dy < 0 && g_camera->m_upperLeftZoom > 0.001f)
				g_camera->m_upperLeftZoom += (CFloat)dy * g_camera->m_upperLeftZoom * 0.004f;
		}
	}
	//upper right viewport
	else if (g_camera->m_activateUpperRightCamera)
	{
		if (m_lMouseDown)
		{
			if (dx > 0)
				g_camera->m_upperRightMoveHorizantal += (CFloat)dx * g_camera->m_upperRightZoom * 2.0f;
			else if (dx < 0)
				g_camera->m_upperRightMoveHorizantal += (CFloat)dx * g_camera->m_upperRightZoom * 2.0f;

			if (dy > 0)
				g_camera->m_upperRightMoveVertical += (CFloat)dy * g_camera->m_upperRightZoom * 2.0f;
			else if (dy < 0)
				g_camera->m_upperRightMoveVertical += (CFloat)dy * g_camera->m_upperRightZoom * 2.0f;
		}
		else if (m_rMouseDown) //zoom in and out
		{
			if (dy > 0 && g_camera->m_upperRightZoom < 0.2f)
				g_camera->m_upperRightZoom += (CFloat)dy * g_camera->m_upperRightZoom * 0.004f;
			else if (dy < 0 && g_camera->m_upperRightZoom > 0.001f)
				g_camera->m_upperRightZoom += (CFloat)dy * g_camera->m_upperRightZoom * 0.004f;
		}
	}

	if (m_notFocus) return;
	if(m_loadScene) return;

	if (m_lockInput || m_isMenu)
	{
		ApplyForce(IDLE, elapsedTime);
		EnableIdleAnimations();
		return;
	}

	//static CBool bEscapeDown = CFalse;

	//if (m_playGameMode)
	//{
	//	if (m_inputSystem->KeyDown(DIK_ESCAPE) && !bEscapeDown)
	//	{
	//		bEscapeDown = CTrue;
	//		PrintInfo("\nGame Will Exit", COLOR_YELLOW);
	//	}
	//	if (m_inputSystem->KeyUp(DIK_ESCAPE))
	//		bEscapeDown = CFalse;
	//}

	if( g_currentCameraType == eCAMERA_PHYSX )
	{
		CBool forceApplied = false;
		CBool move = CFalse;

		if( true/*m_lMouseDown*/ )
		{
			//if( m_inputSystem->KeyDown( DIK_LCONTROL ) && m_inputSystem->KeyDown( DIK_W ) )
			//{
			//	g_multipleView->m_nx->gCharacterWalkSpeed += 1.0f;
			//}
			//else 	if( m_inputSystem->KeyDown( DIK_LCONTROL ) && m_inputSystem->KeyDown( DIK_S ) )
 		//	{
			//	g_multipleView->m_nx->gCharacterWalkSpeed -= 1.0f;
			//	if(	g_multipleView->m_nx->gCharacterWalkSpeed <= 0.0 )
			//		g_multipleView->m_nx->gCharacterWalkSpeed += 1.0f;

			//}
			//else	if( m_inputSystem->KeyDown( DIK_LCONTROL ) && m_inputSystem->KeyDown( DIK_LSHIFT ) )
			//{
			//	g_multipleView->m_nx->gCharacterWalkSpeed = g_physXProperties.m_fCharacterWalkSpeed;
			//}
			//else	if( m_inputSystem->KeyDown( DIK_LCONTROL ) && m_inputSystem->KeyDown( DIK_Z ) )
			//{
			//	g_camera->m_cameraManager->SetAngle( DEFAULT_CAMERA_ANGLE );
			//}
			/*else*/	if( (m_inputSystem->KeyDown( DIK_W ) || m_inputSystem->KeyDown( DIK_UP )) && (m_inputSystem->KeyDown( DIK_D ) || m_inputSystem->KeyDown( DIK_RIGHT )) )
			{
				ApplyForce( MOVE_FRONT_RIGHT, elapsedTime );
				forceApplied = true;
				m_tempMovement = CTrue;
				if (!g_multipleView->m_nx->gJump)
					move = CTrue;
				if (move)
				{
					if (m_inputSystem->KeyDown(DIK_LSHIFT))
					{
						ManageCharacterBlends("run");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						}
					}
					else
					{
						ManageCharacterBlends("walk");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						}
					}
				}

			}
			else	if( (m_inputSystem->KeyDown( DIK_W ) || m_inputSystem->KeyDown( DIK_UP )) && (m_inputSystem->KeyDown( DIK_A ) || m_inputSystem->KeyDown( DIK_LEFT )) )
			{
				ApplyForce( MOVE_FRONT_LEFT, elapsedTime );
				forceApplied = true;
				m_tempMovement = CTrue;
				if (!g_multipleView->m_nx->gJump)
					move = CTrue;
				if (move)
				{
					if (m_inputSystem->KeyDown(DIK_LSHIFT))
					{
						ManageCharacterBlends("run");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						}
					}
					else
					{
						ManageCharacterBlends("walk");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						}
					}
				}

			}
			else if( (m_inputSystem->KeyDown( DIK_S ) || m_inputSystem->KeyDown( DIK_DOWN )) && (m_inputSystem->KeyDown( DIK_D ) || m_inputSystem->KeyDown( DIK_RIGHT )) )
			{
				ApplyForce( MOVE_BACK_RIGHT, elapsedTime );
				forceApplied = true;
				m_tempMovement = CTrue;
				if (!g_multipleView->m_nx->gJump)
					move = CTrue;
				if (move)
				{
					if (m_inputSystem->KeyDown(DIK_LSHIFT))
					{
						ManageCharacterBlends("run");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						}
					}
					else
					{
						ManageCharacterBlends("walk");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						}
					}
				}

			}
			else if( (m_inputSystem->KeyDown( DIK_S ) || m_inputSystem->KeyDown( DIK_DOWN )) && (m_inputSystem->KeyDown( DIK_A ) || m_inputSystem->KeyDown( DIK_LEFT )) )
			{
				ApplyForce( MOVE_BACK_LEFT, elapsedTime );
				forceApplied = true;
				m_tempMovement = CTrue;
				if (!g_multipleView->m_nx->gJump)
					move = CTrue;
				if (move)
				{
					if (m_inputSystem->KeyDown(DIK_LSHIFT))
					{
						ManageCharacterBlends("run");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						}
					}
					else
					{
						ManageCharacterBlends("walk");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						}
					}
				}

			}

			else	if( m_inputSystem->KeyDown( DIK_W ) || m_inputSystem->KeyDown( DIK_UP ) )
			{
				ApplyForce( MOVE_FRONT, elapsedTime );
				forceApplied = true;
				m_tempMovement = CTrue;
				if (!g_multipleView->m_nx->gJump)
					move = CTrue;
				if (move)
				{
					if (m_inputSystem->KeyDown(DIK_LSHIFT))
					{
						ManageCharacterBlends("run");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						}
					}
					else
					{
						ManageCharacterBlends("walk");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						}
					}
				}

			}
			else if( m_inputSystem->KeyDown( DIK_S ) || m_inputSystem->KeyDown( DIK_DOWN ) )
			{
				ApplyForce( MOVE_BACK, elapsedTime );
				forceApplied = true;
				m_tempMovement = CTrue;
				if (!g_multipleView->m_nx->gJump)
					move = CTrue;
				if (move)
				{
					if (m_inputSystem->KeyDown(DIK_LSHIFT))
					{
						ManageCharacterBlends("run");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						}
					}
					else
					{
						ManageCharacterBlends("walk");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						}
					}
				}

			}
			else if( m_inputSystem->KeyDown( DIK_A ) || m_inputSystem->KeyDown( DIK_LEFT ) )
			{
				ApplyForce( MOVE_LEFT, elapsedTime );
				forceApplied = true;
				m_tempMovement = CTrue;
				if (!g_multipleView->m_nx->gJump)
					move = CTrue;
				if (move)
				{
					if (m_inputSystem->KeyDown(DIK_LSHIFT))
					{
						ManageCharacterBlends("run");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						}
					}
					else
					{
						ManageCharacterBlends("walk");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						}
					}
				}
			}
			else if( m_inputSystem->KeyDown( DIK_D ) || m_inputSystem->KeyDown( DIK_RIGHT ) )
			{
				ApplyForce( MOVE_RIGHT, elapsedTime );
				forceApplied = true;
				m_tempMovement = CTrue;
				if (!g_multipleView->m_nx->gJump)
					move = CTrue;
				if (move)
				{
					if (m_inputSystem->KeyDown(DIK_LSHIFT))
					{
						ManageCharacterBlends("run");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						}
					}
					else
					{
						ManageCharacterBlends("walk");
						if (g_mainCharacter)
						{
							g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
							g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
						}
					}
				}

			}
			//else if( m_inputSystem->KeyDown( DIK_Q ) )
			//{
			//	ApplyForce( MOVE_UP, elapsedTime );
			//	forceApplied = true;
			//	m_tempMovement = CTrue;
			//}
			//else if( m_inputSystem->KeyDown( DIK_E ) )
			//{
			//	ApplyForce( MOVE_DOWN, elapsedTime );
			//	forceApplied = true;
			//	m_tempMovement = CTrue;
			//}
		}
		if (!forceApplied || !move)
		{
			if (!forceApplied)
				ApplyForce(IDLE, elapsedTime);

			if (!move && g_mainCharacter)
			{
				CBool idle = CFalse;
				if (!g_multipleView->m_nx->gJump)
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
						g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
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

		if( /*g_physXProperties.m_bApplyGravity &&*/ g_physXProperties.m_bJumping)
			if( g_multipleView->m_inputSystem->KeyDown( DIK_SPACE ) )
				if (!g_multipleView->m_nx->gJump)
				{
					CBool jumping = CFalse;
					if (IsJumping(jumping) && !jumping)
					{
						ManageCharacterBlends("jump");
						g_multipleView->m_nx->StartJump(g_physXProperties.m_fJumpPower);
						g_multipleView->m_soundSystem->PlayALSound(*(g_mainCharacter->m_jumpSound->GetSoundSource()));
						g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
						g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));

					}
					else if (!g_mainCharacter)
					{
						g_multipleView->m_nx->StartJump(g_physXProperties.m_fJumpPower);
					}
				}


		if( m_lMouseDown || m_rMouseDown )
		{
			if (dx != 0 || dy != 0)
				m_calculateDistance = CTrue;

			if( !m_selectObject )
			{
				//perspective viewport
				if( g_camera->m_activatePerspectiveCamera )
				{
					if( m_lMouseDown )
					{
						if( dx > 0 ) 
						{
							g_camera->m_perspectiveCameraYaw -= (CFloat)dx * 0.004f;
							m_tempMovement = CTrue;
						}
						else if( dx < 0 )
						{
							g_camera->m_perspectiveCameraYaw -= (CFloat)dx * 0.004f;
							m_tempMovement = CTrue;
						}
						if( dy > 0 ) 
						{
							g_camera->m_perspectiveCurrentCameraTilt -= (CFloat)dy * 0.01f;
							if (g_camera->m_perspectiveCurrentCameraTilt > g_camera->m_perspectiveCameraMaxTilt)
								g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraMaxTilt;
							else if (g_camera->m_perspectiveCurrentCameraTilt < g_camera->m_perspectiveCameraMinTilt)
								g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraMinTilt;
							m_tempMovement = CTrue;

						}
						else if( dy < 0 )
						{
							g_camera->m_perspectiveCurrentCameraTilt -= (CFloat)dy * 0.01f;
							if (g_camera->m_perspectiveCurrentCameraTilt > g_camera->m_perspectiveCameraMaxTilt)
								g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraMaxTilt;
							else if (g_camera->m_perspectiveCurrentCameraTilt < g_camera->m_perspectiveCameraMinTilt)
								g_camera->m_perspectiveCurrentCameraTilt = g_camera->m_perspectiveCameraMinTilt;
							m_tempMovement = CTrue;
						}
					}
					//else if( m_rMouseDown )
					//{  
					//	if( dy > 0 ) 
					//		g_camera->m_cameraManager->SetZoomOut( elapsedTime * 50.0f );
					//	else if( dy < 0 )
					//		g_camera->m_cameraManager->SetZoomIn( elapsedTime * 50.0f );
					//}
				}
			}
		}
	}
	else if (g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX)
	{
		//add keyboard input here
		if( true/*m_lMouseDown*/ )
		{
			//default speed
			if ((m_inputSystem->KeyDown(DIK_LCONTROL) || m_inputSystem->KeyDown(DIK_RCONTROL)) &&
				(m_inputSystem->KeyDown(DIK_LSHIFT) || m_inputSystem->KeyDown(DIK_RSHIFT)))
			{
				if (fabs(g_camera->m_cameraSpeed - DEFAULT_CAMERA_SPEED) > EPSILON)
				{
					g_camera->m_cameraSpeed = DEFAULT_CAMERA_SPEED;
					PrintInfo("\nCamera Speed Set to Default Speed");
				}
			}
			//default zoom
			if ((m_inputSystem->KeyDown(DIK_LCONTROL) || m_inputSystem->KeyDown(DIK_RCONTROL)) &&
				m_inputSystem->KeyDown(DIK_Z))
			{
				if (fabs(g_render.GetDefaultInstanceCamera()->m_abstractCamera->GetAngle() - DEFAULT_CAMERA_ANGLE) > EPSILON)
				{
					g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetAngle(DEFAULT_CAMERA_ANGLE);
					PrintInfo("\nCamera Zoom Set to Default Zoom");
				}
			}

			if (m_inputSystem->KeyDown( DIK_S ))
			{
				//Decrease Speed?
				if (m_inputSystem->KeyDown(DIK_LCONTROL) || m_inputSystem->KeyDown(DIK_RCONTROL))
				{
					g_camera->m_cameraSpeed -= 5.0f;
					if (g_camera->m_cameraSpeed < EPSILON)
					{
						g_camera->m_cameraSpeed += 5.0f;
						PrintInfo("\nError: Couldn't Decrease the Camera Speed", COLOR_RED);
					}
					else
					{
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "%s%.2f", "\nCamera Speed Decreased to: ", g_camera->m_cameraSpeed);
						PrintInfo(temp);
					}
				}
				else
				{
					// UI code to move the camera closer
					g_render.GetDefaultInstanceCamera()->MoveTransform(elapsedTime * g_camera->m_cameraSpeed, 0.0f, 0.0f);
				}
			}

			if (m_inputSystem->KeyDown( DIK_W ))
			{
				//Increase Speed
				if (m_inputSystem->KeyDown(DIK_LCONTROL) || m_inputSystem->KeyDown(DIK_RCONTROL))
				{
					g_camera->m_cameraSpeed += 5.0f;
					CChar temp[MAX_NAME_SIZE];
					sprintf(temp, "%s%.2f", "\nCamera Speed Increased to: ", g_camera->m_cameraSpeed);
					PrintInfo(temp);
				}
				else
				{
					// UI code to move the camera farther away
					g_render.GetDefaultInstanceCamera()->MoveTransform(-elapsedTime* g_camera->m_cameraSpeed, 0.0f, 0.0f);
				}
			}

			if (m_inputSystem->KeyDown( DIK_Q ))
			{
				// UI code to move the camera farther up
				g_render.GetDefaultInstanceCamera()->MoveTransform(0.0f, 0.0f, elapsedTime* g_camera->m_cameraSpeed);
			}

			if (m_inputSystem->KeyDown( DIK_E ))
			{
				// UI code to move the camera farther down
				g_render.GetDefaultInstanceCamera()->MoveTransform(0.0f, 0.0f, -elapsedTime* g_camera->m_cameraSpeed);
			}

			if (m_inputSystem->KeyDown( DIK_D ))
			{
				// UI code to move the camera farther right
				g_render.GetDefaultInstanceCamera()->MoveTransform(0.0f, -elapsedTime* g_camera->m_cameraSpeed, 0.0f);
			}

			if (m_inputSystem->KeyDown( DIK_A ))
			{
				// UI code to move the camera farther left
				g_render.GetDefaultInstanceCamera()->MoveTransform(0.0f, elapsedTime* g_camera->m_cameraSpeed, 0.0f);
			}
		}
		if( m_lMouseDown || m_rMouseDown )
		{
			if (dx != 0 || dy != 0)
				m_calculateDistance = CTrue;


			if( !m_selectObject )
			{
				//perspective viewport
				if( g_camera->m_activatePerspectiveCamera )
				{
					if( m_lMouseDown )
					{
						g_render.GetDefaultInstanceCamera()->SetPanAndTilt( -(CFloat)dx * 0.2f, -(CFloat)dy * 0.2f );
						if( dx != 0 || dy != 0 )
							m_tempMovement = CTrue;
					}
					else if( m_rMouseDown )
					{  
						if (dy > 0)
							g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetZoomOut(elapsedTime * 50.0f);
						else if (dy < 0)
							g_render.GetDefaultInstanceCamera()->m_abstractCamera->SetZoomIn(elapsedTime * 50.0f);

					}
				}
			}
		}
	}
	else if (g_currentCameraType == eCAMERA_ENGINE)
	{
		if (!g_multipleView->IsPlayGameMode())
		{
			CInstanceCamera* instance_camera = NULL;
			for (CUInt c = 0; c < g_engineCameraInstances.size(); c++)
			{
				if (g_engineCameraInstances[c]->IsActive())
				{
					instance_camera = g_engineCameraInstances[c];
					break;
				}
			}
			if (!instance_camera)
				return;
			//add keyboard input here
			if (true/*m_lMouseDown*/)
			{
				//default speed
				if ((m_inputSystem->KeyDown(DIK_LCONTROL) || m_inputSystem->KeyDown(DIK_RCONTROL)) &&
					(m_inputSystem->KeyDown(DIK_LSHIFT) || m_inputSystem->KeyDown(DIK_RSHIFT)))
				{
					if (fabs(instance_camera->GetCameraSpeed() - DEFAULT_CAMERA_SPEED) > EPSILON)
					{
						instance_camera->SetCameraSpeed(DEFAULT_CAMERA_SPEED);
						PrintInfo("\nCamera Speed Set to Default Speed");
					}
				}

				if (m_inputSystem->KeyDown(DIK_S))
				{
					//Decrease Speed?
					if (m_inputSystem->KeyDown(DIK_LCONTROL) || m_inputSystem->KeyDown(DIK_RCONTROL))
					{
						instance_camera->IncreaseOrDecreaseCameraSpeed(-5.0f);
						if (instance_camera->GetCameraSpeed() < EPSILON)
						{
							instance_camera->IncreaseOrDecreaseCameraSpeed(5.0f);
							PrintInfo("\nError: Couldn't Decrease the Camera Speed", COLOR_RED);
						}
						else
						{
							CChar temp[MAX_NAME_SIZE];
							sprintf(temp, "%s%.2f", "\nCamera Speed Decreased to: ", instance_camera->GetCameraSpeed());
							PrintInfo(temp);
						}
					}
					else
					{
						// UI code to move the camera closer
						instance_camera->MoveTransform(elapsedTime * instance_camera->GetCameraSpeed(), 0.0f, 0.0f);
					}
				}

				if (m_inputSystem->KeyDown(DIK_W))
				{
					//Increase Speed
					if (m_inputSystem->KeyDown(DIK_LCONTROL) || m_inputSystem->KeyDown(DIK_RCONTROL))
					{
						instance_camera->IncreaseOrDecreaseCameraSpeed(5.0f);
						CChar temp[MAX_NAME_SIZE];
						sprintf(temp, "%s%.2f", "\nCamera Speed Increased to: ", instance_camera->GetCameraSpeed());
						PrintInfo(temp);
					}
					else
					{
						// UI code to move the camera farther away
						instance_camera->MoveTransform(-elapsedTime* instance_camera->GetCameraSpeed(), 0.0f, 0.0f);
					}
				}

				if (m_inputSystem->KeyDown(DIK_Q))
				{
					// UI code to move the camera farther up
					instance_camera->MoveTransform(0.0f, 0.0f, elapsedTime* instance_camera->GetCameraSpeed());
				}

				if (m_inputSystem->KeyDown(DIK_E))
				{
					// UI code to move the camera farther down
					instance_camera->MoveTransform(0.0f, 0.0f, -elapsedTime* instance_camera->GetCameraSpeed());
				}

				if (m_inputSystem->KeyDown(DIK_D))
				{
					// UI code to move the camera farther right
					instance_camera->MoveTransform(0.0f, -elapsedTime* instance_camera->GetCameraSpeed(), 0.0f);
				}

				if (m_inputSystem->KeyDown(DIK_A))
				{
					// UI code to move the camera farther left
					instance_camera->MoveTransform(0.0f, elapsedTime* instance_camera->GetCameraSpeed(), 0.0f);
				}
			}
			if (m_lMouseDown || m_rMouseDown)
			{
				if (dx != 0 || dy != 0)
					m_calculateDistance = CTrue;


				if (!m_selectObject)
				{
					//perspective viewport
					if (g_camera->m_activatePerspectiveCamera)
					{
						if (m_lMouseDown)
						{
							instance_camera->SetPanAndTilt(-(CFloat)dx * 0.2f, -(CFloat)dy * 0.2f);
							if (dx != 0 || dy != 0)
								m_tempMovement = CTrue;
						}
						else if (m_rMouseDown)
						{
							if (dy > 0)
								instance_camera->m_abstractCamera->SetZoomOut(elapsedTime * 50.0f);
							else if (dy < 0)
								instance_camera->m_abstractCamera->SetZoomIn(elapsedTime * 50.0f);

						}
					}
				}
			}
		}
	}
	if (g_multipleView->IsPlayGameMode() && (g_currentCameraType == eCAMERA_COLLADA || g_currentCameraType == eCAMERA_ENGINE))
	{
		ApplyForce(IDLE, elapsedTime);
		EnableIdleAnimations(); //for now I enable idle animations for main character everytime PhysX camera is not active
	}

}

CVoid CMultipleWindows::EnableIdleAnimations()
{
	if (g_mainCharacter)
	{
		CBool idle = CFalse;
		if (!g_multipleView->m_nx->gJump)
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
				g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_walkSound->GetSoundSource()));
				g_multipleView->m_soundSystem->StopALSound(*(g_mainCharacter->m_runSound->GetSoundSource()));
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

CVoid CMultipleWindows::InitObjectSelection()
{
	GLint viewport[4];
	glSelectBuffer(MAX_NAME_SIZE, Buffer);
	(void)glRenderMode(GL_SELECT);
	g_render.ProjectionMatrix();
	g_render.PushMatrix();
	g_render.IdentityMatrix();
	glGetIntegerv(GL_VIEWPORT, viewport);

	if (g_menu.m_justPerspective)
	{
		gluPickMatrix((CDouble)m_mousePosition.x - 3.0, (CDouble)m_height - (CDouble)m_mousePosition.y - 3.0, 6.0, 6.0, viewport);
	}
	else
	{
		gluPickMatrix((CInt)m_mousePosition.x - 5, m_height - (CInt)m_mousePosition.y - 5, 10.0, 10.0, viewport);
	}
	if (g_currentCameraType == eCAMERA_COLLADA)
	{
		CFloat m_fov, m_zNear, m_zFar;

		if (g_cameraProperties.m_readDAECameraFOVFromFile)
			m_fov = g_render.GetActiveInstanceCamera()->m_abstractCamera->GetYFov();
		else
			m_fov = g_cameraProperties.m_daeCameraFOV;

		if (g_cameraProperties.m_readDAECameraNCPFromFile)
			m_zNear = g_render.GetActiveInstanceCamera()->m_abstractCamera->GetZNear();
		else
			m_zNear = g_cameraProperties.m_daeCameraNCP;

		if (g_cameraProperties.m_readDAECameraFCPFromFile)
			m_zFar = g_render.GetActiveInstanceCamera()->m_abstractCamera->GetZFar();
		else
			m_zFar = g_cameraProperties.m_daeCameraFCP;

		gluPerspective(m_fov, CDouble(m_width) / CDouble(m_height), m_zNear, m_zFar);
	}
	else if (g_currentCameraType == eCAMERA_DEFAULT_FREE_NO_PHYSX)
	{
		gluPerspective(g_render.GetDefaultInstanceCamera()->m_abstractCamera->GetAngle(), CDouble(m_width) / CDouble(m_height), g_cameraProperties.m_freePerspectiveNCP, g_cameraProperties.m_freePerspectiveFCP);
	}
	else
	{
		gluPerspective(g_camera->m_cameraManager->GetAngle(), CDouble(m_width) / CDouble(m_height), g_cameraProperties.m_playModePerspectiveNCP, g_cameraProperties.m_playModePerspectiveFCP);
	}
	g_render.ModelViewMatrix();
	glInitNames();
}

CVoid CMultipleWindows::FinishObjectSelection()
{
	g_render.ProjectionMatrix();
	g_render.PopMatrix();
	g_render.ModelViewMatrix();
	glFlush();
	GLint hits = glRenderMode(GL_RENDER);

	CScene* scene = NULL;
	if (g_currentTransformType == eCTranslate)
	{
		scene = g_translateArrow;
	}
	else if (g_currentTransformType == eCRotate)
	{
		scene = g_rotateArrow;
	}
	else if (g_currentTransformType == eCScale)
	{
		scene = g_scaleArrow;
	}

	if (hits > 0)
	{
		ptr = Buffer;
		for (CInt j = 0; j < hits; j++)
		{
			ptr++; //skip the first info ( number of names which is one in my engine )and move to the min depth info


			CBool foundTarget = CFalse;
			for (CUInt i = 0; i < scene->m_instanceGeometries.size(); i++)
			{
				if (scene->m_instanceGeometries[i]->m_nameIndex == *(ptr + 2))
				{
					selectedName = *(ptr + 2);
					foundTarget = CTrue;
					break;
				}
			}
			if (!foundTarget)
			{
				if (*ptr < minZ)
				{
					minZ = *ptr;

					selectedName = *(ptr + 2); //name of the object
				}
			}
			else
				break;
			ptr += 3; //move to the next hit
		}
	}
}

//Returns -1 if no selection occurs
CUInt CMultipleWindows::GetSelectedObject(CBool renderArrowOnly)
{
	minZ = 0xffffffff;
	selectedName = -1;
	glPushAttrib(GL_VIEWPORT_BIT);
	if (g_menu.m_justPerspective)
	{
		glViewport(0, 0, m_width, m_height);// resets the viewport to new dimensions.
	}
	else
	{
		glViewport(0, 0, m_width / 2, m_height / 2);// resets the viewport to new dimensions.
	}

	if (!renderArrowOnly)
	{
		if (g_menu.m_insertAndShowSky)
		{
			InitObjectSelection();
			g_skyDome->RenderIcon(CTrue);
			FinishObjectSelection();
		}

		InitObjectSelection();
		//Render light icons after the scene rendering
		//Engine lights
		for (CUInt i = 0; i < g_engineLights.size(); i++)
		{
			g_engineLights[i]->RenderIcon(CTrue);
		}
		//COLLADA Lights. Not recommended
		//for( CUInt i = 0 ; i < g_scene.size(); i++ )
		//{
		//	g_render.SetScene( g_scene[i] );
		//	
		//	//set all the lights here
		//	for( CUInt j = 0; j < g_render.GetScene()->GetLightInstanceCount(); j++ )
		//	{
		//		CInstanceLight *instanceLight = g_render.GetScene()->GetLightInstances(j);
		//		if( instanceLight )
		//			instanceLight->RenderIcon(CTrue);
		//	}
		//}
		FinishObjectSelection();

		InitObjectSelection();
		if (g_menu.m_showSoundIcons)
		{
			for (CUInt i = 0; i < g_engineStaticSounds.size(); i++)
			{
				g_engineStaticSounds[i]->RenderIcon(CTrue);
			}
		}
		FinishObjectSelection();

		InitObjectSelection();
		if (g_menu.m_showCameraIcons)
		{
			for (CUInt i = 0; i < g_engineCameraInstances.size(); i++)
			{
				g_engineCameraInstances[i]->RenderIcon(CTrue);
			}
		}
		FinishObjectSelection();

		if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
		{
			for (CUInt i = 0; i < g_engineWaters.size(); i++)
			{
				InitObjectSelection();
				g_engineWaters[i]->RenderIcon(CTrue); //render in selection mode
				FinishObjectSelection();
			}
		}

		if (g_editorMode == eMODE_PREFAB)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				if (!g_scene[i]->m_isVisible) continue;
				InitObjectSelection();
				//g_scene[i]->RenderAABBWithQuads(); //Here's the code to "draw" the objects in selection mode
				g_render.SetScene(g_scene[i]);
				//g_render.GetScene()->Update();
				g_render.GetScene()->RenderSelectionMode();
				FinishObjectSelection();
			}
		}
		else
		{
			g_render.m_useShader = CFalse;
			for (CUInt i = 0; i < g_instancePrefab.size(); i++)
			{
				if (!g_instancePrefab[i]->GetVisible()) continue;
				g_currentInstancePrefab = g_instancePrefab[i];
				CScene* scene = NULL;

				CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
				scene = g_instancePrefab[i]->GetScene(0);

				if (!scene) continue;
				InitObjectSelection();
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
				FinishObjectSelection();
			}
			if (g_render.UsingShader())
				g_render.m_useShader = CTrue;
		}
	}
	else if (g_showArrow)
	{
		CScene* scene = NULL;
		if (g_currentTransformType == eCTranslate)
		{
			scene = g_translateArrow;
		}
		else if (g_currentTransformType == eCRotate)
		{
			scene = g_rotateArrow;
		}
		else if (g_currentTransformType == eCScale)
		{
			scene = g_scaleArrow;
		}
		g_render.SetScene(scene);
		CFloat distance = sqrt(pow(g_camera->m_perspectiveCameraPos.x - g_arrowPosition.x, 2) +
			pow(g_camera->m_perspectiveCameraPos.y - g_arrowPosition.y, 2) +
			pow(g_camera->m_perspectiveCameraPos.z - g_arrowPosition.z, 2));
		if (distance < 10.0f)
			distance = 10.0f;
		InitObjectSelection();
		//glDisable(GL_DEPTH_TEST);
		glPushMatrix();
		glTranslatef(g_arrowPosition.x, g_arrowPosition.y, g_arrowPosition.z);
		glScalef(distance * 0.15f, distance * 0.15f, distance * 0.15f);
		scene->RenderSelectionMode();
		glPopMatrix();
		//glEnable(GL_DEPTH_TEST);
		FinishObjectSelection();
	}

	glPopAttrib();

	if(!renderArrowOnly)
		m_tempSelectedName = (CUInt)selectedName;
	return selectedName;
}

CVoid CMultipleWindows::InitPrefabSelection(CDouble mouseXPos, CDouble mouseYPos, CDouble selectionWidth, CDouble selectionHeight)
{
	GLint viewport[4];
	glSelectBuffer(MAX_NAME_SIZE, Buffer);
	(void)glRenderMode(GL_SELECT);
	g_render.ProjectionMatrix();
	g_render.PushMatrix();
	g_render.IdentityMatrix();
	glGetIntegerv(GL_VIEWPORT, viewport);

	if (g_menu.m_justPerspective)
	{
		gluPickMatrix(mouseXPos - (selectionWidth / 2.0), mouseYPos - (selectionHeight / 2.0), selectionWidth, selectionHeight, viewport);
	}
	else
	{
		gluPickMatrix(mouseXPos / 2.0 - (selectionWidth / 4.0),  mouseYPos / 2.0 - (selectionHeight / 4.0), selectionWidth / 2.0, selectionHeight / 2.0, viewport);
	}

	if (g_currentCameraType == eCAMERA_PHYSX)
	{
		gluPerspective(g_camera->m_cameraManager->GetAngle(), CDouble(m_width) / CDouble(m_height), g_cameraProperties.m_playModePerspectiveNCP, g_cameraProperties.m_playModePerspectiveFCP);
	}
	g_render.ModelViewMatrix();
	glInitNames();
}

CVoid CMultipleWindows::FinishPrefabSelection()
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

CChar* CMultipleWindows::SelectPrefabInstances(CDouble mouseXPos, CDouble mouseYPos, CDouble selectionWidth, CDouble selectionHeight)
{
	minZ = 0xffffffff;
	m_selectedPrefabName = -1;
	glPushAttrib(GL_VIEWPORT_BIT);
	if (g_menu.m_justPerspective)
	{
		glViewport(0, 0, m_width, m_height);// resets the viewport to new dimensions.
	}
	else
	{
		glViewport(0, 0, m_width / 2, m_height / 2);// resets the viewport to new dimensions.
	}
	if (g_editorMode == eMODE_PREFAB)
	{
		if (g_prefabProperties.m_isSelectable)
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				if (!g_scene[i]->m_isVisible) continue;

				if (CmpIn(g_scene[i]->GetName(), "_COL"))
					continue;

				if (m_bSelectionDistance)
				{
					CFloat distanceFromCamera = 1000000000.f; //infinity
					for (CUInt j = 0; j < g_scene[i]->m_instanceGeometries.size(); j++)
					{
						if (g_scene[i]->m_instanceGeometries[j]->m_distanceFromCamera < distanceFromCamera)
							distanceFromCamera = g_scene[i]->m_instanceGeometries[j]->m_distanceFromCamera;
					}

					if (distanceFromCamera > m_fSelectionDistance)
						continue;
				}
				CBool selectionType = g_menu.m_geometryBasedSelection;
				g_menu.m_geometryBasedSelection = CTrue; // only use geometry based selection in play mode

				InitPrefabSelection(mouseXPos, mouseYPos, selectionWidth, selectionHeight);
				//g_scene[i]->RenderAABBWithQuads(); //Here's the code to "draw" the objects in selection mode
				g_render.SetScene(g_scene[i]);
				//g_render.GetScene()->Update();
				g_render.GetScene()->RenderSelectionMode();
				FinishPrefabSelection();

				g_menu.m_geometryBasedSelection = selectionType;
			}
		}
	}
	else if (g_editorMode == eMODE_VSCENE)
	{
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

	}

	glPopAttrib();

	Cpy(m_selectedPrefabInstanceName, ""); //no name by default

	//run OnSelect function
	if (g_editorMode == eMODE_PREFAB)
	{
		if (m_selectedPrefabName != -1 && g_prefabProperties.m_hasScript)
		{
			lua_getglobal(g_lua, "OnSelect");
			if (lua_isfunction(g_lua, -1))
			{
				lua_pcall(g_lua, 0, 0, 0);
			}
			lua_settop(g_lua, 0);
		}
	}
	else if (g_editorMode == eMODE_VSCENE)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (m_selectedPrefabName != -1 && g_instancePrefab[i]->GetNameIndex() == m_selectedPrefabName)
			{
				g_instancePrefab[i]->OnSelectScript();
				Cpy(m_selectedPrefabInstanceName, g_instancePrefab[i]->GetName());
				break;
			}
		}
	}
	return m_selectedPrefabInstanceName;
}

CBool CMultipleWindows::InitFBOs( CInt channels, CInt type )
{
	//glGetIntegerv( GL_MAX_DRAW_BUFFERS, &g_numBuffers );

	CUInt numSamples;
	GLenum status;
	if( m_multiSample ) //initialize 2 separate FBOs
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
		g_render.AttachRenderBufferToFBODepth( m_rbDepthID );

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
				PrintInfo("\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
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
		g_render.AttachRenderBufferToFBODepth( m_rbDepthMID2 );

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
				PrintInfo("\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
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
	g_render.AttachRenderBufferToFBODepth( m_rbDepthID2 );

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
	GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT};
	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo( "\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
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
	g_render.AttachRenderBufferToFBODepth( m_rbDepthIDSwapLights );

	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo( "\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
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
			PrintInfo( "\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
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
	g_render.AttachRenderBufferToFBODepth( m_rbDepthIDFogDof );
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo( "\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
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
	g_render.AttachRenderBufferToFBODepth( m_rbMDepthIDFogDof );

	glDrawBuffers(eGBUFFER_NUM_TEXTURES, DrawBuffers);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		default:
			PrintInfo("\nCMain::InitFBo(): An error occured while creating the FBO", COLOR_RED );
			break;
	}

	//g_render.BindFBO(0);
	g_render.BindFBO( 0 );
	g_render.BindRenderBuffer(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	return CTrue;
}

CVoid CMultipleWindows::SetInstanceCamera(CInstanceCamera * inst, CFloat sWidth, CFloat sHeight, CFloat fov, CFloat zNear, CFloat zFar)
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

CVoid CMultipleWindows::UpdateDynamicPhysicsObjects()
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
		if (g_editorMode == eMODE_VSCENE)
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
		else
		{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				g_render.SetScene(g_scene[i]);

				if (g_render.GetScene()->m_update)
				{
					g_render.GetScene()->Update();
					g_render.GetScene()->m_update = CFalse;
				}
				g_render.GetScene()->UpdateDynamicPhysicsObjects();
			}
		}
	}
}

CVoid CMultipleWindows::UpdateAnimations(CBool init)
{
	if (g_editorMode == eMODE_VSCENE)
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
							if ((m_enableTimer && g_render.GetScene()->GetAnimationStatus() == eANIM_PLAY && g_render.GetScene()->UpdateAnimationLists()) || (m_enableTimer && g_render.GetScene()->IsTransformable()))
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
	else
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			if (!g_scene[i]->m_isVisible) continue;
			g_render.SetScene(g_scene[i]);
			if (!g_render.GetScene()->m_isTrigger)
			{
				if (g_render.GetScene()->m_hasAnimation && g_render.GetScene()->m_updateAnimation)
				{
					if (m_enableTimer && g_render.GetScene()->GetAnimationStatus() == eANIM_PLAY && g_render.GetScene()->UpdateAnimationLists())
					{
						g_render.GetScene()->Update(elapsedTime);
						g_render.GetScene()->m_updateAnimation = CFalse;
						g_render.GetScene()->m_update = CFalse;
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
		} //for

	}
}

CVoid CMultipleWindows::Render3DModelsControlledByPhysX(CBool sceneManager)
{
	//3D Model data
	if (g_editorMode == eMODE_VSCENE)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (!g_instancePrefab[i]->GetVisible()) continue;
			if (!g_instancePrefab[i]->GetIsControlledByPhysX()) continue;

			g_currentInstancePrefab = g_instancePrefab[i];

			//if (!g_currentInstancePrefab->GetRenderForQuery())
			//	continue;

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
			if (g_editorMode == eMODE_VSCENE && !g_multipleView->IsPlayGameMode())
				g_instancePrefab[i]->UpdateArrow();
		}
	}
	else
	{
		//if (g_multipleView->IsPlayGameMode())
		//{
			for (CUInt i = 0; i < g_scene.size(); i++)
			{
				if (!g_scene[i]->m_isVisible) continue;
				g_render.SetScene(g_scene[i]);

				if (!g_render.GetScene()->m_isTrigger)
				{
					if (g_render.GetScene()->m_update)
					{
						g_render.GetScene()->Update();
						g_render.GetScene()->m_update = CFalse;
					}
					g_render.GetScene()->RenderModelsControlledByPhysX(sceneManager);

					if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
					{
						glUseProgram(0);
					}
				}
			}
		//}
	}

}

CVoid CMultipleWindows::Render3DModelsControlledByPhysXForWater(CWater* water, CBool sceneManager)
{
	//3D Model data
	if (g_editorMode == eMODE_VSCENE)
	{
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
			water->m_instancePrefab[i]->UpdateArrow();
		}
	}

}

CVoid CMultipleWindows::RenderCharacter(CBool sceneManager)
{
	if (g_mainCharacter && g_mainCharacter->GetCameraType() == ePHYSX_CAMERA_FIRST_PERSON && g_multipleView->IsPlayGameMode()) return;

	if (!g_menu.m_insertCharacter)
		return;
	//3D Model data
	if (g_editorMode == eMODE_VSCENE)
	{
		if (!g_mainCharacter)
			return;
		g_currentInstancePrefab = g_mainCharacter->GetInstancePrefab();
		if (!g_currentInstancePrefab->GetVisible()) return;
		g_currentInstancePrefab->SetLight();

		//if (m_bQuery)
		//{
		//	if (g_instancePrefab[i]->GetResult() == 0)
		//	{
		//		if (g_instancePrefab[i]->GetDistanceFromCamera() - g_instancePrefab[i]->GetRadius() >(g_instancePrefab[i]->GetRadius() + 20.0f))
		//			continue;
		//	}
		//}
		static CFloat lerpFactor = 0.0f;

		g_multipleView->m_nx->gCharacterPos = g_multipleView->m_nx->GetFilteredCharacterPos();
		CFloat y = (CFloat)g_multipleView->m_nx->gCharacterPos.y - (g_physXProperties.m_fCapsuleHeight * 0.5f) - g_physXProperties.m_fCapsuleRadius - g_physXProperties.m_fCharacterSkinWidth;
		CVec3f trans(g_multipleView->m_nx->gCharacterPos.x, y, g_multipleView->m_nx->gCharacterPos.z);

		g_mainCharacter->GetInstancePrefab()->SetTranslate(trans);
		if (g_multipleView->m_nx->m_pushCharacter)
		{
			CFloat y = NxMath::radToDeg(atan2(g_multipleView->m_nx->m_horizontalDisp.x, g_multipleView->m_nx->m_horizontalDisp.z));
			if (y < 0.0f)
				y = 360.0f + y;
			if (y == 360.0f)
				y = 0.0f;
			if (g_multipleView->m_nx->m_previousMoveDirection != g_multipleView->m_nx->m_currentMoveDirection)
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
					if (y - g_mainCharacter->GetCurrentRotation() > 360 + g_mainCharacter->GetCurrentRotation() - y )
						g_mainCharacter->SetCurrentRotation(g_mainCharacter->GetCurrentRotation() + 360.0f);
				}

				g_mainCharacter->SetCurrentRotation(Lerp(g_mainCharacter->GetCurrentRotation(), y, lerpFactor));
				lerpFactor += 2.0 * elapsedTime; //total: 0.5
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
			//	if (m_enableTimer && g_render.GetScene()->GetAnimationStatus() == eANIM_PLAY  && g_render.GetScene()->UpdateAnimationLists())
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
		g_currentInstancePrefab->UpdateArrow();
	}

}

CVoid CMultipleWindows::Render3DAnimatedModels(CBool sceneManager)
{
	//3D Model data
	if (g_editorMode == eMODE_VSCENE)
	{
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
			g_instancePrefab[i]->UpdateArrow();
		}
	}
	else
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			if (!g_scene[i]->m_isVisible) continue;
			g_render.SetScene(g_scene[i]);

			if (!g_render.GetScene()->m_isTrigger)
			{
				if (g_render.GetScene()->m_hasAnimation && g_render.GetScene()->m_updateAnimation)
				{
					if (m_enableTimer && g_render.GetScene()->GetAnimationStatus() == eANIM_PLAY  && g_render.GetScene()->UpdateAnimationLists())
					{
						g_render.GetScene()->Update(elapsedTime);
						g_render.GetScene()->m_updateAnimation = CFalse;
						g_render.GetScene()->m_update = CFalse;
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

				g_render.GetScene()->RenderAnimatedModels(sceneManager, CTrue);
				g_render.GetScene()->RenderAnimatedModels(sceneManager, CFalse);

				if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
				{
					glUseProgram(0);
				}

			}
		}

	}

}

CVoid CMultipleWindows::Render3DAnimatedModelsForWater(CWater* water, CBool sceneManager)
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
					if (m_enableTimer && g_render.GetScene()->GetAnimationStatus() == eANIM_PLAY  && g_render.GetScene()->UpdateAnimationLists())
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
		water->m_instancePrefab[i]->UpdateArrow();
	}

}

CVoid CMultipleWindows::RenderTerrain()
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
		else if (g_materialChannels == eCHANNELS_NORMALMAP)
			g_shaderType = g_render.m_terrainNormalMapLayerProgram;
		else if (g_materialChannels == eCHANNELS_DIFFUSE)
			g_shaderType = g_render.m_terrainDiffuseLayerProgram;
		else if (g_materialChannels == eCHANNELS_POSITION)
			g_shaderType = g_render.m_shaderPositionLayerProgram;
		else if (g_materialChannels == eCHANNELS_NORMAL)
			g_shaderType = g_render.m_shaderNormalLayerProgram;
		else if (g_materialChannels != eCHANNELS_ALL)
			g_shaderType = g_render.m_terrainOtherLayerProgram;

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
			glUniform1f(glGetUniformLocation(g_shaderType, "focalDistance"), g_multipleView->m_dof.m_focalDistance);
			glUniform1f(glGetUniformLocation(g_shaderType, "focalRange"), g_multipleView->m_dof.m_focalRange);
			CBool useFog;
			if (g_polygonMode != ePOLYGON_FILL || (g_dofProperties.m_enable && g_dofProperties.m_debug) || (g_shadowProperties.m_shadowType == eSHADOW_SINGLE_HL && g_shadowProperties.m_enable && g_render.UsingShadowShader()))
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
			glUniform4fv(glGetUniformLocation(g_shaderType, "far_d"), 1, g_multipleView->far_bound);
			glUniform2f(glGetUniformLocation(g_shaderType, "texSize"), (float)g_multipleView->m_dynamicShadowMap->depth_size, 1.0f / (float)g_multipleView->m_dynamicShadowMap->depth_size);
			glUniform1f(glGetUniformLocation(g_shaderType, "shadow_intensity"), g_shadowProperties.m_intensity);
			glUniformMatrix4fv(glGetUniformLocation(g_shaderType, "camera_inverse_matrix"), 1, CFalse, g_multipleView->cam_inverse_modelview);
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

CVoid CMultipleWindows::RenderQueries(CBool init)
{
	if (m_bQuery || init)
	{
		COpenGLUtility m_glUtil;
		m_renderQuery = CTrue;
		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		if (!g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
			g_render.BindForWriting(m_mFboID);
		else if (!g_useOldRenderingStyle && g_options.m_enableFBO)
			g_render.BindForWriting(m_fboID);
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
					m_glUtil.DrawSquare(g_engineWaters[j]->m_sidePoint[0], g_engineWaters[j]->m_sidePoint[1], g_engineWaters[j]->m_sidePoint[2], g_engineWaters[j]->m_sidePoint[3]);
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

		//render engine lights here
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
				//m_glUtil.DrawSolidSphere(instanceLight->GetRadius(), 10, 10, pos, color);
				//glEndQuery(GL_SAMPLES_PASSED);
			}
		}
		glEnable(GL_CULL_FACE);
		glPopAttrib();
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		m_renderQuery = CFalse;
		g_render.BindFBO(0);

		//reset render query;
	}
}

CVoid CMultipleWindows::RenderBakedOctree3DModels()
{
	//3D Model data
	if (g_editorMode == eMODE_VSCENE)
	{
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

			if (g_instancePrefab[i]->GetNameIndex() == g_selectedName)
			{
				CVector lineColor(1.0, 1.0, 1.0);
				CVector min(g_instancePrefab[i]->GetMinAABB().x, g_instancePrefab[i]->GetMinAABB().y, g_instancePrefab[i]->GetMinAABB().z);
				CVector max(g_instancePrefab[i]->GetMaxAABB().x, g_instancePrefab[i]->GetMaxAABB().y, g_instancePrefab[i]->GetMaxAABB().z);
				g_glUtil.DrawCWBoxWithLines(min, max, lineColor);
			}
			g_instancePrefab[i]->UpdateArrow();

		}
	}
}

CVoid CMultipleWindows::Render3DModels(CBool sceneManager, CChar* parentTreeNameOfGeometries, CBool checkVisibility, CBool drawGeometry)
{
	//3D Model data
	if (g_editorMode == eMODE_VSCENE)
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (!g_instancePrefab[i]->GetVisible() ) continue;
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
			//		if (g_instancePrefab[i]->GetDistanceFromCamera() - g_instancePrefab[i]->GetRadius() >(g_instancePrefab[i]->GetRadius() + 20.0f))
			//			continue;
			//	}
			//}

			CScene* scene = NULL;

			CPrefab* prefab = g_instancePrefab[i]->GetPrefab();
			for (CUInt j = 0; j < 3; j++)
			{
				if (prefab && prefab->GetHasLod(j))
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
				g_render.GetScene()->Render(sceneManager, parentTreeNameOfGeometries, CFalse, checkVisibility, drawGeometry);

				if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
				{
					glUseProgram(0);
				}

			}

			if (g_instancePrefab[i]->GetNameIndex() == g_selectedName)
			{
				CVector lineColor(1.0, 1.0, 1.0);
				CVector min(g_instancePrefab[i]->GetMinAABB().x, g_instancePrefab[i]->GetMinAABB().y, g_instancePrefab[i]->GetMinAABB().z);
				CVector max(g_instancePrefab[i]->GetMaxAABB().x, g_instancePrefab[i]->GetMaxAABB().y, g_instancePrefab[i]->GetMaxAABB().z);
				g_glUtil.DrawCWBoxWithLines(min, max, lineColor);
			}
			g_instancePrefab[i]->UpdateArrow();

		}
	}
	else
	{
		for (CUInt i = 0; i < g_scene.size(); i++)
		{
			if (!g_scene[i]->m_isVisible) continue;
			g_render.SetScene(g_scene[i]);

			if (!g_render.GetScene()->m_isTrigger)
			{
				if (g_render.GetScene()->m_update)
				{
					g_render.GetScene()->Update();
					g_render.GetScene()->m_update = CFalse;
				}
				g_render.GetScene()->Render(sceneManager, parentTreeNameOfGeometries, CFalse, checkVisibility);

				if (g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader)
				{
					glUseProgram(0);
				}

			}
		}
	}
}

CVoid CMultipleWindows::Render3DModelsForWater(CWater* water, CBool sceneManager, CChar* parentTreeNameOfGeometries)
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

		}

		if (water->m_instancePrefab[i]->GetNameIndex() == g_selectedName)
		{
			CVector lineColor(1.0, 1.0, 1.0);
			CVector min(water->m_instancePrefab[i]->GetMinAABB().x, water->m_instancePrefab[i]->GetMinAABB().y, water->m_instancePrefab[i]->GetMinAABB().z);
			CVector max(water->m_instancePrefab[i]->GetMaxAABB().x, water->m_instancePrefab[i]->GetMaxAABB().y, water->m_instancePrefab[i]->GetMaxAABB().z);
			g_glUtil.DrawCWBoxWithLines(min, max, lineColor);
		}
		water->m_instancePrefab[i]->UpdateArrow();

	}

}

CVoid CMultipleWindows::DrawGUI()
{
	m_timerCounter++;
	m_simpleFont.StartRendering();

	CChar layerMode[MAX_NAME_SIZE];
	CChar polygonMode[MAX_NAME_SIZE];
	if (g_materialChannels == eCHANNELS_DIFFUSE)
		Cpy(layerMode, "Layer Mode: Diffuse");
	else if (g_materialChannels == eCHANNELS_NORMALMAP)
		Cpy(layerMode, "Layer Mode: Normal Map");
	else if (g_materialChannels == eCHANNELS_DIRTMAP)
		Cpy(layerMode, "Layer Mode: Dirt Map");
	else if (g_materialChannels == eCHANNELS_GLOSSMAP)
		Cpy(layerMode, "Layer Mode: Gloss Map");
	else if (g_materialChannels == eCHANNELS_ALPHAMAP)
		Cpy(layerMode, "Layer Mode: Alpha Map");
	else if (g_materialChannels == eCHANNELS_POSITION)
		Cpy(layerMode, "Layer Mode: Positions");
	else if (g_materialChannels == eCHANNELS_NORMAL)
		Cpy(layerMode, "Layer Mode: Normals");

	if(g_polygonMode == ePOLYGON_POINT)
		Cpy(polygonMode, "Polygon Mode: Point");
	else if (g_polygonMode == ePOLYGON_LINE)
		Cpy(polygonMode, "Polygon Mode: Line");

	if (g_menu.m_justPerspective)
	{
		if (g_editorMode == eMODE_PREFAB && g_multipleView->IsPlayGameMode())
			m_simpleFont.Print("Perspective : Prefab : Play Mode", 5.0f, 1000.0f, 0.0f, 0.85f, 0.67f, 0.0f);
		else if ( g_editorMode == eMODE_PREFAB)
			m_simpleFont.Print( "Perspective : Prefab", 5.0f, 1000.0f, 0.0f, 0.85f, 0.67f, 0.0f );
		else if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode())
			m_simpleFont.Print("Perspective : VScene : Play Mode", 5.0f, 1000.0f, 0.0f, 0.85f, 0.67f, 0.0f);
		else
			m_simpleFont.Print("Perspective : VScene", 5.0f, 1000.0f, 0.0f, 0.85f, 0.67f, 0.0f);

		if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode() && !g_mainCharacter)
			m_simpleFont.Print("Warning: Please insert the player", 5.0f, 50.0f, 0.0f, 0.85f, 0.1f, 0.1f);

		if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode() && g_totalLights == 0)
			m_simpleFont.Print("Warning: Please insert at least one light", 5.0f, 20.0f, 0.0f, 0.85f, 0.1f, 0.1f);
		else if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode() && g_numLights == 0)
			m_simpleFont.Print("Warning: No visible light", 5.0f, 20.0f, 0.0f, 0.85f, 0.1f, 0.1f);

		CFloat y = 20.f;
		if (g_materialChannels != eCHANNELS_ALL)
		{
			m_simpleFont.Print(layerMode, 660.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
			y += 35.0f;
		}
		if (g_polygonMode != ePOLYGON_FILL)
		{
			m_simpleFont.Print(polygonMode, 660.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
			y += 35.0f;
		}
		if (g_physXProperties.m_bDebugMode)
		{
			m_simpleFont.Print("PhysX Debug Mode: ON", 660.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
			y += 35.0f;
		}
		for (CUInt c = 0; c < g_engineCameraInstances.size(); c++)
		{
			if (g_engineCameraInstances[c]->IsActive())
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "\nCamera ' %s ' : ON", g_engineCameraInstances[c]->m_abstractCamera->GetName());
				m_simpleFont.Print(temp, 660.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
				y += 35.0f;
				break;
			}
		}
		if (!g_menu.m_geometryBasedSelection && g_editorMode == eMODE_PREFAB)
		{
			m_simpleFont.Print("Material Selection: ON", 660.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
			y += 35.0f;
		}


		if (g_menu.m_showStatistics)
		{
			CChar temp[MAX_NAME_SIZE];

			CInt fps = (CInt)timer.GetFps(m_timerCounter);

			sprintf(temp, "FPS : %i", fps);
			m_simpleFont2.Print(temp, 5.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f);
			sprintf( temp, "Rendered Vertexes : %i", g_numVerts );
			m_simpleFont2.Print(temp, 5.0f, 940.0f, 0.0f, 0.85f, 0.67f, 0.0f);
			sprintf( temp, "Rendered Triangles : %i", g_numVerts / 3 );
			m_simpleFont2.Print(temp, 5.0f, 910.0f, 0.0f, 0.85f, 0.67f, 0.0f);
			m_simpleFont2.Print("Object -------------------- Samples", 5.0f, 880.0f, 0.0f, 0.85f, 0.67f, 0.0f);

			CInt index = 0;
			CInt m_numSamples;
			CInt width, height;
			if (g_menu.m_justPerspective)
			{
				width = g_multipleView->m_width;
				height = g_multipleView->m_height;
			}
			else
			{
				width = g_multipleView->m_width / 2;
				height = g_multipleView->m_height / 2;

			}

			if (g_multipleView->m_multiSample)
				m_numSamples = width * height * g_options.m_numSamples;
			else
				m_numSamples = width * height;

			CFloat percentage;

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

				if (index <= 25)
				{
					char temp[MAX_NAME_SIZE];
					sprintf(temp, "%s LOD Percentage: %.2f", g_instancePrefab[i]->GetName(), percentage);
					m_simpleFont2.Print(temp, 5.0f, 880.0f - (index + 1) * 30, 0.0f, 0.85f, 0.67f, 0.0f);
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
				m_simpleFont2.Print(temp, 5.0f, 880.0f - (index + i + 1) * 30, 0.0f, g_engineLights[i]->m_abstractLight->GetDiffuse()[0], g_engineLights[i]->m_abstractLight->GetDiffuse()[1], g_engineLights[i]->m_abstractLight->GetDiffuse()[2]);
			}

			for (CUInt i = 0; i < g_engineWaters.size(); i++)
			{
				if (g_engineWaters[i]->GetOutsideFrustom()) continue;
				if (g_multipleView->IsPlayGameMode())
					if (!g_engineWaters[i]->GetVisible())
						continue;
				percentage = ((CFloat)g_engineWaters[i]->GetResult() / (CFloat)m_numSamples) * 100.0f;

				char temp[200];
				sprintf(temp, "%s LOD Percentage: %.2f", g_engineWaters[i]->GetName(), percentage);
				m_simpleFont2.Print(temp, 5.0f, 880.0f - (index + g_engineLights.size() + i + 1) * 30, 0.0f, 0.65f, 0.5f, 0.65f);
			}

			for (CUInt i = 0; i < g_instancePrefab.size(); i++)
			{
				if (i <= 25)
				{
					char temp[200];
					sprintf(temp, "%s Distance: %.2f", g_instancePrefab[i]->GetName(), g_instancePrefab[i]->GetDistanceFromCamera());
					m_simpleFont2.Print(temp, 250.0, 1030.0 - (i + 1) * 30, 0.0f, 0.65f, 0.5f, 0.65f);
				}
				else
				{
					break;
				}
			}

		}
	}
	else
	{
		if (g_editorMode == eMODE_PREFAB && g_multipleView->IsPlayGameMode())
		{
			m_simpleFont.Print("Perspective : Prefab : Play Mode", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f);
		}
		else if (g_editorMode == eMODE_PREFAB)
		{
			m_simpleFont.Print("Perspective : Prefab", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f);
		}
		else if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode())
		{
			m_simpleFont.Print("Perspective : VScene : Play Mode", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f);
		}
		else
		{
			m_simpleFont.Print("Perspective : VScene", 10.0f, 970.0f, 0.0f, 0.85f, 0.67f, 0.0f);
		}

		if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode() && !g_mainCharacter)
			m_simpleFont2.Print("Warning: Please insert the player", 10.0f, 100.0f, 0.0f, 0.85f, 0.1f, 0.1f);

		if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode() && g_totalLights == 0)
			m_simpleFont2.Print("Warning: Please insert at least one light", 10.0f, 50.0f, 0.0f, 0.85f, 0.1f, 0.1f);
		else if (g_editorMode == eMODE_VSCENE && g_multipleView->IsPlayGameMode() && g_numLights == 0)
			m_simpleFont2.Print("Warning: No visible light", 10.0f, 50.0f, 0.0f, 0.85f, 0.1f, 0.1f);

		CFloat y = 50.0f;
		if (g_materialChannels != eCHANNELS_ALL)
		{
			m_simpleFont2.Print(layerMode, 575.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
			y += 50.0f;
		}
		if (g_polygonMode != ePOLYGON_FILL)
		{
			m_simpleFont2.Print(polygonMode, 575.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
			y += 50.0f;
		}
		if (g_physXProperties.m_bDebugMode)
		{
			m_simpleFont2.Print("PhysX Debug Mode: ON", 575.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
			y += 50.0f;
		}
		for (CUInt c = 0; c < g_engineCameraInstances.size(); c++)
		{
			if (g_engineCameraInstances[c]->IsActive())
			{
				CChar temp[MAX_NAME_SIZE];
				sprintf(temp, "\nCamera ' %s ' : ON", g_engineCameraInstances[c]->m_abstractCamera->GetName());
				m_simpleFont2.Print(temp, 575.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
				y += 50.0f;
				break;
			}
		}
		if (!g_menu.m_geometryBasedSelection && g_editorMode == eMODE_PREFAB)
		{
			m_simpleFont2.Print("Material Selection: ON", 575.0f, y, 0.0f, 0.75f, 0.75f, 0.3f);
			y += 50.0f;
		}

		if( g_menu.m_showStatistics )
		{
			CChar temp[MAX_NAME_SIZE];
			CInt fps = (CInt)(timer.GetFps(m_timerCounter));
			sprintf( temp, "FPS : %i", fps );
			m_simpleFont2.Print( temp, 10.0f, 920.0f, 0.0f, 0.85f, 0.67f, 0.0f );
			sprintf( temp, "Rendered Vertexes : %i", g_numVerts );
			m_simpleFont2.Print( temp, 10.0f, 870.0f, 0.0f, 0.85f, 0.67f, 0.0f );
			sprintf( temp, "Rendered Triangles : %i", g_numVerts / 3 );
			m_simpleFont2.Print( temp, 10.0f, 820.0f, 0.0f, 0.85f, 0.67f, 0.0f );
		}
	}
	m_simpleFont.EndRendering();

	if (g_transformObject && g_currentTransformType == eCTranslate)
	{
		CFloat val;
		CChar temp1[MAX_NAME_SIZE];
		val = roundf(g_arrowPosition.x * 100) / 100;
		sprintf(temp1, "%.2f", val);
		ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(temp1);

		CChar temp2[MAX_NAME_SIZE];
		val = roundf(g_arrowPosition.y * 100) / 100;
		sprintf(temp2, "%.2f", val);
		ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(temp2);

		CChar temp3[MAX_NAME_SIZE];
		val = roundf(g_arrowPosition.z * 100) / 100;
		sprintf(temp3, "%.2f", val);
		ex_pVandaEngine1Dlg->m_editZ.SetWindowTextA(temp3);
	}
	else if (g_transformObject && (g_currentTransformType == eCRotate || g_currentTransformType == eCScale))
	{
		for (CUInt i = 0; i < g_instancePrefab.size(); i++)
		{
			if (g_instancePrefab[i]->GetNameIndex() == g_selectedName)
			{
				if (g_currentTransformType == eCRotate)
				{
					CInstancePrefab* instancePrefab = g_instancePrefab[i];
					CFloat val;
					CChar temp1[MAX_NAME_SIZE];
					CFloat rotate;
					CVec4f rot;

					rotate = instancePrefab->GetRotate().x;
					if (rotate >= 360.0f)
						rotate -= 360.0f;
					else if (rotate < 0)
						rotate = 360 + rotate;
					rot.x = rotate; rot.y = instancePrefab->GetRotate().y; rot.z = instancePrefab->GetRotate().z; rot.w = instancePrefab->GetRotate().w;
					instancePrefab->SetRotate(rot);

					val = roundf(instancePrefab->GetRotate().x * 100) / 100;
					sprintf(temp1, "%.2f", val);
					ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(temp1);

					rotate = instancePrefab->GetRotate().y;
					if (rotate >= 360.0f)
						rotate -= 360.0f;
					else if (rotate < 0)
						rotate = 360 + rotate;
					rot.x = instancePrefab->GetRotate().x; rot.y = rotate; rot.z = instancePrefab->GetRotate().z; rot.w = instancePrefab->GetRotate().w;
					instancePrefab->SetRotate(rot);

					CChar temp2[MAX_NAME_SIZE];
					val = roundf(instancePrefab->GetRotate().y * 100) / 100;
					sprintf(temp2, "%.2f", val);
					ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(temp2);

					if (g_mainCharacter && Cmp(g_mainCharacter->GetInstancePrefab()->GetName(), g_instancePrefab[i]->GetName()))
					{
						g_mainCharacter->SetCurrentRotation(val);
						CVec4f rot(0.0f, g_mainCharacter->GetCurrentRotation(), 0.0f, 0.0f);
						g_mainCharacter->GetInstancePrefab()->SetRotate(rot);

						g_camera->m_perspectiveCameraYaw = NxMath::degToRad(val) + NxMath::degToRad(180.f);

					}


					rotate = instancePrefab->GetRotate().z;
					if (rotate >= 360.0f)
						rotate -= 360.0f;
					else if (rotate < 0)
						rotate = 360 + rotate;
					rot.x = instancePrefab->GetRotate().x; rot.y = instancePrefab->GetRotate().y; rot.z = rotate; rot.w = instancePrefab->GetRotate().w;
					instancePrefab->SetRotate(rot);

					CChar temp3[MAX_NAME_SIZE];
					val = roundf(instancePrefab->GetRotate().z * 100) / 100;
					sprintf(temp3, "%.2f", val);
					ex_pVandaEngine1Dlg->m_editZ.SetWindowTextA(temp3);
					break;
				}
				else if (g_currentTransformType == eCScale)
				{
					CFloat val;
					CChar temp1[MAX_NAME_SIZE];
					val = roundf(g_instancePrefab[i]->GetScale().x * 100) / 100;
					sprintf(temp1, "%.2f", val);
					ex_pVandaEngine1Dlg->m_editX.SetWindowTextA(temp1);

					CChar temp2[MAX_NAME_SIZE];
					val = roundf(g_instancePrefab[i]->GetScale().y * 100) / 100;
					sprintf(temp2, "%.2f", val);
					ex_pVandaEngine1Dlg->m_editY.SetWindowTextA(temp2);

					CChar temp3[MAX_NAME_SIZE];
					val = roundf(g_instancePrefab[i]->GetScale().z * 100) / 100;
					sprintf(temp3, "%.2f", val);
					ex_pVandaEngine1Dlg->m_editZ.SetWindowTextA(temp3);
					break;
				}

			}
		}
	}
	if( m_totalElapsedTime >= 5 ) //Every 5 seconds update the FPS
	{
		SetElapsedTimeFromBeginning();
	}

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

	CBool renderCursor = CTrue;
	if (!g_menu.m_justPerspective)
	{
		if ((g_multipleView->m_mousePosition.x > g_width / 2.0) || (g_multipleView->m_mousePosition.y < g_height / 2.0))
			renderCursor = CFalse;
	}

	//Draw Cursor
	if (g_multipleView->IsPlayGameMode() && g_currentVSceneProperties.m_isMenu && !g_multipleView->GetCursorIcon()->GetVisible() && renderCursor)
	{

		CFloat cursorSize;
		if (g_editorMode == eMODE_GUI || (g_editorMode == eMODE_VSCENE && g_menu.m_justPerspective))
			cursorSize = ((CFloat)g_currentVSceneProperties.m_cursorSize * (CFloat)g_multipleView->m_width) / 100.0f;
		else if (g_editorMode == eMODE_VSCENE && !g_menu.m_justPerspective)
			cursorSize = ((CFloat)g_currentVSceneProperties.m_cursorSize * 0.5f * (CFloat)g_multipleView->m_width) / 100.0f;

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
		glVertex2f(g_multipleView->m_mousePosition.x - halfCursorSize, g_height - g_multipleView->m_mousePosition.y + halfCursorSize - cursorSize);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(g_multipleView->m_mousePosition.x - halfCursorSize + cursorSize, g_height - g_multipleView->m_mousePosition.y + halfCursorSize - cursorSize);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(g_multipleView->m_mousePosition.x - halfCursorSize + cursorSize, g_height - g_multipleView->m_mousePosition.y + halfCursorSize);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(g_multipleView->m_mousePosition.x - halfCursorSize, g_height - g_multipleView->m_mousePosition.y + halfCursorSize);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		g_render.ProjectionMatrix();
		g_render.PopMatrix();

		g_render.ModelViewMatrix();
		g_render.PopMatrix();
		glPopAttrib(); //viewport
	}

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(&p);
	CVec2f pos(p.x, g_height - p.y);
	m_cursorIcon->Render(pos);
}

CVoid CMultipleWindows::BlendFogWithScene()
{
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
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
	gluOrtho2D(0, m_width, 0, m_height);
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
	if( !g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
	{
		g_render.BindForWriting( m_mFboID );
	}
	else if( !g_useOldRenderingStyle && g_options.m_enableFBO)
	{
		g_render.BindForWriting( m_fboID );
	}
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	if ( g_render.UsingFBOs() && g_options.m_enableFBO )
		glBlitFramebufferEXT(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

CVoid CMultipleWindows::ManageLODs()
{
	CInt m_numSamples;
	CInt width, height;
	if (g_menu.m_justPerspective)
	{
		width = g_multipleView->m_width;
		height = g_multipleView->m_height;
	}
	else
	{
		width = g_multipleView->m_width / 2;
		height = g_multipleView->m_height / 2;

	}

	if (g_multipleView->m_multiSample)
		m_numSamples = width * height * g_options.m_numSamples;
	else
		m_numSamples = width * height;

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

	//	if (resultAvailable)
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


			if ( result == 0 && (g_instancePrefab[i]->GetDistanceFromCamera() > g_instancePrefab[i]->GetRadius() + g_instancePrefabLODPercent.m_lod1) && g_instancePrefab[i]->GetNameIndex() != g_selectedName)
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

CVoid CMultipleWindows::Draw3DObjects()
{
	if (!g_useOldRenderingStyle && g_options.m_enableFBO/* && g_options.m_enableShader && g_render.UsingShader() && g_render.m_useShader*/)
	{
		if (!g_useOldRenderingStyle && m_multiSample && g_options.m_numSamples && g_options.m_enableFBO)
				g_render.BindForWriting(m_mFboID);
		else if (!g_useOldRenderingStyle && g_options.m_enableFBO)
			g_render.BindForWriting(m_fboID);

		if (g_engineLights.size() == 0 || g_editorMode == eMODE_PREFAB)
			SetDefaultLight();

		if (g_editorMode == eMODE_PREFAB)
		{
			g_octree->Render();
		}
		else if (g_editorMode == eMODE_VSCENE)
		{
			g_multipleView->RenderBakedOctree3DModels();
			if (g_menu.m_showOctree)
				g_octree->Render(CFalse, CFalse); //just show octree for debug mode.
		}
		g_multipleView->Render3DAnimatedModels(CTrue);
		g_multipleView->Render3DModelsControlledByPhysX();
		g_multipleView->RenderCharacter(CFalse);

		++g_numLights;
		if (g_editorMode == eMODE_PREFAB || g_editorMode == eMODE_VSCENE)
			g_octree->ResetOctreeGeoCount();
	} //if !g_useOldRenderingStyle
}

CVoid CMultipleWindows::SetDefaultLight()
{
	glEnable(GL_LIGHT0);	 //just for per vertex lighting 	

	//This is the properties of the camera light
	GLfloat light_pos0[4] = {g_camera->m_perspectiveCameraPos.x,g_camera->m_perspectiveCameraPos.y, g_camera->m_perspectiveCameraPos.z, 1.0f };

	GLfloat light_ambient0[4] = { 0.5f, 0.5f, 0.5f, 0.0f };
	GLfloat light_diffuse0[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat light_specular0[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light_shininess0 = 100.0f;

	glLightfv( GL_LIGHT0, GL_POSITION, light_pos0  );
	glLightfv( GL_LIGHT0, GL_AMBIENT , light_ambient0 );
	glLightfv( GL_LIGHT0, GL_DIFFUSE , light_diffuse0 );
	glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular0 );
	glLightf ( GL_LIGHT0, GL_SHININESS, light_shininess0 );
	glLightf ( GL_LIGHT0, GL_CONSTANT_ATTENUATION , (GLfloat)1.0f );
	glLightf ( GL_LIGHT0, GL_LINEAR_ATTENUATION,	(GLfloat)0.0f );
	glLightf ( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, (GLfloat)0.0f );

	g_numLights++;
}

CVoid CMultipleWindows::UpdatePrefabInstanceBB()
{
	//If Prefab Instance is controlled by dynamic PhysX actor that isn't sleeping, update its BB (even if it's invisible)
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

void CMultipleWindows::OnMouseHover(UINT nFlags, CPoint point)
{
	m_mousePosition = point;
	CWnd::OnMouseHover(nFlags, point);
}


CVoid CMultipleWindows::InitGUISelection()
{
	GLint viewport[4];
	glSelectBuffer(MAX_NAME_SIZE, m_guiBuffer);
	(void)glRenderMode(GL_SELECT);
	g_render.ProjectionMatrix();
	g_render.PushMatrix();
	g_render.IdentityMatrix();
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluPickMatrix((CDouble)m_mousePosition.x - 3.0, (CDouble)m_height - (CDouble)m_mousePosition.y - 3.0, 6.0, 6.0, viewport);

	glOrtho(0.0, g_width, 0.0, g_height, -100.0, 100.0);

	g_render.ModelViewMatrix();
	g_render.PushMatrix();
	g_render.IdentityMatrix();

	glInitNames();
}

CVoid CMultipleWindows::FinishGUISelection()
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
CUInt CMultipleWindows::GetSelectedGUI()
{
	m_guiMinZ = 0xffffffff;
	m_guiSelectedName = -1;
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, g_width, g_height);// resets the viewport to new dimensions.

	InitGUISelection();

	if (g_editorMode == eMODE_GUI)
	{
		//render here
		for (CUInt i = 0; i < g_guiBackgrounds.size(); i++)
		{
			g_guiBackgrounds[i]->Render(CTrue);
		}

		//render here
		for (CUInt i = 0; i < g_guiButtons.size(); i++)
		{
			g_guiButtons[i]->Render(CTrue);
		}

		//render here
		for (CUInt i = 0; i < g_guiTexts.size(); i++)
		{
			g_guiTexts[i]->Render(CTrue);
		}
	}
	else if (g_editorMode == eMODE_VSCENE)
	{
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
	}
	FinishGUISelection();

	glPopAttrib();

	return m_guiSelectedName;
}

void CMultipleWindows::GenerateMenuCursorTexture(char* fileName)
{
	DeleteMenuCursorTexture();

	m_menuCursorImg = CNew(CImage);

	Cpy(m_strMenuCursorImg, fileName);

	if (!CTexture::LoadDDSTexture(m_menuCursorImg, m_strMenuCursorImg, NULL))
		PrintInfo("\nGenerateMenuCursorTexture>Couldn't load the cursor image", COLOR_RED);

	m_menuCursorImg->SetFileName(GetAfterPath(m_strMenuCursorImg));
}

void CMultipleWindows::DeleteMenuCursorTexture()
{
	CDelete(m_menuCursorImg);
}

CFloat CMultipleWindows::GetCursorX()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(&p);
	CFloat posX = (CFloat)p.x;
	return posX;
}

CFloat CMultipleWindows::GetCursorY()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(&p);
	CFloat posY = CFloat(m_height - p.y);
	return posY;
}