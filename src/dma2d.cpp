#include "dma2d.h"

#if defined(DMA2D)

#define DEAD_MASK ((uint32_t)0xFFFF00FE)  /* DMA2D DEAD Mask */

static bool dma2d_initialized = false;

void clean_cache_for_matrix(const cv::Mat& mat, const cv::Rect& roi)
{
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  if(mat.elemSize() == 1)
  {
    uint32_t startAddr = (uint32_t)mat.ptr<uint8_t>(roi.y, roi.x);
    uint32_t endAddr = (uint32_t)mat.ptr<uint8_t>(roi.y + roi.height - 1, roi.x + roi.width - 1);
    uint32_t byteSize = endAddr - startAddr + 1;
    uint32_t alignedAddr = startAddr &  ~0x1F;
    SCB_CleanDCache_by_Addr((void*)alignedAddr, byteSize + (startAddr - alignedAddr));
  }
  else
  {
    uint32_t startAddr = (uint32_t)mat.ptr<uint16_t>(roi.y, roi.x);
    uint32_t endAddr = (uint32_t)mat.ptr<uint16_t>(roi.y + roi.height - 1, roi.x + roi.width - 1);
    uint32_t byteSize = endAddr - startAddr + 2;
    uint32_t alignedAddr = startAddr &  ~0x1F;
    SCB_CleanDCache_by_Addr((void*)alignedAddr, byteSize + (startAddr - alignedAddr));
  }
#endif  
}

void dma2d_init()
{
    if(!dma2d_initialized)
    {
        __HAL_RCC_DMA2D_CLK_ENABLE();
        __HAL_RCC_DMA2D_FORCE_RESET();
        __HAL_RCC_DMA2D_RELEASE_RESET();
        dma2d_initialized = true;
    }
}

void dma2d_deinit()
{
    if(dma2d_initialized)
    {
        __HAL_RCC_DMA2D_FORCE_RESET();
        __HAL_RCC_DMA2D_CLK_DISABLE();
        dma2d_initialized = false;
    }
}

void dma2d_wait()
{
  while (DMA2D->CR & DMA2D_CR_START)
  {
      ThisThread::yield();
  }
}

void dma2d_fill(const cv::Mat& mat, uint16_t color)
{
  dma2d_init();
  // See https://www.eet-china.com/mp/a60976.html
  DMA2D->CR = 0x00030000UL; // R2M
  DMA2D->OMAR = reinterpret_cast<uint32_t>(mat.data); // target addr
  DMA2D->NLR  = (uint32_t)(mat.cols << 16) | (uint16_t)mat.rows; // cols & rows
  DMA2D->OOR = mat.step[0] / mat.step[1] - mat.cols; // target offset
  DMA2D->OCOLR   = color; // color
  DMA2D->OPFCCR  = mat.elemSize() == 1 ? 5/*L8*/ : 2/*RGB565*/; // format
  DMA2D->CR |= DMA2D_CR_START;
  dma2d_wait();
}

void dma2d_copy(const cv::Mat& src_mat, const cv::Rect& src_roi, const cv::Mat& dest_mat, const cv::Point& dest_pos)
{
  dma2d_init();
  // See https://www.eet-china.com/mp/a60976.html
  DMA2D->CR = 0x00000000UL; // M2M fetch only
  if(src_mat.elemSize() == 1)
  {
    DMA2D->FGMAR   = reinterpret_cast<uint32_t>(src_mat.ptr<uint8_t>(src_roi.y, src_roi.x)); // source addr
    DMA2D->OMAR = reinterpret_cast<uint32_t>(dest_mat.ptr<uint8_t>(dest_pos.y, dest_pos.x)); // target addr
    DMA2D->FGPFCCR  = 5; // L8
  }
  else
  {
    DMA2D->FGMAR   = reinterpret_cast<uint32_t>(src_mat.ptr<uint16_t>(src_roi.y, src_roi.x)); // source addr
    DMA2D->OMAR = reinterpret_cast<uint32_t>(dest_mat.ptr<uint16_t>(dest_pos.y, dest_pos.x)); // target addr
    DMA2D->FGPFCCR  = 2; // RGB565
  }
  DMA2D->FGOR    = src_mat.step[0] / src_mat.step[1] - src_roi.width;     // source offset
  DMA2D->OOR     = dest_mat.step[0] / dest_mat.step[1] - src_roi.width;     // dest offset
  DMA2D->NLR  = (uint32_t(src_roi.width) << 16) | (uint16_t)src_roi.height; // cols & rows
  clean_cache_for_matrix(src_mat, src_roi);
  DMA2D->CR |= DMA2D_CR_START;
  dma2d_wait();
}

