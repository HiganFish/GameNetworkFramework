//
// Created by rjd
//

#ifndef GAMEPROJECT_FORMATUTILS_H
#define GAMEPROJECT_FORMATUTILS_H

#include <vector>
#include <string>
#include <numeric>

template<class T>
std::string VectorToString(const std::vector<T>& vec)
{
	if (vec.empty())
	{
		return {};
	}
	return std::accumulate(vec.begin() +1, vec.end(), std::to_string(vec[0]),
			[](const std::string& rst, const T& t)
			{
				return rst + ", " + std::to_string(t);
			});
}

#endif //GAMEPROJECT_FORMATUTILS_H
