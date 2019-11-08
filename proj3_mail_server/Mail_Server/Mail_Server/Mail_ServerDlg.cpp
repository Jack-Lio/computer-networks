
// Mail_ServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Mail_Server.h"
#include "Mail_ServerDlg.h"
#include "afxdialogex.h"
#include "base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMail_ServerDlg �Ի���



CMail_ServerDlg::CMail_ServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAIL_SERVER_DIALOG, pParent)
	, m_mail_sender(NULL)
	, picture_ctrl(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMail_ServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mail_text_ctrl);
	DDX_Control(pDX, IDC_LIST1, log_list_ctrl);
	DDX_Control(pDX, IDC_EDIT2, mail_display_ctrl);
	DDX_Control(pDX, IDC_EDIT3, txt_ctrl);
	DDX_Control(pDX, IDC_EDIT6, cur_ctrl);
	DDX_Control(pDX, IDC_EDIT5, total_ctrl);
}

BEGIN_MESSAGE_MAP(CMail_ServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DISPLAYMAIL,OnDisplayMail)
	ON_BN_CLICKED(IDC_BUTTON1, &CMail_ServerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMail_ServerDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMail_ServerDlg ��Ϣ�������

BOOL CMail_ServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	cur_ctrl.SetWindowTextA("0");
	total_ctrl.SetWindowTextA("0");
	//����TCP�׽��֣���25�˿�
	BOOL bFlag = SMTP_Server.Create(25, SOCK_STREAM, FD_ACCEPT | FD_READ | FD_WRITE );
	if (!bFlag) {
		log_list_ctrl.InsertString(log_list_ctrl.GetCount(),"***SMTP����������ʧ��");
	}
	else
	{
		log_list_ctrl.InsertString(log_list_ctrl.GetCount(), "***SMTP������׼����");
	}
	log_list_ctrl.InsertString(log_list_ctrl.GetCount(), "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	if (!SMTP_Server.Listen()) //��Ϊ�����������������������������Ϊ5
		log_list_ctrl.InsertString(log_list_ctrl.GetCount(), "����25�˿�����ʧ��,�˿ڿ����ѱ�ռ��");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMail_ServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMail_ServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMail_ServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//ת���ʼ��߳̿��ƺ���
UINT Send_Mails(PVOID hWnd)
{
	return 0;
}

void CMail_ServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	if(SMTP_Server!= INVALID_SOCKET)
		SMTP_Server.Close();
}

CString CMail_ServerDlg::UTF8toANSI(CString &strUTF8)
{
	//��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�����
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, -1, NULL, NULL);
	WCHAR *wszBuffer = new WCHAR[nLen+1];
	nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR *szBuffer = new CHAR[nLen+1];
	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	strUTF8 = szBuffer;
	//�����ڴ�
	delete[]szBuffer;
	delete[]wszBuffer;
	return  strUTF8;
}