void dma2d_flat_copy(const cv::Mat& mat, const cv::Rect& roi, volatile void *buffer)
{
  dma2d_init();
  // See https://www.eet-china.com/mp/a60976.html
  DMA2D->CR = 0x00000000UL; // M2M fetch only
  if(mat.elemSize() == 1)
  {
      DMA2D->FGMAR   = reinterpret_cast<uint32_t>(mat.ptr<uint8_t>(roi.y, roi.x)); // source addr
      DMA2D->FGPFCCR  = 5; // L8
  }
  else
  {
      DMA2D->FGMAR   = reinterpret_cast<uint32_t>(mat.ptr<uint16_t>(roi.y, roi.x)); // source addr
      DMA2D->FGPFCCR  = 2; // RGB565
  }
  DMA2D->OMAR = reinterpret_cast<uint32_t>(buffer); // target addr
  DMA2D->FGOR    = mat.step[0] / mat.step[1] - roi.width;     // source offset
  DMA2D->OOR     = 0;     // target offset
  DMA2D->NLR  = (uint32_t(roi.width) << 16) | (uint16_t)roi.height; // cols & rows
  clean_cache_for_matrix(mat, roi);
  DMA2D->CR |= DMA2D_CR_START;
  dma2d_wait();
}

const uint8_t RGB332toRGB888LUT[768] = {
    0x00,0x00,0x00,0x55,0x00,0x00,0xaa,0x00,0x00,0xff,0x00,0x00,0x00,0x24,0x00,0x55,
    0x24,0x00,0xaa,0x24,0x00,0xff,0x24,0x00,0x00,0x49,0x00,0x55,0x49,0x00,0xaa,0x49,
    0x00,0xff,0x49,0x00,0x00,0x6d,0x00,0x55,0x6d,0x00,0xaa,0x6d,0x00,0xff,0x6d,0x00,
    0x00,0x92,0x00,0x55,0x92,0x00,0xaa,0x92,0x00,0xff,0x92,0x00,0x00,0xb6,0x00,0x55,
    0xb6,0x00,0xaa,0xb6,0x00,0xff,0xb6,0x00,0x00,0xdb,0x00,0x55,0xdb,0x00,0xaa,0xdb,
    0x00,0xff,0xdb,0x00,0x00,0xff,0x00,0x55,0xff,0x00,0xaa,0xff,0x00,0xff,0xff,0x00,
    0x00,0x00,0x24,0x55,0x00,0x24,0xaa,0x00,0x24,0xff,0x00,0x24,0x00,0x24,0x24,0x55,
    0x24,0x24,0xaa,0x24,0x24,0xff,0x24,0x24,0x00,0x49,0x24,0x55,0x49,0x24,0xaa,0x49,
    0x24,0xff,0x49,0x24,0x00,0x6d,0x24,0x55,0x6d,0x24,0xaa,0x6d,0x24,0xff,0x6d,0x24,
    0x00,0x92,0x24,0x55,0x92,0x24,0xaa,0x92,0x24,0xff,0x92,0x24,0x00,0xb6,0x24,0x55,
    0xb6,0x24,0xaa,0xb6,0x24,0xff,0xb6,0x24,0x00,0xdb,0x24,0x55,0xdb,0x24,0xaa,0xdb,
    0x24,0xff,0xdb,0x24,0x00,0xff,0x24,0x55,0xff,0x24,0xaa,0xff,0x24,0xff,0xff,0x24,
    0x00,0x00,0x49,0x55,0x00,0x49,0xaa,0x00,0x49,0xff,0x00,0x49,0x00,0x24,0x49,0x55,
    0x24,0x49,0xaa,0x24,0x49,0xff,0x24,0x49,0x00,0x49,0x49,0x55,0x49,0x49,0xaa,0x49,
    0x49,0xff,0x49,0x49,0x00,0x6d,0x49,0x55,0x6d,0x49,0xaa,0x6d,0x49,0xff,0x6d,0x49,
    0x00,0x92,0x49,0x55,0x92,0x49,0xaa,0x92,0x49,0xff,0x92,0x49,0x00,0xb6,0x49,0x55,
    0xb6,0x49,0xaa,0xb6,0x49,0xff,0xb6,0x49,0x00,0xdb,0x49,0x55,0xdb,0x49,0xaa,0xdb,
    0x49,0xff,0xdb,0x49,0x00,0xff,0x49,0x55,0xff,0x49,0xaa,0xff,0x49,0xff,0xff,0x49,
    0x00,0x00,0x6d,0x55,0x00,0x6d,0xaa,0x00,0x6d,0xff,0x00,0x6d,0x00,0x24,0x6d,0x55,
    0x24,0x6d,0xaa,0x24,0x6d,0xff,0x24,0x6d,0x00,0x49,0x6d,0x55,0x49,0x6d,0xaa,0x49,
    0x6d,0xff,0x49,0x6d,0x00,0x6d,0x6d,0x55,0x6d,0x6d,0xaa,0x6d,0x6d,0xff,0x6d,0x6d,
    0x00,0x92,0x6d,0x55,0x92,0x6d,0xaa,0x92,0x6d,0xff,0x92,0x6d,0x00,0xb6,0x6d,0x55,
    0xb6,0x6d,0xaa,0xb6,0x6d,0xff,0xb6,0x6d,0x00,0xdb,0x6d,0x55,0xdb,0x6d,0xaa,0xdb,
    0x6d,0xff,0xdb,0x6d,0x00,0xff,0x6d,0x55,0xff,0x6d,0xaa,0xff,0x6d,0xff,0xff,0x6d,
    0x00,0x00,0x92,0x55,0x00,0x92,0xaa,0x00,0x92,0xff,0x00,0x92,0x00,0x24,0x92,0x55,
    0x24,0x92,0xaa,0x24,0x92,0xff,0x24,0x92,0x00,0x49,0x92,0x55,0x49,0x92,0xaa,0x49,
    0x92,0xff,0x49,0x92,0x00,0x6d,0x92,0x55,0x6d,0x92,0xaa,0x6d,0x92,0xff,0x6d,0x92,
    0x00,0x92,0x92,0x55,0x92,0x92,0xaa,0x92,0x92,0xff,0x92,0x92,0x00,0xb6,0x92,0x55,
    0xb6,0x92,0xaa,0xb6,0x92,0xff,0xb6,0x92,0x00,0xdb,0x92,0x55,0xdb,0x92,0xaa,0xdb,
    0x92,0xff,0xdb,0x92,0x00,0xff,0x92,0x55,0xff,0x92,0xaa,0xff,0x92,0xff,0xff,0x92,
    0x00,0x00,0xb6,0x55,0x00,0xb6,0xaa,0x00,0xb6,0xff,0x00,0xb6,0x00,0x24,0xb6,0x55,
    0x24,0xb6,0xaa,0x24,0xb6,0xff,0x24,0xb6,0x00,0x49,0xb6,0x55,0x49,0xb6,0xaa,0x49,
    0xb6,0xff,0x49,0xb6,0x00,0x6d,0xb6,0x55,0x6d,0xb6,0xaa,0x6d,0xb6,0xff,0x6d,0xb6,
    0x00,0x92,0xb6,0x55,0x92,0xb6,0xaa,0x92,0xb6,0xff,0x92,0xb6,0x00,0xb6,0xb6,0x55,
    0xb6,0xb6,0xaa,0xb6,0xb6,0xff,0xb6,0xb6,0x00,0xdb,0xb6,0x55,0xdb,0xb6,0xaa,0xdb,
    0xb6,0xff,0xdb,0xb6,0x00,0xff,0xb6,0x55,0xff,0xb6,0xaa,0xff,0xb6,0xff,0xff,0xb6,
    0x00,0x00,0xdb,0x55,0x00,0xdb,0xaa,0x00,0xdb,0xff,0x00,0xdb,0x00,0x24,0xdb,0x55,
    0x24,0xdb,0xaa,0x24,0xdb,0xff,0x24,0xdb,0x00,0x49,0xdb,0x55,0x49,0xdb,0xaa,0x49,
    0xdb,0xff,0x49,0xdb,0x00,0x6d,0xdb,0x55,0x6d,0xdb,0xaa,0x6d,0xdb,0xff,0x6d,0xdb,
    0x00,0x92,0xdb,0x55,0x92,0xdb,0xaa,0x92,0xdb,0xff,0x92,0xdb,0x00,0xb6,0xdb,0x55,
    0xb6,0xdb,0xaa,0xb6,0xdb,0xff,0xb6,0xdb,0x00,0xdb,0xdb,0x55,0xdb,0xdb,0xaa,0xdb,
    0xdb,0xff,0xdb,0xdb,0x00,0xff,0xdb,0x55,0xff,0xdb,0xaa,0xff,0xdb,0xff,0xff,0xdb,
    0x00,0x00,0xff,0x55,0x00,0xff,0xaa,0x00,0xff,0xff,0x00,0xff,0x00,0x24,0xff,0x55,
    0x24,0xff,0xaa,0x24,0xff,0xff,0x24,0xff,0x00,0x49,0xff,0x55,0x49,0xff,0xaa,0x49,
    0xff,0xff,0x49,0xff,0x00,0x6d,0xff,0x55,0x6d,0xff,0xaa,0x6d,0xff,0xff,0x6d,0xff,
    0x00,0x92,0xff,0x55,0x92,0xff,0xaa,0x92,0xff,0xff,0x92,0xff,0x00,0xb6,0xff,0x55,
    0xb6,0xff,0xaa,0xb6,0xff,0xff,0xb6,0xff,0x00,0xdb,0xff,0x55,0xdb,0xff,0xaa,0xdb,
    0xff,0xff,0xdb,0xff,0x00,0xff,0xff,0x55,0xff,0xff,0xaa,0xff,0xff,0xff,0xff,0xff
};

