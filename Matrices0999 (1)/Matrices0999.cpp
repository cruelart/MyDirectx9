// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface



						// sprite declarations
LPDIRECT3DTEXTURE9 sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_heroRight;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_heroLeft;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_bullet;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_playerbullet;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_Boss;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 Playerimg;
LPDIRECT3DTEXTURE9 BackGround;
LPDIRECT3DTEXTURE9 Hp;



									 // function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory
void init_gameobject(void); //슈팅 게임 객체 생성 
void SetEnemy(int i, float x_pos, float y_pos); //적 생성 함수
void SetEnemy_Bullet(int i, float x_pos, float y_pos); //적 총알 생성 함수
void SetPlayer(LPDIRECT3DTEXTURE9 PlayerImage, int i, float x_pos, float y_pos); //플레이어 생성 함수
void SetPlayer_Bullet(int i, float x_pos, float y_pos); //플레이어 총알 생성 함수
void SetHp(int i, float x_pos, float y_pos); //플레이어 Hp함수
void SetBoss(float x_pos, float y_pos); //플레이어 Hp함수
bool PlayerCollider(float player_x, float player_y, float Enemy_x, float Enemy_y);
bool EnemyCollider(float Enemy_x, float Enemy_y, float player_x, float player_y);
void enemy1Attack(int x, int y, int enemyid);
void enemy2Attack(int x, int y, int enemyid);
void BossAttack(int x, int y, int enemyid);
void stage1();
void stage2();
void stage3();
void BossStage();
void Player_Hp();


							// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;


enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };


//기본 클래스 
class entity {

public:
	float x_pos = 0.0f;
	float y_pos = 0.0f;
	int status;
	int HP = 5;

};


//주인공 클래스 
class Hero :public entity {

public:
	void fire();
	void super_fire();
	void move(int i);
	bool isAttack = false;

};




// 적 클래스 
class Enemy :public entity {

public:
	void fire();


};


// 총알 클래스 
class Bullet :public entity {

public:
	void move();
	float x[10] = { 0 };
	float y[10] = { 0 };
	float bulletDelay[10] = { 0 };

};

float Enemy_sinMove = 0.0f;
float Enemy_flySpeed = 0.0f;
float Enemy_BulletSpeed = 0.0f;
float Enemy_BulletSpeed1 = 0.0f;
float Enemy_BulletSpeed2 = 0.0f;
float background_Looptime = 0.0f;
float PlayerAttackDelay = 0.0f;
int OnPlayer_bullet[10] = { 0 };
float PlayerBullet_x[10] = {0};
float PlayerBullet_y[10] = { 0 };
float colliderBye[20] = { 0.0f };
float BossAttackDelay = 0.0f;



//객체 생성 
Hero hero;
Bullet bullet;
Enemy enemy[100];

RECT enemy_rect[30];
RECT Player_rect[2];
RECT Boss_rect;
RECT PlayerBullet_rect[100];
RECT EnemyBullet_rect[100];
RECT PlayerHp_rect[5];




// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"B977009 컴퓨터그래픽스 기말과제",
		WS_EX_TOPMOST | WS_POPUP, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		DWORD starting_point = GetTickCount();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		render_frame();

		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);

		else if (KEY_DOWN(VK_SPACE))
		{
			hero.isAttack = true;
			//hero.showbullet = 255;
			//hero.isAttack = TRUE;
		}
		else if (KEY_DOWN(VK_RIGHT))
		{
			hero.x_pos += 4.5f;
			Playerimg = sprite_heroRight;
		}
		else if (KEY_DOWN(VK_LEFT))
		{
			hero.x_pos -= 4.5f;
			Playerimg = sprite_heroLeft;
		}
		else if (KEY_UP(VK_RIGHT))
		{
			Playerimg = sprite_hero;
		}
		
		else if (KEY_UP(VK_LEFT))
		{
			Playerimg = sprite_hero;
		}



		while ((GetTickCount() - starting_point) < 25);
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Panel3.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"ship_Idle.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_hero);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"ship_Left.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_heroLeft);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"ship_Right.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_heroRight);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"bullet.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"PlayerLaser.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_playerbullet);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"backgroundSpace_01.1.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&BackGround);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Hp.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&Hp);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Boss.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_Boss);    // load to sprite


	return;
}


// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

											 //UI 창 렌더링 


											 /*
											 static int frame = 21;    // start the program on the final frame
											 if(KEY_DOWN(VK_SPACE)) frame=0;     // when the space key is pressed, start at frame 0
											 if(frame < 21) frame++;     // if we aren't on the last frame, go to the next frame

											 // calculate the x-position
											 int xpos = frame * 182 + 1;

											 RECT part;
											 SetRect(&part, xpos, 0, xpos + 181, 128);
											 D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
											 D3DXVECTOR3 position(150.0f, 50.0f, 0.0f);    // position at 50, 50 with no depth
											 d3dspt->Draw(sprite, &part, &center, &position, D3DCOLOR_ARGB(127, 255, 255, 255));
											 */

											 //주인공 
	background_Looptime += 2.0f; // 백그라운드 loop 조정용 time
	if (background_Looptime > 515.0f)
	{
		background_Looptime = 0.0f;
	}
	/// <summary>
	/// //////배경화면
	/// </summary>
	/// <param name=""></param>
	RECT Background_space;
	SetRect(&Background_space, 0, 0, 1020, 1500);
	D3DXVECTOR3 center2(0.0f, 732.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position2(0.0f, background_Looptime, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(BackGround, &Background_space, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));

	//////////////플레이어 세팅
	SetPlayer(Playerimg, 0, 150+ hero.x_pos, 650); // 플레이어 세팅
	//SetPlayer_Bullet(0, 150 + bullet.x_pos,700+ bullet.y_pos); //플레이어 총알세팅

	PlayerAttackDelay += 1.0f;

	if (PlayerAttackDelay == 1)
	{
		PlayerBullet_x[0] = 195 + hero.x_pos;
		PlayerBullet_y[0] = 0.0f;
		OnPlayer_bullet[0] = true;
	}
	if (PlayerAttackDelay == 11)
	{
		PlayerBullet_x[1] = 195 + hero.x_pos;
		PlayerBullet_y[1] = 0.0f;
		OnPlayer_bullet[1] = true;
	}
	if (PlayerAttackDelay == 21)
	{
		PlayerBullet_x[2] = 195 + hero.x_pos;
		PlayerBullet_y[2] = 0.0f;
		OnPlayer_bullet[2] = true;
	}
	if (PlayerAttackDelay == 31)
	{
		PlayerBullet_x[3] = 195 + hero.x_pos;
		PlayerBullet_y[3] = 0.0f;
		OnPlayer_bullet[3] = true;
	}
	if (PlayerAttackDelay == 41)
	{
		PlayerBullet_x[4] = 195 + hero.x_pos;
		PlayerBullet_y[4] = 0.0f;
		OnPlayer_bullet[4] = true;
	}
	if (PlayerAttackDelay == 51)
	{
		PlayerBullet_x[5] = 195 + hero.x_pos;
		PlayerBullet_y[5] = 0.0f;
		OnPlayer_bullet[5] = true;
	}
	if (PlayerAttackDelay == 61)
	{
		PlayerBullet_x[6] = 195 + hero.x_pos;
		PlayerBullet_y[6] = 0.0f;
		OnPlayer_bullet[6] = true;
	}
	if (PlayerAttackDelay == 71)
	{
		PlayerAttackDelay = 0.0f;
	}
	
	/// <summary>
	/// ///////////////////
	/// </summary>
	/// <param name=""></param>
	for (int b = 0; b < 7; b++)
	{
		if (PlayerCollider(-30 + PlayerBullet_x[b], 600 + PlayerBullet_y[b], 150 + enemy[0].x_pos, -150 + enemy[0].y_pos))//-> 플레이어 총알과 몬스터 충돌처리
		{
			enemy[0].x_pos = 2000;
		}
		if (PlayerCollider(-30 + PlayerBullet_x[b], 600 + PlayerBullet_y[b], 800 + enemy[1].x_pos, -150 + enemy[1].y_pos))//-> 플레이어 총알과 몬스터 충돌처리
		{
			enemy[1].x_pos = 2000;
		}

		if (PlayerCollider(-30 + PlayerBullet_x[b], 600 + PlayerBullet_y[b], 450 + enemy[2].x_pos + 400 * sinf((enemy[2].y_pos) / 100), -120 + enemy[2].y_pos))//-> 플레이어 총알과 몬스터 충돌처리
		{
			enemy[2].x_pos = 2000;
		}

		if (PlayerCollider(-30 + PlayerBullet_x[b], 600 + PlayerBullet_y[b], 350 + enemy[3].x_pos + 400 * cosf((enemy[3].y_pos) / 100), -150 + enemy[3].y_pos))//-> 플레이어 총알과 몬스터 충돌처리
		{
			enemy[3].x_pos = 2000;
		}

		if (PlayerCollider(-30 + PlayerBullet_x[b], 600 + PlayerBullet_y[b], 150 + enemy[4].x_pos, -150 + enemy[4].y_pos))//-> 플레이어 총알과 몬스터 충돌처리
		{
			enemy[4].x_pos = 2000;
		}

		if (PlayerCollider(-30 + PlayerBullet_x[b], 600 + PlayerBullet_y[b], 600 + enemy[5].x_pos, -100 + enemy[5].y_pos))//-> 플레이어 총알과 몬스터 충돌처리
		{
			enemy[5].x_pos = 2000;
		}



		if (PlayerBullet_y[b] < -690.0f)
		{
			PlayerBullet_y[b] = 300.0f;

			OnPlayer_bullet[b] = false;
		}

		if (OnPlayer_bullet[b])
		{
			PlayerBullet_y[b] -= 10.0000001f;

		}
	}


	SetPlayer_Bullet(0, -30 +PlayerBullet_x[0], 600 + PlayerBullet_y[0]);
	SetPlayer_Bullet(1, -30 +PlayerBullet_x[1], 600 + PlayerBullet_y[1]);
	SetPlayer_Bullet(2, -30 +PlayerBullet_x[2], 600 + PlayerBullet_y[2]);
	SetPlayer_Bullet(3, -30 +PlayerBullet_x[3], 600 + PlayerBullet_y[3]);
	SetPlayer_Bullet(4, -30 +PlayerBullet_x[4], 600 + PlayerBullet_y[4]);
	SetPlayer_Bullet(5, -30 +PlayerBullet_x[5], 600 + PlayerBullet_y[5]);
	SetPlayer_Bullet(6, -30 +PlayerBullet_x[6], 600 + PlayerBullet_y[6]);

	if (PlayerCollider(150 + hero.x_pos, 650, 150 + enemy[0].x_pos + colliderBye[0], -150 + enemy[0].y_pos + Enemy_BulletSpeed + colliderBye[0]))
	{
		hero.HP -= 1;
		colliderBye[0] = 1000.0f;
	}
	if (PlayerCollider(150 + hero.x_pos, 650, 150 + enemy[0].x_pos + Enemy_BulletSpeed + colliderBye[1], -150 + enemy[0].y_pos + Enemy_BulletSpeed + colliderBye[1]))
	{
		hero.HP -= 1;
		colliderBye[1] = 1000.0f;
	}
	if (PlayerCollider(150 + hero.x_pos, 650, 150 + enemy[0].x_pos - Enemy_BulletSpeed + colliderBye[2], -150 + enemy[0].y_pos + Enemy_BulletSpeed + colliderBye[2]))
	{
		hero.HP -= 1;
		colliderBye[2] = 1000.0f;
	}

	if (PlayerCollider(150 + hero.x_pos, 650, 800 + enemy[1].x_pos + colliderBye[3], -150 + enemy[1].y_pos + Enemy_BulletSpeed + colliderBye[3]))
	{
		hero.HP -= 1;
		colliderBye[3] = 1000.0f;
	}
	if (PlayerCollider(150 + hero.x_pos, 650, 800 + enemy[1].x_pos + Enemy_BulletSpeed + colliderBye[4], -150 + enemy[1].y_pos + Enemy_BulletSpeed + colliderBye[4]))
	{
		hero.HP -= 1;
		colliderBye[4] = 1000.0f;
	}
	if (PlayerCollider(150 + hero.x_pos, 650, 800 + enemy[1].x_pos - Enemy_BulletSpeed + +colliderBye[5], -150 + enemy[1].y_pos + Enemy_BulletSpeed + colliderBye[5]))
	{
		hero.HP -= 1;
		colliderBye[5] = 1000.0f;
	}

	/// <summary>
	/// //////////적 세팅
	/// </summary>
	/// <param name=""></param>
	stage1();
	if (enemy[0].y_pos > 980)
	{
		stage2();
	}
	if (enemy[3].y_pos > 980)
	{
		stage3();
	}
	if (enemy[5].y_pos > 980)
	{
		BossStage();
	}

	
	////////////////////////UI부분
	Player_Hp();
	



	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}

