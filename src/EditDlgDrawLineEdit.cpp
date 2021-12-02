/*
 TinyCAD program for schematic capture
 Copyright 1994/1995/2002,2003 Matt Pyne.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "stdafx.h"
#include "TinyCadView.h"
#include "TinyCad.h"
#include "EditDlgDrawLineEdit.h"

////// The Line editing Dialog //////


CEditDlgDrawLineEdit::CEditDlgDrawLineEdit()
{
	//{{AFX_DATA_INIT(CEditDlgDrawLineEdit)
	//}}AFX_DATA_INIT
}

void CEditDlgDrawLineEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDlgDrawLineEdit)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditDlgDrawLineEdit, CEditDlg)
//{{AFX_MSG_MAP(CEditDlgDrawLineEdit)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Create this window
void CEditDlgDrawLineEdit::Create()
{
	CDialog::Create(CEditDlgDrawLineEdit::IDD, &g_EditToolBar);

	m_angle_wire = LINEBOX_90;
}

void CEditDlgDrawLineEdit::Open(CTinyCadDoc *pDesign, CDrawingObject *pObject)
{
	Show(pDesign, pObject);

	CheckRadioButton(LINEBOX_FREE, LINEBOX_ARC2, m_angle_wire);
}

