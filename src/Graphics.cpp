#include <d3dx9.h>
#include <d3d9.h>
#include <windows.h>   // include important windows stuff
#include <windowsx.h> 

#include "Graphics.h"
#include "rmxftmpl.h"
#include "rmxfguid.h"

 
//****** Globals

extern HWND main_window_handle;

IDirect3D9 * pID3D                   = NULL;
IDirect3DDevice9 * pID3DDevice       = NULL;
ID3DXSprite      *m_pSprite;

D3DPRESENT_PARAMETERS present;
D3DDISPLAYMODE d3ddm;


BOOL EnableAlphaTesting(BOOL Enable);
 
//******  DirectX Graphics Class For Preparing The Device 


HRESULT cGraphics::Init3D(HWND hwnd,long height,long width,bool bwindowed)
{
	pID3D = Direct3DCreate9
		(D3D_SDK_VERSION);

	HRESULT hr;
	do
	{
		// we need the display mode so we can get
		// the properties of our back buffer
		hr = pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
		if(FAILED(hr))
			break;

		ZeroMemory(&present, sizeof(present));
		present.Windowed= bwindowed;
		present.BackBufferWidth=height;
		present.BackBufferHeight=width;
		present.BackBufferCount=1;
		present.BackBufferFormat = d3ddm.Format;
		present.SwapEffect=D3DSWAPEFFECT_FLIP;
		present.EnableAutoDepthStencil = true;
		present.AutoDepthStencilFormat = D3DFMT_D16;
		d3ddm.Width=width;
		d3ddm.Height=height;
		d3ddm.RefreshRate=0;
		d3ddm.Format=D3DFMT_R5G6B5;


		hr = pID3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hwnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&present,
			&pID3DDevice);

		if(FAILED(hr))
			break;
 
		// Enable texture rendering stages and filter types
		//    hr=pID3DDevice ->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

		/* // Enable texture rendering stages and filter types
		pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

		pID3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pID3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		*/
		EnableAlphaTesting(FALSE);
		///  
		SetAmbientLight(255,255,255);

	} while(0);

	return hr;
}


void cGraphics::Free3D()
{
	ReleaseCOM(pID3DDevice);
	ReleaseCOM(pID3D);
}

bool cGraphics::Clear()
{
	pID3DDevice->Clear( 0, 0, D3DCLEAR_TARGET| D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 100 ), 1.0f, 0 );
	return true;
}

bool cGraphics::BeginDraw()
{

	pID3DDevice->BeginScene();


	return true;
}

bool cGraphics::EndDraw()
{
	pID3DDevice->EndScene();

	return true;
}

bool cGraphics::Draw()
{
	pID3DDevice->Present(NULL, NULL, NULL, NULL);

	return true;
}

IDirect3DDevice9 * cGraphics::GetDeviceCOM()
{

	return pID3DDevice; 
}


BOOL cGraphics::EnableZBuffer(BOOL Enable)
{
	if(pID3DDevice  == NULL)
		return FALSE;

	if(FAILED(pID3DDevice ->SetRenderState(D3DRS_ZENABLE, (Enable == TRUE) ? D3DZB_TRUE : D3DZB_FALSE)))
		return FALSE;

	return TRUE;
}
BOOL cGraphics::SetPerspective(float FOV, float Aspect, float Near, float Far)
{
	D3DXMATRIX matProjection;

	if(pID3DDevice  == NULL)
		return FALSE;

	D3DXMatrixPerspectiveFovLH(&matProjection, FOV, Aspect, Near, Far);
	if(FAILED(pID3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection)))
		return FALSE;
	return TRUE;
}

BOOL cGraphics::SetAmbientLight(char Red, char Green, char Blue)
{
	D3DCOLOR Color;
	char             m_AmbientRed; 
	char              m_AmbientGreen;
	char              m_AmbientBlue;


	Color = D3DCOLOR_XRGB((m_AmbientRed = Red), (m_AmbientGreen = Green), (m_AmbientBlue = Blue));
	if(FAILED(pID3DDevice->SetRenderState(D3DRS_AMBIENT, Color)))
		return FALSE;

	return TRUE;
}
 
