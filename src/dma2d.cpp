#include "dma2d.h"

#if HAS_DMA2D

static bool dma2d_initialized = false;

void init_dma2d()
{
    if(!dma2d_initialized)
    {
        __HAL_RCC_DMA2D_CLK_ENABLE();
        __HAL_RCC_DMA2D_FORCE_RESET();
        __HAL_RCC_DMA2D_RELEASE_RESET();
        dma2d_initialized = true;
    }
}

void deinit_dma2d()
{
    if(dma2d_initialized)
    {
        __HAL_RCC_DMA2D_FORCE_RESET();
        __HAL_RCC_DMA2D_CLK_DISABLE();
        dma2d_initialized = false;
    }
}

void dma2d_fill(const cv::Mat& mat, uint16_t color)
{
  init_dma2d();
  // See https://www.eet-china.com/mp/a60976.html
  DMA2D->CR = 0x00030000UL; // R2M
  DMA2D->OMAR = reinterpret_cast<uint32_t>(mat.data); // target addr
  DMA2D->NLR  = (uint32_t)(mat.cols << 16) | (uint16_t)mat.rows; // cols & rows
  DMA2D->OOR = mat.step[0] / mat.step[1] - mat.cols; // target offset
  DMA2D->OCOLR   = color; // color
  DMA2D->OPFCCR  = mat.type == cv::MONO8 ? 5/*L8*/ : 2/*RGB565*/; // format
  DMA2D->CR |= DMA2D_CR_START;
  while (DMA2D->CR & DMA2D_CR_START) {}
}

void dma2d_copy(const cv::Mat& src_mat, const cv::Rect& src_roi, const cv::Mat& dest_mat, const cv::Point& dest_pos)
{
  init_dma2d();
  // See https://www.eet-china.com/mp/a60976.html
  DMA2D->CR = 0x00000000UL; // M2M fetch only
  if(src_mat.type == cv::MONO8)
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
  DMA2D->NLR  = (uint32_t)(src_roi.width << 16) | (uint16_t)src_roi.height; // cols & rows
  DMA2D->CR |= DMA2D_CR_START;
  while (DMA2D->CR & DMA2D_CR_START) {}
}

void dma2d_flat_copy(const cv::Mat& mat, const cv::Rect& roi, void *buffer)
{
  init_dma2d();
  // See https://www.eet-china.com/mp/a60976.html
  DMA2D->CR = 0x00000000UL; // M2M fetch only
  if(mat.type == cv::MONO8)
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
  DMA2D->NLR  = (uint32_t)(roi.width << 16) | (uint16_t)roi.height; // cols & rows
  DMA2D->CR |= DMA2D_CR_START;
  while (DMA2D->CR & DMA2D_CR_START) {}
}

#endif