void dma2d_flat_rgb332_to_rgb565(const cv::Mat& mat, const cv::Rect& roi, volatile void *buffer)
{
  dma2d_init();
  DMA2D->CR = 0x00010000UL; // M2M with PFC
  DMA2D->FGMAR   = reinterpret_cast<uint32_t>(mat.ptr<uint8_t>(roi.y, roi.x)); // source addr
  DMA2D->FGPFCCR  = 0xFF15; // Input L8, CLUT RGB888, 256 entries
  DMA2D->FGCMAR = reinterpret_cast<uint32_t>(RGB332toRGB888LUT); // CLUT Address
  DMA2D->FGPFCCR |= DMA2D_FGPFCCR_START; // Load CLUT
  while (DMA2D->FGPFCCR & DMA2D_FGPFCCR_START) {}
  DMA2D->OMAR = reinterpret_cast<uint32_t>(buffer); // target addr
  DMA2D->FGOR    = mat.step[0] / mat.step[1] - roi.width;     // source offset
  DMA2D->OOR     = 0;     // target offset
  DMA2D->OPFCCR  = 2;  // RGB565
  DMA2D->NLR  = (uint32_t(roi.width) << 16) | (uint16_t)roi.height; // cols & rows
  clean_cache_for_matrix(mat, roi);
  DMA2D->CR |= DMA2D_CR_START;
  dma2d_wait();
}

#endif