BOOL cGraphics::SetTexture(short Num, cTexture *Texture)
{
	// Error checking
	if( pID3DDevice == NULL || Num < 0 || Num > 7)
		return FALSE;

	if(Texture == NULL) {
		// Clear the texture
		if(FAILED( pID3DDevice->SetTexture(Num, NULL)))
			return NULL;
	} else {
		// Set the texture
		if(FAILED( pID3DDevice->SetTexture(Num, Texture->GetTextureCOM())))
			return FALSE;
	}

	return TRUE;
}


BOOL cGraphics::BeginSprite()
{
	if(m_pSprite == NULL)
		return FALSE;

	//if(FAILED(m_pSprite->Begin()))
	//	return FALSE;

	return TRUE;
}

BOOL cGraphics::EndSprite()
{
	if(m_pSprite == NULL)
		return FALSE;

	if(FAILED(m_pSprite->End()))
		return FALSE;

	return TRUE;
}



BOOL EnableAlphaTesting(BOOL Enable)
{


	if(FAILED(pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, Enable)))
		return FALSE;

	// Set test type
	if(Enable == TRUE) {
		pID3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
		pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}

	return TRUE;
}

BOOL cGraphics::EnableAlphaBlending(BOOL Enable , DWORD Src , DWORD Dest)
{
	if(pID3DDevice == NULL)
		return FALSE;

	// Enable or disable
	if(FAILED(pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, Enable)))
		return FALSE;

	// Set blend type
	if(Enable == TRUE) {
		pID3DDevice->SetRenderState(D3DRS_SRCBLEND,  Src);
		pID3DDevice->SetRenderState(D3DRS_DESTBLEND, Dest);
	}

	return TRUE;
}
cWorldPosition::cWorldPosition()
{
	m_Billboard = FALSE;
	m_matCombine1 = m_matCombine2 = NULL;

	Move(0.0f,0.0f,0.0f);
	Rotate(0.0f,0.0f,0.0f);
	Scale(1.0f, 1.0f, 1.0f);

	Update();
}

BOOL cWorldPosition::Copy(cWorldPosition *DestPos)
{
	DestPos->Move(m_XPos, m_YPos, m_ZPos);
	DestPos->Rotate(m_XRotation, m_YRotation, m_ZRotation);
	DestPos->Scale(m_XScale, m_YScale, m_ZScale);
	DestPos->EnableBillboard(m_Billboard);

	return TRUE;
}

BOOL cWorldPosition::Move(float XPos, float YPos, float ZPos)
{
	m_XPos = XPos;
	m_YPos = YPos;
	m_ZPos = ZPos;

	D3DXMatrixTranslation(&m_matTranslation, m_XPos, m_YPos, m_ZPos);

	return TRUE;
}

BOOL cWorldPosition::MoveRel(float XAdd, float YAdd, float ZAdd)
{
	return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);
}

BOOL cWorldPosition::Rotate(float XRot, float YRot, float ZRot)
{
	m_XRotation = XRot;
	m_YRotation = YRot;
	m_ZRotation = ZRot;

	D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_YRotation, m_XRotation, m_ZRotation);

	return TRUE;
}

BOOL cWorldPosition::RotateRel(float XAdd, float YAdd, float ZAdd)
{
	return Rotate(m_XRotation + XAdd, m_YRotation + YAdd, m_ZRotation + ZAdd);
}

BOOL cWorldPosition::Scale(float XScale, float YScale, float ZScale)
{
	m_XScale = XScale;
	m_YScale = YScale;
	m_ZScale = ZScale;

	D3DXMatrixScaling(&m_matScale, XScale, YScale, ZScale);

	return TRUE;
}

BOOL cWorldPosition::ScaleRel(float XAdd, float YAdd, float ZAdd)
{
	return Scale(m_XScale + XAdd, m_YScale + YAdd, m_ZScale + ZAdd);
}

BOOL cWorldPosition::Update(cGraphics *Graphics)
{
	D3DXMATRIX matView, matTransposed;

	// Setup billboarding matrix
	if(m_Billboard == TRUE) {
		if(Graphics != NULL && Graphics->GetDeviceCOM() != NULL) {
			Graphics->GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);
			D3DXMatrixTranspose(&matTransposed, &matView);
			matTransposed._41 = matTransposed._42 = matTransposed._43 = matTransposed._14 = matTransposed._24 = matTransposed._34 = 0.0f;
		} else {
			D3DXMatrixIdentity(&matTransposed);
		}
	}

	// Combine scaling and rotation matrices first
	D3DXMatrixMultiply(&m_matWorld, &m_matScale, &m_matRotation);

	// Apply billboard matrix
	if(m_Billboard == TRUE)
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &matTransposed);  

	// Combine with translation matrix
	D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTranslation);

	// Combine with combined matrices (if any)
	if(m_matCombine1 != NULL) 
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine1);
	if(m_matCombine2 != NULL) 
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine2);

	return TRUE;
}

