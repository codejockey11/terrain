#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800

#include <comdef.h>
#include <iphlpapi.h>
#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <mmsystem.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>
#include <tCHAR.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <d3d12.h>
#include <D3d12SDKLayers.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#include <DirectXMath.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

#pragma comment(lib, "dinput8.lib")

//#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ws2_32.lib")

/*
#include <X3DAudio.h>
#include <XAudio2.h>
#include <XAudio2fx.h>

#pragma comment(lib, "X3DAudio.lib")
#pragma comment(lib, "XAudio2.lib")
*/

#define LMB		0
#define RMB		1
#define MMB		2
#define SCR		3

#define ENGINELIB_API __declspec(dllexport)

#ifndef _CUSTOMENUMERATIONS
#define _CUSTOMENUMERATIONS

enum _VERTEXTYPE
{
	VT_VERTEXRGBA = 0,
	VT_VERTEXT,
	VT_VERTEXNT,
	MAXVERTEXTYPE = 0xff
};

enum _KEYREPEATTYPE
{
	KT_ONESHOT = 0,
	KT_REPEATER,
	MAXKEYREPEATTYPE = 0xff
};

enum _CONNECTIONSTATE
{
	CS_NOTCONNECTED = 0,
	CS_CONNECTING,
	CS_CONNECTED,

	MAXCONNECTIONSTATE = 0xff
};

enum _PACKETTYPE
{
	PT_EMPTY = 0,

	PT_PLAYERORTHOENTER,
	PT_PLAYERORTHOEXIT,

	PT_PLAYERENTER,
	PT_PLAYEREXIT,

	PT_CONNECT,
	PT_DISCONNECT,

	PT_STARTSERVER,
	PT_STOPSERVER,
	PT_ACCEPTED,

	PT_PING,
	PT_PONG,

	PT_TOGGLECONSOLE,
	PT_CONSOLEPAGING,
	PT_CONSOLEINPUT,

	PT_LOADBRUSHMAP,
	PT_LOADTERRAIN,
	PT_LOADORTHOLEVEL,

	PT_CHAT,

	PT_SINGLE,
	PT_TEAM,
	PT_GLOBAL,
	PT_LOCAL,

	PT_MOVE_DOWN,
	PT_MOVE_UP,
	PT_MOVE_LEFT,
	PT_MOVE_RIGHT,

	PT_FORWARD,
	PT_BACKWARD,
	PT_STRAFELEFT,
	PT_STRAFERIGHT,

	PT_ATTACK,

	PT_WINDOWMODE,
	PT_RENDERFRAME,

	PT_EXIT,

	MAXPACKETTYPE = 0xff
};

enum _BRUSHSIDE
{
	BRUSH_TOP = 0,
	BRUSH_BOTTOM,
	BRUSH_LEFT,
	BRUSH_RIGHT,
	BRUSH_FRONT,
	BRUSH_BACK,

	MAXBRUSHSIDE = 0xff
};

enum _SURFACECLIP
{
	ST_CLIP = 0,
	ST_NOCLIP,

	MAXSURFACECLIP = 0xff
};

enum _SURFACEDRAW
{
	SD_DRAW = 0,
	SD_NODRAW,

	MAXSURFACEDRAW = 0xff
};

ENGINELIB_API WCHAR* MBSToWide(const CHAR* str);
ENGINELIB_API CHAR* WideToMBS(const WCHAR* str);
#endif