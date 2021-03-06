//Copyright (C) 2021 Ehsan Kamrani 
//This file is licensed and distributed under MIT license
// CAddAmbientSound.cpp : implementation file
//

#include "stdafx.h"
#include "GUIButton.h"

#include "..\\VandaEngine1Dlg.h"

CGUIButton::CGUIButton()
{
	m_mainImage = NULL;
	m_leftClickImage = NULL;
	m_hoverImage = NULL;
	m_rightClickImage = NULL;
	m_disableImage = NULL;

	m_updateMainImage = CFalse;
	m_updateLeftClickImage = CFalse;
	m_updateHoverImage = CFalse;
	m_updateRightClickImage = CFalse;
	m_updateDisableImage = CFalse;
	m_updateScript = CFalse;

	m_currentImageType = eBUTTON_IMAGE_MAIN;
	SetIndex();

	m_rightClickTimer = m_leftClickTimer = 0.0f;
	m_maxRightClickTimer = 0.1f;
	m_maxLeftClickTimer = 0.1f;

	m_lua = LuaNewState();
	LuaOpenLibs(m_lua);
	LuaRegisterFunctions(m_lua);
}

CGUIButton::~CGUIButton()
{
	CDelete(m_mainImage);
	CDelete(m_leftClickImage);
	CDelete(m_hoverImage);
	CDelete(m_rightClickImage);
	CDelete(m_disableImage);
	LuaClose(m_lua);
}

CBool CGUIButton::LoadMainImage()
{
	if (m_mainImage)
		CDelete(m_mainImage);
	m_mainImage = CNew(CImage);
	if (!CTexture::LoadDDSTexture(m_mainImage, m_mainImagePath, NULL))
	{
		PrintInfo("Couldn't load main image");
		CDelete(m_mainImage);
		return CFalse;
	}
	return CTrue;

}

CBool CGUIButton::LoadLeftClickImage()
{
	if (m_leftClickImage)
		CDelete(m_leftClickImage);
	m_leftClickImage = CNew(CImage);
	if (!CTexture::LoadDDSTexture(m_leftClickImage, m_leftClickImagePath, NULL))
	{
		PrintInfo("Couldn't load left click image");
		CDelete(m_leftClickImage);
		return CFalse;
	}
	return CTrue;

}

CBool CGUIButton::LoadHoverImage()
{
	if (m_hoverImage)
		CDelete(m_hoverImage);
	m_hoverImage = CNew(CImage);
	if (!CTexture::LoadDDSTexture(m_hoverImage, m_hoverImagePath, NULL))
	{
		PrintInfo("Couldn't load hover image");
		CDelete(m_hoverImage);
		return CFalse;
	}
	return CTrue;

}

CBool CGUIButton::LoadRightClickImage()
{
	if (m_rightClickImage)
		CDelete(m_rightClickImage);
	m_rightClickImage = CNew(CImage);
	if (!CTexture::LoadDDSTexture(m_rightClickImage, m_rightClickImagePath, NULL))
	{
		PrintInfo("Couldn't load hover image");
		CDelete(m_rightClickImage);
		return CFalse;
	}
	return CTrue;

}

CBool CGUIButton::LoadDisableImage()
{
	if (m_disableImage)
		CDelete(m_disableImage);
	m_disableImage = CNew(CImage);
	if (!CTexture::LoadDDSTexture(m_disableImage, m_disableImagePath, NULL))
	{
		PrintInfo("Couldn't load hover image");
		CDelete(m_disableImage);
		return CFalse;
	}
	return CTrue;
}


