//=============================================================================
//
// �Ǐ��� [Wall.h]
//
//=============================================================================
#pragma once

#include "main.h"

#define MAX_WALL (256)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitWall(void);
void UninitWall(void);
void UpdateWall(void);
void DrawWall(void);

int SetWall(XMFLOAT3 pos);

bool CollisionWall(XMFLOAT3 Apos, XMFLOAT3 Asize, XMFLOAT3 Bpos, XMFLOAT3 Bsize);


XMFLOAT3 GetWallPos();
XMFLOAT3 GetWallSize();


