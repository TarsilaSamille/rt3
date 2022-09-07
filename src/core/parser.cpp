/*!
 * Implementation of XML processing functions.
 * @file parser.h
 */

#include "parser.h"

#include "api.h"
#include "paramset.h"
#include "rt3.h"

// === Function Implementation

namespace rt3 {

using rt3::Point3f;
using rt3::Vector3f;
using rt3::Vector3i;

/// Lambda expression that returns a lowercase version of the input string.
auto CSTR_LOWERCASE = [](const char *t) -> std::string {
  std::string str{t};
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
};

/// This is the entry function for the parsing process.
void parse(const char *scene_file_name) {
  tinyxml2::XMLDocument xml_doc;

  // Load file.
  if (xml_doc.LoadFile(scene_file_name) != tinyxml2::XML_SUCCESS)
    RT3_ERROR(std::string{"The file \""} + scene_file_name +
              std::string{"\" either is not available OR contains an invalid "
                          "RT3 scene provided!"});

  // ===============================================
  // Get a pointer to the document's root node.
  tinyxml2::XMLNode *p_root = xml_doc.FirstChild();
  if (p_root == nullptr)
    RT3_ERROR("Error while trying to find \"RT3\" tag in the scene file.");
  // ===============================================

  // Get the first-level tag inside the root node.
  auto *p_child{p_root->FirstChildElement()};
  if (p_child == nullptr)
    RT3_ERROR(
        "No \"children\" tags found inside the \"RT3\" tag. Empty scene file?");

  parse_tags(p_child, /* initial level */ 0);
}

/// Main loop that handles each possible tag we may find in a RT3 scene file.
void parse_tags(tinyxml2::XMLElement *p_element, int level) {
  // All messages sent to std::clog is for DEBUG purposes.
  std::clog << "[parse_tags()]: level is " << level << std::endl;

  // Traverse all items on the children's level.
  while (p_element != nullptr) {
    // Convert the attribute name to lowecase before testing it.
    auto tag_name = CSTR_LOWERCASE(p_element->Value());
    std::clog << "\n"
              << std::setw(level * 3) << ""
              << "***** Tag id is `" << tag_name << "`, at level " << level
              << std::endl;

    // Big switch for each possible RT3 tag type.
    if (tag_name == "background") {
      ParamSet ps;

      // TODO: retrieve information from the XML child into the ParamSet.
      vector<std::pair<param_type_e, string>> param_list{
          {param_type_e::STRING, "type"},
          {param_type_e::STRING, "filename"},  // Texture file name.
          {param_type_e::STRING, "mapping"},   // Type of mapping required.
          {param_type_e::COLOR,
           "color"},  // Single color for the entire background.
          {param_type_e::COLOR, "tl"},  // Top-left corner
          {param_type_e::COLOR, "tr"},  // Top-right corner
          {param_type_e::COLOR, "bl"},  // Bottom-left corner
          {param_type_e::COLOR, "br"}   // Bottom-right corner
      };
      parse_parameters(p_element, param_list, /* out */ &ps);

      // Calling the corresponding API method.
      API::background(ps);
    } else if (tag_name == "film") {
      ParamSet ps;

      // TODO: retrieve information from the XML child into the ParamSet.
      vector<std::pair<param_type_e, string>> param_list{
          {param_type_e::STRING, "type"},
          {param_type_e::STRING, "filename"},
          {param_type_e::STRING, "img_type"},
          {param_type_e::INT, "x_res"},
          {param_type_e::INT, "y_res"},
          {param_type_e::ARR_REAL, "crop_window"},
          {param_type_e::STRING, "gamma_corrected"}  // bool
      };
      parse_parameters(p_element, param_list, /* out */ &ps);

      // Calling the corresponding API method.
      API::film(ps);
    } else if (tag_name == "world_begin") {
      // std::clog << ">>> Entering WorldBegin, at level " << level+1 <<
      // std::endl;
      //  We should get only one `world` tag per scene file.
      API::world_begin();
    } else if (tag_name == "world_end") {
      API::world_end();
      // std::clog << ">>> Leaving WorldBegin, at level " << level+1 <<
      // std::endl;
    } else
      RT3_WARNING("Undefined tag `" + tag_name + "` found!");

    // Get next (to the right) sibling on this tree level.
    p_element = p_element->NextSiblingElement();
  }
}

/// Universal parameters parser.
/*!
 * This function receives a list of pairs <param_type, name>, traverse all the
 * attributes found in `p_element` and extract the attribute values into the
 * `ps_out` `ParamSet` object. Only named attributes found are read into the
 * `ps_out`.
 *
 * @param p_element XML element we are extracting information from.
 * @param param_list List of pairs <param_type, name> we need to extract from
 * the XML element.
 * @param ps_out The `ParamSet` object we need to fill in with parameter
 * information extracted from the XML element.
 */
void parse_parameters(tinyxml2::XMLElement *p_element,
                      const vector<std::pair<param_type_e, string>> param_list,
                      ParamSet *ps_out) {
  std::clog << "parse_parameters(): p_element = " << p_element << '\n';

  // Traverse the list of paramters pairs: type + name.
  for (const auto &e : param_list) {
    const auto &[type, name] = e;  // structured binding, requires C++ 17
    std::clog << "---Parsing att \"" << name << "\", type = " << (int)type
              << "\n";

    // This is just a dispatcher to the proper extraction functions.
    switch (type) {
      // TODO: Provide code or function call to each case you need at this
      // point.
      //=============================================================================
      // ATTENTION: We do not parse bool from the XML file because TinyXML2
      // cannot parse one. Bools are treated as strings.
      // case param_type_e::BOOL:
      //   parse_single_BASIC_attrib<bool>( p_element, ps_out, lname );
      //   break;
      //=============================================================================
      case param_type_e::UINT:
        break;
      case param_type_e::INT:
        break;
      case param_type_e::REAL:
        break;
      case param_type_e::STRING:
        parse_single_BASIC_attrib<std::string>(p_element, ps_out, name);
        break;
      case param_type_e::VEC3F:
        break;
      case param_type_e::VEC3I:
        break;
      case param_type_e::NORMAL3F:
        break;
      case param_type_e::POINT3F:
        break;
      // case param_type_e::POINT2I:
      // parse_single_COMPOSITE_attrib<int, Point2i, int(2)>( p_element, ps_out,
      // name ); break;
      case param_type_e::COLOR:
        break;
      case param_type_e::SPECTRUM:
        break;
      case param_type_e::ARR_REAL:
        break;
      case param_type_e::ARR_INT:
        break;
      case param_type_e::ARR_VEC3F:
        break;
      case param_type_e::ARR_VEC3I:
        break;
      case param_type_e::ARR_NORMAL3F:
        break;
      case param_type_e::ARR_POINT3F:
        break;
      case param_type_e::ARR_COLOR:
        break;
      default:
        RT3_WARNING(string{"parse_params(): unkonwn param type received!"});
        break;
    }
    std::clog << "---Done!\n";
  }
}

/*!
 * Parse the XML element `p_element` looking for an attribute `att_key` and
 * extract one or more values into the `ParamSet` `ps`.
 * @return `true` if the required attribute was successfuly retrieved into `ps`.
 */
template <typename T>
bool parse_single_BASIC_attrib(tinyxml2::XMLElement *p_element,
                               rt3::ParamSet *ps, string att_key) {
  // Test whether the att_key exists. Attribute() returns the value of the
  // attribute, as a const char *, or nullptr if such attribute does not exist.
  if (p_element->Attribute(att_key.c_str())) {
    std::cout << "\tAttribute \"" << att_key
              << "\" present, let us extract it!\n";
    auto result = read_single_value<T>(p_element, att_key);
    if (result.has_value()) {
      // Store the BASIC value in the ParamSet object.
      // Recall that `ps` is a dictionary, that receives a pair { key, value }.
      // ps->operator[]( att_key ) = values;
      (*ps)[att_key] = std::make_shared<Value<T>>(result.value());
      // const auto [ it, success ] = ps->insert({att_key, std::make_shared<
      // Value<T> >( values )});
      // Show message
      std::cout << "\tInsertion of \"" << att_key << "\" succeeded\n";
      std::clog << "\tAdded attribute (" << att_key << ": \"" << result.value()
           << "\" )\n";
      return true;
    }
  }
  return false;
}

/*!
 * Queries the attribute for the required type and return the value (if found).
 */
template <typename T>
std::optional<T> read_single_value(tinyxml2::XMLElement *p_element,
                                   const string &att_key) {
  // C-style string that will store the attributes read from the XML doc.
  const char *value_cstr;
  // Retrieve the string value into the `value_str` C-style string.
  p_element->QueryStringAttribute(att_key.c_str(), &value_cstr);

  // Separate individual BASIC elements as tokens.
  string str{value_cstr};
  // outgoing value retrieved from the XML doc.
  T value;
  // Convert string to the BASIC type and add it to the outgoing vector.
  // =======================================================================
  // RT3_WARNING: THIS DOES NOT WORK FOR BOOL VALUES!!!
  // That's why we use string instead of bool in the XML file.
  // =======================================================================
  if (std::stringstream(str) >> value)
    return value;
  else
    return {};  // No value found, return an "empty" value.
}

//-------------------------------------------------------------------------------

}  // namespace rt3
