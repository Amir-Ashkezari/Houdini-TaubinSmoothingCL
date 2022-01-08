__kernel
void firstphase( const int ptnum,
				 __global float* P,
				 __global float* __scratch,
				 __global int* neighbours_index,
				 __global int* neighbours,
				 const float firstphasestep )
{
    const int idx = get_global_id( 0 );
    if ( idx >= ptnum )
        return;

    const int start = neighbours_index[ idx   ];
    const int   end = neighbours_index[ idx+1 ];
    const int   len = end - start;

	if ( len < 1)
		return;

    float3 avg = { 0.f, 0.f, 0.f };
    float3 pos = vload3( idx, P);

    for ( int i = start; i < end; ++i )
    {
        int npt = neighbours[ i ];
        avg += vload3( npt, P ) - pos;
    }

    float weight = 1.f / (float)len * firstphasestep;
    avg *= weight;

    vstore3( pos + avg, idx, __scratch );
}


__kernel
void secondphase( const int ptnum,
				  __global float* P,
				  __global float* __scratch,
				  __global int* neighbours_index,
				  __global int* neighbours,
				  const float secondphasestep )
{
    const int idx = get_global_id( 0 );
    if ( idx >= ptnum )
        return;

    const int start = neighbours_index[ idx   ];
    const int   end = neighbours_index[ idx+1 ];
    const int   len = end - start;

	if ( len < 1 )
		return;

    float3 avg = { 0.f, 0.f, 0.f };
    float3 pos = vload3( idx, __scratch);

    for ( int i = start; i < end; ++i )
    {
        int npt = neighbours[ i ];
        avg += vload3( npt, __scratch ) - pos;
    }

    float weight = 1.f / (float)len * secondphasestep;
    avg *= weight;

    vstore3( pos + avg, idx, P );
}