BOOL cWorldPosition::EnableBillboard(BOOL Enable)
{
	m_Billboard = Enable;
	return TRUE;
}

D3DXMATRIX *cWorldPosition::GetMatrix(cGraphics *Graphics)
{
	Update(Graphics);
	return &m_matWorld;
}

BOOL cWorldPosition::SetCombineMatrix1(D3DXMATRIX *Matrix)
{
	m_matCombine1 = Matrix;
	return TRUE;
}

BOOL cWorldPosition::SetCombineMatrix2(D3DXMATRIX *Matrix)
{
	m_matCombine2 = Matrix;
	return TRUE;
}

float cWorldPosition::GetXPos()
{
	return m_XPos;
}

float cWorldPosition::GetYPos()
{
	return m_YPos;
}

float cWorldPosition::GetZPos()
{
	return m_ZPos;
}

float cWorldPosition::GetXRotation()
{
	return m_XRotation;
}

float cWorldPosition::GetYRotation()
{
	return m_YRotation;
}

float cWorldPosition::GetZRotation()
{
	return m_ZRotation;
}

float cWorldPosition::GetXScale()
{
	return m_XScale;
}

float cWorldPosition::GetYScale()
{
	return m_YScale;
}

float cWorldPosition::GetZScale()
{
	return m_ZScale;
}
 
//***************************************************
// Class For Loading and Blitting Different types 
// of Images, .bmp,.jpg...etc 
 
// Ahmed S.Mohamed 2003

//***************************************************
 
cTexture::cTexture()
{
	m_Texture = NULL;
	m_Width = m_Height = 0;
}

cTexture::~cTexture()
{
	Free();
}

BOOL cTexture::Load( char *Filename)
{
	Free();


	if(Filename == NULL)
		return FALSE;
	if(FAILED(D3DXCreateSprite(pID3DDevice, &pSprite)))
		return FALSE;
	D3DXCreateTextureFromFileEx(pID3DDevice,Filename,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_A1R5G5B5,D3DPOOL_MANAGED,
		D3DX_FILTER_TRIANGLE,D3DX_FILTER_TRIANGLE,D3DCOLOR_RGBA(255,255,255,255),NULL,NULL,&m_Texture);


	m_Width = GetWidth();
	m_Height = GetHeight();

	return TRUE;
}

BOOL cTexture::Create(IDirect3DTexture9 *Texture)
{
	D3DLOCKED_RECT SrcRect, DestRect;
	D3DSURFACE_DESC Desc;

	Free();



	if(Texture == NULL)
		return TRUE;

	// Copy texture over
	Texture->GetLevelDesc(0, &Desc);
	m_Width  = Desc.Width;
	m_Height = Desc.Height;
	pID3DDevice->CreateTexture(m_Width, m_Height, 0, 0, Desc.Format, D3DPOOL_MANAGED, &m_Texture,NULL);

	Texture->LockRect(0, &SrcRect, NULL, D3DLOCK_READONLY);
	m_Texture->LockRect(0, &DestRect, NULL, 0);

	memcpy(DestRect.pBits, SrcRect.pBits, SrcRect.Pitch * m_Height);

	m_Texture->UnlockRect(0);
	Texture->UnlockRect(0);

	return TRUE;
}

BOOL cTexture::Free()
{
	ReleaseCOM(m_Texture);

	m_Width = m_Height = 0;

	return TRUE;
}

BOOL cTexture::IsLoaded()
{
	if(m_Texture == NULL)
		return FALSE;
	return TRUE;
}

IDirect3DTexture9 *cTexture::GetTextureCOM()
{
	return m_Texture;
}

long cTexture::GetWidth()
{
	D3DSURFACE_DESC d3dsd;

	if(m_Texture == NULL)
		return 0;

	if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return 0;

	return d3dsd.Width;
}

long cTexture::GetHeight()
{
	D3DSURFACE_DESC d3dsd;

	if(m_Texture == NULL)
		return 0;

	if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return 0;

	return d3dsd.Height;
}

