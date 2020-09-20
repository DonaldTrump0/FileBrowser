
// FileBrowserDlg.h : header file
//

#pragma once


// CFileBrowserDlg dialog
class CFileBrowserDlg : public CDialogEx
{
// Construction
public:
	CFileBrowserDlg(CWnd* pParent = nullptr);	// standard constructor

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
	BOOL ShowFileList(CString strFilePath);
	void ShowDriverList();

public:
	CString m_strFilePath;
	CListCtrl m_ctlListFile;
	afx_msg void OnBnClickedButtonGo();
	afx_msg void OnIdok();
	afx_msg void OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult);
};
