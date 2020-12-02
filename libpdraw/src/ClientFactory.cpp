#define WIN32_LEAN_AND_MEAN
#include "LFClientEngine.h"
#include "ClientFactory.h"
#include <QtPositioning/QtPositioning>
//#include "gdal.h"
//#include "gdal_alg.h"
//#include "gdal_priv.h"
//#include "cpl_conv.h" // for CPLMalloc()


ClientFactory::ClientFactory(void)
{
}


ClientFactory::~ClientFactory(void)
{
	
}


// This creates an LFClient object
boost::shared_ptr<LFClient> ClientFactory::CreateClient(void)
{
	boost::shared_ptr<LFClient> spClient(new LFClientEngine());
    std::string strfile("/home/nvidia/.smartcamjet/SCJetData/elevation1.tif");
    //std::string strfile("/mapdata/dae_dted/n45w125_W122_N44.dt1");
   // GDALDatasetH ff = GDALOpen(strfile.c_str(), ga_up);

//    GDALDataset  *poDataset;
//        GDALAllRegister();
//        poDataset = (GDALDataset *) GDALOpen( strfile.c_str(), GA_Update );
//        GDALRasterBandH g = poDataset->GetRasterBand(1);
//        CPLErr err = GDALFillNodata(g, g, 5, 0, 0, NULL, NULL, NULL);
//        if( poDataset == NULL )
//        {
//            //...;
//            std::cout<<"didnt' work";
//        }
//    poDataset->FlushCache();
    //poDataset->CloseDependentDatasets();
	return spClient;
}
