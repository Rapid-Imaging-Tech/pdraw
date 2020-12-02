#pragma once
#include "LFClient.h"

/**
 * use this class to create the LFClient object which is an abstract class.
 */
class ClientFactory
{
public:
	ClientFactory(void);
	~ClientFactory(void);
	// This creates an LFClient object
        boost::shared_ptr<LFClient> CreateClient(void);///< create the actual object that can be referenced with function calls
};

