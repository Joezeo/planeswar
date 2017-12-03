#pragma once
#include "windows.h"
#ifndef __GAME_H__
#define __GAME_H__

#ifdef __cplusplus
extern "C" {
#endif

//声明结构体
typedef enum {
	WELCOME,
	RUNING,
	OVER,
}GAMESTATUS;//游戏所处状态

typedef enum {
	BIG = 0,
	MIDDLE = 1,
	SMALL = 2,
}PLANETYPE;

typedef struct {
	GAMESTATUS		g_status;
	UINT			g_score;
	UINT			g_plNum;
	BOOL			g_bigAdd;
	UINT			g_pause_status;
	UINT			g_sound_status;
	UINT			g_backGround_status;
	UINT			g_backGround_couter;//用于调节背景动画播放速度
	UINT			g_bullet_counter;	//辅助调节子弹生成速度
}GAME;

typedef struct {
	PLANETYPE		p_type;
	SIZE			p_size;
	POINT			p_point;
	UINT			p_speed;
	UINT			p_maxHP;
	UINT			p_hitCount;
}PLANE;

typedef struct {
	UINT			t_speed;
	UINT			t_maxHP;
	SIZE			t_size;
}TYPEINFO;

typedef struct {
	HBITMAP hbmpPlane;
	UINT	moveSpeed;
	UINT	fireSpeed;
	POINT	postion;
	SIZE	size;
	UINT	hitCounter;
	UINT	maxHp;
}MYPLANE;

typedef struct bullet {
	HBITMAP hbmp;
	UINT	move_speed;
	POINT	bullet_pos;
	SIZE	bullet_size;
	struct bullet *next;
	struct bullet *pre;
}BULLET;

typedef struct {
	BULLET *head;
	BULLET *tail;
}BULLET_LINK;


#ifdef __cplusplus
}
#endif

#endif // __GAME_H__