CVoid CGUIButton::Render(CBool selectionMode)
{
	CFloat w;
	if (g_editorMode == eMODE_GUI || (g_editorMode == eMODE_VSCENE && g_menu.m_justPerspective))
		w = m_size * g_width / 100.0f;
	else if (g_editorMode == eMODE_VSCENE && !g_menu.m_justPerspective)
		w = m_size * g_width / 200.0f;

	CFloat h = (w / m_mainImage->GetWidth()) * m_mainImage->GetHeight();

	if (selectionMode)
	{
		glPushName(m_nameIndex);
		glUseProgram(0);

		if (g_editorMode == eMODE_GUI || (g_editorMode == eMODE_VSCENE && g_menu.m_justPerspective))
		{
			glBegin(GL_QUADS);
			glVertex3f(m_position.x, m_position.y - h, 0.0f);
			glVertex3f(m_position.x + w, m_position.y - h, 0.0f);
			glVertex3f(m_position.x + w, m_position.y, 0.0f);
			glVertex3f(m_position.x, m_position.y, 0.0f);
			glEnd();
		}
		else if (g_editorMode == eMODE_VSCENE && !g_menu.m_justPerspective)
		{
			glBegin(GL_QUADS);
			glVertex3f(m_position.x * 0.5f, m_position.y * 0.5f - h, 0.0f);
			glVertex3f(m_position.x * 0.5f + w, m_position.y * 0.5f - h, 0.0f);
			glVertex3f(m_position.x * 0.5f + w, m_position.y * 0.5f, 0.0f);
			glVertex3f(m_position.x * 0.5f, m_position.y * 0.5f, 0.0f);
			glEnd();
		}
		glPopName();

		return;
	}


	glUseProgram(0);

	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_VIEWPORT_BIT);
	glViewport(0, 0, g_width, g_height);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, g_width, 0.0, g_height, -2.0, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	//Upper Left: Position.x, Position.y

	if (g_multipleView->IsPlayGameMode() && m_currentImageType == eBUTTON_IMAGE_LEFT_CLICK)
	{
		m_leftClickTimer += g_multipleView->timer.GetElapsedSeconds2();

		if (m_leftClickTimer >= m_maxLeftClickTimer)
		{
			m_leftClickTimer = 0.0f;
			CUInt index = g_multipleView->GetSelectedGUI();
			if (GetIndex() == index)
			{
				if (GetHasHoverImage())
					m_currentImageType = eBUTTON_IMAGE_HOVER;
				else
					m_currentImageType = eBUTTON_IMAGE_MAIN;
			}
			else
			{
				m_currentImageType = eBUTTON_IMAGE_MAIN;
			}

			OnSelectMouseLButtonDownScript();
		}

	}
	else if (g_multipleView->IsPlayGameMode() && m_currentImageType == eBUTTON_IMAGE_RIGHT_CLICK)
	{
		m_rightClickTimer += g_multipleView->timer.GetElapsedSeconds2();
		if (m_rightClickTimer >= m_maxRightClickTimer)
		{
			m_rightClickTimer = 0.0f;
			CUInt index = g_multipleView->GetSelectedGUI();
			if (GetIndex() == index)
			{
				if (GetHasHoverImage())
					m_currentImageType = eBUTTON_IMAGE_HOVER;
				else
					m_currentImageType = eBUTTON_IMAGE_MAIN;
			}
			else
			{
				m_currentImageType = eBUTTON_IMAGE_MAIN;
			}

			OnSelectMouseRButtonDownScript();
		}
	}

	glActiveTextureARB(GL_TEXTURE0_ARB);
	switch (m_currentImageType)
	{
	case eBUTTON_IMAGE_MAIN:
		glBindTexture(GL_TEXTURE_2D, m_mainImage->GetId());
		break;
	case eBUTTON_IMAGE_HOVER:
		if (GetHasHoverImage())
			glBindTexture(GL_TEXTURE_2D, m_hoverImage->GetId());
		else
			glBindTexture(GL_TEXTURE_2D, m_mainImage->GetId());
		break;
	case eBUTTON_IMAGE_DISABLE:
		if (GetHasDisableImage())
			glBindTexture(GL_TEXTURE_2D, m_disableImage->GetId());
		else
			glBindTexture(GL_TEXTURE_2D, m_mainImage->GetId());
		break;
	case eBUTTON_IMAGE_RIGHT_CLICK:
		if (GetHasRightClickImage())
			glBindTexture(GL_TEXTURE_2D, m_rightClickImage->GetId());
		else
			glBindTexture(GL_TEXTURE_2D, m_mainImage->GetId());
		break;
	case eBUTTON_IMAGE_LEFT_CLICK:
		if (GetHasLeftClickImage())
			glBindTexture(GL_TEXTURE_2D, m_leftClickImage->GetId());
		else
			glBindTexture(GL_TEXTURE_2D, m_mainImage->GetId());
		break;
	}

	if (m_mainImage->GetFormat() == 4)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	if (g_editorMode == eMODE_GUI || (g_editorMode == eMODE_VSCENE && g_menu.m_justPerspective))
	{
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 1);  glVertex3f(m_position.x, m_position.y, -1.0f);
		glTexCoord2d(1, 1);  glVertex3f(m_position.x + w, m_position.y, -1.0f);
		glTexCoord2d(1, 0);  glVertex3f(m_position.x + w, m_position.y - h, -1.0f);
		glTexCoord2d(0, 0);  glVertex3f(m_position.x, m_position.y - h, -1.0f);
		glEnd();
	}
	else if (g_editorMode == eMODE_VSCENE && !g_menu.m_justPerspective)
	{
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 1);  glVertex3f(m_position.x * 0.5f, m_position.y * 0.5f, -1.0f);
		glTexCoord2d(1, 1);  glVertex3f(m_position.x * 0.5f + w, m_position.y * 0.5f, -1.0f);
		glTexCoord2d(1, 0);  glVertex3f(m_position.x * 0.5f + w, m_position.y * 0.5f - h, -1.0f);
		glTexCoord2d(0, 0);  glVertex3f(m_position.x * 0.5f, m_position.y * 0.5f - h, -1.0f);
		glEnd();
	}


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