D3DFORMAT cTexture::GetFormat()
{
	D3DSURFACE_DESC d3dsd;

	if(m_Texture == NULL)
		return D3DFMT_UNKNOWN;

	if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return D3DFMT_UNKNOWN;

	return d3dsd.Format;
}

BOOL cTexture::Blit(long DestX, long DestY,                   \
	long SrcX, long SrcY,                     \
	long Width, long Height,                  \
	float XScale, float YScale,               \
	D3DCOLOR Color)
{
	RECT Rect;

	if(m_Texture == NULL)
		return FALSE;


	if(!Width)
		Width = m_Width;
	if(!Height)
		Height = m_Height;

	Rect.left = SrcX;
	Rect.top  = SrcY;
	Rect.right = Rect.left + Width;
	Rect.bottom = Rect.top + Height;

	/*if(FAILED(pSprite->Draw(m_Texture,                          \
		&Rect, &D3DXVECTOR2(XScale, YScale),                 \
		NULL, 0.0f,                                          \
		&D3DXVECTOR2((float)DestX, (float)DestY),            \
		Color)))
		return FALSE;*/
	return TRUE;
}
//***************************************************
// 2D Font Engine For Drawing English-Arabic Texts. 



// * Ahmed S.Mohamed *

//***************************************************




cFont::cFont()
{
	m_Font = NULL;
}

cFont::~cFont()
{
	Free();
}



BOOL cFont::Create( char *Name, long Size, BOOL Bold, BOOL Italic, BOOL Underline, BOOL Strikeout)
{
	LOGFONT lf;

	if( Name == NULL)
		return FALSE;

	ZeroMemory(&lf, sizeof(LOGFONT));

	strcpy(lf.lfFaceName, Name);
	lf.lfHeight = -Size;
	lf.lfWeight = (Bold == TRUE) ? 700 : 0;
	lf.lfItalic = Italic;
	lf.lfUnderline = Underline;
	lf.lfStrikeOut = Strikeout;

	//  if(FAILED(D3DXCreateFontIndirect(pID3DDevice, &lf, &m_Font)))
	// return FALSE;
	return TRUE;
}

BOOL cFont::Free()
{
	ReleaseCOM(m_Font);
	return TRUE;
}

BOOL cFont::Begin()
{
	if(m_Font == NULL)
		return FALSE;
	// if(FAILED(m_Font->Begin()))
	//  return FALSE;
	return TRUE;
}

BOOL cFont::End()
{
	if(m_Font == NULL)
		return FALSE;
	//  if(FAILED(m_Font->End()))
	// return FALSE;
	return TRUE;
}

BOOL cFont::Print(char *Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color, DWORD Format)
{
	RECT Rect;

	if(m_Font == NULL)
		return FALSE;

	if(!Width)
		Width = 65535;
	if(!Height)
		Height = 65536;

	Rect.left   = XPos;
	Rect.top    = YPos;
	Rect.right  = Rect.left + Width;
	Rect.bottom = Rect.top + Height;
	//  if(FAILED(m_Font->DrawText(Text, -1, &Rect, Format, Color)))
	// return FALSE;
	return TRUE;
}

//***************************************************
// 2D Sprite Engine using different types of images .


// * Ahmed S.Mohamed *

//***************************************************



cSprite::~cSprite()
{
	// call shutdown function
	Shutdown();
}

// Arguments:
// pDevice - pointer to valid direct3d device object
// iNumFramesX - number of animation frames the texture contains in x direction
// iNumFramesY - number of animation frames the texture contains in y direction
// iFrameWidth - width (in pixels) of each animation frame
// iFrameHeight - height (in pixels) of each animation frame
// pTexture - pointer to valid direct3d texture object
HRESULT cSprite::Initialize(char *Filename)
{

	// initialize variables
	m_pd3dDevice = pID3DDevice;

	m_fLastTime = 0.0f;

	// create sprite object
	if(FAILED(D3DXCreateSprite(pID3DDevice, &m_pd3dxSprite)))
		return E_FAIL;

	if(FAILED(D3DXCreateTextureFromFileEx(pID3DDevice,
		Filename, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0,D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, 
		D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE,
		D3DCOLOR_RGBA(0,0,0,255), NULL, NULL, &m_pd3dtexTexture)))
		return FALSE;

	// return success
	return S_OK;
}

