#include "thmxParser.hpp"
#include <xmlParser/xmlParser.h>
#include <sstream>

namespace thmxParser
{
    // The original THERM code used CString which is not cross-platform
    // However XMLParser returns NULL if an attribute is missing.
    // CString operator= with NULL generates an empty string
    // However std::string gives a runtime error when trying to initialize with NULL
    // Implementing the following to get same functionality, i.e. empty string
    // when an attribute is missing, rather than make everything std::optional
    std::string getAttribute(XMLParser::XMLNode const & node,
                             std::string const & attribute,
                             int * i = nullptr)
    {
        if(node.isAttributeSet(attribute.c_str()))
        {
            return node.getAttribute(attribute.c_str(), i);
        }
        return std::string();
    }

	// See the comment for getAttribute
	std::string getChildNodeText(XMLParser::XMLNode const & node,
		std::string const & childNodeName)
	{
		std::string contents;
		const char * txt = node.getChildNode(childNodeName.c_str()).getText();
		if(txt != nullptr)
		{
			contents = txt;
		}
		return contents;
	}

    std::optional<MeshParameters> parseMeshParameters(XMLParser::XMLNode const & meshNode)
    {
        std::optional<MeshParameters> meshParams;
        if(!meshNode.isEmpty())
        {
            std::optional<int> quadTreeMeshLevel;
            std::optional<bool> errorCheckFlag;
            std::optional<float> errorLimit;
            std::optional<int> maxIterations;
            std::optional<bool> cmaFlag;

            std::string str;
            str = getAttribute(meshNode, "MeshLevel");
            if(!str.empty())
                quadTreeMeshLevel = std::stoi(str);
            str = getAttribute(meshNode, "ErrorCheckFlag");
            if(!str.empty())
                errorCheckFlag = static_cast<bool>(std::stoi(str));
            str = getAttribute(meshNode, "ErrorLimit");
            if(!str.empty())
                errorLimit = std::stof(str);
            str = getAttribute(meshNode, "MaxIterations");
            if(!str.empty())
                maxIterations = std::stoi(str);
            str = getAttribute(meshNode, "CMAflag");
            if(!str.empty())
                cmaFlag = static_cast<bool>(std::stoi(str));

            meshParams =
              MeshParameters{quadTreeMeshLevel, errorCheckFlag, errorLimit, maxIterations, cmaFlag};
        }

        return meshParams;
    }

    Material parseMaterial(XMLParser::XMLNode const & materialNode)
    {
        std::string name = getAttribute(materialNode, "Name");
        // Name = GetDocName() + ":" + Name;
        std::string str = getAttribute(materialNode, "Type");
        int type = std::stoi(str);
        str = getAttribute(materialNode, "Conductivity");
        float conductivity = std::stof(str);
#if 0
		if(Conductivity < 0)
		{
			Conductivity = 1;
			ResultsOK = FALSE;
		}
#endif
        // str = getAttribute(materialNode,  "Absorptivity" );
        // float Absorptivity = std::stoi( str );
        str = getAttribute(materialNode, "EmissivityFront");
        const float emissivityFront = std::stof(str);
        str = getAttribute(materialNode, "EmissivityBack");
        const float emissivityBack = std::stof(str);
        str = getAttribute(materialNode, "Tir");
        float tir = std::stof(str);
        // str = getAttribute(materialNode,  "WindowDB" );
        // std::string windowDatabase = str;
        // str = getAttribute(materialNode,  "WindowID" );
        // int shadeID = std::stoi( str );
        str = getAttribute(materialNode, "RGBColor");
        int red, green, blue;
        std::sscanf(str.c_str(), "0x%02x%02x%02x", &red, &green, &blue);
        ColorRGB color{red, green, blue};
        int cavityModel = 0;
        str = getAttribute(materialNode, "CavityModel");
        if(!str.empty())
        {
            cavityModel = std::stoi(str);
            // Tir for frame cavities is saved as -1. This should set Tir of any cavity
            // back to 1 regardless to what is set in THMX file
            tir = 1.0;
        }

        int iProp = 0;
        XMLParser::XMLNode materialPropertiesNode = materialNode.getChildNode("Property", &iProp);

        std::map<OpticalTuple, float> transmittances;
        std::map<OpticalTuple, float> reflectances;

        while(!materialPropertiesNode.isEmpty())
        {
            std::string side = getAttribute(materialPropertiesNode, "Side");
            std::string range = getAttribute(materialPropertiesNode, "Range");
            std::string specularity = getAttribute(materialPropertiesNode, "Specularity");
            transmittances[OpticalTuple(side, range, specularity)] =
              std::stof(materialPropertiesNode.getAttribute("T"));
            reflectances[OpticalTuple(side, range, specularity)] =
              std::stof(materialPropertiesNode.getAttribute("R"));

            materialPropertiesNode = materialNode.getChildNode("Property", &iProp);
        }
        return Material{name,
                        type,
                        conductivity,
                        emissivityFront,
                        emissivityBack,
                        tir,
                        color,
                        cavityModel,
                        transmittances,
                        reflectances};
    }

