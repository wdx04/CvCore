#pragma once

#include "mbed.h"
#include "cvcore.h"

#if defined(DMA2D)

// clean DCACHE for the mat
void clean_cache_for_matrix(const cv::Mat& mat, const cv::Rect& roi);

// fill the mat with the given color
void dma2d_fill(const cv::Mat& mat, uint16_t color);

// copy roi of source mat to the given position of dest mat
void dma2d_copy(const cv::Mat& src_mat, const cv::Rect& src_roi, const cv::Mat& dest_mat, const cv::Point& dest_pos);

// copy the mat to the target continuous buffer
void dma2d_flat_copy(const cv::Mat& mat, const cv::Rect& roi, volatile void *buffer);

// transform a RGB332 mat to RGB565 and output to the target continuous buffer
void dma2d_flat_rgb332_to_rgb565(const cv::Mat& mat, const cv::Rect& roi, volatile void *buffer);

#endif
