#pragma once

#ifndef __TaubinSmoothingCl_kernel_h__
#define __TaubinSmoothingCl_kernel_h__


void TaubinSmoothingCl( const exint numpt, fpreal32* h_P,
						const exint neighbours_index_length, const int32* h_neighbours_index,
						const exint neighbours_length, const int32* h_neighbours,
						const int32 bluriter, const fpreal32 firstphasestep, const fpreal32 secondphasestep );


#endif