    std::vector<Material> parseMaterials(XMLParser::XMLNode const & materialsNode)
    {
        std::vector<Material> materials;

        if(!materialsNode.isEmpty())
        {
            int iMaterial = 0;
            while(true)
            {
                XMLParser::XMLNode materialNode =
                  materialsNode.getChildNode("Material", &iMaterial);
                if(materialNode.isEmpty())
                    break;
                materials.push_back(parseMaterial(materialNode));
            }
        }

        return materials;
    }

    BoundaryCondition parseBoundaryCondition(XMLParser::XMLNode const & bcondNode)
    {
        std::string name = getAttribute(bcondNode, "Name");
        std::string str = getAttribute(bcondNode, "Type");
        int type = std::stoi(str);
        str = getAttribute(bcondNode, "H");
        float H = std::stof(str);
        str = getAttribute(bcondNode, "HeatFLux");
        float heatFlux = std::stof(str);
        str = getAttribute(bcondNode, "Temperature");
        float temperature = std::stof(str);
        str = getAttribute(bcondNode, "RGBColor");
        int red, green, blue;
        std::sscanf(str.c_str(), "0x%02x%02x%02x", &red, &green, &blue);
        ColorRGB color{red, green, blue};

        str = getAttribute(bcondNode, "Tr");
        float Tr = std::stof(str);
        str = getAttribute(bcondNode, "Hr");
        float Hr = std::stof(str);
        str = getAttribute(bcondNode, "Ei");
        float Ei = std::stof(str);
        str = getAttribute(bcondNode, "Viewfactor");
        float viewFactor = std::stof(str);
        str = getAttribute(bcondNode, "RadiationModel");
        int radiationModel = std::stoi(str);

        str = getAttribute(bcondNode, "ConvectionFlag");
        bool convectionFlag = static_cast<bool>(std::stoi(str));
        str = getAttribute(bcondNode, "FluxFlag");
        bool fluxFlag = static_cast<bool>(std::stoi(str));
        str = getAttribute(bcondNode, "RadiationFlag");
        bool radiationFlag = static_cast<bool>(std::stoi(str));
        str = getAttribute(bcondNode, "ConstantTemperatureFlag");
        bool constantTemperatureFlag = static_cast<bool>(std::stoi(str));
        str = getAttribute(bcondNode, "EmisModifier");
        float emisModifier = std::stof(str);

        return BoundaryCondition{name,
                                 type,
                                 H,
                                 heatFlux,
                                 temperature,
                                 color,
                                 Tr,
                                 Hr,
                                 Ei,
                                 viewFactor,
                                 radiationModel,
                                 convectionFlag,
                                 fluxFlag,
                                 radiationFlag,
                                 constantTemperatureFlag,
                                 emisModifier};
    }

    std::vector<BoundaryCondition> parseBoundaryConditions(XMLParser::XMLNode const & bcondsNode)
    {
        std::vector<BoundaryCondition> boundaryConditions;

        if(!bcondsNode.isEmpty())
        {
            int ibc = 0;
            while(true)
            {
                XMLParser::XMLNode bcondNode = bcondsNode.getChildNode("BoundaryCondition", &ibc);
                if(bcondNode.isEmpty())
                    break;
                boundaryConditions.push_back(parseBoundaryCondition(bcondNode));
            }
        }

        return boundaryConditions;
    }

