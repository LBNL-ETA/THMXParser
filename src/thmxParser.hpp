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
        unsigned int r;
        unsigned int g;
        unsigned int b;
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

    struct CMABestWorstOption
    {
        std::string option;
        double insideConvectiveFilmCoefficient;
        double outsideConvectiveFilmCoefficient;
        double glazingGapConductance;
        double spacerConductance;
    };

    struct CMAOptions
    {
        double interiorLayerConductivity;
        double interiorLayerThickness;
        double interiorLayerEmissivity;
        double exteriorLayerConductivity;
        double exteriorLayerThickness;
        double exteriorLayerEmissivity;
        double interiorTemperature;
        double exteriorTemperature;
        std::map<std::string, CMABestWorstOption> bestWorstOptions;
    };

    struct UFactorProjectionResult
    {
        std::string lengthType;
        std::string lengthUnits;
        float length;
        std::string ufactorUnits;
        std::string ufactor;
    };

    struct UFactorResults
    {
        std::string tag;
        std::string deltaTUnits;
        float deltaT;
        std::vector<UFactorProjectionResult> projectionResults;
    };

    struct Result
    {
        std::string modelType;
        std::string glazingCase;
        std::string spacerCase;
        std::vector<UFactorResults> ufactorResults;
    };

    struct GlazingSystem
    {
        int index;
        int nLayers;
        float width;
        std::string units;
    };

    struct ThmxFileContents
    {
        std::string fileVersion;
        std::optional<MeshParameters> meshParameters;
        std::vector<Material> materials;
        std::vector<BoundaryCondition> boundaryConditions;
        std::vector<GlazingSystem> glazingSystems;
        std::vector<Polygon> polygons;
        std::vector<BoundaryConditionPolygon> boundaryConditionPolygons;
        std::optional<CMAOptions> cmaOptions;
        std::vector<Result> results;
    };

    ThmxFileContents parseFile(std::string const & path);
    ThmxFileContents parseString(std::string const & data);
}   // namespace thmxParser
