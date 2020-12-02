#include <LFClient.h>

placemarkCollection::placemarkCollection(bool setValue)

{
    reset(setValue);
}


void placemarkCollection::reset(bool setValue)//set all flags to "val"
{
    _placemarkVisibleSettings[PLACE_CITY] = setValue;
    _placemarkVisibleSettings[PLACE_TOWN] = setValue;
    _placemarkVisibleSettings[PLACE_VILLAGE] = setValue;
    _placemarkVisibleSettings[PLACE_HAMLET] = setValue;
    _placemarkVisibleSettings[PLACE_SUBURB] = setValue;
    _placemarkVisibleSettings[PLACE_NEIGHBOURHOOD] = setValue;
    _placemarkVisibleSettings[PLACE_ISLAND] = setValue;
    _placemarkVisibleSettings[PLACE_ISLET] = setValue;
    _placemarkVisibleSettings[PLACE_ARCHIPELAGO] = setValue;
    _placemarkVisibleSettings[PLACE_RESIDENTIAL] = setValue;
    _placemarkVisibleSettings[PLACE_ABORIGINAL_LANDS] = setValue;

    _placemarkVisibleSettings[PM_UNKNOWN] = setValue;
    _placemarkVisibleSettings[PM_AIRFIELD] = setValue;
    _placemarkVisibleSettings[PM_ALCOHOL_SHOP] = setValue;
    _placemarkVisibleSettings[PM_AMUSEMENT_PARK] = setValue;
    _placemarkVisibleSettings[PM_AQUARIUM] = setValue;
    _placemarkVisibleSettings[PM_ART_GALLERY] = setValue;
    _placemarkVisibleSettings[PM_ATTRACTION] = setValue;
    _placemarkVisibleSettings[PM_BAKERY] = setValue;
    _placemarkVisibleSettings[PM_BANK] = setValue;
    _placemarkVisibleSettings[PM_BAR] = setValue;
    _placemarkVisibleSettings[PM_BEER] = setValue;
    _placemarkVisibleSettings[PM_BICYCLE] = setValue;
    _placemarkVisibleSettings[PM_BICYCLE_SHARE] = setValue;
    _placemarkVisibleSettings[PM_BUS] = setValue;
    _placemarkVisibleSettings[PM_CAFE] = setValue;
    _placemarkVisibleSettings[PM_CAR] = setValue;
    _placemarkVisibleSettings[PM_CAMPSITE] = setValue;
    _placemarkVisibleSettings[PM_CASTLE] = setValue;
    _placemarkVisibleSettings[PM_CEMETERY] = setValue;
    _placemarkVisibleSettings[PM_CINEMA] = setValue;
    _placemarkVisibleSettings[PM_CLOTHING_STORE] = setValue;
    _placemarkVisibleSettings[PM_COLLEGE] = setValue;
    _placemarkVisibleSettings[PM_DENTIST] = setValue;
    _placemarkVisibleSettings[PM_DOG_PARK] = setValue;
    _placemarkVisibleSettings[PM_DRINKING_WATER] = setValue;
    _placemarkVisibleSettings[PM_EMBASSY] = setValue;
    _placemarkVisibleSettings[PM_ENTRANCE] = setValue;
    _placemarkVisibleSettings[PM_FAST_FOOD] = setValue;
    _placemarkVisibleSettings[PM_FERRY] = setValue;
    _placemarkVisibleSettings[PM_FUEL] = setValue;
    _placemarkVisibleSettings[PM_GARDEN] = setValue;
    _placemarkVisibleSettings[PM_GROCERY] = setValue;
    _placemarkVisibleSettings[PM_HARBOR] = setValue;
    _placemarkVisibleSettings[PM_HELIPORT] = setValue;
    _placemarkVisibleSettings[PM_ICE_CREAM] = setValue;
    _placemarkVisibleSettings[PM_INFORMATION] = setValue;
    _placemarkVisibleSettings[PM_LAUNDRY] = setValue;
    _placemarkVisibleSettings[PM_LODGING] = setValue;
    _placemarkVisibleSettings[PM_MONUMENT] = setValue;
    _placemarkVisibleSettings[PM_MUSEUM] = setValue;
    _placemarkVisibleSettings[PM_MUSIC] = setValue;
    _placemarkVisibleSettings[PM_PARK] = setValue;
    _placemarkVisibleSettings[PM_PHARMACY] = setValue;
    _placemarkVisibleSettings[PM_PICNIC_SITE] = setValue;
    _placemarkVisibleSettings[PM_PLACE_OF_WORSHIP] = setValue;
    _placemarkVisibleSettings[PM_PLAYGROUND] = setValue;
    _placemarkVisibleSettings[PM_POLICE] = setValue;
    _placemarkVisibleSettings[PM_POST] = setValue;
    _placemarkVisibleSettings[PM_PRISON] = setValue;
    _placemarkVisibleSettings[PM_RELIGIOUS_CHRISTIAN] = setValue;
    _placemarkVisibleSettings[PM_RELIGIOUS_JEWISH] = setValue;
    _placemarkVisibleSettings[PM_RELIGIOUS_MUSLIM] = setValue;
    _placemarkVisibleSettings[PM_RESTAURANT] = setValue;
    _placemarkVisibleSettings[PM_ROCKET] = setValue;
    _placemarkVisibleSettings[PM_SCHOOL] = setValue;
    _placemarkVisibleSettings[PM_SHOP] = setValue;
    _placemarkVisibleSettings[PM_STADIUM] = setValue;
    _placemarkVisibleSettings[PM_SWIMMING] = setValue;
    _placemarkVisibleSettings[PM_SUITCASE] = setValue;
    _placemarkVisibleSettings[PM_THEATRE] = setValue;
    _placemarkVisibleSettings[PM_TOILET] = setValue;
    _placemarkVisibleSettings[PM_TOWN_HALL] = setValue;
    _placemarkVisibleSettings[PM_VETERINARY] = setValue;
    _placemarkVisibleSettings[PM_ZOO] = setValue;
    _placemarkVisibleSettings[PM_DOCTOR] = setValue;
    _placemarkVisibleSettings[PM_HOSPITAL] = setValue;
    _placemarkVisibleSettings[PM_LIBRARY] = setValue;
    _placemarkVisibleSettings[PM_GOLF] = setValue;
    _placemarkVisibleSettings[PM_FIRE_STATION] = setValue;
}



bool placemarkCollection::isVisible(POIENUM placemark)
{
    bool retValue = true;
    if ( _placemarkVisibleSettings.find(placemark) == _placemarkVisibleSettings.end() ) {
      // not found

    } else {
      // found
        retValue = _placemarkVisibleSettings[placemark];
    }
    return retValue;
}

void placemarkCollection::setValue (POIENUM placemark, bool val)
{
    _placemarkVisibleSettings[placemark] = val;

}
