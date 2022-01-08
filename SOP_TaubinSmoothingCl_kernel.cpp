#if defined( __APPLE__ ) || defined( __MACOSX )
#include <OpenCL/cl.hpp>
#else
#include <CE/CE_API.h>
#include <CE/CE_Context.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <iostream>


void TaubinSmoothingCl( const exint numpt, fpreal32* h_P,
						const exint neighbours_index_length, const int32* h_neighbours_index,
						const exint neighbours_length, const int32* h_neighbours,
						const int32 bluriter, const fpreal32 firstphasestep, const fpreal32 secondphasestep )
{
	cl_int err = CL_SUCCESS;

	CE_Context* ctx = CE_Context::getContext( );
	if ( !ctx->isValid( ) )
	{
		std::cout << "Failed to initialize the context!\n";
		return;
	}

	try
	{
		const char* progname = "taubinsmoothingkernels.cl";
		cl::Program program = ctx->loadProgram( progname );

		std::vector<cl::Kernel> kernels;
		kernels.emplace_back( ctx->loadKernel( program, "firstphase" ) );
		kernels.emplace_back( ctx->loadKernel( program, "secondphase" ) );
		err = program.createKernels( &kernels );

		const size_t                P_size = numpt * 3 * sizeof( fpreal32 );
		const size_t neighbours_index_size = neighbours_index_length * sizeof( int32 );
		const size_t       neighbours_size = neighbours_length * sizeof( int32 );

		cl::Buffer                d_P = ctx->allocBuffer( P_size );
		cl::Buffer         d__scratch = ctx->allocBuffer( P_size );
		cl::Buffer d_neighbours_index = ctx->allocBuffer( neighbours_index_size, true, true, false );
		cl::Buffer       d_neighbours = ctx->allocBuffer( neighbours_size, true, true, false );

		ctx->writeBuffer( d_P, P_size, h_P );
		ctx->writeBuffer( d_neighbours_index, neighbours_index_size, h_neighbours_index );
		ctx->writeBuffer( d_neighbours, neighbours_size, h_neighbours );
		ctx->finish( );

		for ( size_t i = 0; i < kernels.size( ); ++i )
		{
			err = kernels[ i ].setArg( 0, (int32)numpt );
			err = kernels[ i ].setArg( 1, d_P );
			err = kernels[ i ].setArg( 2, d__scratch );
			err = kernels[ i ].setArg( 3, d_neighbours_index );
			err = kernels[ i ].setArg( 4, d_neighbours );
			err = kernels[ i ].setArg( 5, i == 0 ? firstphasestep : secondphasestep );
		}

		cl::NDRange global_work_size = cl::NDRange( numpt );
		for ( size_t i = 0; i < bluriter; ++i )
		{
			ctx->enqueueKernel( kernels[ 0 ], global_work_size, cl::NullRange );
			ctx->enqueueKernel( kernels[ 1 ], global_work_size, cl::NullRange );
		}
		ctx->finish( );

		ctx->readBuffer( d_P, P_size, h_P );
		ctx->finish( );

		ctx->releaseBuffer( std::move( d_neighbours_index ) );
		ctx->releaseBuffer( std::move( d_neighbours ) );
		ctx->releaseBuffer( std::move( d__scratch ) );
		ctx->releaseBuffer( std::move( d_P ) );
		ctx->clearMemoryPool( );
	}
	catch ( cl::Error& err )
	{
		ctx->reportError( err );
	}
}