#include "game.h"
#include "stdafx.h"
#include <stdlib.h>
int uiBmpName[BMPCOUNT] = {						// 储存位图ID的数组
	IDB_BG1,		//0
	IDB_BIG,		//1
	IDB_EXIT,		//2
	IDB_GAMEOVER,	//3
	IDB_LOGO,		//4
	IDB_MIDDLE,		//5
	IDB_RESTART,	//6
	IDB_SMALL,		//7
	IDB_START,		//8
	IDB_PLAYORPAUSE,//9
	IDB_MYPLANE,	//10
	IDB_MUSICON,	//11
	IDB_MUSICOFF,	//12
	IDB_BG1,		//13
	IDB_BG1,		//14
	IDB_BG2,		//15
	IDB_BG3,		//16
	IDB_BG4,		//17
	IDB_BG5,		//18
	IDB_BG6,		//19
	IDB_BG7,		//20
	IDB_BG8,		//21
	IDB_BG9,		//22
	IDB_BG10,		//23
	IDB_BG11,		//24
	IDB_BG12,		//25
	IDB_BG13,		//26
	IDB_BG14,		//27
	IDB_BG15,		//28
	IDB_BG16,		//29
	IDB_BG17,		//30
	IDB_BG18,		//31
	IDB_BG19,		//32
	IDB_BG20,		//33
	IDB_BG21,		//34
	IDB_BG22,		//35
	IDB_BG23,		//36
	IDB_BG24,		//37
	IDB_BULLET,		//38
};
TYPEINFO typeinfo[3];			//三种类型飞机的信息

//	自定义函数
//
//  声明在planeswar.h头文件中
//

/*
init/free模块：
*/
VOID
init_items(LPARAM lParam, HWND hwnd, HBITMAP **backGrounds, BULLET_LINK *bullet_link,
	MYPLANE **myplane, PLANE **planes, GAME *s_game, HBITMAP *hbBmp) {
	// 初始化游戏的变量...
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

	*myplane = (MYPLANE *)malloc(sizeof(MYPLANE));
	GetObject(hbBmp[10], sizeof(BITMAP), &bmp);
	((*myplane)->size).cx = bmp.bmWidth;
	((*myplane)->size).cy = bmp.bmHeight;
	((*myplane)->postion).x = WNDWIDTH / 2 - bmp.bmWidth / 2;
	((*myplane)->postion).y = WNDHEIGHT - 100;
	(*myplane)->moveSpeed = 5;
	(*myplane)->fireSpeed = 10;
	(*myplane)->hbmpPlane = hbBmp[10];
	(*myplane)->hitCounter = 0;
	(*myplane)->maxHp = 5;

	s_game->g_status = WELCOME;
	s_game->g_score = 0;
	s_game->g_bigAdd = FALSE;
	s_game->g_plNum = 10;
	s_game->g_pause_status = 0;//0为未暂停
	s_game->g_sound_status = 1;//0为关闭音乐
	s_game->g_backGround_status = 0;//当前绘制第几张背景图
	s_game->g_backGround_couter = 0;
	s_game->g_bullet_counter = 0;

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

	bullet_link->head = NULL;
	bullet_link->tail = NULL;

	*planes = (PLANE *)malloc(MAXPLANENUM * sizeof(PLANE));

	SendMessage(hwnd, WM_PLAYBACKSOUND, 0, 0);

}
VOID
free_items(HBITMAP *backGrouds, BULLET_LINK *bullet_link, MYPLANE *myplane, PLANE *planes) {
	free(backGrouds);
	backGrouds = NULL;
	free_myplane(myplane);
	free_planes(planes);
	free_allBullet(bullet_link);
}
VOID
restart_game(HWND hwnd, BULLET_LINK *bullet_link, MYPLANE *myplane, GAME *s_game) {
	s_game->g_score = 0;
	s_game->g_status = RUNING;

	myplane->hitCounter = 0;
	(myplane->postion).x = WNDWIDTH / 2 - (myplane->size).cx / 2;
	(myplane->postion).y = WNDHEIGHT - 100;

	free_allBullet(bullet_link);
	SendMessage(hwnd, NULL, 0, 0);
}

