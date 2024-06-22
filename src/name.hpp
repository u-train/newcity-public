#pragma once

#include "item.hpp"

enum NameTypes {
  RoadName, GivenMaleName, GivenFemaleName, FamilyName,
  RetailName, OfficeName, FarmName, IndustrialName, GovRetailName,
  CityName, AmenityName, BuildingName, ContributorName, EngineeringFirmName,
  NewspaperName, SportsTeamName, HotelName,
  numNameTypes
};

void initNames();
void resetNames();
char* randomName(item type);