// Arguments:
// none
HRESULT cSprite::Shutdown()
{
	// if sprite object exists, release it
	if(m_pd3dxSprite != NULL)
	{
		m_pd3dxSprite->Release();
		m_pd3dxSprite = NULL;
	}

	// return success
	return S_OK;
}

// Arguments:
// vecScale - vector representing scale values in x and y directions
// vector <1.0f, 1.0f> should be passed if no scaling occurs
// vecRotationCenter - vector representing rotation center in x and y positions
// fAngle - angle in radians controlling the amount of rotation
// vecPosition - vector representing position of the upper left corner of the frame
// rendering position. Vector should contain pixel values, and not directx units.
// fTimeSinceLast - the time it takes to render one frame
HRESULT cSprite::Render(int iNumFramesX, int iFrameWidth, int iFrameHeight,int iCurrentFrameY,D3DXVECTOR2 *vecScale, D3DXVECTOR2 *vecRotationCenter, float fAngle, D3DXVECTOR2 *vecPosition, float fTimeSinceLast)
{

	static int nFrame=0;
	static float fAnimationTimer = 0.0f;
	m_iNumFramesX = iNumFramesX;
	m_iFrameWidth = iFrameWidth;
	m_iFrameHeight = iFrameHeight;
	m_iCurrentFrameY =iCurrentFrameY;

	RECT rTemp;
	rTemp.left = ((nFrame) * m_iFrameWidth);						// left frame coordinate
	rTemp.top =m_iCurrentFrameY;
	rTemp.right = rTemp.left + m_iFrameWidth;							// right coordinate is left one plus the width
	rTemp.bottom = rTemp.top + m_iFrameHeight;							// bottom coordinate is top one plus the height


	fAnimationTimer +=fTimeSinceLast;

	if( fAnimationTimer > 1.01f )
	{
		++nFrame;

		if( nFrame >m_iNumFramesX )

			nFrame = 0;


		fAnimationTimer = 0.0f;

	}

	// attempt to render only if the sprite object began successfully
	//if(SUCCEEDED(m_pd3dxSprite->Begin()))
	//{


	//	// render the current frame with the passed parameters
	//	//m_pd3dxSprite->Draw(m_pd3dtexTexture, &rTemp, NULL, vecRotationCenter, fAngle, vecPosition, 0xffffffff);
	//	// end rendering with the sprite object
	//	m_pd3dxSprite->End();




	//	// return success
	//	return S_OK;
	//}



	// return failture
	return E_FAIL;
}
///////////////////////////////////////////////////////////////////////////////
// End Definition /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


Camera::Camera()
{
	_cameraType = AIRCRAFT;

	_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

Camera::Camera(CameraType cameraType)
{
	_cameraType = cameraType;

	_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

Camera::~Camera()
{

}

void Camera::getPosition(D3DXVECTOR3* pos)
{
	*pos = _pos;
}

void Camera::setPosition(D3DXVECTOR3* pos)
{
	_pos = *pos;
}

void Camera::getRight(D3DXVECTOR3* right)
{
	*right = _right;
}

void Camera::getUp(D3DXVECTOR3* up)
{
	*up = _up;
}

void Camera::getLook(D3DXVECTOR3* look)
{
	*look = _look;
}

void Camera::walk(float units)
{
	// move only on xz plane for land object
	if( _cameraType == LANDOBJECT )
		_pos += D3DXVECTOR3(_look.x, 0.0f, _look.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _look * units;
}

void Camera::strafe(float units)
{
	// move only on xz plane for land object
	if( _cameraType == LANDOBJECT )
		_pos += D3DXVECTOR3(_right.x, 0.0f, _right.z) * units;

	if( _cameraType == AIRCRAFT )
		_pos += _right * units;
}

void Camera::fly(float units)
{
	if( _cameraType == AIRCRAFT )
		_pos += _up * units;
}

void Camera::pitch(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &_right,	angle);

	// rotate _up and _look around _right vector
	D3DXVec3TransformCoord(&_up,&_up, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);
}

void Camera::yaw(float angle)
{
	D3DXMATRIX T;

	// rotate around world y (0, 1, 0) always for land object
	if( _cameraType == LANDOBJECT )
		D3DXMatrixRotationY(&T, angle);

	// rotate around own up vector for aircraft
	if( _cameraType == AIRCRAFT )
		D3DXMatrixRotationAxis(&T, &_up, angle);

	// rotate _right and _look around _up or y-axis
	D3DXVec3TransformCoord(&_right,&_right, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);
}

void Camera::roll(float angle)
{
	// only roll for aircraft type
	if( _cameraType == AIRCRAFT )
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_look,	angle);

		// rotate _up and _right around _look vector
		D3DXVec3TransformCoord(&_right,&_right, &T);
		D3DXVec3TransformCoord(&_up,&_up, &T);
	}
}

