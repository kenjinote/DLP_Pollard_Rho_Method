#pragma comment(linker,"/opt:nowin98")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"ntl.lib")
#include <NTL/ZZ.h>
#include <windows.h>

#define IDU_BUTTON1 100

CHAR szClassName[]="window";

#define G1 0
#define G2 1
#define G3 2

typedef struct List_{
	ZZ beta;
	ZZ x;
	ZZ y;
}List;

ZZ alpha,gamma,prime;

int group(ZZ n)
{
	switch(n%3){
	case 0:return G1;
	case 1:return G2;
	case 2:return G3;
	}
}

List f(List num)
{
	switch(group(num.beta)){
	case G1:
		num.beta = (gamma * num.beta) % prime;
		num.x = (num.x + 1) % (prime-1);
		num.y = num.y;
		return num;
	case G2:
		num.beta = (num.beta * num.beta) % prime;
		num.x = (2 * num.x) % (prime-1);
		num.y = (2 * num.y) % (prime-1);
		return num;
	case G3:
		num.beta = (alpha * num.beta) % prime;
		num.x = num.x;
		num.y = (num.y + 1) % (prime-1);
		return num;
	}
}

ZZ find_x(ZZ a, ZZ b)
{
	ZZ m;
	ZZ c, d, e, f;
	ZZ x, r;
	ZZ tmp;
	m = prime-1;
	c = a;
	e = b;
	d = m;
	f = 0;
	while ((r = c % d) != 0) {
		x = c / d;
		c = d;
		d = r;
		tmp = f;
		f = e - x * f;
		e = tmp;
	}
	if (b % d != to_ZZ(0))return to_ZZ(-1);
	x = (f/d) % (m/d);
	while(PowerMod(gamma, x, prime) != alpha) x += m/d;
	return x;
}

ZZ pollard_rho(CHAR*szAlpha,CHAR*szGamma,CHAR*szPrime)
{
	alpha = to_ZZ(szAlpha);
	gamma = to_ZZ(szGamma);
	prime = to_ZZ(szPrime);
	ZZ x;
	int count = 0, i = 0;
	List a1, a2;
	do{
		ZZ x0 = to_ZZ(rand());
		a1.beta = PowerMod(gamma, x0, prime);
		a1.x = x0;
		a1.y = to_ZZ(0);
		a2 = f(a1);
		while(a1.beta != a2.beta){
			if(count == 1<<i){a1 = a2; i++;}
			a2 = f(a2);
			count++;
		}
	}while((x=find_x(a2.y-a1.y,a1.x-a2.x))==-1);
	return x;
}

long ZZ_len(const ZZ& n)
{
	long i;
	ZZ num;
	num=abs(n);
	for(i=0;num!=0;i++)num/=10;
	return (n>0)?i:i+1;
}

void ZZtostr(char* str,const ZZ& n,int len)
{
	ZZ num;
	num=abs(n);
	str[len--]='\0';
	do{
		str[len--]='0'+(char)(num%10);
		num/=10;
	}while(num!=0);
	if(n<0)str[0]='-';
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static HWND hEdit1,hEdit2,hEdit3,hEdit4,hEdit5,hButton;
	CHAR *str1,*str2,*str3,*str4,*str5;
	int len;
	ZZ ans;
	DWORD time;
	switch(msg){
	case WM_CREATE:
		CreateWindow("STATIC","αの値：",WS_CHILD|WS_VISIBLE,10,10,130,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		hEdit1=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","123456",WS_CHILD|WS_VISIBLE,150,10,400,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		CreateWindow("STATIC","γの値(原始根)：",WS_CHILD|WS_VISIBLE,10,50,130,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		hEdit2=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","7",WS_CHILD|WS_VISIBLE,150,50,400,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		CreateWindow("STATIC","Ｐの値(素数)：",WS_CHILD|WS_VISIBLE,10,90,130,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		hEdit3=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","2147483647",WS_CHILD|WS_VISIBLE,150,90,400,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		CreateWindow("STATIC","答え：",WS_CHILD|WS_VISIBLE,10,130,130,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		hEdit4=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",NULL,WS_CHILD|WS_VISIBLE,150,130,400,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		CreateWindow("STATIC","計算時間：",WS_CHILD|WS_VISIBLE,10,170,130,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		hEdit5=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",NULL,WS_CHILD|WS_VISIBLE,150,170,400,28,hWnd,NULL,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		hButton=CreateWindow("BUTTON","計算開始",WS_CHILD|WS_VISIBLE,10,210,150,28,hWnd,(HMENU)IDU_BUTTON1,((LPCREATESTRUCT)lParam)->hInstance,NULL);
		break;
	case WM_GETMINMAXINFO:
		{// 窓の最小サイズを設定
			MINMAXINFO* lpMMI=(MINMAXINFO*)lParam;
			lpMMI->ptMinTrackSize.x=570;
			lpMMI->ptMinTrackSize.y=290;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDU_BUTTON1:
			EnableWindow(hButton,0);
			len=GetWindowTextLength(hEdit1)+1;str1=(char *)malloc(len);GetWindowText(hEdit1,str1,len);
			len=GetWindowTextLength(hEdit2)+1;str2=(char *)malloc(len);GetWindowText(hEdit2,str2,len);
			len=GetWindowTextLength(hEdit3)+1;str3=(char *)malloc(len);GetWindowText(hEdit3,str3,len);
			time=timeGetTime();ans=pollard_rho(str1,str2,str3);time=timeGetTime()-time;
			len=ZZ_len(ans);str4=(char *)malloc(len+1);ZZtostr(str4,ans,len);SetWindowText(hEdit4,str4);
			len=16;str5=(char *)malloc(len+1);wsprintf(str5,"%d msec",time);SetWindowText(hEdit5,str5);
			free(str1);free(str2);free(str3);free(str4);free(str5);
			EnableWindow(hButton,1);
			break;
		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return(DefWindowProc(hWnd,msg,wParam,lParam));
	}
	return(0L);
}

int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hPreInst,
				   LPSTR pCmdLine,int nCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndclass;
	if(!hPreInst){
		wndclass.style=CS_HREDRAW|CS_VREDRAW;
		wndclass.lpfnWndProc=WndProc;
		wndclass.cbClsExtra=0;
		wndclass.cbWndExtra=0;
		wndclass.hInstance =hinst;
		wndclass.hIcon=NULL;
		wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
		wndclass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
		wndclass.lpszMenuName=NULL;
		wndclass.lpszClassName=szClassName;
		if(!RegisterClass(&wndclass))
			return FALSE;
	}
	hWnd=CreateWindow(szClassName,
		"Pollard's Rho Method を使って離散対数問題を解く",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hinst,
		NULL);
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(msg.wParam);
}

