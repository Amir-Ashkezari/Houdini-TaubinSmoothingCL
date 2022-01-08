#include "SOP_TaubinSmoothingCl.h"
#include "ThreadedObject.h"
#include "SOP_TaubinSmoothingCl_kernel.h"

#include "SOP_TaubinSmoothingCl.proto.h"

#include <SOP/SOP_NodeVerb.h>
#include "GEO/GEO_PrimType.h"
#include <GU/GU_Detail.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <SYS/SYS_Math.h>
#include <iostream>
#include <limits.h>


using namespace AKA_CL;

const UT_StringHolder SOP_TaubinSmoothingCl::theSOPTypeName( "taubinsmoothingcl"_sh );

void newSopOperator( OP_OperatorTable* table )
{
	OP_Operator* op = new OP_Operator(
		SOP_TaubinSmoothingCl::theSOPTypeName,
		"Taubin Smoothing CL",
		SOP_TaubinSmoothingCl::myConstructor,
		SOP_TaubinSmoothingCl::buildTemplates( ),
		1,
		1,
		nullptr,
		0 );

    op->setOpTabSubMenuPath( "Utility" );
    op->setIsThreadSafe( true );

    table->addOperator( op );         
}


static const char* theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "neighboursname"
        label   "Neighbours Name"
        type    string
		joinnext
        default { "__neighbours" }
		disablewhen "{ export == 0 }"
    }
	parm {
            name        "export"
            label       "Export"
            type        toggle
            default     { "0" }
	}
	parm {
            name        "sepp"
            label       ""
            type        separator
			nolabel
	}
	parm {
        name    "bluriteration"
        label   "Blur Iteration"
        type    integer
        default { "1" }
		range   { 0! 50 }
		export  all
    }
	parm {
        name    "firstphasestep"
        label   "First Phase Step"
        type    float
        default { "0.333" }
		range   { -1! 1 }
		disablewhen "{ bluriteration == 0 }"
    }
	parm {
        name    "secondphasestep"
        label   "Second Phase Step"
        type    float
        default { "-0.342" }
		range   { -1! 1 }
		disablewhen "{ bluriteration == 0 }"
    }
}
)THEDSFILE";


PRM_Template* SOP_TaubinSmoothingCl::buildTemplates( )
{
	static PRM_TemplateBuilder templ( "SOP_TaubinSmoothingCl.cpp"_sh, theDsFile );
	return templ.templates( );
}


OP_Node* SOP_TaubinSmoothingCl::myConstructor( OP_Network* net, const char* name, OP_Operator* op )
{
	return new SOP_TaubinSmoothingCl( net, name, op );
}


SOP_TaubinSmoothingCl::SOP_TaubinSmoothingCl( OP_Network* net, const char* name, OP_Operator* op )
	: SOP_Node( net, name, op )
{
	mySopFlags.setManagesDataIDs( true );
}


SOP_TaubinSmoothingCl::~SOP_TaubinSmoothingCl( )
{
}


OP_ERROR SOP_TaubinSmoothingCl::cookMySop( OP_Context& context )
{
	return cookMyselfAsVerb( context );
}


class SOP_TaubinSmoothingClVerb : public SOP_NodeVerb
{
public:
	SOP_TaubinSmoothingClVerb( ) { }
	virtual ~SOP_TaubinSmoothingClVerb( ) { }

	virtual SOP_NodeParms* allocParms( ) const { return new SOP_TaubinSmoothingClParms( ); }
	virtual UT_StringHolder name( ) const { return SOP_TaubinSmoothingCl::theSOPTypeName; }

	virtual CookMode cookMode( const SOP_NodeParms* parms ) const { return COOK_INPLACE; }
	virtual void cook( const CookParms& cookparms ) const;

	static const SOP_NodeVerb::Register<SOP_TaubinSmoothingClVerb> theVerb;

};

const SOP_NodeVerb::Register<SOP_TaubinSmoothingClVerb> SOP_TaubinSmoothingClVerb::theVerb;

const SOP_NodeVerb* SOP_TaubinSmoothingCl::cookVerb( ) const
{
	return SOP_TaubinSmoothingClVerb::theVerb.get( );
}


void SOP_TaubinSmoothingClVerb::cook( const CookParms& cookparms ) const
{
	auto&& sopparms = cookparms.parms<SOP_TaubinSmoothingClParms>( );
	GU_Detail* gdp = cookparms.gdh( ).gdpNC();

	if ( !gdp->getNumPrimitives( ) || !gdp->containsPrimitiveType( GEO_PrimitiveType::GEO_PRIMPOLY ) )
	{
		cookparms.sopAddError( SOP_MESSAGE, "Operator only works with polygonal geometry type!" );
		return;
	}

	const UT_String& neighs_name = sopparms.getNeighboursname( ).buffer( );
	const bool export_neighs = sopparms.getExport( );
	const int32 bluriter = sopparms.getBluriteration( );
	const fpreal32 firstphase = sopparms.getFirstphasestep( );
	const fpreal32 secondphase = sopparms.getSecondphasestep( );

	const GA_Range ptrange = gdp->getPointRange( );
	const GA_Size numpt = ptrange.getMaxEntries( );

	GA_Attribute* neighs_attrib = gdp->findIntArray( GA_ATTRIB_POINT, neighs_name, -1, -1 );
	if ( !neighs_attrib )
		neighs_attrib = gdp->addIntArray( GA_ATTRIB_POINT, neighs_name, 1 );

	const GA_AIFNumericArray* neighs_aif = neighs_attrib->getAIFNumericArray( );
	neighs_aif->setStorage( neighs_attrib, GA_STORE_INT32 );

	UT_AutoInterrupt progress( "Taubin Smoothing..." );
	if ( progress.wasInterrupted( ) )
		return;

	ThreadedObject threadedObj;
	threadedObj.neighboursLookup( ptrange, gdp, neighs_attrib, neighs_aif );

	neighs_attrib->bumpDataId( );

	if ( bluriter )
	{
		GA_Attribute* p_attrib = gdp->getP( );
		const GA_AIFTuple* p_aif = p_attrib->getAIFTuple( );

		fpreal32* h_p_data = new fpreal32[ numpt * 3 ];
		p_aif->getRange( p_attrib, ptrange, h_p_data, 0, 3 );

		UT_Array<int32> __neighsoff, __neighs;
		neighs_aif->getBlockFromIndices( neighs_attrib, 0, numpt, __neighsoff, __neighs );

		TaubinSmoothingCl( numpt, h_p_data,
						   __neighsoff.size( ), __neighsoff.getRawArray( ),
						   __neighs.size( ), __neighs.getRawArray( ),
						   bluriter, firstphase, secondphase );

		p_aif->setRange( p_attrib, ptrange, h_p_data, 0, 3 );
		p_attrib->bumpDataId( );

		delete[ ] h_p_data;
	}

	if ( !export_neighs )
	{
		threadedObj.setEmpty( ptrange, neighs_attrib, neighs_aif );
		gdp->destroyAttribute( GA_ATTRIB_POINT, neighs_name );
	}
}