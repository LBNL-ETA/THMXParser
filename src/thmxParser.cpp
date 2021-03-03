#include "thmxParser.hpp"
#include <xmlParser/xmlParser.h>

namespace thmxParser
{
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
            str = meshNode.getAttribute("MeshLevel");
            if(!str.empty())
                quadTreeMeshLevel = std::stoi(str);
            str = meshNode.getAttribute("ErrorCheckFlag");
            if(!str.empty())
                errorCheckFlag = static_cast<bool>(std::stoi(str));
            str = meshNode.getAttribute("ErrorLimit");
            if(!str.empty())
                errorLimit = std::stof(str);
            str = meshNode.getAttribute("MaxIterations");
            if(!str.empty())
                maxIterations = std::stoi(str);
            str = meshNode.getAttribute("CMAflag");
            if(!str.empty())
                cmaFlag = static_cast<bool>(std::stoi(str));

            meshParams =
              MeshParameters{quadTreeMeshLevel, errorCheckFlag, errorLimit, maxIterations, cmaFlag};
        }

        return meshParams;
    }

    Material parseMaterial(XMLParser::XMLNode const & materialNode)
    {
        std::string name = materialNode.getAttribute("Name");
        // Name = GetDocName() + ":" + Name;
        std::string str = materialNode.getAttribute("Type");
        int type = std::stoi(str);
        str = materialNode.getAttribute("Conductivity");
        float conductivity = std::stof(str);
#if 0
		if(Conductivity < 0)
		{
			Conductivity = 1;
			ResultsOK = FALSE;
		}
#endif
        // str = materialNode.getAttribute( "Absorptivity" );
        // float Absorptivity = std::stoi( str );
        str = materialNode.getAttribute("EmissivityFront");
        const float emissivityFront = std::stof(str);
        str = materialNode.getAttribute("EmissivityBack");
        const float emissivityBack = std::stof(str);
        str = materialNode.getAttribute("Tir");
        float tir = std::stof(str);
        // str = materialNode.getAttribute( "WindowDB" );
        // std::string windowDatabase = str;
        // str = materialNode.getAttribute( "WindowID" );
        // int shadeID = std::stoi( str );
        str = materialNode.getAttribute("RGBColor");
        int red, green, blue;
        std::sscanf(str.c_str(), "0x%02x%02x%02x", &red, &green, &blue);
        ColorRGB color{red, green, blue};
        str = materialNode.getAttribute("CavityModel");
        int cavityModel = 0;
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
            std::string side = materialPropertiesNode.getAttribute("Side");
            std::string range = materialPropertiesNode.getAttribute("Range");
            std::string specularity = materialPropertiesNode.getAttribute("Specularity");
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
        std::string name = bcondNode.getAttribute("Name");
        std::string str = bcondNode.getAttribute("Type");
        int type = std::stoi(str);
        str = bcondNode.getAttribute("H");
        float H = std::stof(str);
        str = bcondNode.getAttribute("HeatFLux");
        float heatFlux = std::stof(str);
        str = bcondNode.getAttribute("Temperature");
        float temperature = std::stof(str);
        str = bcondNode.getAttribute("RGBColor");
        int red, green, blue;
        std::sscanf(str.c_str(), "0x%02x%02x%02x", &red, &green, &blue);
        ColorRGB color{red, green, blue};

        str = bcondNode.getAttribute("Tr");
        float Tr = std::stof(str);
        str = bcondNode.getAttribute("Hr");
        float Hr = std::stof(str);
        str = bcondNode.getAttribute("Ei");
        float Ei = std::stof(str);
        str = bcondNode.getAttribute("Viewfactor");
        float viewFactor = std::stof(str);
        str = bcondNode.getAttribute("RadiationModel");
        int radiationModel = std::stoi(str);

        str = bcondNode.getAttribute("ConvectionFlag");
        bool convectionFlag = static_cast<bool>(std::stoi(str));
        str = bcondNode.getAttribute("FluxFlag");
        bool fluxFlag = static_cast<bool>(std::stoi(str));
        str = bcondNode.getAttribute("RadiationFlag");
        bool radiationFlag = static_cast<bool>(std::stoi(str));
        str = bcondNode.getAttribute("ConstantTemperatureFlag");
        bool constantTemperatureFlag = static_cast<bool>(std::stoi(str));
        str = bcondNode.getAttribute("EmisModifier");
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
        std::string indexStr = pointNode.getAttribute("index");
        int index = std::stoi(indexStr);
        std::string xstr = pointNode.getAttribute("x");
        float x = std::stof(xstr);
        std::string ystr = pointNode.getAttribute("y");
        float y = std::stof(ystr);
        return PolygonPoint{index, x, y};
    }

    Polygon parsePolygon(XMLParser::XMLNode const & polygonNode)
    {
        std::string id = polygonNode.getAttribute("ID");
        int iPoly = std::stoi(id);
        std::string material = polygonNode.getAttribute("Material");
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
                XMLParser::XMLNode polygonNode =
                  polygonsNode.getChildNode("BoundaryCondition", &ibc);
                if(polygonNode.isEmpty())
                    break;
                polygons.push_back(parsePolygon(polygonNode));
            }
        }

        return polygons;
    }

    BoundaryConditionPolygon parseBoundaryConditionPolygon(XMLParser::XMLNode const & bcNode)
    {
        std::string id = bcNode.getAttribute("ID");
        int iPoly = std::stoi(id);
        std::string bcName = bcNode.getAttribute("BC");
        std::string polygonId = bcNode.getAttribute("PolygonID");
        int iPolygonId = std::stoi(polygonId);
        std::string enclosureID = bcNode.getAttribute("EnclosureID");
        int iEnclosureID;
        iEnclosureID = std::stoi(enclosureID);
        std::string ufactorTag = bcNode.getAttribute("UFactorTag");
        std::string radiationModel = bcNode.getAttribute("RadiationModel");
        int iRadiationModel = std::stoi(radiationModel);
        std::string emissivity = bcNode.getAttribute("Emissivity");
        float fEmissivity = -1.0;
        if(!emissivity.empty())
        {
            fEmissivity = std::stof(emissivity);
        }

        std::string surfaceSide = bcNode.getAttribute("MaterialSide");
        std::string illuminatedSurface = bcNode.getAttribute("IlluminatedSurface");

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
#if 0
        if(!versionNode.isEmpty())
        {
            const auto fileVerionNumber = std::stoi(versionNode.getText());
            fileVersionError =
              fileVersionError || (fileVerionNumber != LBNLConstants::THMXFileVersion);
        }
#endif

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

        return ThmxFileContents{fileVersion,
                                meshParams,
                                materials,
                                boundaryConditions,
                                polygons,
                                boundaryConditionPolygons};
    }
}   // namespace thmxParser
