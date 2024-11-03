#ifndef ELASTIC_ATMOSPHERE_HPP
#define ELASTIC_ATMOSPHERE_HPP
#include "atmosphere.hpp"

namespace ZAtmos {
struct ElasticAtmosphere : public Atmosphere {
	double externalPressure = 101.325;
	ElasticAtmosphere(double initialVolume);
	void tick(double dt) override;
};
}

#endif
