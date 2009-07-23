/**
 * This file is part of Pandion.
 *
 * Pandion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pandion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pandion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    ServerMPFDPost.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"
#include "Transfer.h"
#include "ServerMPFDPost.h"
#include "Socket.h"
#include "HTTPRequest.h"
#include "File.h"

CServerMPFDPost::CServerMPFDPost(IEventRouter* pER, CPile* pPile, CHTTPRequest* pRequest, Socket* pSocket, DWORD sessionID) :
	CTransfer(pER, pPile, pSocket, sessionID), m_pRequest(pRequest)
{
	m_fp = 0;
	m_buf = new BYTE[BLOCK_SIZE];
}
CServerMPFDPost::~CServerMPFDPost()
{
	delete m_buf;
}
bool CServerMPFDPost::EatBoundary()
{
	BYTE* buf = new BYTE[_tcslen(m_boundary)];
	m_pSocket->Recv(buf, _tcslen(m_boundary));
	
	LPTSTR c = _tcsstr(m_boundary, CA2T((char*)buf));
	delete buf;

	return c ? true : false;
}
DWORD CServerMPFDPost::PreCommand()
{
	VARIANT Content_Type;
	m_pRequest->m_pHeaders->get_Item(&_variant_t(_bstr_t("Content-Type")), &Content_Type);

	LPTSTR tmp = _tcsstr(CW2T(Content_Type.bstrVal), TEXT("boundary=")) + 9;
	if(_tcslen(tmp)>= 128) /// buffer overflow
		return -1;

	StringCchCopy(m_boundary, 128, tmp);
	return 0;
}
DWORD CServerMPFDPost::Command()
{
	return 1;
}
DWORD CServerMPFDPost::PostCommand()
{
	delete this;
	return 0;
}
DWORD CServerMPFDPost::GetProgress()
{
	return m_fp;
}
DWORD CServerMPFDPost::GetLength()
{
	unsigned u;
	m_pFile->get_Size(&u);
	return u;
}