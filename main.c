#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <stdbool.h>
#include <stdio.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "D3DX10.lib")

typedef struct Window {
  IDXGISwapChain *_swapchain;
  ID3D11Device *_device;
  ID3D11DeviceContext *_context;
  ID3D11RenderTargetView *_backbuffer;
  SDL_Window *_window;
  uint32_t _width, _height;
} Window;

static Window *win;

static void CleanD3D(void);

static void InitWindow(const char *title, uint32_t width, uint32_t height)
{
  win->_width = width;
  win->_height = height;
  
  win->_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win->_width, win->_height, 0);

  SDL_SysWMinfo WMinfo;
  SDL_VERSION(&WMinfo.version);
  SDL_GetWindowWMInfo(win->_window, &WMinfo);
  HWND hwnd = (HWND)WMinfo.info.win.window;

  if (!InitD3D(hwnd)) {
    printf("failed to initialize D3D11 Renderer \n");
    CleanD3D();
  }
}

static BOOL InitD3D(HWND hwnd)
{
  DXGI_SWAP_CHAIN_DESC scd;
  ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

  scd.BufferCount = 1;
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scd.OutputWindow = hwnd;
  scd.SampleDesc.Count = 4;
  scd.Windowed = TRUE;

  D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd,
                                &win->_swapchain, &win->_device, NULL, &win->_context);

  ID3D11Texture2D *pBackBuffer;
  win->_swapchain->lpVtbl->GetBuffer(win->_swapchain, 0, &IID_ID3D11Texture2D, (LPVOID *)&pBackBuffer);

  win->_device->lpVtbl->CreateRenderTargetView(win->_device, pBackBuffer, NULL, &win->_backbuffer);
  pBackBuffer->lpVtbl->Release(pBackBuffer);

  win->_context->lpVtbl->OMSetRenderTargets(win->_context, 1, &win->_backbuffer, NULL);

  D3D11_VIEWPORT pViewport;
  ZeroMemory(&pViewport, sizeof(D3D11_VIEWPORT));

  pViewport.TopLeftX = 0;
  pViewport.TopLeftY = 0;
  pViewport.Width = win->_width;
  pViewport.Height = win->_height;

  win->_context->lpVtbl->RSSetViewports(win->_context, 1, &pViewport);

  return true;
}

static void CleanD3D(void)
{
  win->_swapchain->lpVtbl->Release(win->_swapchain);
  win->_backbuffer->lpVtbl->Release(win->_backbuffer);
  win->_device->lpVtbl->Release(win->_device);
  win->_context->lpVtbl->Release(win->_context);
}

static void Paint(FLOAT *col)
{
  win->_context->lpVtbl->ClearRenderTargetView(win->_context, win->_backbuffer, col);
  win->_swapchain->lpVtbl->Present(win->_swapchain, 0, 0);
}

int main(int argc, char *argv[])
{
  win = malloc(sizeof(Window));
  FLOAT _col[4] = {0.9f, 0.2f, 0.4f, 1.0f};

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("Failed to initialize SDL: %s \n", SDL_GetError());
    return -1;
  }

  InitWindow("Hello D3D11", 1024, 768);

  bool quit = false;

  while (!quit) {
    SDL_Event ev;
    SDL_WaitEvent(&ev);
    if (ev.type == SDL_QUIT) {
      quit = true;
      break;
    }
    Paint(_col);
  }

  CleanD3D();
  SDL_DestroyWindow(win->_window);
  free(win);
  SDL_Quit();
  return 0;
}