void Camera::getViewMatrix(D3DXMATRIX* V)
{
	// Keep camera's axes orthogonal to eachother
	D3DXVec3Normalize(&_look, &_look);

	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);

	// Build the view matrix:
	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	(*V)(0,0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;
}

void Camera::setCameraType(CameraType cameraType)
{
	_cameraType = cameraType;
}







//******** FUNCTIONS ! **************



DWORD                start_clock_count = 0;     // used for timing

DWORD Get_Clock(void)
{
	// this function returns the current tick count

	// return time
	return(GetTickCount());

} // end Get_Clock

///////////////////////////////////////////////////////////

DWORD Start_Clock(void)
{
	// this function starts the clock, that is, saves the current
	// count, use in conjunction with Wait_Clock()

	return(start_clock_count = Get_Clock());

} // end Start_Clock

////////////////////////////////////////////////////////////

DWORD Wait_Clock(DWORD count)
{
	// this function is used to wait for a specific number of clicks
	// since the call to Start_Clock

	while((Get_Clock() - start_clock_count) < count);
	return(Get_Clock());

} // end Wait_Clock
long GetNextLong(FILE *fp)
{
	char Buf[1024];
	long Pos = 0;
	int c;

	// Read until EOF or EOL
	while(1) {
		if((c = fgetc(fp)) == EOF)
			break;
		if(c == 0x0a || (c == ' ' && Pos))
			break;
		if((c >= '0' && c <= '9') || c == '.' || c == '-')
			Buf[Pos++] = c;
	}
	if(!Pos)
		return -1;
	Buf[Pos] = 0;

	return atol(Buf);
}

FILE *fp_error;

int Close_Error_File(void)
{
	// this function closes the error file

	if (fp_error)
	{
		// write close file string
		Write_Error("\nClosing Error Output File.");

		if (fp_error!=stdout || fp_error!=stderr)
		{
			// close the file handle
			fclose(fp_error);
		} 

		fp_error = NULL;

		// return success
		return(1);
	} // end if
	else
		return(0);

} // end Close_Error_File

///////////////////////////////////////////////////////////

int Write_Error(char *string, ...)
{
	// this function prints out the error string to the error file

	char buffer[1024]; // working buffer

	va_list arglist; // variable argument list

	// make sure both the error file and string are valid
	if (!string || !fp_error)
		return(0);

	// print out the string using the variable number of arguments on stack
	va_start(arglist,string);
	vsprintf(buffer,string,arglist);
	va_end(arglist);

	// write string to file
	fprintf(fp_error,buffer);

	// flush buffer incase the system bails
	fflush(fp_error);

	// return success
	return(1);
} // end Write_Error
int Open_Error_File(char *filename, FILE *fp_override)
{
	// this function creates the output error file

	// is user requesting special file handle? stdout, stderr, etc.?
	if (fp_override)
	{
		fp_error = fp_override;
	}
	else
	{
		// test if this file is valid
		if ((fp_error = fopen(filename,"w"))==NULL)
			return(0);
	}

	// write out error header with time
	Write_Error("\nOpening Error Output File (%s) on %s\n",filename);

	// now the file is created, re-open with append mode

	if (!fp_override)
	{
		fclose(fp_error);
		if ((fp_error = fopen(filename,"a+"))==NULL)
			return(0);
	}

	// return success
	return(1);

} // end Open_Error_File



int RandomNumber(int iMin, int iMax)
{
	if (iMin == iMax) return(iMin);
	return((rand() % (abs(iMax-iMin)+1))+iMin);
}

float RandomNumber(float fMin, float fMax)
{
	if (fMin == fMax) return(fMin);
	float fRandom = (float)rand() / (float)RAND_MAX;
	return((fRandom * (float)fabs(fMax-fMin))+fMin);
}


