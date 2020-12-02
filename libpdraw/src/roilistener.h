#ifndef ROILISTENER_H
#define ROILISTENER_H
#include <coordinateconverter.h>
#include <vector>
#include <boost/thread.hpp>
class RoiListener{
public:
    virtual bool PendingSwapFromListener();
    //RoiListener();
    //~RoiListener();
    virtual void OnPendingRoiChange() = 0;//
    virtual void OnRoiChangeSwap() = 0;//notify that we swapped to a different roi
    virtual void OnRoiChange(double lowLat, double lowLon, double upperLat, double upperLon) = 0;//new roi occured with current starepoint (ie isNewRoi in roimgr was triggered)
    void OnNewROI(CoordinateConverter newRoi);
    CoordinateConverter GetLatestCoordConverter();
    CoordinateConverter GetCoordConverterAtIndex(int index);
    void RefreshCoordConverters();
    int GetCurrentID();
    int _latestUsedConverterID;
    //int GetLatestConverterID();
private:
    std::vector<CoordinateConverter> _coordinateConverters;
    boost::mutex _coordConverterMutex;
};

#endif // ROILISTENER_H