// �����ʼ����ĵ���Ϣ������
afx_msg LRESULT CMail_ServerDlg::OnDisplayMail(WPARAM wParam, LPARAM lParam)
{
	if (mail_text_ctrl.GetLineCount() == 0) 
		return afx_msg LRESULT();		//������ʾ����Ϊ�գ�������������
	//����������µ��������֮ǰ��Ҫ�����ԭ�ļ�����
	mail_display_ctrl.SetWindowTextA("");
	txt_ctrl.SetWindowTextA("");
	//���ÿռ��ͼƬΪ��
	((CStatic*)GetDlgItem(IDC_picture))->SetBitmap(NULL);
	UpdateData(true);
	//��ȡ�����ı�
	CString mail_post;
	mail_text_ctrl.GetWindowTextA(mail_post);
	//��ȡ��Ӧ�ı����ı�
	int start_base64_code = 0;  
	while (1) {
		start_base64_code = mail_post.Find("Content-Transfer-Encoding: base64", start_base64_code);
		if (start_base64_code == -1)
		{
			break;
		}
		int text_type_begin = mail_post.Find("\r\n", start_base64_code);
		int code_begin = mail_post.Find("\r\n\r\n", text_type_begin+2);//�ҵ�base64���뿪ʼλ��
		int code_end = mail_post.Find("\r\n\r\n", code_begin+4);//�ҵ�ͼƬ�����������λ��
		CString code= mail_post.Mid(code_begin + 4, code_end - code_begin - 4); //�ضϱ���;
		while (1) {
			int pos = code.Find("\r\n");
			if (pos == -1) break;
			CString a = code.Mid(0, pos);
			CString b = code.Mid(pos + 2, code.GetLength()-2);
			code = a + b;
		}
		CString type = mail_post.Mid(text_type_begin + 2, code_begin - text_type_begin - 2);
		if (type.Find("attachment") != -1)		//������
		{
			int p1 = type.Find('"', 0);
			int p2 = type.Find('"', p1 + 1);
			CFile file;
			CString file_name = type.Mid(p1 + 1, p2 - p1 - 1);
			//log_list_ctrl.InsertString(log_list_ctrl.GetCount(),"�򿪸�����"+file_name);
			file.Open(file_name, CFile::modeWrite | CFile::modeCreate| CFile::typeBinary);
			char *output = new char[code.GetLength() * 3 / 4];
			base64_decode(code, output);
			file.Write(output, code.GetLength() * 3 / 4);
			file.SeekToEnd();
			file.Close();
			if (type.Find("txt") != -1)
			{
				txt_ctrl.SetWindowTextA(output);
			}
			else if(type.Find("jpg")!=-1||
				type.Find("bmp") != -1||
				type.Find("PNG") != -1||
				type.Find("png") != -1||
				type.Find("BMP") != -1||
				type.Find("JPG") != -1) {

				CImage image;
				image.Load(file_name);

				//��������������Ҫ�����ǣ���ȡ�Ի��������Picture Control��width��height��
				//�����õ�ͼƬ����rectPicture������ͼƬ����rectPicture��ͼƬ���д���
				//������ͼƬ���Ի�����Picture Control����
				CRect rectControl;                        //�ؼ����ζ���
				CRect rectPicture;                        //ͼƬ���ζ���


				int x = image.GetWidth();
				int y = image.GetHeight();
				//Picture Control��IDΪIDC_picture
				CWnd  *pWnd = GetDlgItem(IDC_picture);
				pWnd->GetClientRect(rectControl);


				CDC *pDc = GetDlgItem(IDC_picture)->GetDC();
				SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);
				//��������ͼƬ�Ĵ�СΪ�ռ�Ĺ��
				rectPicture = CRect(rectControl.TopLeft(), CSize((int)rectControl.Width(), (int)rectControl.Height()));
				//���ÿռ��ͼƬΪ��
				((CStatic*)GetDlgItem(IDC_picture))->SetBitmap(NULL);

				//�������ַ������ɻ���ͼƬ��������ֱ���ڿؼ���������ͼ���ܹ�ʵ����ͬ�Ĺ���
				image.StretchBlt(pDc->m_hDC, rectPicture, SRCCOPY); //��ͼƬ���Ƶ�Picture�ؼ���ʾ�ľ�������
				//image.Draw(pDc->m_hDC, rectPicture);                //��ͼƬ���Ƶ�Picture�ؼ���ʾ�ľ�������

				image.Destroy();
				pWnd->ReleaseDC(pDc);
				UpdateData(true);
			}
		}
		else if (type.Find("charset=") != -1) //���������ı�
		{
			char *output = new char[code.GetLength() * 3 / 4];
			output = base64_decode(code, output);
			mail_display_ctrl.SetWindowTextA(UTF8toANSI(CString(output))); //��ʾ����
		}
		start_base64_code = code_end;
	}
	return afx_msg LRESULT();
}


void CMail_ServerDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString cur, total;
	cur_ctrl.GetWindowTextA(cur);
	total_ctrl.GetWindowTextA(total);
	int cur_mail = _ttoi(cur),total_mail = _ttoi(total);
	//������������
	if (total_mail == 0 || cur_mail == 1) return;
	//���½������ʼ���ű�ʶ
	cur.Format("%d", cur_mail -1);
	cur_ctrl.SetWindowTextA(cur);
	//��ǰһ���ʼ��ı�����ʾ��չʾ����
	mail_text_ctrl.SetWindowTextA(mail_list.GetAt(mail_list.FindIndex(cur_mail - 2))->mail_post);
	PostMessage(WM_DISPLAYMAIL, 0, 0);    //������Ϣ���ý����ʼ�����
	if (cur_mail-1 == 0)  //������һ�ⰴť
	{
		CWnd *pwnd = GetDlgItem(IDC_BUTTON1);
		pwnd->EnableWindow(false);
	}
	//������һ�ⰴť
	CWnd *pwnd = GetDlgItem(IDC_BUTTON2);
	pwnd->EnableWindow(true);
}


void CMail_ServerDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString cur, total;
	cur_ctrl.GetWindowTextA(cur);
	total_ctrl.GetWindowTextA(total);
	int cur_mail = _ttoi(cur), total_mail = _ttoi(total);
	//������������
	if (total_mail == 0 || cur_mail == total_mail) return;
	//���½������ʼ���ű�ʶ
	cur.Format("%d", cur_mail + 1);
	cur_ctrl.SetWindowTextA(cur);
	//��ǰһ���ʼ��ı�����ʾ��չʾ����
	mail_text_ctrl.SetWindowTextA(mail_list.GetAt(mail_list.FindIndex(cur_mail))->mail_post);
	PostMessage(WM_DISPLAYMAIL, 0, 0);    //������Ϣ���ý����ʼ�����
	if (cur_mail+1 == total_mail)  //������һ�ⰴť
	{
		CWnd *pwnd = GetDlgItem(IDC_BUTTON2);
		pwnd->EnableWindow(false);
	}
	//������һ�ⰴť
	CWnd *pwnd = GetDlgItem(IDC_BUTTON1);
	pwnd->EnableWindow(true);
}
