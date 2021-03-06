//
// PageExpert.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) � 2008,2012-2014
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once

#include "WizardSheet.h"

#define LVS_EX_LABELTIP			0x00004000
#define LVS_EX_DOUBLEBUFFER 	0x00010000

class CExpertPage : public CWizardPage
{
	DECLARE_DYNCREATE(CExpertPage)

// Construction
public:
	CExpertPage();
	//virtual ~CExpertPage();

	enum { IDD = IDD_EXPERT_PAGE };

// Dialog Data
public:
	QWORD 		m_nTotalSize;
	CString 	m_sFileCount;
	CString 	m_sName;
	CString 	m_sFolder;
	CString 	m_sTracker;
	CString 	m_sComment;
//	CString 	m_sSource;
	BOOL		m_bPrivate;
	CEdit		m_wndName;
	CComboBox	m_wndFolders;
	CComboBox	m_wndTracker;
	CButton 	m_wndRemoveFile;
	CButton 	m_wndRemoveTracker;
	CListCtrl	m_wndList;
	CListBox	m_wndTrackers;
	HIMAGELIST	m_hImageList;

// Operations
protected:
	void	AddFile(LPCTSTR pszFile);
	void	AddFolder(LPCTSTR pszPath, int nRecursive);
	void	GetTrackerHistory();
	void	SetTrackerHistory();

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual void OnReset();
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

// Implementation
protected:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnItemChangedFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBrowseFolder();
	afx_msg void OnAddFolder();
	afx_msg void OnAddFile();
	afx_msg void OnRemoveFile();
	afx_msg void OnAddTracker();
	afx_msg void OnRemoveTracker();
	afx_msg void OnSelectTracker();

	DECLARE_MESSAGE_MAP()
};