bool PlayerCollider(float player_x, float player_y, float Enemy_x, float Enemy_y)
{
	if (player_x - Enemy_x < 30 && player_x - Enemy_x > -30)
	{
		if (player_y - Enemy_y < 30 && player_y - Enemy_y > -30)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void Player_Hp()
{
	if (hero.HP == 5)
	{
		SetHp(0, 50, 50);
		SetHp(1, 100, 50);
		SetHp(2, 150, 50);
		SetHp(3, 200, 50);
		SetHp(4, 250, 50);
	}


	if (hero.HP == 4)
	{
		SetHp(0, 50, 50);
		SetHp(1, 100, 50);
		SetHp(2, 150, 50);
		SetHp(3, 200, 50);
		//SetHp(4, 250, 50);
	}

	if (hero.HP == 3)
	{
		SetHp(0, 50, 50);
		SetHp(1, 100, 50);
		SetHp(2, 150, 50);
		//SetHp(3, 200, 50);
		//SetHp(4, 250, 50);
	}

	if (hero.HP == 2)
	{
		SetHp(0, 50, 50);
		SetHp(1, 100, 50);
		//SetHp(2, 150, 50);
		//SetHp(3, 200, 50);
		//SetHp(4, 250, 50);
	}

	if (hero.HP == 1)
	{
		SetHp(0, 50, 50);
		//SetHp(1, 100, 50);
		//SetHp(2, 150, 50);
		//SetHp(3, 200, 50);
		//SetHp(4, 250, 50);
	}
}

void enemy1Attack(int x, int y , int enemyid) // 공격패턴 1번
{
	if (Enemy_BulletSpeed < 700)
	{
		Enemy_BulletSpeed += 3.0f;
		SetEnemy_Bullet(0, x + enemy[enemyid].x_pos, y + enemy[enemyid].y_pos + Enemy_BulletSpeed);
		SetEnemy_Bullet(1, x + enemy[enemyid].x_pos + Enemy_BulletSpeed, y + enemy[enemyid].y_pos + Enemy_BulletSpeed);
		SetEnemy_Bullet(2, x + enemy[enemyid].x_pos - Enemy_BulletSpeed, y + enemy[enemyid].y_pos + Enemy_BulletSpeed);
	}

}

void enemy2Attack(int x, int y, int enemyid) // 공격패턴 2번
{
	if (Enemy_BulletSpeed1 < 700)
	{
		Enemy_BulletSpeed1 += 3.0f;
		SetEnemy_Bullet(3, x + enemy[enemyid].x_pos, y + enemy[enemyid].y_pos + Enemy_BulletSpeed1);
		SetEnemy_Bullet(4, x + enemy[enemyid].x_pos + Enemy_BulletSpeed1, y + enemy[enemyid].y_pos + Enemy_BulletSpeed1);
		SetEnemy_Bullet(5, x + enemy[enemyid].x_pos - Enemy_BulletSpeed1, y + enemy[enemyid].y_pos + Enemy_BulletSpeed1);
	}

}

void BossAttack(int x, int y, int enemyid)
{
	if (Enemy_BulletSpeed2 < 1200)
	{
		Enemy_BulletSpeed2 += 1.5f;
		SetEnemy_Bullet(6, x + enemy[enemyid].x_pos, y + enemy[enemyid].y_pos + Enemy_BulletSpeed2);
		SetEnemy_Bullet(7, x + enemy[enemyid].x_pos, y + enemy[enemyid].y_pos - Enemy_BulletSpeed2);
		SetEnemy_Bullet(8, x + enemy[enemyid].x_pos, y - enemy[enemyid].y_pos - Enemy_BulletSpeed2);
		SetEnemy_Bullet(9, x + enemy[enemyid].x_pos, y - enemy[enemyid].y_pos + Enemy_BulletSpeed2);
		SetEnemy_Bullet(10, x + enemy[enemyid].x_pos + Enemy_BulletSpeed2, y + enemy[enemyid].y_pos + Enemy_BulletSpeed2);
		SetEnemy_Bullet(11, x + enemy[enemyid].x_pos - Enemy_BulletSpeed2, y + enemy[enemyid].y_pos + Enemy_BulletSpeed2);
		SetEnemy_Bullet(12, x + enemy[enemyid].x_pos - Enemy_BulletSpeed2, y + enemy[enemyid].y_pos - Enemy_BulletSpeed2);
		SetEnemy_Bullet(13, x + enemy[enemyid].x_pos + Enemy_BulletSpeed2, y + enemy[enemyid].y_pos - Enemy_BulletSpeed2);
	}
	else
	{
		Enemy_BulletSpeed2 = 0.0f; x += 20;
	}
}



void stage1()
{
	if (enemy[0].y_pos < 1000)
	{
		enemy[0].y_pos += 3.0f;
	}
	SetEnemy(0, 150 + enemy[0].x_pos, -150 + enemy[0].y_pos); // 적 세팅
	if (-150 + enemy[0].y_pos > 200)
	{
		enemy1Attack(150, -150, 0);
	}

	if (enemy[1].y_pos < 1000)
	{
		enemy[1].y_pos += 3.0f;
	}
	SetEnemy(1, 800 + enemy[1].x_pos, -150 + enemy[1].y_pos); // 적 세팅
	if (-150 + enemy[1].y_pos > 200)
	{
		enemy1Attack(800, -150, 1);
	}

}

void stage2()
{
	if (enemy[2].y_pos < 1000)
	{
		enemy[2].y_pos += 3.0f;

	}

	SetEnemy(2, 450 + enemy[2].x_pos + 400 * sinf((enemy[2].y_pos) / 100), -120 + enemy[2].y_pos); // 적 세팅

	if (enemy[3].y_pos < 1000)
	{
		enemy[3].y_pos += 3.0f;

	}

	SetEnemy(3, 450 + enemy[3].x_pos + 350 * cosf((enemy[3].y_pos) / 100), -150 + enemy[3].y_pos); // 적 세팅
}

void stage3()
{
	if (enemy[4].y_pos < 1000)
	{
		enemy[4].y_pos += 3.0f;
	}
	SetEnemy(4, 150 + enemy[4].x_pos, -150 + enemy[4].y_pos); // 적 세팅
	if (-150 + enemy[4].y_pos > 200)
	{
		enemy2Attack(150, -150, 4);
	}

	if (enemy[5].y_pos < 1000)
	{
		enemy[5].y_pos += 3.0f;
	}
	SetEnemy(5, 600 + enemy[5].x_pos, -100 + enemy[5].y_pos); // 적 세팅
	if (-100 + enemy[5].y_pos > 300)
	{
		enemy2Attack(600, -100, 5);
	}
}

void BossStage()
{
	BossAttackDelay += 1.0f;
	if (enemy[6].y_pos < 600)
	{
		enemy[6].y_pos += 3.0f;
	}
	SetBoss(150 + enemy[6].x_pos, -650 + enemy[6].y_pos);
	if (BossAttackDelay > 200.0f)
	{
		BossAttack(350, -250, 6);
		BossAttack(450, -350, 6);
		BossAttack(600, -330, 6);
		BossAttack(700, -430, 6);
	}

}


void SetHp(int i, float x_pos, float y_pos)
{
	SetRect(&PlayerHp_rect[i], 0, 0, 32, 32);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(0.0f + x_pos, 0.0f + y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(Hp, &PlayerHp_rect[i], &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
}


void SetPlayer_Bullet(int i, float x_pos, float y_pos)
{
	SetRect(&PlayerBullet_rect[i], 0, 0, 25, 64);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(0.0f + x_pos, 0.0f + y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_playerbullet, &PlayerBullet_rect[i], &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void SetEnemy_Bullet(int i, float x_pos, float y_pos)
{
	SetRect(&EnemyBullet_rect[i], 0, 0, 16, 16);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(0.0f + x_pos, 0.0f + y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_bullet, &EnemyBullet_rect[i], &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
}


void SetPlayer(LPDIRECT3DTEXTURE9 PlayerImage, int i, float x_pos, float y_pos) // 플레이어 배치 함수
{
	SetRect(&Player_rect[i], 0, 0, 64, 64);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(0.0f + x_pos, 0.0f + y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(PlayerImage, &Player_rect[i], &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
}


void SetEnemy(int i, float x_pos, float y_pos) // 적 배치 함수
{
	SetRect(&enemy_rect[i], 0, 0, 64, 64);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(0.0f + x_pos, 0.0f + y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_enemy, &enemy_rect[i], &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
}


void SetBoss(float x_pos, float y_pos) // 적 배치 함수
{
	SetRect(&Boss_rect, 0, 0, 512, 512);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(0.0f + x_pos, 0.0f + y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_Boss, &Boss_rect, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	sprite->Release();
	d3ddev->Release();
	d3d->Release();

	//객체 해제 
	sprite_hero->Release();
	sprite_enemy->Release();
	sprite_bullet->Release();

	return;
}