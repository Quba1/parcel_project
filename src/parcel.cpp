#include "environment.h"
#include "parcel.h"
#include "thermodynamic_calc.h"
#include <cmath>
#include <map>
#include <memory>
#include <string>

Parcel::Parcel(const std::map<std::string, std::string>& parcelConfiguration):
    parcelConfiguration(parcelConfiguration)
{
    calculateConstants();
    setupVariableFields();
    setInitialConditionsAndLocation();
}

void Parcel::calculateConstants()
{
    double period = std::stod(parcelConfiguration.at("period"));

    timeDelta = std::stod(parcelConfiguration.at("timestep"));
    timeDeltaSquared = timeDelta * timeDelta;
    
    ascentSteps = static_cast<size_t>(floor((period * 3600) / timeDelta) + 1); //including step zero
}

void Parcel::setupVariableFields()
{
    position = std::make_unique<double[]>(ascentSteps);
    velocity = std::make_unique<double[]>(ascentSteps);
    pressure = std::make_unique<double[]>(ascentSteps);
    temperature = std::make_unique<double[]>(ascentSteps);
    temperatureVirtual = std::make_unique<double[]>(ascentSteps);
    mixingRatio = std::make_unique<double[]>(ascentSteps);
    mixingRatioSaturated = std::make_unique<double[]>(ascentSteps);
}

void Parcel::setInitialConditionsAndLocation()
{
    //write initial conditions into parcel and convert to SI units

    //initial conditions from configuration
    position[0] = std::stod(parcelConfiguration.at("init_height"));
    velocity[0] = std::stod(parcelConfiguration.at("init_velocity"));
    temperature[0] = std::stod(parcelConfiguration.at("init_temp")) + 273.15;

    currentLocation.position = position[0];
    currentLocation.updateSector();

    //intermediate variables initial conditions
    pressure[0] = Environment::getPressureAtLocation(currentLocation);

    mixingRatio[0] = calcMixingRatio((std::stod(parcelConfiguration.at("init_dewpoint")) + 273.15), pressure[0]);
    temperatureVirtual[0] = calcVirtualTemperature(temperature[0], mixingRatio[0]);
    mixingRatioSaturated[0] = calcMixingRatio(temperature[0], pressure[0]);
}
