
#include "app.h"
#include "pfi.h"

int SetEmuParameters(struct tagPIEMU_CONTEXT* context, EMU* pEmuInfo, void* pUser)
{
  FILE* fp;
  PFIHEADER pfi;

  fp = fopen("piece.pfi", "rb");
  if(fp == NULL) DIE();
  if(!fread(&pfi, sizeof(PFIHEADER), 1, fp))
    DIE();
  if(memcmp(&pfi.dwSignature,  "1IFP", 4) != 0) DIE();
  fclose(fp);

  if(pfi.siSysInfo.size != sizeof(SYSTEMINFO))
    DIE();

  pEmuInfo->sysinfo = pfi.siSysInfo;

  return 1;
}

#define FLASH_TOP 0x0c00000
#define READ_UNIT 0x1000        // 4096

int LoadFlashImage(struct tagPIEMU_CONTEXT* context, FLASH* pFlashInfo, void* pUser)
{
  FILE* fp;
  PFIHEADER pfi;
  unsigned int nOffset = 0;

  fp = fopen("piece.pfi", "rb");
  if(!fp) DIE();
  if(!fread(&pfi, sizeof(PFIHEADER), 1, fp)) DIE();
  if(memcmp(&pfi.dwSignature,  "1IFP", 4) != 0) DIE();
  if(!pfi.dwOffsetToFlash) DIE();
  if(pfi.siSysInfo.size != sizeof(SYSTEMINFO))
    DIE();

  fseek(fp, pfi.dwOffsetToFlash, SEEK_SET);

  for(nOffset = 0; nOffset < (unsigned)pFlashInfo->mem_size; nOffset += READ_UNIT)
  {
    fread(pFlashInfo->mem + nOffset, 1, READ_UNIT, fp);
  }

  fclose(fp);
  return 1;
}

int UpdateScreen(PIEMU_CONTEXT* context, void* pUser)
{
  SDL_Surface* surface;
  static const unsigned int palette[] = { 0x00ffffff, 0x00aaaaaa, 0x00555555, 0x00000000 };
  unsigned int* px;
  unsigned int* py;
  char* pp;

  surface = context->screen;
  if(SDL_MUSTLOCK(surface))
    SDL_LockSurface(surface);

  pp = *context->vbuff;
  for(py = (unsigned int*)surface->pixels;
      py != (unsigned int*)surface->pixels + DISP_X * DISP_Y;
      py += DISP_X)
  {
    for(px = py; px != py + DISP_X; px++)
    {
      *px = palette[*pp++ & 0x03];
    }
  }
  if(SDL_MUSTLOCK(surface))
    SDL_UnlockSurface(surface);

//  SDL_BlitSurface(context->buffer, NULL, context->screen, NULL);
  SDL_UpdateRect(context->screen, 0, 0, 0, 0);
//  SDL_Flip(context->screen);
//  SDL_UpdateRects(context->screen, 1, &rctDest);
  return 1;
}
