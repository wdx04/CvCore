#pragma once

#ifndef HAS_DMA2D
#define HAS_DMA2D 0
#endif

#if HAS_DMA2D

#include "mbed.h"
#include "cvcore.h"

// fill the mat with the given color
void dma2d_fill(const cv::Mat& mat, uint16_t color);

// copy roi of source mat to the given position of dest mat
void dma2d_copy(const cv::Mat& src_mat, const cv::Rect& src_roi, const cv::Mat& dest_mat, const cv::Point& dest_pos);

// copy the mat to the target continuous buffer
void dma2d_flat_copy(const cv::Mat& mat, const cv::Rect& roi, void *buffer);

#endif