    PolygonPoint parsePolygonPoint(XMLParser::XMLNode const & pointNode)
    {
        std::string indexStr = getAttribute(pointNode, "index");
        int index = std::stoi(indexStr);
        std::string xstr = getAttribute(pointNode, "x");
        float x = std::stof(xstr);
        std::string ystr = getAttribute(pointNode, "y");
        float y = std::stof(ystr);
        return PolygonPoint{index, x, y};
    }

    Polygon parsePolygon(XMLParser::XMLNode const & polygonNode)
    {
        std::string id = getAttribute(polygonNode, "ID");
        int iPoly = std::stoi(id);
        std::string material = getAttribute(polygonNode, "Material");
        // pPoly->SetName(material);
        std::vector<PolygonPoint> points;
        int i = 0;
        while(1)
        {
            XMLParser::XMLNode pointNode = polygonNode.getChildNode("Point", &i);
            if(pointNode.isEmpty())
                break;
            points.push_back(parsePolygonPoint(pointNode));
        }
        return Polygon{iPoly, material, points};
    }

    std::vector<Polygon> parsePolygons(XMLParser::XMLNode const & polygonsNode)
    {
        std::vector<Polygon> polygons;

        if(!polygonsNode.isEmpty())
        {
            int ibc = 0;
            while(true)
            {
                XMLParser::XMLNode polygonNode = polygonsNode.getChildNode("Polygon", &ibc);
                if(polygonNode.isEmpty())
                    break;
                polygons.push_back(parsePolygon(polygonNode));
            }
        }

        return polygons;
    }

    BoundaryConditionPolygon parseBoundaryConditionPolygon(XMLParser::XMLNode const & bcNode)
    {
        std::string id = getAttribute(bcNode, "ID");
        int iPoly = std::stoi(id);
        std::string bcName = getAttribute(bcNode, "BC");
        std::string polygonId = getAttribute(bcNode, "PolygonID");
        int iPolygonId = std::stoi(polygonId);
        std::string enclosureID = getAttribute(bcNode, "EnclosureID");
        int iEnclosureID;
        iEnclosureID = std::stoi(enclosureID);
        std::string ufactorTag = getAttribute(bcNode, "UFactorTag");
        std::string radiationModel = getAttribute(bcNode, "RadiationModel");
        int iRadiationModel = 0;
        if(!radiationModel.empty())
        {
            iRadiationModel = std::stoi(radiationModel);
        }
        std::string emissivity = getAttribute(bcNode, "Emissivity");
        float fEmissivity = -1.0;
        if(!emissivity.empty())
        {
            fEmissivity = std::stof(emissivity);
        }

        std::string surfaceSide = getAttribute(bcNode, "MaterialSide");
        std::string illuminatedSurface = getAttribute(bcNode, "IlluminatedSurface");

        std::vector<PolygonPoint> points;
        int i = 0;
        while(1)
        {
            XMLParser::XMLNode pointNode = bcNode.getChildNode("Point", &i);
            if(pointNode.isEmpty())
                break;
            points.push_back(parsePolygonPoint(pointNode));
        }

        return BoundaryConditionPolygon{iPoly,
                                        bcName,
                                        iPolygonId,
                                        iEnclosureID,
                                        ufactorTag,
                                        iRadiationModel,
                                        fEmissivity,
                                        surfaceSide,
                                        illuminatedSurface,
                                        points};
    }

    std::vector<BoundaryConditionPolygon>
      parseBoundaryConditionPolygons(XMLParser::XMLNode const & bcsNode)
    {
        std::vector<BoundaryConditionPolygon> boundaryConditionPolygons;

        if(!bcsNode.isEmpty())
        {
            int iPolygon = 0;
            while(true)
            {
                XMLParser::XMLNode bcNode = bcsNode.getChildNode("BCPolygon", &iPolygon);
                if(bcNode.isEmpty())
                    break;
                boundaryConditionPolygons.push_back(parseBoundaryConditionPolygon(bcNode));
            }
        }

        return boundaryConditionPolygons;
    }