CVec2f CGUIButton::SetPosition(CVec2f pos)
{
	if (m_mainImage)
	{
		CFloat w = m_size * g_width / 100.0f;
		CFloat h = (w / m_mainImage->GetWidth()) * m_mainImage->GetHeight();

		m_position.x = pos.x;
		if (m_position.x < 0.0)
			m_position.x = 0.0;
		else if (m_position.x > g_multipleView->m_width - w)
			m_position.x = g_multipleView->m_width - w;

		m_position.y = pos.y;

		if (m_position.y <  h)
			m_position.y = h;
		else if (m_position.y > g_multipleView->m_height)
			m_position.y = g_multipleView->m_height;
	}
	else
	{
		m_position = pos;
	}
	return m_position;
}

CVoid CGUIButton::SetPosition2(CVec2f pos)
{
	if (m_mainImage)
	{
		CFloat w = m_size * g_width / 100.0f;
		CFloat h = (w / m_mainImage->GetWidth()) * m_mainImage->GetHeight();

		m_position.x += pos.x;
		if (m_position.x < 0.0)
			m_position.x = 0.0;
		else if (m_position.x > g_multipleView->m_width - w)
			m_position.x = g_multipleView->m_width - w;

		m_position.y -= pos.y;

		if (m_position.y <  h)
			m_position.y = h;
		else if (m_position.y > g_multipleView->m_height)
			m_position.y = g_multipleView->m_height;
	}
	else
	{
		m_position.x += pos.x;
		m_position.y -= pos.y;
	}
}

CVoid CGUIButton::ResetLua()
{
	LuaClose(m_lua);
	m_lua = LuaNewState();
	LuaOpenLibs(m_lua);
	LuaRegisterFunctions(m_lua);
}

CBool CGUIButton::LoadLuaFile()
{
	ResetLua();
	if (!LuaLoadFile(m_lua, m_ScriptPath))
		return CFalse;
	return CTrue;
}

CVoid CGUIButton::OnSelectMouseLButtonDownScript()
{
	if (m_hasScript)
	{
		g_currentInstancePrefab = NULL;

		lua_getglobal(m_lua, "OnSelectMouseLButtonDown");
		if (lua_isfunction(m_lua, -1))
		{
			lua_pcall(m_lua, 0, 0, 0);
		}

		lua_settop(m_lua, 0);
	}
}

CVoid CGUIButton::OnSelectMouseRButtonDownScript()
{
	if (m_hasScript)
	{
		g_currentInstancePrefab = NULL;

		lua_getglobal(m_lua, "OnSelectMouseRButtonDown");
		if (lua_isfunction(m_lua, -1))
		{
			lua_pcall(m_lua, 0, 0, 0);
		}

		lua_settop(m_lua, 0);
	}
}

CVoid CGUIButton::OnSelectMouseHoverScript()
{
	if (m_hasScript)
	{
		g_currentInstancePrefab = NULL;

		lua_getglobal(m_lua, "OnSelectMouseHover");
		if (lua_isfunction(m_lua, -1))
		{
			lua_pcall(m_lua, 0, 0, 0);
		}

		lua_settop(m_lua, 0);
	}

}
