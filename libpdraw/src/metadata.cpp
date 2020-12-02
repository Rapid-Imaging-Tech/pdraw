
#include "metadata.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;
void Metadata::Init()
{
    SetupDictionary2();
    SetupDictionary3();
}

//metavr = dict2
//smartopo = dict3
unsigned char dict2[] = {0x06, 0x0E, 0x2B, 0x34, 0x02, 0x0B, 0x01, 0x01, 0x0E, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00};
unsigned char dict3[] = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

DICTTYPE Metadata::FindDictType(const unsigned char* buffer)
{
    bool isDict2 = true;
    bool isDict3 = true;

    for(int i = 0 ; i < 16 ; i++){
        if(buffer[i] != dict2[i])
        {
            isDict2 = false;
        }
    }
    if(isDict2 == true){
        return DICT2;
    }
    for(int i = 0 ; i < 16 ; i++){
        if(buffer[i] != dict3[i])
        {
            isDict3 = false;
        }
    }
    if(isDict3 == true){
        return DICT3;
    }
    return DICTUNKNOWN;
}
void Metadata::ParseDict2(const unsigned char* buffer, int length)
{
    _klvCollection._klvItems.clear();

    int packetIndex = 0;
    unsigned char chlength = buffer[18];//metavr was 18
    unsigned char chlength2 = 0x00;
    unsigned int packetLength = 0x00000000;

    if(buffer[16] == 0x82)
    {
        //wingnut
        chlength = buffer[18];
        chlength2 = buffer[17];
        packetLength = 0x00000000 | chlength | (chlength2<<8);
        packetIndex = 19;// metavr was 19

    }
    else if(buffer[16] == 0x81)
    {
        //wingnut
        chlength = buffer[17];
        packetLength = 0x00000000 | chlength;
        packetIndex = 18;// metavr was 19

    }
    else //if(buffer[16] == 0x82)
    {
        //smartTopo
        chlength = buffer[16];
        packetLength = 0x00000000 | chlength;
        packetIndex = 17;
    }


     //take packet, parse out data then set lat/lon/altitude accordingly
    //unsigned char data[1024];
    int dataIndex = 0;
    //unsigned int packetLength = 0x00000000 | chlength;////static_cast<unsigned int>(buffer[18]);
    boost::shared_ptr<KLVItem> klvItem;
    do
    {
        dataIndex = 0;
        klvItem.reset(new KLVItem);
        unsigned char key = buffer[packetIndex++];
        unsigned char length = buffer[packetIndex++];
        klvItem->_key = key;
        klvItem->_length = length;

        //cout<<"key = "<<hex<<static_cast<int>(key)<<" length ="<<hex<<static_cast<int>(length)<<" data=";
        for(int i = 0 ; i < length ; i++)
        {
            //std::cout<<" 0x"<<setfill('0')<<setw(2)<<hex<<static_cast<int>(buffer[packetIndex]);
            klvItem->_value[dataIndex++] = buffer[packetIndex++];
        }
        _klvCollection._klvItems.push_back(klvItem);
        //cout<<endl;
    }while(packetIndex < packetLength + 17);
    //cout<<endl;
}
void Metadata::ParseDict3(const unsigned char* buffer, int length)
{
    _klvCollection._klvItems.clear();
    int packetIndex = 0;
     //take packet, parse out data then set lat/lon/altitude accordingly
    //packetIndex = 18;// metavr was 19
    //unsigned char data[1024];
    int dataIndex = 0;
    unsigned char chlength = 0x00;//= buffer[17];//metavr was 18
    //unsigned int packetLength = 0x00000000 | chlength;
    boost::shared_ptr<KLVItem> klvItem;
    if(buffer[16] == 0x81)
    {
        //wingnut
        chlength = buffer[17];
        packetIndex = 18;
    }
    else if(buffer[16] == 0x82)
    {
        //smartTopo
        chlength = buffer[18];
        packetIndex = 19;
    }
    unsigned int packetLength = 0x00000000 | chlength;
    do
    {
        dataIndex = 0;
        klvItem.reset(new KLVItem);
        unsigned char key = buffer[packetIndex++];
        unsigned char length = buffer[packetIndex++];
        klvItem->_key = key;
        klvItem->_length = length;

        //cout<<"key = "<<hex<<static_cast<int>(key)<<" length ="<<hex<<static_cast<int>(length)<<" data=";
        for(int i = 0 ; i < length ; i++)
        {
            //std::cout<<" 0x"<<setfill('0')<<setw(2)<<hex<<static_cast<int>(buffer[packetIndex]);
            klvItem->_value[dataIndex++] = buffer[packetIndex++];
        }
        _klvCollection._klvItems.push_back(klvItem);
        //cout<<endl;
    }while(packetIndex < packetLength);
    //cout<<endl;
}
void Metadata::ParsePacket(const unsigned char* buffer, int length)
{
    if(buffer[0] != 0x06)//PSI HACK
    {//PSI HACK
        //PSI HACK
        buffer++;//PSI HACK
        buffer++;//PSI HACK
        buffer++;//PSI HACK
    }//PSI HACK
    DICTTYPE dtype = FindDictType(buffer);
    if(dtype == DICT2){
        ParseDict2(buffer, length);
    }
    if(dtype == DICT3){
        ParseDict3(buffer, length);
    }
}
#include <inttypes.h>
#include <stdio.h>
//#include <Windows.h>