    double getDoubleFromChildNode(XMLParser::XMLNode const & xmlNode, std::string const & nodeName)
    {
        XMLParser::XMLNode node = xmlNode.getChildNode(nodeName.c_str());
        const std::string str = node.getText();
        return std::stod(str.c_str());
    }

    CMABestWorstOption parseCMABestWorstOption(XMLParser::XMLNode const & bestWorstOptionNode)
    {
        std::string option = getChildNodeText(bestWorstOptionNode, "Case");

        double insideConvectiveFilmCoefficient =
          getDoubleFromChildNode(bestWorstOptionNode, "InsideEffectiveFilmCoefficient");
        double outsideConvectiveFilmCoefficient =
          getDoubleFromChildNode(bestWorstOptionNode, "OutsideEffectiveFilmCoefficient");
        double glazingGapConductance =
          getDoubleFromChildNode(bestWorstOptionNode, "GlazingGapConductance");
        double spacerConductance = getDoubleFromChildNode(bestWorstOptionNode, "SpacerConductance");

        return CMABestWorstOption{option,
                                  insideConvectiveFilmCoefficient,
                                  outsideConvectiveFilmCoefficient,
                                  glazingGapConductance,
                                  spacerConductance};
    }

    std::optional<CMAOptions> parseCMAOptions(XMLParser::XMLNode const & cmaNode)
    {
        if(cmaNode.isEmpty())
        {
            return std::optional<CMAOptions>();
        }

        double interiorLayerConductivity =
          getDoubleFromChildNode(cmaNode, "InteriorLayerConductivity");
        double interiorLayerThickness = getDoubleFromChildNode(cmaNode, "InteriorLayerThickness");
        double interiorLayerEmissivity = getDoubleFromChildNode(cmaNode, "InteriorLayerEmissivity");
        double exteriorLayerConductivity =
          getDoubleFromChildNode(cmaNode, "ExteriorLayerConductivity");
        double exteriorLayerThickness = getDoubleFromChildNode(cmaNode, "ExteriorLayerThickness");
        double exteriorLayerEmissivity = getDoubleFromChildNode(cmaNode, "ExteriorLayerEmissivity");
        double interiorTemperature = getDoubleFromChildNode(cmaNode, "InteriorTemperature");
        double exteriorTemperature = getDoubleFromChildNode(cmaNode, "ExteriorTemperature");

        std::map<std::string, CMABestWorstOption> bestWorstOptions;
        int i{0};
        XMLParser::XMLNode bestWorstOptionNode = cmaNode.getChildNode("BestWorstOptions", &i);
        while(!bestWorstOptionNode.isEmpty())
        {
            auto bestWorstOption = parseCMABestWorstOption(bestWorstOptionNode);
            if(bestWorstOptions.find(bestWorstOption.option) != bestWorstOptions.end())
            {
                std::stringstream msg;
                msg << "Error parsing THMX file.  CMA option repeated: " << bestWorstOption.option;
                throw std::runtime_error(msg.str());
            }
            bestWorstOptions[bestWorstOption.option] = bestWorstOption;
            bestWorstOptionNode = cmaNode.getChildNode("BestWorstOptions", &i);
        }

        return CMAOptions{interiorLayerConductivity,
                          interiorLayerThickness,
                          interiorLayerEmissivity,
                          exteriorLayerConductivity,
                          exteriorLayerThickness,
                          exteriorLayerEmissivity,
                          interiorTemperature,
                          exteriorTemperature,
                          bestWorstOptions};
    }

    UFactorProjectionResult parseUFactorProjection(XMLParser::XMLNode const & ufactorProjectionNode)
    {
        std::string lengthType = getChildNodeText(ufactorProjectionNode, "Length-type");
        auto lengthNode = ufactorProjectionNode.getChildNode("Length");
        auto ufactorNode = ufactorProjectionNode.getChildNode("U-factor");
        std::string lengthUnit = getAttribute(lengthNode, "units");
        float length = std::stof(getAttribute(lengthNode, "value"));
        std::string ufactorUnit = getAttribute(ufactorNode, "units");
        float ufactor = std::stof(getAttribute(ufactorNode, "value"));
        return UFactorProjectionResult{lengthType, lengthUnit, length, ufactorUnit, ufactor};
    }

