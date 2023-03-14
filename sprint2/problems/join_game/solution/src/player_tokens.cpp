#include "player_tokens.h"
#include <sstream>

std::string PlayerTokens::GetToken()
{
 	auto hexConverter = [](const auto& value) -> std::string
					{
	    					std::stringstream sstream;
	    					sstream << std::hex << value;
	    					return sstream.str();
	    				};

	return hexConverter(generator1_()) + hexConverter(generator2_());
}
