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

        float frontAbsorptivity = 0;
        float backAbsorptivity = 0;


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

    void parseFile(std::string const & path)
    {
        XMLParser::XMLNode topNode = XMLParser::XMLNode::openFileHelper(path.c_str(), "THERM-XML");

        auto versionNode = topNode.getChildNode("FileVersion");

        bool fileVersionError = versionNode.isEmpty();

        if(fileVersionError)
        {
            throw std::runtime_error("Missing file version");
        }
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
		auto boundaryConditions = parseBoundaryConditions(bCondsNode);
#if 0
        
        XMLNode polygonsNode = topNode.getChildNode("Polygons");
        if(!polygonsNode.isEmpty())
            try
            {
                int iPolygon = 0;
                while(1)
                {
                    XMLNode polygonNode = polygonsNode.getChildNode("Polygon", &iPolygon);
                    if(polygonNode.isEmpty())
                        break;
                    std::string id = polygonNode.getAttribute("ID");
                    int iPoly = std::stoi(id);
                    CPolyShape * pPoly = FindPolyID(iPoly);
                    std::string material = polygonNode.getAttribute("Material");
                    BOOL newPoint = FALSE;
                    if(pPoly == NULL && creatPoly)
                    {
                        pPoly = new CPolyMaterial(this, material);
                        pPoly->SetID(iPoly);
                        m_PolyShapeList.AddTail(pPoly);
                        newPoint = TRUE;
                        SetModifiedFlag(YES,
                                        YES);   // Mark the document as having been modified, for
                                                // purposes of confirming File Close.
                    }
                    if(pPoly != NULL)
                    {
                        pPoly->SetName(material);
                        int i = 0;
                        while(1)
                        {
                            XMLNode pointNode = polygonNode.getChildNode("Point", &i);
                            if(pointNode.isEmpty())
                                break;
                            std::string indexStr = pointNode.getAttribute("index");
                            int index = std::stoi(indexStr);
                            std::string xstr = pointNode.getAttribute("x");
                            float x = std::stof(xstr);
                            std::string ystr = pointNode.getAttribute("y");
                            float y = std::stof(ystr);
                            // convert to internal coords and set!
                            CRPoint pt;
                            // CRPoint org = GetOrigin();
                            pt.x = x;   //  + org.x;  (used to adjust for origin)
                            pt.y = y;
#    ifdef _DEBUG
                            RPOINT old = pPoly->GetPoint(index);
#    endif
                            if(newPoint)
                                pPoly->AddPoint(pt);
                            else
                                pPoly->SetPoint(pt, index);
                        }
                    }
                }
            }
            catch(int iThrow)
            {
                return iThrow;
            }

        XMLNode bcsNode = topNode.getChildNode("Boundaries");
        if(!bcsNode.isEmpty())
            try
            {
                int iPolygon = 0;
                while(1)
                {
                    XMLNode bcNode = bcsNode.getChildNode("BCPolygon", &iPolygon);
                    if(bcNode.isEmpty())
                        break;
                    std::string id = bcNode.getAttribute("ID");
                    int iPoly = std::stoi(id);
                    CPolyShape * pPoly = FindPolyID(iPoly);
                    std::string bcName = bcNode.getAttribute("BC");
                    std::string PolygonId = bcNode.getAttribute("PolygonID");
                    int iPolygonId = std::stoi(PolygonId);
                    std::string EnclosureID = bcNode.getAttribute("EnclosureID");
                    int iEnclosureID;
                    iEnclosureID = std::stoi(EnclosureID);
                    std::string UFactorTag = bcNode.getAttribute("UFactorTag");
                    std::string RadiationModel = bcNode.getAttribute("RadiationModel");
                    int iRadiationModel = std::stoi(RadiationModel);
                    std::string Emissivity = bcNode.getAttribute("Emissivity");
                    float fEmissivity = -1.0;
                    if(!Emissivity.IsEmpty())
                        fEmissivity = std::stoi(Emissivity);

                    BOOL newPoint = FALSE;
                    if(pPoly == NULL && creatPoly)
                    {
                        pPoly = new CPolyBC(this, MAKESTRING(IDS_ADIABATIC));
                        pPoly->SetID(iPoly);
                        pPoly->SetEnclosure(iEnclosureID);
                        ((CPolyBC *)pPoly)->SetFluxTag(UFactorTag);
                        newPoint = TRUE;
                    }
                    if(pPoly != NULL)
                    {
                        pPoly->SetName(bcName);
                        if(fEmissivity > 0.0)
                        {
                            ((CPolyBC *)pPoly)->SetEmissivity(fEmissivity);
                            ((CPolyBC *)pPoly)->SetUseGlobalEmissivity(FALSE);
                        }

                        std::string surfaceSide = bcNode.getAttribute("MaterialSide");
                        if(!surfaceSide.IsEmpty())
                        {
                            if(surfaceSide.CompareNoCase("Front") == 0)
                                ((CPolyBC *)pPoly)->setSurfaceSide(FenestrationCommon::Front);
                            else
                                ((CPolyBC *)pPoly)->setSurfaceSide(FenestrationCommon::Back);
                        }

                        std::string IlluminatedSurface = bcNode.getAttribute("IlluminatedSurface");
                        if(!IlluminatedSurface.IsEmpty())
                        {
                            if(IlluminatedSurface.CompareNoCase("True") == 0)
                            {
                                ((CPolyBC *)pPoly)->setIlluminatedSurface(ILLUMINATION_SOURCE_YES);
                            }
                            else
                            {
                                ((CPolyBC *)pPoly)->setIlluminatedSurface(ILLUMINATION_SOURCE_NO);
                            }
                        }

                        int i = 0;
                        while(1)
                        {
                            XMLNode pointNode = bcNode.getChildNode("Point", &i);
                            if(pointNode.isEmpty())
                                break;
                            std::string indexStr = pointNode.getAttribute("index");
                            int index = std::stoi(indexStr);
                            std::string xstr = pointNode.getAttribute("x");
                            float x = std::stof(xstr);
                            std::string ystr = pointNode.getAttribute("y");
                            float y = std::stof(ystr);
                            // convert to internal coords and set!
                            CRPoint pt;
                            pt.x = x;
                            pt.y = y;
                            if(newPoint)
                                pPoly->AddPoint(pt);
                            else
                                pPoly->SetPoint(pt, index);
                        }
                    }
                    if(newPoint)
                    {
                        CPolyShape * pAssociatedPoly = NULL;
                        for(POSITION pos = GetFirstPolyShapePos(); pos != NULL;)
                        {
                            pAssociatedPoly = GetNextPolyShape(pos);
                            if(pAssociatedPoly == NULL)
                                continue;
                            if(pAssociatedPoly->GetType() & POLYSHAPE_MATERIAL == 0)
                                continue;
                            if(pAssociatedPoly->GetID() == iPolygonId)
                                break;
                        }
                        if(!pAssociatedPoly)
                            return -1;
                        ((CPolyBC *)pPoly)->SetPoly(pAssociatedPoly);
                        pPoly->FinishPolyShape(NO);
                        m_PolyShapeList.AddTail(pPoly);
                        SetModifiedFlag(YES,
                                        YES);   // Mark the document as having been modified, for
                                                // purposes of confirming File Close.
                    }
                }
            }
            catch(int iThrow)
            {
                return iThrow;
            }
#endif
    }
}   // namespace thmxParser
