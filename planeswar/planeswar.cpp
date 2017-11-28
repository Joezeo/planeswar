//planeswar.cpp: 定义应用程序的入口点。
//
#include "stdafx.h"
#include "planeswar.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLANESWAR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANESWAR));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIGICON));
    wcex.hCursor        = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MYCURSOR));
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中
   int cxScreen = GetSystemMetrics(SM_CXSCREEN);
   int cyScreen = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindowW(
	   szWindowClass, 
	   szTitle, 
	   WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
       (cxScreen - WNDWIDTH) / 2, 
	   (cyScreen - WNDHEIGHT - 50) / 2, 
	   WNDWIDTH, 
	   WNDHEIGHT + 35, 
	   nullptr, 
	   nullptr, 
	   hInstance, 
	   nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND		 - 处理应用程序菜单
//  WM_PAINT		 - 绘制主窗口，游戏界面
//  WM_DESTROY		 - 发送退出消息并返回
//	WM_CREATE		 - 创建窗口时初始化
//	WM_LBUTTONDOWN	 - 鼠标左键按下，在不同场景有不同作用
//	WM_START		 - 自定义消息，场景二开始的标识
//	WM_TIMER		 - 计时器事件，用于定时更新界面，检测飞机信息和位置
//	WM_ERASEBKGND	 - 窗口重绘时产生的消息，用于解决屏幕闪烁的问题
//	WM_PLAYACKSOUND	 - 自定义事件，在切换场景时产生的消息，用于控制不同场景的背景音乐播放
//	WM_KEYDOWN		 - 检测方向键是否被按下，用于玩家飞机的控制
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP *backGrouds;
	static BULLET_LINK bullet_link;
    switch (message)
    {
	case WM_CREATE: {
		initgame(lParam, hWnd, &backGrouds, &bullet_link);
	}
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		drawSenceUI(hdc, backGrouds, &bullet_link);
		EndPaint(hWnd, &ps);
	}
        break;
	case WM_LBUTTONDOWN: {
		lButtondownProc(hWnd, lParam);
	}
		break;
	case WM_START: {
		SetTimer(hWnd, TIMER, 50, NULL);
		updatePlaneInfo(TRUE, 0);
		SendMessage(hWnd, WM_PLAYBACKSOUND, 0, 0);
		SendMessage(hWnd, WM_PAINT, 0, 0);
	}
		break;
	case WM_TIMER: {
		timerProc(hWnd, &bullet_link);
	}
		break;
	case WM_ERASEBKGND://窗口必须重绘时产生消息，return 0 不擦除背景，return 非 0 擦除背景
		return FALSE;
	case WM_PLAYBACKSOUND: {
		playBackSound();
	}
		break;
	case WM_KEYDOWN: {
		moveMyplane(wParam, hWnd);
	}
		break;
	case WM_DESTROY: {
		freeItem(backGrouds);
		PostQuitMessage(0);
	}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//	自定义函数
//
//  声明在planeswar.h头文件中
//

/*
	init/free模块：
*/
VOID initgame(LPARAM lParam, HWND hwnd, HBITMAP **backGrounds, BULLET_LINK *bullet_link) {// 初始化游戏的全局变量...（需要更换掉全局变量）
	BITMAP bmp;
	UINT flag = 0;
	for (int i = 0; i < BMPCOUNT; i++) {
		hbBmp[i] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(uiBmpName[i]));
	}
	*backGrounds = (HBITMAP *)malloc(24 * sizeof(HBITMAP));
	if (!backGrounds) exit(-2);
	for (int i = 14; i < 38; i++) {
		*(*(backGrounds)+flag) = hbBmp[i];
		flag++;
	}

	GetObject(hbBmp[10], sizeof(BITMAP), &bmp);
	myplane.size.cx = bmp.bmWidth;
	myplane.size.cy = bmp.bmHeight;
	myplane.postion.x = WNDWIDTH / 2 - bmp.bmWidth / 2;
	myplane.postion.y = WNDHEIGHT - 100;
	myplane.moveSpeed = 5;
	myplane.fireSpeed = 10;
	myplane.hbmpPlane = hbBmp[10];

	s_game.g_status = WELCOME;
	s_game.g_score = 0;
	s_game.g_bigAdd = FALSE;
	s_game.g_plNum = 10;
	s_game.g_pause_status = 0;//0为未暂停
	s_game.g_sound_status = 1;//0为关闭音乐
	s_game.g_backGround_status = 0;//当前绘制第几张背景图
	s_game.g_backGround_couter = 0;
	s_game.g_bullet_counter = 0;

	GetObject(hbBmp[1], sizeof(BITMAP), &bmp);
	typeinfo[BIG].t_maxHP = 3;
	typeinfo[BIG].t_speed = 1;
	typeinfo[BIG].t_size.cx = bmp.bmWidth;
	typeinfo[BIG].t_size.cy = bmp.bmHeight / 4;	// 裁剪，一张 bmp 图内有四张小图

	GetObject(hbBmp[5], sizeof(BITMAP), &bmp);
	typeinfo[MIDDLE].t_maxHP = 2;
	typeinfo[MIDDLE].t_speed = 2;
	typeinfo[MIDDLE].t_size.cx = bmp.bmWidth;
	typeinfo[MIDDLE].t_size.cy = bmp.bmHeight / 3;	// 裁剪，一张 bmp 图内有三张小图

	GetObject(hbBmp[7], sizeof(BITMAP), &bmp);
	typeinfo[SMALL].t_maxHP = 1;
	typeinfo[SMALL].t_speed = 3;
	typeinfo[SMALL].t_size.cx = bmp.bmWidth;
	typeinfo[SMALL].t_size.cy = bmp.bmHeight / 2;	// 裁剪，一张 bmp 图内有两张小图

	bullet_link->head = nullptr;
	bullet_link->tail = nullptr;

	SendMessage(hwnd, WM_PLAYBACKSOUND, 0, 0);

}
VOID freeItem(HBITMAP *backGrouds) {
	free(backGrouds);
	backGrouds = nullptr;
}