    UFactorResults parseUFactorResults(XMLParser::XMLNode const & ufactorResultsNode)
    {
        std::string tag = getChildNodeText(ufactorResultsNode, "Tag");
        auto deltaTNode = ufactorResultsNode.getChildNode("DeltaT");
        std::string deltaTUnits = getAttribute(deltaTNode, "units");
        float deltaT = std::stof(getAttribute(deltaTNode, "value"));

        std::vector<UFactorProjectionResult> projectionResults;
        int i{0};
        XMLParser::XMLNode projectionResultsNode =
          ufactorResultsNode.getChildNode("Projection", &i);
        while(!projectionResultsNode.isEmpty())
        {
            projectionResults.push_back(parseUFactorProjection(projectionResultsNode));
            projectionResultsNode = ufactorResultsNode.getChildNode("Projection", &i);
        }

        return UFactorResults{tag, deltaTUnits, deltaT, projectionResults};
    }

    Result parseResult(XMLParser::XMLNode const & resultNode)
    {
        std::string modelType = getChildNodeText(resultNode, "ModelType");
        std::string glazingCase = getChildNodeText(resultNode, "GlazingCase");
        std::string spacerCase = getChildNodeText(resultNode, "SpacerCase");

        std::vector<UFactorResults> ufactorResults;
        int i{0};
        XMLParser::XMLNode ufactorResultsNode = resultNode.getChildNode("U-factors", &i);
        while(!ufactorResultsNode.isEmpty())
        {
			ufactorResults.push_back(parseUFactorResults(ufactorResultsNode));
			ufactorResultsNode = resultNode.getChildNode("U-factors", &i);
        }

        return Result{modelType, glazingCase, spacerCase, ufactorResults};
    }

    std::vector<Result> parseResults(XMLParser::XMLNode const & resultsNode)
    {
        std::vector<Result> results;
        int i{0};
        XMLParser::XMLNode resultNode = resultsNode.getChildNode("Case", &i);
        while(!resultNode.isEmpty())
        {
            results.push_back(parseResult(resultNode));
            resultNode = resultsNode.getChildNode("Case", &i);
        }
        return results;
    }

    ThmxFileContents parseFile(std::string const & path)
    {
        XMLParser::XMLNode topNode = XMLParser::XMLNode::openFileHelper(path.c_str(), "THERM-XML");

        auto versionNode = topNode.getChildNode("FileVersion");

        bool fileVersionError = versionNode.isEmpty();

        if(fileVersionError)
        {
            throw std::runtime_error("Missing file version");
        }

        std::string fileVersion = versionNode.getText();

        XMLParser::XMLNode meshNode = topNode.getChildNode("MeshControl");
        auto meshParams = parseMeshParameters(meshNode);

        // materials
        XMLParser::XMLNode materialsNode = topNode.getChildNode("Materials");
        auto materials = parseMaterials(materialsNode);

        // boundary conditions
        XMLParser::XMLNode bcondsNode = topNode.getChildNode("BoundaryConditions");
        auto boundaryConditions = parseBoundaryConditions(bcondsNode);

        XMLParser::XMLNode polygonsNode = topNode.getChildNode("Polygons");
        auto polygons = parsePolygons(polygonsNode);

        XMLParser::XMLNode bcsNode = topNode.getChildNode("Boundaries");
        auto boundaryConditionPolygons = parseBoundaryConditionPolygons(bcsNode);

        XMLParser::XMLNode cmaNode = topNode.getChildNode("CMAGlazingSystemSettings");
        auto cmaOptions = parseCMAOptions(cmaNode);

        XMLParser::XMLNode resultsNode = topNode.getChildNode("Results");
        auto cmaResults = parseResults(resultsNode);

        return ThmxFileContents{fileVersion,
                                meshParams,
                                materials,
                                boundaryConditions,
                                polygons,
                                boundaryConditionPolygons,
                                cmaOptions,
                                cmaResults};
    }
}   // namespace thmxParser
