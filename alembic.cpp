// alembicPlugin
// Initial code generated by Softimage SDK Wizard
// Executed Fri Aug 19 09:14:49 UTC+0200 2011 by helge
// 
// Tip: You need to compile the generated code before you can load the plug-in.
// After you compile the plug-in, you can load it by clicking Update All in the Plugin Manager.
#include <xsi_application.h>
#include <xsi_context.h> 
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_menu.h>
#include <xsi_uitoolkit.h> 
#include <xsi_progressbar.h>
#include <xsi_comapihandler.h>
#include <xsi_project.h>
#include <xsi_selection.h>
#include <xsi_model.h>
#include <xsi_null.h>
#include <xsi_camera.h>
#include <xsi_customoperator.h>
#include <xsi_expression.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_factory.h>
#include <xsi_primitive.h> 
#include <xsi_math.h>
#include <xsi_cluster.h>
#include <xsi_clusterproperty.h>
#include <xsi_primitive.h>
#include <xsi_geometry.h>
#include <xsi_polygonmesh.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgitem.h>
#include <xsi_ppgeventcontext.h>
#include <xsi_icetree.h>
#include <xsi_icenode.h>
#include <xsi_icenodeinputport.h>
#include <xsi_icecompoundnode.h>
#include <xsi_utils.h>
#include <xsi_time.h>
#include <time.h>
#include <xsi_customoperator.h>
#include <xsi_operatorcontext.h>
#include <xsi_outputport.h>
#include "arnoldHelpers.h" 

using namespace XSI; 
using namespace MATH; 

#include <boost/exception/all.hpp>

#include "AlembicLicensing.h"

#include "AlembicWriteJob.h"
#include "AlembicPoints.h"
#include "AlembicCurves.h"
#include "CommonProfiler.h"
#include "CommonMeshUtilities.h"
#include "CommonUtilities.h"

SICALLBACK XSILoadPlugin_2( PluginRegistrar& in_reg ) ;

SICALLBACK XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"Helge Mathee");
	in_reg.PutName(L"ExocortexAlembicSoftimage");
	in_reg.PutVersion(1,0);

	//if( HasAlembicWriterLicense() ) {
		in_reg.RegisterCommand(L"alembic_export",L"alembic_export");

		in_reg.RegisterMenu(siMenuMainFileExportID,L"alembic_MenuExport",false,false);

		in_reg.RegisterProperty(L"alembic_export_settings");
	//}

	//if( HasAlembicReaderLicense() ) {
		in_reg.RegisterCommand(L"alembic_import",L"alembic_import");
		in_reg.RegisterCommand(L"alembic_attach_metadata",L"alembic_attach_metadata");
		in_reg.RegisterCommand(L"alembic_create_item",L"alembic_create_item");
		in_reg.RegisterCommand(L"alembic_path_manager",L"alembic_path_manager");
		in_reg.RegisterCommand(L"alembic_profile_stats",L"alembic_profile_stats");

		in_reg.RegisterOperator(L"alembic_xform");
		in_reg.RegisterOperator(L"alembic_camera");
		in_reg.RegisterOperator(L"alembic_polymesh");
		in_reg.RegisterOperator(L"alembic_polymesh_topo");
		in_reg.RegisterOperator(L"alembic_nurbs");
		in_reg.RegisterOperator(L"alembic_bbox");
		in_reg.RegisterOperator(L"alembic_normals");
		in_reg.RegisterOperator(L"alembic_uvs");
		in_reg.RegisterOperator(L"alembic_crvlist");
		in_reg.RegisterOperator(L"alembic_crvlist_topo");
		in_reg.RegisterOperator(L"alembic_visibility");
		in_reg.RegisterOperator(L"alembic_geomapprox");
		in_reg.RegisterOperator(L"alembic_standinop");

		in_reg.RegisterMenu(siMenuMainFileImportID,L"alembic_MenuImport",false,false);
		in_reg.RegisterMenu(siMenuMainFileProjectID,L"alembic_MenuPathManager",false,false);
		in_reg.RegisterMenu(siMenuMainFileProjectID,L"alembic_ProfileStats",false,false);
		in_reg.RegisterMenu(siMenuTbGetPropertyID,L"alembic_MenuMetaData",false,false);

		in_reg.RegisterProperty(L"alembic_import_settings");
		in_reg.RegisterProperty(L"alembic_timecontrol");
		in_reg.RegisterProperty(L"alembic_metadata");

		// register ICE nodes
		Register_alembic_curves(in_reg);
		Register_alembic_points(in_reg);

		XSILoadPlugin_2( in_reg );

		// register events
		in_reg.RegisterEvent(L"alembic_OnCloseScene",siOnCloseScene);
	//}

   ESS_LOG_INFO("PLUGIN loaded");

 	return CStatus::OK;
}

SICALLBACK XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
   deleteAllArchives();

	CString strPluginName;
	strPluginName = in_reg.GetName();
	Application().LogMessage(strPluginName + L" has been unloaded.",siVerboseMsg);
	return CStatus::OK;
}




ESS_CALLBACK_START(alembic_MenuExport_Init,CRef&)
	Context ctxt( in_ctxt );
	Menu oMenu;
	oMenu = ctxt.GetSource();
	MenuItem oNewItem;
	oMenu.AddCommandItem(L"Alembic 1.0",L"alembic_export",oNewItem);
	return CStatus::OK;
ESS_CALLBACK_END

ESS_CALLBACK_START(alembic_MenuImport_Init,CRef&)
	Context ctxt( in_ctxt );
	Menu oMenu;
	oMenu = ctxt.GetSource();
	MenuItem oNewItem;
	oMenu.AddCommandItem(L"Alembic 1.0",L"alembic_import",oNewItem);
	return CStatus::OK;
ESS_CALLBACK_END

ESS_CALLBACK_START(alembic_MenuPathManager_Init,CRef&)
	Context ctxt( in_ctxt );
	Menu oMenu;
	oMenu = ctxt.GetSource();
	MenuItem oNewItem;
	oMenu.AddCommandItem(L"Alembic Path Manager",L"alembic_path_manager",oNewItem);
	return CStatus::OK;
ESS_CALLBACK_END

ESS_CALLBACK_START(alembic_ProfileStats_Init,CRef&)
	Context ctxt( in_ctxt );
	Menu oMenu;
	oMenu = ctxt.GetSource();
	MenuItem oNewItem;
	oMenu.AddCommandItem(L"Alembic Profile Stats",L"alembic_profile_stats",oNewItem);
	return CStatus::OK;
ESS_CALLBACK_END

ESS_CALLBACK_START(alembic_profile_stats_Init,CRef&)
	Context ctxt( in_ctxt );
	Command oCmd;
	oCmd = ctxt.GetSource();
	oCmd.PutDescription(L"");
	oCmd.EnableReturnValue(true);

	return CStatus::OK;
ESS_CALLBACK_END

ESS_CALLBACK_START(alembic_profile_stats_Execute, CRef&)
    ESS_PROFILE_REPORT();
	return CStatus::OK;
ESS_CALLBACK_END

ESS_CALLBACK_START(alembic_OnCloseScene_OnEvent,CRef&)
   deleteAllArchives();
	return CStatus::OK;
ESS_CALLBACK_END
