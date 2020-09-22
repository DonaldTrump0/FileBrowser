#include "pch.h"
#include "framework.h"
#include "FileBrowser.h"
#include "FileBrowserDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
	DDX_Control(pDX, IDC_LIST_FILE, m_ListFile);
	DDX_Control(pDX, IDC_TREE_FILE, m_TreeFile);
}

BEGIN_MESSAGE_MAP(CFileBrowserDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GO, &CFileBrowserDlg::OnBnClickedButtonGo)
	ON_COMMAND(IDOK, &CFileBrowserDlg::OnIdok)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CFileBrowserDlg::OnDblclkListFile)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FILE, &CFileBrowserDlg::OnSelchangedTreeFile)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_FILE, &CFileBrowserDlg::OnItemexpandedTreeFile)
END_MESSAGE_MAP()


// CFileBrowserDlg message handlers

BOOL CFileBrowserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	char* aryCols[] = { "名称", "修改日期", "类型", "大小" };
	int nCnt = sizeof(aryCols) / sizeof(char*);
	// 添加列名
	for (int i = 0; i < nCnt; i++)
	{
		m_ListFile.InsertColumn(i, aryCols[i], 0, 200);
	}
	// 设置列表控件样式
	m_ListFile.SetExtendedStyle(
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | m_ListFile.GetExtendedStyle());

	// 获取所有盘符显示
	list<CString> ltDriveList = GetDriveList();
	int i = 0;
	for (CString strDrive : ltDriveList)
	{
		// 插入到列表控件
		m_ListFile.InsertItem(i++, strDrive);
		// 插入到树控件
		HTREEITEM hTreeItem = m_TreeFile.InsertItem(strDrive, NULL, TVI_LAST);

		CFileFind fileFind;
		if (fileFind.FindFile(strDrive + "\\*.*"))
		{
			// 占位符，使之显示+号
			m_TreeFile.InsertItem("", hTreeItem);
		}
	}

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

// 获取所有盘符
list<CString> CFileBrowserDlg::GetDriveList()
{
	list<CString> lt;

	char szBuf[MAXBYTE] = { 0 };
	::GetLogicalDriveStrings(MAXBYTE, szBuf);

	char* p = szBuf;
	while (*p != 0)
	{
		p[2] = 0;	// 去掉\ 
		lt.push_back(p);
		p += 4;
	}

	return lt;
}

BOOL CFileBrowserDlg::ShowFileList(CString strFilePath)
{
	BOOL bFind = FALSE;
	CFileFind fileFind;

	if (strFilePath == "")
	{
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
	m_ListFile.DeleteAllItems();

	int i = 0;
	char szBuf[MAXBYTE] = { 0 };
	while (bFind)
	{
		bFind = fileFind.FindNextFile();
		if (fileFind.GetFileName() == ".")
		{
			continue;
		}

		m_ListFile.InsertItem(i, NULL);
		m_ListFile.SetItemText(i, 0, fileFind.GetFileName());
		CTime time;
		fileFind.GetLastWriteTime(time);
		m_ListFile.SetItemText(i, 1, time.Format("%Y/%m/%d %H:%M"));
		if (fileFind.IsDirectory())
		{
			m_ListFile.SetItemText(i, 2, "文件夹");
		}
		else
		{
		}
		sprintf(szBuf, "%u KB", fileFind.GetLength() / 1024.0);
		m_ListFile.SetItemText(i, 3, szBuf);

		i++;
	}

	return TRUE;
}

CString CFileBrowserDlg::GetFilePath(HTREEITEM hItem)
{
	CString strFilePath;
	while (hItem)
	{
		strFilePath.Insert(0, m_TreeFile.GetItemText(hItem) + "\\");
		hItem = m_TreeFile.GetParentItem(hItem);
	}
	return strFilePath;
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
}


void CFileBrowserDlg::OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	UpdateData(TRUE);

	// 获取选中的文件
	CString strFile = m_ListFile.GetItemText(pNMItemActivate->iItem, 0);

	if (strFile == "..")
	{
		int i = m_strFilePath.GetLength() - 1;
		i--;
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
}

// 切换树控件item
void CFileBrowserDlg::OnSelchangedTreeFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	m_strFilePath = GetFilePath(pNMTreeView->itemNew.hItem);
	ShowFileList(m_strFilePath);

	UpdateData(FALSE);
}

// 展开树控件item
void CFileBrowserDlg::OnItemexpandedTreeFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	// 收缩合并
	if (pNMTreeView->action == TVE_COLLAPSE)
	{
		return;
	}

	HTREEITEM hParentItem = pNMTreeView->itemNew.hItem;

	// 清除所有子项
	HTREEITEM hItem;
	while (hItem = m_TreeFile.GetChildItem(hParentItem))
	{
		m_TreeFile.DeleteItem(hItem);
	}

	// 获取路径
	CString strFilePath = GetFilePath(hParentItem);

	// 重新添加
	CFileFind fileFind;
	bool bFind = fileFind.FindFile(strFilePath + "*.*");
	while (bFind)
	{
		bFind = fileFind.FindNextFile();
		CString strFileName = fileFind.GetFileName();

		if (strFileName == "." || strFileName == "..")
		{
			continue;
		}

		HTREEITEM h = m_TreeFile.InsertItem(strFileName, hParentItem);
		CFileFind f;
		if (f.FindFile(strFilePath + strFileName + "\\*.*"))
		{
			m_TreeFile.InsertItem("", h);
		}
	}
}