/*
	sence模块：
*/
VOID drawSenceUI(HDC hdc, HBITMAP *backGrouds, BULLET_LINK *bullet_link) {// 分不同的场景，画出不同的界面 //
	HBITMAP hBmpMem;
	HDC hdcMem, hdcTmp;
	static BITMAP bmp;
	
	// 双缓冲法绘图，解决重绘时闪烁的问题 
	// 创建与hDC环境相关的设备兼容的位图内存画布 
	hBmpMem = CreateCompatibleBitmap(hdc, WNDWIDTH, WNDHEIGHT);
	// 创建与hdc相兼容的内存dc 
	hdcMem = CreateCompatibleDC(hdc);
	SelectObject(hdcMem, hBmpMem);
	// hDcTmp是另一个临时的内存dc，用来储存部件，如背景，飞机，按钮等等 
	hdcTmp = CreateCompatibleDC(hdc);

	//画出背景图
	drawbackground(hdcMem, hdcTmp, backGrouds, bmp);

	switch (s_game.g_status) {
	case WELCOME:
		//TODO:welcome场景下的界面绘制
		drawSenceI(hdcMem, hdcTmp, hbBmp, bmp);
		break;
	case RUNING:
		//TODO:runing场景下的界面绘制
		drawSenceII(hdcMem, hdcTmp, hbBmp, bmp, bullet_link);	
		break;
	case OVER:
		//TODO:over场景下的界面绘制
		drawSenceIII(hdcMem, hdcTmp, hbBmp, bmp);
		break;
	}
	BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcMem, 0, 0, SRCCOPY);
	
	DeleteObject(hBmpMem);
	DeleteDC(hdcMem);
	DeleteDC(hdcTmp);
}
VOID drawbackground(HDC hdcMem, HDC hdcTmp, HBITMAP *backGrounds, BITMAP bmp) {// 画背景，在三个场景都需要做的事情 //
	//画背景
	SelectObject(hdcTmp, *(backGrounds + s_game.g_backGround_status));
	BitBlt(hdcMem, 0, 0, WNDWIDTH, WNDHEIGHT,
		hdcTmp, 0, 0, SRCCOPY);
	if (s_game.g_status == RUNING) {
		s_game.g_backGround_couter++;
		if (s_game.g_backGround_couter == 2) {
			s_game.g_backGround_couter = 0;
			s_game.g_backGround_status++;
		}	
	}
	if (s_game.g_backGround_status == 24) {
		s_game.g_backGround_status = 0;
	}
	//画背景音乐开关按钮
	int tmp = 11;
	if (s_game.g_sound_status == 0)
		tmp = 12;
	SelectObject(hdcTmp, hbBmp[tmp]);
	GetObject(hbBmp[tmp], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 330, 0, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));

}
VOID drawSenceI(HDC hdcMem, HDC hdcTmp, HBITMAP *hbBmp, BITMAP bmp) {// 画场景一，包括logo，开始按钮 //
	//画logo
	SelectObject(hdcTmp, hbBmp[4]);
	GetObject(hbBmp[4], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 35, 150, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
	//画start button
	SelectObject(hdcTmp, hbBmp[8]);
	GetObject(hbBmp[8], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 125, 380, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
}
VOID drawSenceII(HDC hdcMem, HDC hdcTmp, HBITMAP *hbBmp, BITMAP bmp, BULLET_LINK *bullet_link) {// 画场景二，包括敌机，己方飞机，开始/暂停按钮 //
	for (UINT i = 0; i < s_game.g_plNum; i++) {
		switch (planes[i].p_type) {
		case BIG:
			SelectObject(hdcTmp, hbBmp[1]);
			break;
		case MIDDLE:
			SelectObject(hdcTmp, hbBmp[5]);
			break;
		case SMALL:
			SelectObject(hdcTmp, hbBmp[7]);
			break;
		default:
			assert(0);
		}
		TransparentBlt(hdcMem, 
			planes[i].p_point.x, planes[i].p_point.y, 
			planes[i].p_size.cx, planes[i].p_size.cy,
			hdcTmp, 0, (planes[i].p_hitCount) * (planes[i].p_size.cy),
			planes[i].p_size.cx, planes[i].p_size.cy,
			RGB(255, 255, 255));	
	}
	//画我的飞机
	SelectObject(hdcTmp, myplane.hbmpPlane);
	TransparentBlt(hdcMem,
		myplane.postion.x, myplane.postion.y,
		myplane.size.cx, myplane.size.cy,
		hdcTmp, 0, 0,
		myplane.size.cx, myplane.size.cy, RGB(255, 255, 255));

	//画play/pause button
	SelectObject(hdcTmp, hbBmp[9]);
	GetObject(hbBmp[9], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight / 2,
		hdcTmp, 0, s_game.g_pause_status * bmp.bmHeight / 2, bmp.bmWidth, bmp.bmHeight / 2, RGB(255, 255, 255));

	//画子弹
	drawBullet(bullet_link, hdcTmp, hdcMem);
}
VOID drawSenceIII(HDC hdcMem, HDC hdcTmp, HBITMAP *hbBmp, BITMAP bmp) {// 画场景三，包括分数框，分数，退出按钮，重新开始按钮 //
	LOGFONT lf;
	HFONT	hf;
	RECT	rt;
	TCHAR	strScore[10];
	//画restart button
	SelectObject(hdcTmp, hbBmp[6]);
	GetObject(hbBmp[6], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 95, 320, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
	//画exit button
	SelectObject(hdcTmp, hbBmp[2]);
	GetObject(hbBmp[2], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 95, 380, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
	//画分数框
	SelectObject(hdcTmp, hbBmp[3]);
	GetObject(hbBmp[3], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 35, 70, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
	//画分数
	lf.lfHeight = 30;
	lf.lfWidth = 30;
	lf.lfEscapement = 0;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = CHINESEBIG5_CHARSET;

	hf = CreateFontIndirect(&lf);
	SelectObject(hdcMem, hf);

	rt.left = 75;
	rt.top = 140;
	rt.right = 300;
	rt.bottom = 260;

	_itot_s(s_game.g_score, strScore, 10);
	SetBkMode(hdcMem, TRANSPARENT);
	DrawText(hdcMem, strScore, -1, &rt, DT_CENTER);
}

/*
	message event模块：
*/
VOID lButtondownProc(HWND hwnd, LPARAM lParam) {// 鼠标左键按下事件，在不同的场景有不同的作用 //
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);
	if (checkbutton_sound_on_off(ptMouse)) {
		if (s_game.g_sound_status) {
			s_game.g_sound_status = 0;
		}
		else {
			s_game.g_sound_status = 1;
		}
		InvalidateRect(hwnd, NULL, TRUE);
	}
	switch (s_game.g_status) {
	case WELCOME:
		// TODO:welcome场景下的鼠标事件
		if (checkbutton_start(ptMouse)) {
			s_game.g_status = RUNING;
			SendMessage(hwnd, WM_START, 0, 0);
		}
		break;
	case RUNING:
		// TODO:runing场景下的鼠标事件
		if (checkbutton_pause_play(ptMouse)) {
			if (s_game.g_pause_status) {
				SetTimer(hwnd, TIMER, 50, NULL);
				s_game.g_pause_status = 0;
			}
			else {
				KillTimer(hwnd, TIMER);
				s_game.g_pause_status = 1;
			}
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	case OVER:
		// TODO:over场景下的鼠标事件
		if (checkbutton_restart(ptMouse)) {
			s_game.g_score = 0;
			s_game.g_status = RUNING;
			SendMessage(hwnd, WM_START, 0, 0);
		}
		if (checkbutton_exit(ptMouse)) {
			SendMessage(hwnd, WM_DESTROY, 0, 0);
		}
		break;
	}
}
BOOL checkbutton_start(POINT ptMouse) {// 检测开始按钮是否被按下，如是，返回TRUE //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[8], sizeof(BITMAP), &bmp);
	rect.left = 125;
	rect.top = 380;
	rect.right = bmp.bmWidth + rect.left;
	rect.bottom = bmp.bmHeight + rect.top;
	return PtInRect(&rect, ptMouse);
}
BOOL checkbutton_restart(POINT ptMouse) {// 检测重新开始按钮是否被按下，如是，返回TRUE //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[6], sizeof(BITMAP), &bmp);
	rect.left = 95;
	rect.top = 320;
	rect.right = rect.left + bmp.bmWidth;
	rect.bottom = rect.top + bmp.bmHeight;
	return PtInRect(&rect, ptMouse);
}
BOOL checkbutton_exit(POINT ptMouse) {// 检测退出按钮是否被按下，如是，返回TRUE //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[2], sizeof(BITMAP), &bmp);
	rect.left = 95;
	rect.top = 380;
	rect.right = rect.left + bmp.bmWidth;
	rect.bottom = rect.top + bmp.bmHeight;
	return PtInRect(&rect, ptMouse);
}
BOOL checkbutton_pause_play(POINT ptMouse) {// 检测暂停/开始按钮区域是否被按下，如是，返回TRUE //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[9], sizeof(BITMAP), &bmp);
	rect.left = 0;
	rect.top = 0;
	rect.right = rect.left + bmp.bmWidth;
	rect.bottom = rect.top + bmp.bmHeight / 2;
	return PtInRect(&rect, ptMouse);
}
BOOL checkbutton_sound_on_off(POINT ptMouse) {
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[11], sizeof(BITMAP), &bmp);
	rect.left = 330;
	rect.top = 0;
	rect.right = rect.left + bmp.bmWidth;
	rect.bottom = rect.top + bmp.bmHeight;
	return PtInRect(&rect, ptMouse);
}
VOID timerProc(HWND hwnd, BULLET_LINK *bullet_link) {// Timer计时器所触发的事件，每单位时间检测执行一次checkPlaneInfo，checkPlanePos，
													 //移动敌机，重绘屏幕 
	//敌机模块：
	checkPlaneInfo();
	if (checkPlanePos()) {
		s_game.g_status = OVER;
		SendMessage(hwnd, WM_PLAYBACKSOUND, 0, 0);
		KillTimer(hwnd, TIMER);
	}
	for (UINT i = 0; i < s_game.g_plNum; i++) {
		planes[i].p_point.y += planes[i].p_speed;
	}
	//子弹模块：
	s_game.g_bullet_counter++;
	if ((s_game.g_bullet_counter == myplane.fireSpeed || bullet_link->head == nullptr) 
			&& (myplane.postion.y >= 5)){
		createBullet(bullet_link);
		s_game.g_bullet_counter = 0;
	}
	moveBullet(bullet_link);
	BULLET *tmp1 = checkBulletPos(bullet_link);
	BULLET *tmp2 = checkBulletInfo(bullet_link);
	if ( tmp1 != nullptr) {
		freeBullet(bullet_link, tmp1);
	}
	if (tmp2 != nullptr) {
		freeBullet(bullet_link, tmp2);
	}

	InvalidateRect(hwnd, NULL, TRUE);	
}

/*
	enime模块
*/
VOID updatePlaneInfo(BOOL bReset, UINT pIndex) { // 更新飞机的信息	bRest：是否是初次更新飞机信息 pIndex：需要更新飞机信息的飞机的下标 //
	UINT begin, end;

	if (bReset) {
		begin = 0;
		end = s_game.g_plNum;
	}
	else {
		begin = pIndex;
		end = pIndex + 1;
	}

	srand(GetTickCount());
	for ( UINT i = begin; i < end; i++) {
		UINT idex;
		idex = rand() % 3;
		while (idex == 0 && s_game.g_bigAdd) {
			idex = rand() % 3;
		}
		if (idex == 0 && !s_game.g_bigAdd) {
			s_game.g_bigAdd = TRUE;
		}
		switch (idex) {
		case BIG: {
			//srand(GetTickCount());
			planes[i].p_maxHP = typeinfo[BIG].t_maxHP;
			planes[i].p_speed = typeinfo[BIG].t_speed;
			planes[i].p_size = typeinfo[BIG].t_size;
			planes[i].p_hitCount = 0;
			planes[i].p_type = BIG;
			(planes[i].p_point).x = rand() % (WNDWIDTH - (planes[i].p_size).cx);
			(planes[i].p_point).y = -(rand() % 110 + 100);//- (plane->p_size).cy - rand() % ((plane->p_size).cy * (4 - idex));
		}
			 break;
		case MIDDLE: {
			//srand(GetTickCount());
			planes[i].p_maxHP = typeinfo[MIDDLE].t_maxHP;
			planes[i].p_speed = typeinfo[MIDDLE].t_speed;
			planes[i].p_size = typeinfo[MIDDLE].t_size;
			planes[i].p_hitCount = 0;
			planes[i].p_type = MIDDLE;
			(planes[i].p_point).x = rand() % (WNDWIDTH - (planes[i].p_size).cx);
			(planes[i].p_point).y = -(rand() % 350 + 80);//-(plane->p_size).cy - rand() % ((plane->p_size).cy * (4 - idex));

		}
			break;
		case SMALL: {
			//srand(GetTickCount());
			planes[i].p_maxHP = typeinfo[SMALL].t_maxHP;
			planes[i].p_speed = typeinfo[SMALL].t_speed;
			planes[i].p_size = typeinfo[SMALL].t_size;
			planes[i].p_hitCount = 0;
			planes[i].p_type = SMALL;
			(planes[i].p_point).x = rand() % (WNDWIDTH - (planes[i].p_size).cx);
			(planes[i].p_point).y = -(rand() % 470 + 30);//-(plane->p_size).cy - rand() % ((plane->p_size).cy * (4 - idex));
		}
			break;
		}
	}
}
VOID checkPlaneInfo() { // 检查飞机信息，如果飞机被击数等于最大生命，updatePlaneInfo //
	for (UINT i = 0; i < s_game.g_plNum; i++) {
		if (planes[i].p_hitCount == planes[i].p_maxHP) {
			if (planes[i].p_type == BIG) {
				s_game.g_bigAdd = FALSE;
			}
			switch (planes[i].p_type) {
			case BIG:
				s_game.g_score += 5;
				break;
			case MIDDLE:
				s_game.g_score += 3;
				break;
			case SMALL:
				s_game.g_score += 1;
				break;
			}
			updatePlaneInfo(FALSE, i);
		}
	}
}
BOOL checkPlanePos() {// 检查飞机位置，如果飞机到达屏幕底部，updatePlaneInfo //
	for (UINT i = 0; i < s_game.g_plNum; i++) {
		if (planes[i].p_point.y > WNDHEIGHT - 15) {
			if (planes[i].p_type == BIG)
				s_game.g_bigAdd = FALSE;
			updatePlaneInfo(FALSE, i);
			return FALSE;
		}
	}
	return FALSE;
}

/*
	sound模块：
*/
VOID playBackSound() {// 播放背景音乐 //
	switch (s_game.g_status) {
	case WELCOME:
		if (s_game.g_sound_status) {
			PlaySound(
				MAKEINTRESOURCE(IDR_WELSOUND),
				GetModuleHandle(NULL),
				SND_RESOURCE | SND_ASYNC | SND_LOOP);
		}
		break;
	case RUNING: {
		if (s_game.g_sound_status) {
			PlaySound(
				MAKEINTRESOURCE(IDR_RUNSOUND),
				GetModuleHandle(NULL),
				SND_RESOURCE | SND_ASYNC | SND_LOOP);
		}
	}
		break;
	case OVER: {
		if (s_game.g_sound_status) {
			PlaySound(
				MAKEINTRESOURCE(IDR_OVERS),
				GetModuleHandle(NULL),
				SND_RESOURCE | SND_ASYNC | SND_LOOP);
		}
	}
		break;
	}
}

/*
	myplane模块：
*/
VOID moveMyplane(WPARAM wParam, HWND hWnd) {// 玩家控制己方飞机移动 //
	if (s_game.g_pause_status) {
		return;
	}
	switch (wParam) {
	case VK_UP:
		if (myplane.postion.y < 0) return;
		myplane.postion.y -= myplane.moveSpeed;
		break;
	case VK_DOWN:
		if (myplane.postion.y > WNDHEIGHT - 90) return;
		myplane.postion.y += myplane.moveSpeed;
		break;
	case VK_LEFT:
		if (myplane.postion.x < 0) return;
		myplane.postion.x -= myplane.moveSpeed;
		break;
	case VK_RIGHT:
		if (myplane.postion.x > WNDWIDTH - myplane.size.cx - 15) return;
		myplane.postion.x += myplane.moveSpeed;
		break;
	}
	//SendMessage(hWnd, WM_PAINT, 0, 0);
}

/*
	bullet模块：
*/
VOID createBullet(BULLET_LINK *bullet_link) {// 生成子弹 //
	BITMAP bmp;
	BULLET *newBullet = (BULLET *)malloc(sizeof(BULLET));
	if (!newBullet)	exit(-2);
	GetObject(hbBmp[38], sizeof(BITMAP), &bmp);
	(newBullet->bullet_size).cx = bmp.bmWidth;
	(newBullet->bullet_size).cy = bmp.bmHeight;
	(newBullet->bullet_pos).x = myplane.postion.x + myplane.size.cx / 2 - 5;
	(newBullet->bullet_pos).y = myplane.postion.y - 19;
	newBullet->move_speed = 4;
	newBullet->next = nullptr;
	newBullet->pre = nullptr;
	newBullet->hbmp = hbBmp[38];
	if (bullet_link->head == nullptr) {
		bullet_link->head = newBullet;
		bullet_link->tail = newBullet;
		return;
	}
	(bullet_link->tail)->next = newBullet;
	newBullet->pre = bullet_link->tail;
	bullet_link->tail = newBullet;
}
VOID drawBullet(BULLET_LINK *bullet_link, HDC hdcTmp, HDC hdcMem) {// 画出子弹 //
	BULLET *p = bullet_link->head;
	while (p != nullptr) {
		SelectObject(hdcTmp, p->hbmp);
		TransparentBlt(hdcMem,
			(p->bullet_pos).x, (p->bullet_pos).y,
			(p->bullet_size).cx, (p->bullet_size).cy,
			hdcTmp, 0, 0,
			(p->bullet_size).cx, (p->bullet_size).cy, RGB(255, 255, 255));
		p = p->next;
	}
}
VOID moveBullet(BULLET_LINK *bullet_link) {// 子弹移动 //
	BULLET *p = bullet_link->head;
	while (p != nullptr) {
		(p->bullet_pos).y -= p->move_speed;
		p = p->next;
	}
}
BULLET* checkBulletPos(BULLET_LINK *bullet_link) {// 因为在head位的子弹一定是在最前面，故检查head位即可，出界返回出界飞机的地址 //
	if (bullet_link->head == nullptr) {
		return nullptr;
	}
	if (((bullet_link->head)->bullet_pos).y < -60) {
		return bullet_link->head;
	}
	return nullptr;
}
BULLET* checkBulletInfo(BULLET_LINK *bullet_link) {// 判断子弹是否击中敌机 现在就不能只检查head位的子弹了 //
	BULLET *p = bullet_link->head;
	RECT rect;
	POINT point;
	while (p != nullptr) {
		point.x = (p->bullet_pos).x / 2;
		point.y = (p->bullet_pos).y;
		for (UINT i = 0; i < s_game.g_plNum; i++) {
			rect.left = planes[i].p_point.x;
			rect.top = planes[i].p_point.y;
			rect.right = rect.left + planes[i].p_size.cx;
			rect.bottom = rect.top + planes[i].p_size.cy;
			if (PtInRect(&rect, point) && (planes[i].p_point.y > - 30 && planes[i].p_point.y < WNDHEIGHT)) {
				return p;
			}
		}
		p = p->next;
	}
	return nullptr;
}
VOID freeBullet(BULLET_LINK *bullet_link, BULLET *bul) {	
	if (bullet_link->head == bullet_link->tail) {
		bullet_link->head = nullptr;
		bullet_link->tail = nullptr;
		free(bul);
		bul = nullptr;
		return;
	}
	if (bul == bullet_link->head) {
		bullet_link->head = bul->next;
		if(bullet_link->head != nullptr)
			(bullet_link->head)->pre = nullptr;
		free(bul);
		bul = nullptr;
		return;
	}
	if (bul == bullet_link->tail) {
		bullet_link->tail = bul->pre;
		if (bullet_link->tail != nullptr)
			(bullet_link->tail)->next = nullptr;
		free(bul);
		bul = nullptr;
		return;
	}
	assert(bul->pre);
	if(bul->pre != nullptr)
		(bul->pre)->next = bul->next;
	if(bul->next != nullptr)
		(bul->next)->pre = bul->pre;
	free(bul);
	bul = nullptr;
}