#pragma once

#include <string>
#include <optional>
#include <vector>
#include <tuple>
#include <map>

namespace thmxParser
{
    struct MeshParameters
    {
        std::optional<int> quadTreeMeshLevel;
		std::optional<bool> errorCheckFlag;
		std::optional<float> errorLimit;
		std::optional<int> maxIterations;
		std::optional<bool> cmaFlag;
    };

	struct ColorRGB
	{
		int r;
		int g;
		int b;
	};

	using Side = std::string;
	using Range = std::string;
	using Specularity = std::string;
	using OpticalTuple = std::tuple<Side, Range, Specularity>;

	struct Material
	{
		std::string name;
		int type;
		float conductivity;
		float emissivityFront;
		float emissivityBack;
		float tir;
		ColorRGB color;
		int cavityModel;
		std::map<OpticalTuple, float> transmittances;
		std::map<OpticalTuple, float> reflectances;
	};

    void parseFile(std::string const & path);
}   // namespace thmxParser