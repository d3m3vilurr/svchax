#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "svchax.h"

extern char* fake_heap_start;
extern char* fake_heap_end;
extern u32 __ctru_heap;
extern u32 __ctru_heap_size;
extern u32 __ctru_linear_heap;
extern u32 __ctru_linear_heap_size;

void __system_allocateHeaps(void)
{
   u32 tmp = 0;

//    u32 size = (osGetMemRegionFree(MEMREGION_APPLICATION) / 2) & 0xFFFFF000;
   __ctru_heap_size = 0x1000000;
   __ctru_linear_heap_size = 0x1000000;

   // Allocate the application heap
   __ctru_heap = 0x08000000;
   svcControlMemory(&tmp, __ctru_heap, 0x0, __ctru_heap_size, MEMOP_ALLOC, MEMPERM_READ | MEMPERM_WRITE);

   // Allocate the linear heap
   svcControlMemory(&__ctru_linear_heap, 0x0, 0x0, __ctru_linear_heap_size, MEMOP_ALLOC_LINEAR,
                    MEMPERM_READ | MEMPERM_WRITE);

   // Set up newlib heap
   fake_heap_start = (char*)__ctru_heap;
   fake_heap_end = fake_heap_start + __ctru_heap_size;

}

static const char* test_result ="FAILED!!";

s32 set_test_result(void)
{
   __asm__ volatile("cpsid aif \n\t");
   test_result = "SUCCESS !!!";
   return 0;
}

int main(int argc, char** argv)
{
   gfxInit(GSP_BGR8_OES, GSP_RGB565_OES, false);
   gfxSet3D(false);
   consoleInit(GFX_BOTTOM, NULL);
   printf("svchax\n");
   printf("Press Start to exit.\n");

   osSetSpeedupEnable(false);
   svchax_init();
   osSetSpeedupEnable(true);

   if(__ctr_svchax)
      svcBackdoor(set_test_result);

   u32 frames = 0;
   printf("result : %s\n", test_result);
   printf("kernel version : %i.%i.%i", (int)*(u8*)(0x1FF80003), (int)*(u8*)(0x1FF80002), (int)*(u8*)(0x1FF80001));
   printf("\n");
   while (aptMainLoop())
   {
      hidScanInput();
      u32 kDown = hidKeysDown();

      if (kDown & KEY_START)
         break;

      printf("frames : %u\r", (unsigned)frames++);
      gspWaitForVBlank();
      fflush(stdout);

   }

   gfxExit();
   return 0;
}





