#include "thmxParser.hpp"
#include <xmlParser/xmlParser.h>

void thmxParser::parseFile(std::string const & path)
{
    XMLParser::XMLNode topNode = XMLParser::XMLNode::openFileHelper(path.c_str(), "THERM-XML");

    auto versionNode = topNode.getChildNode(_T("FileVersion"));

    bool fileVersionError = versionNode.isEmpty();

    if(fileVersionError)
    {
        throw std::runtime_error("Missing file version");
    }
}
