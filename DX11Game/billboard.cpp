//=============================================================================
//
// 2D���� [billboard.cpp]
//
//=============================================================================
#include "billboard.h"
#include "Camera.h"
//#include "shadow.h"
#include "Texture.h"
#include "mesh.h"
#include "Light.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TEXTURE_BILLBOARD	L"data/texture/wall00.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	BILLBOARD_WIDTH		(50.0f)					    // ��
#define	BILLBOARD_HEIGHT	(80.0f)						// ����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBillboard(ID3D11Device* pDevice);
void SetVertexBillboard(int idxBillboard, float width, float height);
void SetColorBillboard(int idxBillboard, XMFLOAT4 col);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static MESH				g_mesh;				// ���b�V�����
static MATERIAL			g_material;			// �}�e���A��
static TBillboard		g_billboard[MAX_BILLBOARD];	// �r���{�[�h���[�N

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBillboard(void)
{
	ID3D11Device* pDevice = GetDevice();

	// ���_���̍쐬
	MakeVertexBillboard(pDevice);

	// �e�N�X�`���̓ǂݍ���
	CreateTextureFromFile(pDevice,				// �f�o�C�X�ւ̃|�C���^
		TEXTURE_BILLBOARD,			// �t�@�C���̖��O
		&g_mesh.pTexture);	// �ǂݍ��ރ�����
	XMStoreFloat4x4(&g_mesh.mtxTexture, XMMatrixIdentity());

	g_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Power = 0.0f;
	g_mesh.pMaterial = &g_material;

	for (int cntBillboard = 0; cntBillboard < MAX_BILLBOARD; ++cntBillboard) {
		g_billboard[cntBillboard].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_billboard[cntBillboard].col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_billboard[cntBillboard].width = BILLBOARD_WIDTH;
		g_billboard[cntBillboard].height = BILLBOARD_HEIGHT;
		g_billboard[cntBillboard].use = false;
	}

		
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBillboard(void)
{
	for (int cntBillboard = 0; cntBillboard < MAX_BILLBOARD; ++cntBillboard) {
		if (g_billboard[cntBillboard].use) {
			//ReleaseShadow(g_billboard[cntBillboard].idxShadow);
			g_billboard[cntBillboard].idxShadow = -1;
			g_billboard[cntBillboard].use = false;
		}
	}
	// ���b�V���̊J��
	ReleaseMesh(&g_mesh);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBillboard(void)
{
	for (int cntBillboard = 0; cntBillboard < MAX_BILLBOARD; ++cntBillboard) {
		// ���g�p�Ȃ�X�L�b�v
		if (!g_billboard[cntBillboard].use) {
			continue;
		}
		// �e�̈ʒu�ݒ�
		//MoveShadow(g_billboard[cntBillboard].idxShadow, XMFLOAT3(g_billboard[cntBillboard].pos.x, 0.1f, g_billboard[cntBillboard].pos.z));
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBillboard(void)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxScale, mtxTranslate;

	CLight::Get()->SetDisable();	// ��������
	SetBlendState(BS_ALPHABLEND);	// ���u�����f�B���O�L��

	// �r���[�}�g���b�N�X���擾
	XMFLOAT4X4& mtxView = CCamera::Get()->GetViewMatrix();

	for (int cntBillboard = 0; cntBillboard < MAX_BILLBOARD; ++cntBillboard) {
		// ���g�p�Ȃ�X�L�b�v
		if (!g_billboard[cntBillboard].use) {
			continue;
		}
		// ���[���h �}�g���b�N�X������
		mtxWorld = XMMatrixIdentity();
		XMStoreFloat4x4(&g_mesh.mtxWorld, mtxWorld);

		// ��]�𔽉f
		g_mesh.mtxWorld._11 = mtxView._11;
		g_mesh.mtxWorld._12 = mtxView._21;
		g_mesh.mtxWorld._13 = mtxView._31;
		g_mesh.mtxWorld._21 = mtxView._12;
		g_mesh.mtxWorld._22 = mtxView._22;
		g_mesh.mtxWorld._23 = mtxView._32;
		g_mesh.mtxWorld._31 = mtxView._13;
		g_mesh.mtxWorld._32 = mtxView._23;
		g_mesh.mtxWorld._33 = mtxView._33;
		mtxWorld = XMLoadFloat4x4(&g_mesh.mtxWorld);

		// �X�P�[���𔽉f (��]����ɔ��f)
		mtxScale = XMMatrixScaling(g_billboard[cntBillboard].width, g_billboard[cntBillboard].height, 1.0f);
		mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_billboard[cntBillboard].pos.x, g_billboard[cntBillboard].pos.y, g_billboard[cntBillboard].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		XMStoreFloat4x4(&g_mesh.mtxWorld, mtxWorld);

		// �F�̐ݒ�
		g_material.Diffuse = g_billboard[cntBillboard].col;

		// �|���S���̕`��
		DrawMesh(pDeviceContext, &g_mesh);
	}

	//�w�肵���ꏊ�ɕ`��
	SetBillboard(XMFLOAT3(100.0f, 0.0f, 0.0f), 30.0f, 130.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetBillboard(XMFLOAT3(-100.0f, 0.0f, 0.0f), 30.0f, 130.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetBillboard(XMFLOAT3(0.0f, 120.0f, 30.0f), 230.0f, 30.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	SetBlendState(BS_NONE);		// ���u�����f�B���O����
	CLight::Get()->SetEnable();	// �����L��
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexBillboard(ID3D11Device* pDevice)
{
	// �ꎞ�I�Ȓ��_�z��𐶐�
	g_mesh.nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh.nNumVertex];

	// ���_�z��̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;

	// ���_���W�̐ݒ�
	pVtx[0].vtx = XMFLOAT3(-1.0f / 2.0f, 0.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-1.0f / 2.0f, 1.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f / 2.0f, 0.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3(1.0f / 2.0f, 1.0f, 0.0f);

	// �@���̐ݒ�
	pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// ���ˌ��̐ݒ�
	pVtx[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

	g_mesh.nNumIndex = 4;
	int* pIndexWk = new int[g_mesh.nNumIndex];
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh, pVertexWk, pIndexWk);

	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetVertexBillboard(int idxBillboard, float width, float height)
{
	if (idxBillboard >= 0 && idxBillboard < MAX_BILLBOARD) {
		g_billboard[idxBillboard].width = width;
		g_billboard[idxBillboard].height = height;
	}
}

//=============================================================================
// ���_�J���[�̐ݒ�
//=============================================================================
void SetColorBillboard(int idxBillboard, XMFLOAT4 col)
{
	if (idxBillboard >= 0 && idxBillboard < MAX_BILLBOARD) {
		g_billboard[idxBillboard].col = col;
	}
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
int SetBillboard(XMFLOAT3 pos, float width, float height, XMFLOAT4 col)
{
	int idxBillboard = -1;

	for (int cntBillboard = 0; cntBillboard < MAX_BILLBOARD; ++cntBillboard) {
		// �g�p���Ȃ�X�L�b�v
		if (g_billboard[cntBillboard].use) {
			continue;
		}
		g_billboard[cntBillboard].use = true;
		g_billboard[cntBillboard].pos = pos;

		// ���_���W�̐ݒ�
		SetVertexBillboard(cntBillboard, width, height);

		// ���_�J���[�̐ݒ�
		SetColorBillboard(cntBillboard, col);

		// �e�̐ݒ�
		//g_billboard[cntBillboard].idxShadow = CreateShadow(g_billboard[cntBillboard].pos, g_billboard[cntBillboard].width * 0.5f);

		idxBillboard = cntBillboard;
		break;
	}

	return idxBillboard;
}
