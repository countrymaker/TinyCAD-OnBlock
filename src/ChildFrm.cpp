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
#include "TinyCad.h"
#include "TinyCadView.h"

#include "ChildFrm.h"

// Private MFC function only sets the title if it's different
//
extern void AFXAPI AfxSetWindowText(HWND hWndCtrl, LPCTSTR lpszNew);

/////////////////////////////////////////////////////////////////////////////
// CChildFrame
IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
//{{AFX_MSG_MAP(CChildFrame)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code !
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here

}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if (!CMDIChildWndEx::PreCreateWindow(cs)) return FALSE;

	return TRUE;
}

void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ( (GetStyle() & FWS_ADDTOTITLE) == 0) return;

	if (bAddToTitle)
	{
		CDocument* pDoc = GetActiveDocument();
		CString csText;
		if (pDoc == NULL) csText = m_strTitle;
		else {
			csText = pDoc->GetTitle();
		}

		if (m_nWindow > 0) csText.Format(_T("%s: %d"), (LPCTSTR)csText, (LPCTSTR)m_nWindow);
		if (pDoc->IsModified()) csText += " *";

		AfxSetWindowText(m_hWnd, csText);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}

#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{

	if (pContext->m_pNewViewClass == RUNTIME_CLASS(CTinyCadView))
	{
		// Special folder frame
		return m_wndFolderFrame.Create(this, RUNTIME_CLASS(CTinyCadView), pContext, 0);
	}
	else
	{
		// TODO: Add your specialized code here and/or call the base class
		return CMDIChildWndEx::OnCreateClient(lpcs, pContext);
	}
}