void Metadata::GetData(boost::shared_ptr<SViewData> sviewdata)
{
    // This union is used to pack bytes in the time unsigned int64
    union u_tag {
        unsigned char bytes[8];
        uint64_t val;
        //unsigned __int64 val;
    } uval;

    //take klvcollection and dictionary, match up keys.
    //go through data and fill in sviewdata accordingly
    boost::shared_ptr<DictionaryEntry> dict;
    if(_dictType == DICT2){
        dict = this->_mapDictionary2[0x0d];
    }else{
        dict = this->_mapDictionary3[0x0d];
    }
    char stroutput[1024];
    char keyLat = dict->_key;
    for(int i = 0 ; i < _klvCollection._klvItems.size() ; i++)
    {
        boost::shared_ptr<KLVItem> klvItem = _klvCollection._klvItems[i];
        boost::shared_ptr<DictionaryEntry> dictEntry;
        unsigned char k = klvItem->_key;
        if(_dictType == DICT2)
        {
            //todo: what if no dict entry?
            dictEntry = _mapDictionary2[klvItem->_key];
        }
        else
        {
            dictEntry = _mapDictionary3[klvItem->_key];
        }
        if(dictEntry)
        {
            if(dictEntry->_type == Uint64)//, Int32, Uint16, Int16, Uint8, Int8, Uint64, ISO7
            {
                uval.bytes[7] = (static_cast<int>(klvItem->_value[0]));
                uval.bytes[6] = (static_cast<int>(klvItem->_value[1]));
                uval.bytes[5] = (static_cast<int>(klvItem->_value[2]));
                uval.bytes[4] = (static_cast<int>(klvItem->_value[3]));
                uval.bytes[3] = (static_cast<int>(klvItem->_value[4]));
                uval.bytes[2] = (static_cast<int>(klvItem->_value[5]));
                uval.bytes[1] = (static_cast<int>(klvItem->_value[6]));
                uval.bytes[0] = (static_cast<int>(klvItem->_value[7]));
                //unsigned __int64 klvval;
                uint64_t klvval;
                klvval = uval.val;
/// I could not make the byte 'or'ing working in this case.  It gave wrong answers for time, so I feel back to using unions to achieve the end.
/// MFA 10/13/2015 to support KLv timestamp decoding
                switch(dictEntry->_name)
                {
                 case TIME:
                    sprintf(stroutput, "TIME = %f seconds\n", ((double) klvval) / 1000000.0);
                    sviewdata->dTime = ((double) klvval) / 1000000.0; // convert from microseconds to seconds.
                    //OutputDebugString(stroutput);
                    break;
                }
    ///
            }
            if(dictEntry->_type == Int32)//, Int32, Uint16, Int16, Uint8, Int8, Uint64, ISO7
            {
                //klvItem == the lat value;
                unsigned long klvval;
                int byte1 = (static_cast<int>(klvItem->_value[0]))<<24;
                int byte2 = (static_cast<int>(klvItem->_value[1]))<<16;
                int byte3 = (static_cast<int>(klvItem->_value[2]))<<8;
                int byte4 = (static_cast<int>(klvItem->_value[3]));


                klvval = byte1 | byte2 | byte3 | byte4 ;
                double norm;
                std::cout<<klvval;
                norm = (klvval + 0x7fffffff)/(double)0xfffffffe;
                double max = dictEntry->_maxRange;
                double min = dictEntry->_minRange;
                double mappedVal = (norm * (max - min)) + min;
                std::cout<<"mapped value = "<<mappedVal<<endl;
                switch(dictEntry->_name)
                {
                 case LATITUDE:
                    sviewdata->dVehicleLat = mappedVal;
                    sprintf(stroutput, "latitude = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                case LONGITUDE:
                    sviewdata->dVehicleLon = mappedVal;
                    sprintf(stroutput, "longitude = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                case CAM_DEPRESSION:
                    sviewdata->dCameraPitch = mappedVal;
                    sprintf(stroutput, "camera depression = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                }
            }
            if(dictEntry->_type == Uint32)
            {
                //klvItem == the lat value;
                unsigned long klvval;
                int byte1 = (static_cast<int>(klvItem->_value[0]))<<24;
                int byte2 = (static_cast<int>(klvItem->_value[1]))<<16;
                int byte3 = (static_cast<int>(klvItem->_value[2]))<<8;
                int byte4 = (static_cast<int>(klvItem->_value[3]));


                klvval = byte1 | byte2 | byte3 | byte4 ;
                double norm;
                std::cout<<klvval;
                norm = klvval/(double)0xffffffff;
                double max = dictEntry->_maxRange;
                double min = dictEntry->_minRange;
                double mappedVal = (norm * (max - min)) + min;
                std::cout<<"mapped value = "<<mappedVal<<endl;
                switch(dictEntry->_name)
                {
                 case CAM_BEARING:
                    sviewdata->dCameraHeading = mappedVal;
                    sprintf(stroutput, "camera bearing = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                case CAM_TWIST:
                    sviewdata->dCameraRoll = mappedVal;
                    sprintf(stroutput, "camera twist = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                case SlantRange:
                    sviewdata->dSlantDistance = mappedVal;
                    sprintf(stroutput, "slantrange = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                }
            }
            if(dictEntry->_type == Uint8)//, Int32, Uint16, Int16, Uint8, Int8, Uint64, ISO7
            {
                //klvItem == the lat value;
                unsigned long klvval;
                int byte1 = (static_cast<int>(klvItem->_value[0]));

                klvval = byte1 ;
                double norm;
                std::cout<<klvval;
                norm = klvval/(double)0xff;
                double max = dictEntry->_maxRange;
                double min = dictEntry->_minRange;
                double mappedVal = (norm * (max - min)) + min;
                std::cout<<"mapped value = "<<mappedVal<<endl;
                /*switch(dictEntry->_name)
                {

                }*/
            }
            if(dictEntry->_type == Uint16)//, Int32, Uint16, Int16, Uint8, Int8, Uint64, ISO7
            {
                //klvItem == the lat value;
                unsigned long klvval;
                int byte1 = (static_cast<int>(klvItem->_value[0]))<<8;
                int byte2 = (static_cast<int>(klvItem->_value[1]));

                klvval = byte1 | byte2;
                double norm;
                std::cout<<klvval;
                norm = klvval/(double)0xffff;
                double max = dictEntry->_maxRange;
                double min = dictEntry->_minRange;
                double mappedVal = (norm * (max - min)) + min;
                std::cout<<"mapped value = "<<mappedVal<<endl;
                switch(dictEntry->_name)
                {
                case ALTITUDE:
                    sviewdata->dVehicleAltitude = mappedVal;
                    sprintf(stroutput, "altitude = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                case HEADING:
                    sviewdata->dVehicleHeading = mappedVal;
                    sprintf(stroutput, "heading = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                case VFOV:
                    //sviewdata->dFovVerticalAngle = mappedVal;
                    sviewdata->dfov = mappedVal;
                    sprintf(stroutput, "vfov = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;

                }
            }
            if(dictEntry->_type == Int16)//, Int32, Uint16, Int16, Uint8, Int8, Uint64, ISO7
            {
                //klvItem == the lat value;
                unsigned short int klvval;
                int byte1 = (static_cast<int>(klvItem->_value[0]))<<8;
                int byte2 = (static_cast<int>(klvItem->_value[1]));

                klvval = byte1 | byte2;
                double norm;
                std::cout<<klvval;
                norm = ((unsigned short int)(klvval + 0x7fff))/(double)0xfffe;
                double max = dictEntry->_maxRange;
                double min = dictEntry->_minRange;
                double mappedVal = (norm * (max - min)) + min;
                std::cout<<"mapped value = "<<mappedVal<<endl;
                switch(dictEntry->_name)
                {
                 case PITCH:
                    sviewdata->dVehiclePitch = mappedVal;
                    sprintf(stroutput, "pitch = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                 case ROLL:
                    sviewdata->dVehicleRoll = mappedVal;
                    sprintf(stroutput, "roll = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                /*case HEADING:
                    sviewdata->dVehicleHeading = mappedVal;
                    sprintf(stroutput, "heading = %f \n", mappedVal);
                    OutputDebugString(stroutput);
                    break;*/
                case VFOV:
                    sviewdata->dfov = mappedVal;
                    sprintf(stroutput, "vfov = %f \n", mappedVal);
                    //OutputDebugString(stroutput);
                    break;
                }
            }
        }
    }
}
Metadata::Metadata():
_dictType(DICTUNKNOWN)
{
}
Metadata::~Metadata()
{
}

void Metadata::SetupDictionary2()
{
    boost::shared_ptr<DictionaryEntry> dictEntry(new DictionaryEntry);
    dictEntry->_key = 0x02;
    //dictEntry->_type = DataType::Uint64;
    dictEntry->_type = Uint64;
    dictEntry->_name = TIME;
    _mapDictionary2[0x02] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x03;
    dictEntry->_type = ISO7;
    dictEntry->_name = MissionID;
    _mapDictionary2[0x03] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x04;
    dictEntry->_type = ISO7;
    dictEntry->_name = PlatformTailNumber;
    _mapDictionary2[0x04] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x05;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 360.0;
    dictEntry->_type = Uint16;
    dictEntry->_name = HEADING;
    _mapDictionary2[0x05] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x06;
    dictEntry->_minRange = -20.0;
    dictEntry->_maxRange = 20.0;
    dictEntry->_type = Int16;
    dictEntry->_name = PITCH;
    _mapDictionary2[0x06] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x07;
    dictEntry->_minRange = -50.0;
    dictEntry->_maxRange = 50.0;
    dictEntry->_type = Int16;
    dictEntry->_name = ROLL;
    _mapDictionary2[0x07] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x08;
    dictEntry->_type = Uint8;
    dictEntry->_name = PlatformTrueAirspeed;
    _mapDictionary2[0x08] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x09;
    dictEntry->_type = Uint8;
    dictEntry->_name = PlatformIndicatedAirspeed;
    _mapDictionary2[0x09] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0a;
    dictEntry->_type = ISO7;
    dictEntry->_name = PlatformDesignation;
    _mapDictionary2[0x0a] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0b;
    dictEntry->_type = ISO7;
    dictEntry->_name = ImageSourceSensor;
    _mapDictionary2[0x0b] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0c;
    dictEntry->_type = ISO7;
    dictEntry->_name = ImageCoordinateSystem;
    _mapDictionary2[0x0c] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0d;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.0;
    dictEntry->_type = Int32;
    dictEntry->_name = LATITUDE;
    _mapDictionary2[0x0d] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0e;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.0;
    dictEntry->_type = Int32;
    dictEntry->_name = LONGITUDE;
    _mapDictionary2[0x0e] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0f;
    dictEntry->_minRange = -900.0;
    dictEntry->_maxRange = 19000.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = ALTITUDE;
    _mapDictionary2[0x0f] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x10;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = SensorHorizontalFoV;
    _mapDictionary2[0x10] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x11;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = VFOV;
    _mapDictionary2[0x11] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x12;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 360.00;
    dictEntry->_type = Uint32;
    dictEntry->_name = CAM_BEARING;
    _mapDictionary2[0x12] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x13;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int32;
    dictEntry->_name = CAM_DEPRESSION;
    _mapDictionary2[0x13] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x14;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 360.00;
    dictEntry->_type = Uint32;
    dictEntry->_name = CAM_TWIST;
    _mapDictionary2[0x14] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x15;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 5000000.00;
    dictEntry->_type = Uint32;
    dictEntry->_name = SlantRange;
    _mapDictionary2[0x15] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x16;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 5000000.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = TargetWidth;
    _mapDictionary2[0x16] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x17;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int32;
    dictEntry->_name = FrameCenterLatitude;
    _mapDictionary2[0x17] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x18;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int32;
    dictEntry->_name = FrameCenterLongitude;
    _mapDictionary2[0x18] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x19;
    dictEntry->_minRange = -900.0;
    dictEntry->_maxRange = 19000.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = FrameCenterElevation;
    _mapDictionary2[0x19] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1a;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLatitudePoint1;
    _mapDictionary2[0x1a] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1b;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLongitudePoint1;
    _mapDictionary2[0x1b] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1c;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLatitudePoint2;
    _mapDictionary2[0x1c] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1d;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLongitudePoint2;
    _mapDictionary2[0x1d] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1e;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLatitudePoint3;
    _mapDictionary2[0x1e] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1f;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLongitudePoint3;
    _mapDictionary2[0x1f] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x20;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLatitudePoint4;
    _mapDictionary2[0x20] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x21;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLongitudePoint4;
    _mapDictionary2[0x21] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x22;
    dictEntry->_type = Uint8;
    dictEntry->_name = IcingDetected;
    _mapDictionary2[0x22] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x23;
    dictEntry->_type = Uint16;
    dictEntry->_name = WindDirection;
    _mapDictionary2[0x23] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x24;
    dictEntry->_type = Uint8;
    dictEntry->_name = WindSpeed;
    _mapDictionary2[0x24] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x25;
    dictEntry->_type = Uint16;
    dictEntry->_name = StaticPressure;
    _mapDictionary2[0x25] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x26;
    dictEntry->_type = Uint16;
    dictEntry->_name = DensityAltitude;
    _mapDictionary2[0x26] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x27;
    dictEntry->_type = Int8;
    dictEntry->_name = OutsideAirTemperature;
    _mapDictionary2[0x27] = dictEntry;
}
void Metadata::SetupDictionary3()
{
    boost::shared_ptr<DictionaryEntry> dictEntry(new DictionaryEntry);
    dictEntry->_key = 0x02;
    dictEntry->_type = Uint64;
    dictEntry->_name = TIME;
    _mapDictionary3[0x02] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x03;
    dictEntry->_type = ISO7;
    dictEntry->_name = MissionID;
    _mapDictionary3[0x03] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x04;
    dictEntry->_type = ISO7;
    dictEntry->_name = PlatformTailNumber;
    _mapDictionary3[0x04] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x05;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 360.0;
    dictEntry->_type = Uint16;
    dictEntry->_name = HEADING;
    _mapDictionary3[0x05] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x06;
    dictEntry->_minRange = -20.0;
    dictEntry->_maxRange = 20.0;
    dictEntry->_type = Int16;
    dictEntry->_name = PITCH;
    _mapDictionary3[0x06] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x07;
    dictEntry->_minRange = -50.0;
    dictEntry->_maxRange = 50.0;
    dictEntry->_type = Int16;
    dictEntry->_name = ROLL;
    _mapDictionary3[0x07] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x08;
    dictEntry->_type = Uint8;
    dictEntry->_name = PlatformTrueAirspeed;
    _mapDictionary3[0x08] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x09;
    dictEntry->_type = Uint8;
    dictEntry->_name = PlatformIndicatedAirspeed;
    _mapDictionary3[0x09] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0a;
    dictEntry->_type = ISO7;
    dictEntry->_name = PlatformDesignation;
    _mapDictionary3[0x0a] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0b;
    dictEntry->_type = ISO7;
    dictEntry->_name = ImageSourceSensor;
    _mapDictionary3[0x0b] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0c;
    dictEntry->_type = ISO7;
    dictEntry->_name = ImageCoordinateSystem;
    _mapDictionary3[0x0c] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0d;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.0;
    dictEntry->_type = Int32;
    dictEntry->_name = LATITUDE;
    _mapDictionary3[0x0d] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0e;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.0;
    dictEntry->_type = Int32;
    dictEntry->_name = LONGITUDE;
    _mapDictionary3[0x0e] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x0f;
    dictEntry->_minRange = -900.0;
    dictEntry->_maxRange = 19000.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = ALTITUDE;
    _mapDictionary3[0x0f] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x10;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = SensorHorizontalFoV;
    _mapDictionary3[0x10] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x11;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = VFOV;
    _mapDictionary3[0x11] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x12;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 360.00;
    dictEntry->_type = Uint32;
    dictEntry->_name = CAM_BEARING;
    _mapDictionary3[0x12] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x13;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int32;
    dictEntry->_name = CAM_DEPRESSION;
    _mapDictionary3[0x13] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x14;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 360.00;
    dictEntry->_type = Uint32;
    dictEntry->_name = CAM_TWIST;
    _mapDictionary3[0x14] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x15;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 5000000.00;
    dictEntry->_type = Uint32;
    dictEntry->_name = SlantRange;
    _mapDictionary3[0x15] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x16;
    dictEntry->_minRange = 0.0;
    dictEntry->_maxRange = 5000000.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = TargetWidth;
    _mapDictionary3[0x16] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x17;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int32;
    dictEntry->_name = FrameCenterLatitude;
    _mapDictionary3[0x17] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x18;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int32;
    dictEntry->_name = FrameCenterLongitude;
    _mapDictionary3[0x18] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x19;
    dictEntry->_minRange = -900.0;
    dictEntry->_maxRange = 19000.00;
    dictEntry->_type = Uint16;
    dictEntry->_name = FrameCenterElevation;
    _mapDictionary3[0x19] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1a;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLatitudePoint1;
    _mapDictionary3[0x1a] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1b;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLongitudePoint1;
    _mapDictionary3[0x1b] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1c;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLatitudePoint2;
    _mapDictionary3[0x1c] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1d;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLongitudePoint2;
    _mapDictionary3[0x1d] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1e;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLatitudePoint3;
    _mapDictionary3[0x1e] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x1f;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLongitudePoint3;
    _mapDictionary3[0x1f] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x20;
    dictEntry->_minRange = -90.0;
    dictEntry->_maxRange = 90.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLatitudePoint4;
    _mapDictionary3[0x20] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x21;
    dictEntry->_minRange = -180.0;
    dictEntry->_maxRange = 180.00;
    dictEntry->_type = Int16;
    dictEntry->_name = CornerLongitudePoint4;
    _mapDictionary3[0x21] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x22;
    dictEntry->_type = Uint8;
    dictEntry->_name = IcingDetected;
    _mapDictionary3[0x22] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x23;
    dictEntry->_type = Uint16;
    dictEntry->_name = WindDirection;
    _mapDictionary3[0x23] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x24;
    dictEntry->_type = Uint8;
    dictEntry->_name = WindSpeed;
    _mapDictionary3[0x24] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x25;
    dictEntry->_type = Uint16;
    dictEntry->_name = StaticPressure;
    _mapDictionary3[0x25] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x26;
    dictEntry->_type = Uint16;
    dictEntry->_name = DensityAltitude;
    _mapDictionary3[0x26] = dictEntry;

    dictEntry.reset(new DictionaryEntry);
    dictEntry->_key = 0x27;
    dictEntry->_type = Int8;
    dictEntry->_name = OutsideAirTemperature;
    _mapDictionary3[0x27] = dictEntry;
}
