//=============================================================================
//
// �Ǐ��� [Wall.cpp]
//
//=============================================================================
#include "Wall.h"
#include "main.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "collision.h"
#include "player.h"
#include "explosion.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MODEL_WALL			"data/model/wall002.X"

#define MAX_LIFE			(2)			// �Ǒϋv�u
#define MAX_WALL			(256)		// �Ǎő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct TWall {
	XMFLOAT3	m_pos;		// ���݂̈ʒu
	XMFLOAT3	m_rot;		// ���݂̌���
	XMFLOAT3    m_size;		// ���݂̃T�C�Y
	XMFLOAT4X4	m_mtxWorld;	// ���[���h�}�g���b�N�X

	int			m_nLife;	// �ǂ̑ϋv�u
	bool		use;		// �g�p���Ă��邩

};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CAssimpModel	g_model;			// ���f��
static TWall		g_wall[MAX_WALL];	// �Ǐ��

//=============================================================================
// ����������
//=============================================================================
HRESULT InitWall(void)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	for (int i = 0; i < MAX_WALL; ++i)
	{ 
	g_wall[i].m_size= XMFLOAT3(38.0f, 38.0f, 38.0f);
	//g_wall->m_pos = XMFLOAT3(0.0f, 50.0f, 150.0f);
	g_wall[i].m_nLife = MAX_LIFE;
	g_wall[i].use = false;
	}

	// ���f���f�[�^�̓ǂݍ���
	if (!g_model.Load(pDevice, pDeviceContext, MODEL_WALL)) {
		MessageBoxA(GetMainWnd(), "���f���f�[�^�ǂݍ��݃G���[", "InitWall", MB_OK);
		return E_FAIL;
	}

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void UninitWall(void)
{
	// ���f���̉��
	g_model.Release();
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateWall(void)
{

	XMMATRIX mtxWorld, mtxRot, mtxTranslate;

	for (int i = 0; i < MAX_WALL; ++i) 
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(g_wall[i].m_rot.x),
			XMConvertToRadians(g_wall[i].m_rot.y),
			XMConvertToRadians(g_wall[i].m_rot.z));
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(
			g_wall[i].m_pos.x,
			g_wall[i].m_pos.y,
			g_wall[i].m_pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�ݒ�
		XMStoreFloat4x4(&g_wall[i].m_mtxWorld, mtxWorld);

	}

	for (int i = 0; i < MAX_WALL; ++i)
	{
		if (!g_wall[i].use)
		{// ���g�p�Ȃ玟��
			continue;
		}

		// �ǂƃv���C���[���Փ˂��Ă�����
		if (CollisionWall(g_wall[i].m_pos, g_wall[i].m_size, GetPlayerPos(), GetPlayerSize()))
		{

			StartExplosion(g_wall[i].m_pos, XMFLOAT2(80.0f, 80.0f));
			g_wall[i].m_nLife--;

			if(g_wall[i].m_nLife <= 0)
			{ 
			g_wall[i].use = false;
			}
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawWall(void)
{
	ID3D11DeviceContext* pDC = GetDeviceContext();

	
	// �s����������`��
	for (int i = 0; i < MAX_WALL; ++i) 
	{
		if (!g_wall[i].use)
		{
			continue;
		}
		g_model.Draw(pDC, g_wall[i].m_mtxWorld, eOpacityOnly);
	}

	// ������������`��
	for (int i = 0; i < MAX_WALL; ++i) {
		SetBlendState(BS_ALPHABLEND);	// �A���t�@�u�����h�L��
		SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�
		g_model.Draw(pDC, g_wall[i].m_mtxWorld, eTransparentOnly);
		SetZWrite(true);				// Z�o�b�t�@�X�V����
		SetBlendState(BS_NONE);			// �A���t�@�u�����h����
	}
	

}

//===============================================================================
//
//  �@�@�ǔz�u
//
//===============================================================================
int SetWall(XMFLOAT3 pos)
{
	int Wall = -1;

	for (int cntWall = 0; cntWall < MAX_WALL; ++cntWall) {
		// �g�p���Ȃ�X�L�b�v
		if (g_wall[cntWall].use) {
			continue;
		}
		g_wall[cntWall].use = true;
		g_wall[cntWall].m_pos = pos;

		Wall = cntWall;
		break;
	}

	return Wall;
}

// �ǈʒu�擾
XMFLOAT3 GetWallPos()
{
	return g_wall->m_pos;
}

// �ǃT�C�Y�擾
XMFLOAT3 GetWallSize()
{
	return g_wall->m_size;
}

//=============================================================================
// �Փ˔���
//=============================================================================
bool CollisionWall(XMFLOAT3 Apos, XMFLOAT3 Asize, XMFLOAT3 Bpos, XMFLOAT3 Bsize)
{

	return CollisionAABB(Apos, Asize, Bpos, Bsize);

}




