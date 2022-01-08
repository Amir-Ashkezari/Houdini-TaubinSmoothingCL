#pragma once

#ifndef __SOP_TaubinSmoothingCl_h__
#define __SOP_TaubinSmoothingCl_h__

#include <SOP/SOP_Node.h>
#include <UT/UT_StringHolder.h>


namespace AKA_CL
{

class SOP_TaubinSmoothingCl : public SOP_Node
{
public:
	static const UT_StringHolder theSOPTypeName;

	static PRM_Template* buildTemplates( );
	static OP_Node* myConstructor( OP_Network* net, const char* name, OP_Operator* op );

	const SOP_NodeVerb* cookVerb( ) const override;

protected:
	SOP_TaubinSmoothingCl( OP_Network* net, const char* name, OP_Operator* op );
	~SOP_TaubinSmoothingCl( ) override;

	OP_ERROR cookMySop( OP_Context& context ) override;

};
}


#endif