
// FileBrowserDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FileBrowser.h"
#include "FileBrowserDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFileBrowserDlg dialog



CFileBrowserDlg::CFileBrowserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEBROWSER_DIALOG, pParent)
	, m_strFilePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_strFilePath);
	DDX_Control(pDX, IDC_LIST_FILE, m_ctlListFile);
}

BEGIN_MESSAGE_MAP(CFileBrowserDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GO, &CFileBrowserDlg::OnBnClickedButtonGo)
	ON_COMMAND(IDOK, &CFileBrowserDlg::OnIdok)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CFileBrowserDlg::OnDblclkListFile)
END_MESSAGE_MAP()


// CFileBrowserDlg message handlers

BOOL CFileBrowserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	char* aryCols[] = { "名称", "修改日期", "类型", "大小" };
	int nCnt = sizeof(aryCols) / sizeof(char*);
	// 添加列名
	for (int i = 0; i < nCnt; i++)
	{
		m_ctlListFile.InsertColumn(i, aryCols[i], 0, 200);
	}
	// 设置样式
	m_ctlListFile.SetExtendedStyle(
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_ctlListFile.GetExtendedStyle());

	ShowDriverList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileBrowserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileBrowserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CFileBrowserDlg::ShowFileList(CString strFilePath)
{
	BOOL bFind = FALSE;
	CFileFind fileFind;

	if (strFilePath == "")
	{
		ShowDriverList();
		return TRUE;
	}

	if (strFilePath[strFilePath.GetLength() - 1] == '\\')
	{
		bFind = fileFind.FindFile(strFilePath + "*.*");
	}
	else
	{
		bFind = fileFind.FindFile(strFilePath + "\\*.*");
	}

	if (!bFind)
	{
		return FALSE;
	}

	// 清空列表
	m_ctlListFile.DeleteAllItems();

	int i = 0;
	char szBuf[MAXBYTE] = { 0 };
	while (bFind)
	{
		bFind = fileFind.FindNextFile();
		if (fileFind.GetFileName() == ".")
		{
			continue;
		}

		m_ctlListFile.InsertItem(i, NULL);
		m_ctlListFile.SetItemText(i, 0, fileFind.GetFileName());
		CTime time;
		fileFind.GetLastWriteTime(time);
		m_ctlListFile.SetItemText(i, 1, time.Format("%Y/%m/%d %H:%M"));
		if (fileFind.IsDirectory())
		{
			m_ctlListFile.SetItemText(i, 2, "文件夹");
		}
		else
		{

		}
		sprintf(szBuf, "%u KB", fileFind.GetLength() / 1024.0);
		m_ctlListFile.SetItemText(i, 3, szBuf);

		i++;
	}

	return TRUE;
}

void CFileBrowserDlg::ShowDriverList()
{
	m_ctlListFile.DeleteAllItems();

	// 获取所有盘符
	char szBuf[MAXBYTE] = { 0 };
	GetLogicalDriveStrings(MAXBYTE, szBuf);

	char* p = szBuf;
	int nCnt = 0;
	while (*p != 0)
	{
		m_ctlListFile.InsertItem(nCnt++, p);
		p += 4;
	}
}


void CFileBrowserDlg::OnBnClickedButtonGo()
{
	CString strPreFilePath = m_strFilePath;
	UpdateData(TRUE);
	if (!ShowFileList(m_strFilePath))
	{
		m_strFilePath = strPreFilePath;
	}
}


void CFileBrowserDlg::OnIdok()
{
	// TODO: Add your command handler code here
}


void CFileBrowserDlg::OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	// 获取选中的文件
	CString strFile = m_ctlListFile.GetItemText(pNMItemActivate->iItem, 0);

	if (strFile == "..")
	{
		int i = m_strFilePath.GetLength() - 1;
		while (m_strFilePath[i] != '\\')
		{
			i--;
		}

		if (m_strFilePath[i - 1] == ':')
		{
			m_strFilePath.Delete(i + 1, m_strFilePath.GetLength() - i - 1);
		}
		else
		{
			m_strFilePath.Delete(i, m_strFilePath.GetLength() - i);
		}

		ShowFileList(m_strFilePath);
		UpdateData(FALSE);

		return;
	}

	if (m_strFilePath == "")
	{
		ShowFileList(strFile);
		m_strFilePath = strFile;
		UpdateData(FALSE);
	}
	else
	{
		CString strNewFilePath;
		if (m_strFilePath[m_strFilePath.GetLength() - 1] == '\\')
		{
			strNewFilePath = m_strFilePath + strFile;
		}
		else
		{
			strNewFilePath = m_strFilePath + "\\" + strFile;
		}
		BOOL bIsSuccess = ShowFileList(strNewFilePath);
		if (bIsSuccess)
		{
			m_strFilePath = strNewFilePath;
			UpdateData(FALSE);
		}
	}

	*pResult = 0;
}