/*
sence模块：
*/
VOID
draw_senceUI(HDC hdc, HBITMAP *backGrouds, BULLET_LINK *bullet_link,
	MYPLANE *myplane, PLANE *planes, GAME *s_game, HBITMAP *hbBmp) {// 分不同的场景，画出不同的界面 //
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
	draw_background(hdcMem, hdcTmp, backGrouds, bmp, s_game, hbBmp);

	switch (s_game->g_status) {
	case WELCOME:
		//TODO:welcome场景下的界面绘制
		draw_senceI(hdcMem, hdcTmp, hbBmp, bmp);
		break;
	case RUNING:
		//TODO:runing场景下的界面绘制
		draw_senceII(hdcMem, hdcTmp, hbBmp, bmp, bullet_link, myplane, planes, s_game);
		break;
	case OVER:
		//TODO:over场景下的界面绘制
		draw_senceIII(hdcMem, hdcTmp, hbBmp, bmp, s_game);
		break;
	}
	BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcMem, 0, 0, SRCCOPY);

	DeleteObject(hBmpMem);
	DeleteDC(hdcMem);
	DeleteDC(hdcTmp);
}
VOID
draw_background(HDC hdcMem, HDC hdcTmp, HBITMAP *backGrounds,
	BITMAP bmp, GAME *s_game, HBITMAP *hbBmp) {// 画背景，在三个场景都需要做的事情 //
											   //画背景
	SelectObject(hdcTmp, *(backGrounds + s_game->g_backGround_status));
	BitBlt(hdcMem, 0, 0, WNDWIDTH, WNDHEIGHT,
		hdcTmp, 0, 0, SRCCOPY);
	if (s_game->g_status == RUNING) {
		s_game->g_backGround_couter++;
		if (s_game->g_backGround_couter == 2) {
			s_game->g_backGround_couter = 0;
			s_game->g_backGround_status++;
		}
	}
	if (s_game->g_backGround_status == 24) {
		s_game->g_backGround_status = 0;
	}
	//画背景音乐开关按钮
	int tmp = 11;
	if (s_game->g_sound_status == 0)
		tmp = 12;
	SelectObject(hdcTmp, hbBmp[tmp]);
	GetObject(hbBmp[tmp], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 330, 0, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));

}
VOID
draw_senceI(HDC hdcMem, HDC hdcTmp, HBITMAP *hbBmp, BITMAP bmp) {// 画场景一，包括logo，开始按钮 //
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
VOID
draw_senceII(HDC hdcMem, HDC hdcTmp, HBITMAP *hbBmp,
	BITMAP bmp, BULLET_LINK *bullet_link, MYPLANE *myplane, PLANE *planes, GAME *s_game) {// 画场景二，包括敌机，己方飞机，开始/暂停按钮 //
	for (UINT i = 0; i < s_game->g_plNum; i++) {
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
	SelectObject(hdcTmp, myplane->hbmpPlane);
	TransparentBlt(hdcMem,
		(myplane->postion).x, (myplane->postion).y,
		(myplane->size).cx, (myplane->size).cy,
		hdcTmp, 0, 0,
		(myplane->size).cx, (myplane->size).cy, RGB(255, 255, 255));

	//画play/pause button
	SelectObject(hdcTmp, hbBmp[9]);
	GetObject(hbBmp[9], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight / 2,
		hdcTmp, 0, s_game->g_pause_status * bmp.bmHeight / 2, bmp.bmWidth, bmp.bmHeight / 2, RGB(255, 255, 255));

	//画子弹
	draw_bullet(bullet_link, hdcTmp, hdcMem);
}
VOID
draw_senceIII(HDC hdcMem, HDC hdcTmp, HBITMAP *hbBmp, BITMAP bmp, GAME *s_game) {// 画场景三，包括分数框，分数，退出按钮，重新开始按钮 //
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

	_itot_s(s_game->g_score, strScore, 10, 0);
	SetBkMode(hdcMem, TRANSPARENT);
	DrawText(hdcMem, strScore, -1, &rt, DT_CENTER);
}

/*
message event模块：
*/
VOID
proc_lButtondown(HWND hwnd, LPARAM lParam, BULLET_LINK *bullet_link,
	MYPLANE *myplane, GAME *s_game, HBITMAP *hbBmp) {// 鼠标左键按下事件，在不同的场景有不同的作用 //
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);
	if (check_button_sound_on_off(ptMouse, hbBmp)) {
		if (s_game->g_sound_status) {
			s_game->g_sound_status = 0;
		}
		else {
			s_game->g_sound_status = 1;
		}
		InvalidateRect(hwnd, NULL, TRUE);
	}
	switch (s_game->g_status) {
	case WELCOME:
		// TODO:welcome场景下的鼠标事件
		if (check_button_start(ptMouse, hbBmp)) {
			s_game->g_status = RUNING;
			SendMessage(hwnd, WM_START, 0, 0);
		}
		break;
	case RUNING:
		// TODO:runing场景下的鼠标事件
		if (check_button_pause_play(ptMouse, hbBmp)) {
			if (s_game->g_pause_status) {
				SetTimer(hwnd, TIMER, 50, NULL);
				s_game->g_pause_status = 0;
			}
			else {
				KillTimer(hwnd, TIMER);
				s_game->g_pause_status = 1;
			}
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	case OVER:
		// TODO:over场景下的鼠标事件
		if (check_button_restart(ptMouse, hbBmp)) {
			restart_game(hwnd, bullet_link, myplane, s_game);
		}
		if (check_button_exit(ptMouse, hbBmp)) {
			SendMessage(hwnd, WM_DESTROY, 0, 0);
		}
		break;
	}
}
BOOL
check_button_start(POINT ptMouse, HBITMAP *hbBmp) {// 检测开始按钮是否被按下，如是，返回TRUE //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[8], sizeof(BITMAP), &bmp);
	rect.left = 125;
	rect.top = 380;
	rect.right = bmp.bmWidth + rect.left;
	rect.bottom = bmp.bmHeight + rect.top;
	return PtInRect(&rect, ptMouse);
}
BOOL
check_button_restart(POINT ptMouse, HBITMAP *hbBmp) {// 检测重新开始按钮是否被按下，如是，返回TRUE //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[6], sizeof(BITMAP), &bmp);
	rect.left = 95;
	rect.top = 320;
	rect.right = rect.left + bmp.bmWidth;
	rect.bottom = rect.top + bmp.bmHeight;
	return PtInRect(&rect, ptMouse);
}
BOOL
check_button_exit(POINT ptMouse, HBITMAP *hbBmp) {// 检测退出按钮是否被按下，如是，返回TRUE //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[2], sizeof(BITMAP), &bmp);
	rect.left = 95;
	rect.top = 380;
	rect.right = rect.left + bmp.bmWidth;
	rect.bottom = rect.top + bmp.bmHeight;
	return PtInRect(&rect, ptMouse);
}
BOOL
check_button_pause_play(POINT ptMouse, HBITMAP *hbBmp) {// 检测暂停/开始按钮区域是否被按下，如是，返回TRUE //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[9], sizeof(BITMAP), &bmp);
	rect.left = 0;
	rect.top = 0;
	rect.right = rect.left + bmp.bmWidth;
	rect.bottom = rect.top + bmp.bmHeight / 2;
	return PtInRect(&rect, ptMouse);
}
BOOL
check_button_sound_on_off(POINT ptMouse, HBITMAP *hbBmp) {// 检测声音按钮是否被按下 //
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[11], sizeof(BITMAP), &bmp);
	rect.left = 330;
	rect.top = 0;
	rect.right = rect.left + bmp.bmWidth;
	rect.bottom = rect.top + bmp.bmHeight;
	return PtInRect(&rect, ptMouse);
}
VOID
proc_timer(HWND hwnd, BULLET_LINK *bullet_link, MYPLANE *myplane,
	PLANE *planes, GAME *s_game, HBITMAP *hbBmp) {// Timer计时器所触发的事件，
												  //每单位时间检测执行一次check_plane_info，check_plane_pos，
												  //移动敌机，重绘屏幕 
												  //敌机模块：
	check_plane_info(planes, s_game);
	check_plane_pos(planes, s_game);
	if (check_myplane_crash(myplane, planes, s_game)) {
		myplane->hitCounter++;
	}
	if (myplane->hitCounter == myplane->maxHp) {
		s_game->g_status = OVER;
		SendMessage(hwnd, WM_PLAYBACKSOUND, 0, 0);
		KillTimer(hwnd, TIMER);
	}
	for (UINT i = 0; i < s_game->g_plNum; i++) {
		planes[i].p_point.y += planes[i].p_speed;
	}
	//子弹模块：
	s_game->g_bullet_counter++;
	if ((s_game->g_bullet_counter == myplane->fireSpeed || bullet_link->head == NULL)
		&& (myplane->postion.y >= 5)) {// 生成子弹的条件（子弹为空 || 每firespeed单位时间）
		create_bullet(bullet_link, myplane, hbBmp);
		s_game->g_bullet_counter = 0;
	}
	move_bullet(bullet_link);
	BULLET *tmp1 = check_bullet_pos(bullet_link);
	BULLET *tmp2 = check_bullet_info(bullet_link, planes, s_game);
	if (tmp1 != NULL) {
		free_bullet(bullet_link, tmp1);
	}
	if (tmp2 != NULL) {
		free_bullet(bullet_link, tmp2);
	}

	InvalidateRect(hwnd, NULL, TRUE);
}

/*
enime模块
*/
VOID
update_plane_info(BOOL bReset, UINT pIndex, PLANE *planes, GAME *s_game) { // 更新飞机的信息	bRest：是否是初次更新飞机信息 pIndex：需要更新飞机信息的飞机的下标 //
	UINT begin, end;

	if (bReset) {
		begin = 0;
		end = s_game->g_plNum;
	}
	else {
		begin = pIndex;
		end = pIndex + 1;
	}

	srand(GetTickCount());
	for (UINT i = begin; i < end; i++) {
		UINT idex;
		idex = rand() % 3;
		while (idex == 0 && s_game->g_bigAdd) {
			idex = rand() % 3;
		}
		if (idex == 0 && !s_game->g_bigAdd) {
			s_game->g_bigAdd = TRUE;
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
VOID
check_plane_info(PLANE *planes, GAME *s_game) { // 检查飞机信息，如果飞机被击数等于最大生命，update_plane_info //
	for (UINT i = 0; i < s_game->g_plNum; i++) {
		if (planes[i].p_hitCount == planes[i].p_maxHP) {
			if (planes[i].p_type == BIG) {
				s_game->g_bigAdd = FALSE;
			}
			switch (planes[i].p_type) {
			case BIG:
				s_game->g_score += 5;
				break;
			case MIDDLE:
				s_game->g_score += 3;
				break;
			case SMALL:
				s_game->g_score += 1;
				break;
			}
			update_plane_info(FALSE, i, planes, s_game);
		}
	}
}
VOID
check_plane_pos(PLANE *planes, GAME *s_game) {// 检查飞机位置，如果飞机到达屏幕底部，update_plane_info //
	for (UINT i = 0; i < s_game->g_plNum; i++) {
		if (planes[i].p_point.y > WNDHEIGHT - 15) {
			if (planes[i].p_type == BIG)
				s_game->g_bigAdd = FALSE;
			update_plane_info(FALSE, i, planes, s_game);
		}
	}
}
VOID
free_planes(PLANE *planes) {// 释放planes资源 //
	free(planes);
}

/*
sound模块：
*/
VOID
play_backsound(GAME *s_game) {// 播放背景音乐 //
	switch (s_game->g_status) {
	case WELCOME:
		if (s_game->g_sound_status) {
			PlaySound(
				MAKEINTRESOURCE(IDR_WELSOUND),
				GetModuleHandle(NULL),
				SND_RESOURCE | SND_ASYNC | SND_LOOP);
		}
		break;
	case RUNING: {
		if (s_game->g_sound_status) {
			PlaySound(
				MAKEINTRESOURCE(IDR_RUNSOUND),
				GetModuleHandle(NULL),
				SND_RESOURCE | SND_ASYNC | SND_LOOP);
		}
	}
				 break;
	case OVER: {
		if (s_game->g_sound_status) {
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
VOID
move_myplane(WPARAM wParam, HWND hWnd, MYPLANE *myplane, GAME *s_game) {// 玩家控制己方飞机移动 //
	if (s_game->g_pause_status) {
		return;
	}
	switch (wParam) {
	case VK_UP:
		if (myplane->postion.y < 0) return;
		myplane->postion.y -= myplane->moveSpeed;
		break;
	case VK_DOWN:
		if (myplane->postion.y > WNDHEIGHT - 90) return;
		myplane->postion.y += myplane->moveSpeed;
		break;
	case VK_LEFT:
		if (myplane->postion.x < 0) return;
		myplane->postion.x -= myplane->moveSpeed;
		break;
	case VK_RIGHT:
		if (myplane->postion.x > WNDWIDTH - myplane->size.cx - 15) return;
		myplane->postion.x += myplane->moveSpeed;
		break;
	}
	//SendMessage(hWnd, WM_PAINT, 0, 0);
}
BOOL
check_myplane_crash(MYPLANE *myplane, PLANE *planes, GAME *s_game) {// 检查敌机和玩家飞机是否相撞 //
	POINT pt;
	RECT rect;
	for (UINT i = 0; i < s_game->g_plNum; i++) {
		pt.x = planes[i].p_point.x + planes[i].p_size.cx / 2;
		pt.y = planes[i].p_point.y + planes[i].p_size.cy;
		rect.left = myplane->postion.x;
		rect.top = myplane->postion.y;
		rect.right = rect.left + myplane->size.cx;
		rect.bottom = rect.top + myplane->size.cy;
		if (PtInRect(&rect, pt)) {
			planes[i].p_hitCount = planes[i].p_maxHP;
			return TRUE;
		}
	}
	return FALSE;
}
VOID
free_myplane(MYPLANE *myplane) {
	free(myplane);
	myplane = NULL;
}

/*
bullet模块：
*/
VOID
create_bullet(BULLET_LINK *bullet_link, MYPLANE *myplane, HBITMAP *hbBmp) {// 生成子弹 //
	BITMAP bmp;
	BULLET *newBullet = (BULLET *)malloc(sizeof(BULLET));
	if (!newBullet)	exit(-2);
	GetObject(hbBmp[38], sizeof(BITMAP), &bmp);
	(newBullet->bullet_size).cx = bmp.bmWidth;
	(newBullet->bullet_size).cy = bmp.bmHeight;
	(newBullet->bullet_pos).x = myplane->postion.x + myplane->size.cx / 2 - 5;
	(newBullet->bullet_pos).y = myplane->postion.y - 19;
	newBullet->move_speed = 4;
	newBullet->next = NULL;
	newBullet->pre = NULL;
	newBullet->hbmp = hbBmp[38];
	if (bullet_link->head == NULL) {
		bullet_link->head = newBullet;
		bullet_link->tail = newBullet;
		return;
	}
	(bullet_link->tail)->next = newBullet;
	newBullet->pre = bullet_link->tail;
	bullet_link->tail = newBullet;
}
VOID
draw_bullet(BULLET_LINK *bullet_link, HDC hdcTmp, HDC hdcMem) {// 画出子弹 //
	BULLET *p = bullet_link->head;
	while (p != NULL) {
		SelectObject(hdcTmp, p->hbmp);
		TransparentBlt(hdcMem,
			(p->bullet_pos).x, (p->bullet_pos).y,
			(p->bullet_size).cx, (p->bullet_size).cy,
			hdcTmp, 0, 0,
			(p->bullet_size).cx, (p->bullet_size).cy, RGB(255, 255, 255));
		p = p->next;
	}
}
VOID
move_bullet(BULLET_LINK *bullet_link) {// 子弹移动 //
	BULLET *p = bullet_link->head;
	while (p != NULL) {
		(p->bullet_pos).y -= p->move_speed;
		p = p->next;
	}
}
BULLET*
check_bullet_pos(BULLET_LINK *bullet_link) {// 因为在head位的子弹一定是在最前面，故检查head位即可，出界返回出界飞机的地址 //
	if (bullet_link->head == NULL) {
		return NULL;
	}
	if (((bullet_link->head)->bullet_pos).y < -150) {
		return bullet_link->head;
	}
	return NULL;
}
BULLET*
check_bullet_info(BULLET_LINK *bullet_link, PLANE *planes, GAME *s_game) {// 判断子弹是否击中敌机 现在就不能只检查head位的子弹了 //
	BULLET *p = bullet_link->head;
	RECT rect;
	POINT point;
	while (p != NULL) {
		point.x = (p->bullet_pos).x + (p->bullet_size).cx / 2;
		point.y = (p->bullet_pos).y;
		for (UINT i = 0; i < s_game->g_plNum; i++) {
			rect.left = planes[i].p_point.x;
			rect.top = planes[i].p_point.y;
			rect.right = rect.left + planes[i].p_size.cx;
			rect.bottom = rect.top + planes[i].p_size.cy;

			if (PtInRect(&rect, point) && planes[i].p_point.y > -110) {
				planes[i].p_hitCount++;
				if (planes[i].p_hitCount > planes[i].p_maxHP) {
					planes[i].p_hitCount = planes[i].p_maxHP;
				}
				return p;
			}
		}
		p = p->next;
	}
	return NULL;
}
VOID
free_bullet(BULLET_LINK *bullet_link, BULLET *bul) {
	if (bullet_link->head == bullet_link->tail) {
		bullet_link->head = NULL;
		bullet_link->tail = NULL;
		free(bul);
		bul = NULL;
		return;
	}
	if (bul == bullet_link->head) {
		bullet_link->head = bul->next;
		if (bullet_link->head != NULL)
			(bullet_link->head)->pre = NULL;
		free(bul);
		bul = NULL;
		return;
	}
	if (bul == bullet_link->tail) {
		bullet_link->tail = bul->pre;
		if (bullet_link->tail != NULL)
			(bullet_link->tail)->next = NULL;
		free(bul);
		bul = NULL;
		return;
	}
	assert(bul->pre);
	if (bul->pre != NULL)
		(bul->pre)->next = bul->next;
	if (bul->next != NULL)
		(bul->next)->pre = bul->pre;
	free(bul);
	bul = NULL;
}
VOID
free_allBullet(BULLET_LINK *bullet_link) {// 重新开始游戏时，释放所有子弹资源 //
	BULLET *p = bullet_link->head;
	BULLET *tmp = NULL;
	while (p != NULL) {
		tmp = p->next;
		free(p);
		p = tmp;
	}
	bullet_link->head = NULL;
	bullet_link->tail = NULL;
}