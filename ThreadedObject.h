#pragma once

#ifndef __ThreadedObject_h__
#define __ThreadedObject_h__

#include <GU/GU_Detail.h>
#include <GA/GA_SplittableRange.h>
#include <GA/GA_PageIterator.h>


namespace AKA_CL
{

class ThreadedObject
{
public:
	THREADED_METHOD4( ThreadedObject, range.canMultiThread( ), neighboursLookup,
					  const GA_SplittableRange&, range,
					  GU_Detail*, gdp,
					  GA_Attribute*, neighs_attrib,
					  const GA_AIFNumericArray*, neighs_aif );

	void neighboursLookupPartial( const GA_SplittableRange& range,
								  GU_Detail* gdp,
								  GA_Attribute* neighs_attrib,
								  const GA_AIFNumericArray* neighs_aif,
								  const UT_JobInfo& info );

	THREADED_METHOD3( ThreadedObject, range.canMultiThread( ), setEmpty,
					  const GA_SplittableRange&, range,
					  GA_Attribute*, neighs_attrib,
					  const GA_AIFNumericArray*, neighs_aif );

	void setEmptyPartial( const GA_SplittableRange& range,
						  GA_Attribute* neighs_attrib,
						  const GA_AIFNumericArray* neighs_aif,
						  const UT_JobInfo& info );

};
}


#endif