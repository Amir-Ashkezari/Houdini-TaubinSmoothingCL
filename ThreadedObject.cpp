#include <iostream>
#include "ThreadedObject.h"


using namespace AKA_CL;

void ThreadedObject::neighboursLookupPartial( const GA_SplittableRange& range,
											  GU_Detail* gdp,
											  GA_Attribute* neighs_attrib,
											  const GA_AIFNumericArray* neighs_aif,
											  const UT_JobInfo& info )
{
	for ( GA_PageIterator pit = range.beginPages( info ); !pit.atEnd( ); ++pit )
	{
		GA_Offset start, end;
		GA_Offset pptoff, nptoff, pvtxoff, nvtxoff, vtxoff;
		GA_Index pptidx, nptidx;
		GA_Size vtxcount, foundvtx;
		GA_OffsetArray primsoff;
		GA_OffsetListRef primvtcs;
		UT_Array<int32> __scratch( 10 );

		for ( GA_Iterator it( pit.begin( ) ); it.blockAdvance( start, end ); )
		{
			for ( GA_Offset ptoff = start; ptoff < end; ++ptoff )
			{
				if ( !gdp->getPrimitivesReferencingPoint( primsoff, ptoff ) )
					continue;

				for ( GA_Offset i = 0; i < primsoff.size( ); ++i )
				{
					vtxoff = gdp->getVertexReferencingPoint( ptoff, primsoff[ i ] );
					vtxcount = gdp->getPrimitiveVertexCount( primsoff[ i ] );
					primvtcs = gdp->getPrimitiveVertexList( primsoff[ i ] );
					foundvtx = primvtcs.find( vtxoff );

					pvtxoff = foundvtx == 0 ? primvtcs.get( vtxcount - 1 ) : primvtcs.get( foundvtx - 1 );
					nvtxoff = foundvtx == vtxcount - 1 ? primvtcs.get( 0 ) : primvtcs.get( foundvtx + 1 );
					pptoff = gdp->vertexPoint( pvtxoff );
					nptoff = gdp->vertexPoint( nvtxoff );
					pptidx = gdp->pointIndex( pptoff );
					nptidx = gdp->pointIndex( nptoff );

					if ( i == 0 || __scratch.find( pptidx ) == -1 )
						__scratch.emplace_back( pptidx );
					if ( i == 0 || __scratch.find( nptidx ) == -1 )
						__scratch.emplace_back( nptidx );
				}

				neighs_aif->set( neighs_attrib, ptoff, __scratch );
				__scratch.clear( );
			}
		}
	}
};


void ThreadedObject::setEmptyPartial( const GA_SplittableRange& range,
									  GA_Attribute* neighs_attrib,
									  const GA_AIFNumericArray* neighs_aif,
									  const UT_JobInfo& info )
{
	for ( GA_PageIterator pit = range.beginPages( info ); !pit.atEnd( ); ++pit )
	{
		GA_Offset start, end;
		for ( GA_Iterator it( pit.begin( ) ); it.blockAdvance( start, end ); )
		{
			for ( GA_Offset ptoff = start; ptoff < end; ++ptoff )
				neighs_aif->set( neighs_attrib, ptoff, (const UT_Array<int32>)0 );
		}
	}
};