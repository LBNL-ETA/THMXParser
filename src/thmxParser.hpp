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

	struct BoundaryCondition
	{
		std::string name;
		int type;
		float H;		
		float heatFlux;
		float temperature;
		ColorRGB color;
		float Tr;
		float Hr;
		float Ei;
		float viewFactor;
		int radiationModel;
		bool convectionFlag;
		bool fluxFlag;
		bool radiationFlag;
		bool constantTemperatureFlag;
		float emissivityModifier;

	};

	struct PolygonPoint
	{
		int index;
		float x;
		float y;
	};

	struct Polygon
	{
		int id;
		std::string material;
		std::vector<PolygonPoint> points;
	};

	struct BoundaryConditionPolygon
	{
		int id;
		std::string name;
		int polygonId;
		int enclosureId;
		std::string ufactorTag;
		int ratationModel;
		float emissivity;
		std::string surfaceSide;
		std::string illuminatedSurface;
		std::vector<PolygonPoint> points;
	};

	struct ThmxFileContents
	{
		std::string fileVersion;
		std::optional<MeshParameters> meshParameters;
		std::vector<Material> materials;
		std::vector<BoundaryCondition> boundaryConditions;
		std::vector<Polygon> polygons;
		std::vector<BoundaryConditionPolygon> boundaryConditionPolygons;
	};

	ThmxFileContents parseFile(std::string const & path);
}   // namespace thmxParser