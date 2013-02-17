
#include <d3dx9.h>
#include <d3d9.h>
#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <vector>
#include "string.h"
 #include <stdexcept>


class cGraphics;
 
class Camera;
class cTexture;
class cFont;
class cSprite;

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }
extern HINSTANCE hinstance_app      ; // globally track hinstance
extern HWND main_window_handle;

	



//---------------------------------------------------------------------------------

class Camera
{
public:
	enum CameraType { LANDOBJECT, AIRCRAFT };

	Camera();
	Camera(CameraType cameraType);
	~Camera();

	void strafe(float units); // left/right
	void fly(float units);    // up/down
	void walk(float units);   // forward/backward
	
	void pitch(float angle); // rotate on right vector
	void yaw(float angle);   // rotate on up vector
	void roll(float angle);  // rotate on look vector

	void getViewMatrix(D3DXMATRIX* V); 
	void setCameraType(CameraType cameraType); 
	void getPosition(D3DXVECTOR3* pos); 
	void setPosition(D3DXVECTOR3* pos); 

	void getRight(D3DXVECTOR3* right);
	void getUp(D3DXVECTOR3* up);
	void getLook(D3DXVECTOR3* look);
private:
	CameraType  _cameraType;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;
};

class cTexture
{
  protected:
  
    IDirect3DTexture9 *m_Texture;
    unsigned long      m_Width, m_Height;

  public:
    cTexture();
    ~cTexture();

    IDirect3DTexture9 *GetTextureCOM();
  ID3DXSprite *pSprite;

    BOOL Load( char *Filename);
    BOOL Create( IDirect3DTexture9 *Texture);
    BOOL Free();

    BOOL      IsLoaded();

    long      GetWidth();
    long      GetHeight();
    D3DFORMAT GetFormat();

    BOOL Blit(long DestX, long DestY,                         \
              long SrcX = 0, long SrcY = 0,                   \
              long Width = 0, long Height = 0,                \
              float XScale = 1.0f, float YScale = 1.0f,       \
              D3DCOLOR Color = 0xFFFFFFFF);
};

class cWorldPosition
{
  protected:
    BOOL  m_Billboard;

    float m_XPos,      m_YPos,      m_ZPos;
    float m_XRotation, m_YRotation, m_ZRotation;
    float m_XScale,    m_YScale,    m_ZScale;
    
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matScale;
    D3DXMATRIX m_matRotation;
    D3DXMATRIX m_matTranslation;
    D3DXMATRIX *m_matCombine1;
    D3DXMATRIX *m_matCombine2;

  public:
    cWorldPosition();

    D3DXMATRIX *GetMatrix(cGraphics *Graphics = NULL);
    BOOL SetCombineMatrix1(D3DXMATRIX *Matrix = NULL);
    BOOL SetCombineMatrix2(D3DXMATRIX *Matrix = NULL);

    BOOL Copy(cWorldPosition *DestPos);

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd);

    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd);

    BOOL Scale(float XScale, float YScale, float ZScale);
    BOOL ScaleRel(float XAdd, float YAdd, float ZAdd);

    BOOL Update(cGraphics *Graphics = NULL);
    BOOL EnableBillboard(BOOL Enable = TRUE);

    float GetXPos();
    float GetYPos();
    float GetZPos();
    float GetXRotation();
    float GetYRotation();
    float GetZRotation();
    float GetXScale();
    float GetYScale();
    float GetZScale();
};
class cFont
{
  private:
    ID3DXFont *m_Font;

  public:
    cFont();
    ~cFont();

    ID3DXFont *GetFontCOM();

    BOOL Create(char *Name, long Size = 16, BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE);
    BOOL Free();

    BOOL Begin();
    BOOL End();
    BOOL Print(char *Text, long XPos, long YPos, long Width = 0, long Height = 0, D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0);
};
class cSprite
{
public:
	// default destructor
	~cSprite();

	// initialization method that is called from constructor, but also can be called manually
	HRESULT Initialize(char *Filename);
	// shutdown method that is called from the destructor, but also can be called manually
	HRESULT Shutdown();
	
	// rendering method used to render the animation
	HRESULT Render(int iNumFramesX, int iFrameWidth, int iFrameHeight,int iCurrentFrameY,D3DXVECTOR2 *vecScale, D3DXVECTOR2 *vecRotationCenter, float fAngle, D3DXVECTOR2 *vecPosition, float fTimeSinceLast);
	
public:
	IDirect3DTexture9 *m_pd3dtexTexture;			// stored pointer to direct3d texture object
	IDirect3DDevice9 *m_pd3dDevice;					// stored pointer to direct3d device object
	ID3DXSprite *m_pd3dxSprite;						// pointer to d3dxsprite object

	int m_iCurrentFrameX;							// current frame in x direction
	int m_iCurrentFrameY;							// current frame in y direction

	float m_fLastTime;								// time since last frame change

	int m_iNumFramesX;								// number of frames in x direction
	int m_iFrameWidth;								// width of each frame
	int m_iFrameHeight;								// height of each frame
};
 

class cVertexBuffer
{
  private:
    cGraphics              *m_Graphics;
    IDirect3DVertexBuffer9 *m_pVB;

    DWORD                   m_NumVertices;
    DWORD                   m_VertexSize;
    DWORD                   m_FVF;

    BOOL                    m_Locked;
    char                   *m_Ptr;

  public:
    cVertexBuffer();
    ~cVertexBuffer();

    IDirect3DVertexBuffer9 *GetVertexBufferCOM();
    unsigned long           GetVertexSize();
    unsigned long           GetVertexFVF();
    unsigned long           GetNumVertices();

    BOOL Create(cGraphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize);
    BOOL Free();

    BOOL IsLoaded();

    BOOL Set(unsigned long FirstVertex, unsigned long NumVertices, void *VertexList);
    BOOL Render(unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type);

    BOOL  Lock(unsigned long FirstVertex = 0, unsigned long NumVertices = 0);
    BOOL  Unlock();
    void *GetPtr();
};

class cGraphics
{

public :


HRESULT Init3D(HWND hwnd,long height,long width,bool bwindowed);
void Free3D();
bool Clear();
bool BeginDraw();
bool EndDraw();
bool Draw();

BOOL EnableZBuffer(BOOL Enable);
BOOL SetPerspective(float FOV, float Aspect, float Near, float Far);
BOOL SetAmbientLight(char Red, char Green, char Blue);
BOOL SetTexture(short Num, cTexture *Texture);
BOOL BeginSprite();
BOOL EndSprite();
BOOL EnableAlphaBlending(BOOL Enable = TRUE, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA);

IDirect3DDevice9 *GetDeviceCOM();

};


float static KeyPressed(float fValue, DWORD dwKEY);
long GetNextLong(FILE *fp);
DWORD Wait_Clock(DWORD count);
DWORD Start_Clock(void);
DWORD Get_Clock(void);
int Close_Error_File(void);
int Write_Error(char *string, ...);
int Open_Error_File(char *filename, FILE *fp_override=NULL);
int RandomNumber(int iMin, int iMax);
float RandomNumber(float fMin, float fMax);
BOOL CheckIntersect(float XStart, float YStart, float ZStart,
                                   float XEnd,   float YEnd,   float ZEnd,
                                   float *Length);




