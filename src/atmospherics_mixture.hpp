#ifndef MIXTURE_HPP
#define MIXTURE_HPP


#include <string>
#include <vector>
namespace ZAtmos {
struct AtmosphericsQuantity {
public:
	std::string chemicalId;
	double moles;
	inline AtmosphericsQuantity(std::string const &chemicalId, double moles)
		: chemicalId(chemicalId), moles(moles)
	{}
};

typedef std::vector<AtmosphericsQuantity> AtmosphericsMixture;
}

#endif
