/*
 TinyCAD program for schematic capture
 Copyright 1994/1995/2002-2005 Matt Pyne.

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

// ImageJpeg.h: interface for the CImageJpeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEJPEG_H__64B86D8A_4B0C_42A6_90DF_5823F056C3FB__INCLUDED_)
#define AFX_IMAGEJPEG_H__64B86D8A_4B0C_42A6_90DF_5823F056C3FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ImageBitmap.h"

class CImageJpeg: public CImageBitmap
{
protected:

	// Read from a memory block
	virtual bool Read(CDC &ref_dc, const unsigned char *buf, size_t size);

public:
	CImageJpeg();
	virtual ~CImageJpeg();

	// Get the type of this object
	virtual CString GetType();
};

#endif // !defined(AFX_IMAGEJPEG_H__64B86D8A_4B0C_42A6_90DF_5823F056C3FB__INCLUDED_)
