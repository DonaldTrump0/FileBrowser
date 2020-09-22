#pragma once
#include <string>
#include <list>
using namespace std;

class CFileBrowserDlg : public CDialogEx
{
// Construction
public:
	CFileBrowserDlg(CWnd* pParent = nullptr);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEBROWSER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	list<CString> GetDriveList();
	BOOL ShowFileList(CString strFilePath);
	CString GetFilePath(HTREEITEM hItem);

public:
	CString m_strFilePath;
	CListCtrl m_ListFile;
	CTreeCtrl m_TreeFile;
	afx_msg void OnBnClickedButtonGo();
	afx_msg void OnIdok();
	afx_msg void OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandedTreeFile(NMHDR* pNMHDR, LRESULT* pResult);
};
