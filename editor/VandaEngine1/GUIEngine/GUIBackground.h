//Copyright (C) 2021 Ehsan Kamrani 
//This file is licensed and distributed under MIT license

#pragma once
#include "afxwin.h"
#include "Utility.h"
#include "..\\GraphicsEngine\\Image.h"
#include "..\\GraphicsEngine\\PerspectiveWindow.h"

class CGUIBackground
{
public:
	CGUIBackground();
	~CGUIBackground();
private:
	CChar m_name[MAX_NAME_SIZE];
	CVec2f m_position;
	CFloat m_size;
	CChar m_packageName[MAX_NAME_SIZE];
	CChar m_guiName[MAX_NAME_SIZE];

	CChar m_imagePath[MAX_NAME_SIZE];

	CImage* m_image;
	CBool m_updateImage;

	CInt m_nameIndex;

public:
	inline CVoid SetImagePath(CChar* path) { Cpy(m_imagePath, path); }

	CVec2f SetPosition(CVec2f pos);
	CVoid SetPosition2(CVec2f pos);
	inline CVoid SetName(CChar* name) { Cpy(m_name, name); }
	inline CVoid SetPackageName(CChar* name) { Cpy(m_packageName, name); }
	inline CVoid SetGUIName(CChar* name) { Cpy(m_guiName, name); }
	inline CVoid SetSize(CFloat size) { m_size = size; }

	inline CChar* GetName() { return m_name; }
	inline CChar* GetPackageName() { return m_packageName; }
	inline CChar* GetGUIName() { return m_guiName; }
	inline CChar* GetImagePath() { return m_imagePath; }
	inline CFloat GetSize() { return m_size; }
	inline CVec2f GetPosition() { return m_position; }
	inline CImage* GetImage() { return m_image; }
	CBool LoadBackgroundImage();

	inline CVoid SetUpdateImage(CBool state) { m_updateImage = state; }
	inline CBool GetUpdateImage() { return m_updateImage; }

	CVoid Render(CBool selectionMode = CFalse);

	CVoid SetIndex() { m_nameIndex = g_nameIndex++; }
	CInt GetIndex() { return m_nameIndex; }

};
