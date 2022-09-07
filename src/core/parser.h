#ifndef PARSER_H
#define PARSER_H 1

/*!
 * Implementation of XML processing functions.
 * @file parser.cpp
 */

#include <tinyxml2.h>

#include <iomanip>
#include <iostream>
#include <string>
using std::string;
#include <algorithm>
#include <iterator>
#include <memory>
#include <sstream>
#include <vector>
using std::vector;
#include <utility>
using std::pair;
#include <optional>

#include "error.h"
#include "paramset.h"

namespace rt3 {

// === Enumerations
/// Type of possible paramter types we may read from the input scene file.
enum class param_type_e : int {
  BOOL = 0,
  INT,          //!< Single integet
  UINT,         //!< Single unsigned int
  REAL,         //!< Single real number
  VEC3F,        //!< Single Vector3f
  VEC3I,        //!< Single Vector3i
  NORMAL3F,     //!< Single Normal3f
  POINT3F,      //!< Single Point3f
  POINT2I,      //!< Single Point2i
  COLOR,        //!< Single Color
  SPECTRUM,     //!< Single Spectrum
  STRING,       //!< Single string
  ARR_INT,      //!< An array of integers
  ARR_REAL,     //!< An array of real numbers
  ARR_VEC3F,    //!< An array of Vector3f
  ARR_VEC3I,    //!< An array of Vector3i
  ARR_POINT3F,  //!< An array of Point3f
  ARR_COLOR,    //!< An array of Color
  ARR_NORMAL3F  //!< An array of Normal3f
};
// === parsing functions.
void parse(const char *);
void parse_tags(tinyxml2::XMLElement *, int);
void parse_parameters(tinyxml2::XMLElement *p_element,
                      const vector<std::pair<param_type_e, string>> param_list,
                      ParamSet *ps_out);

/// Extracts a single BASIC element.
template < typename T >
bool parse_single_BASIC_attrib( tinyxml2::XMLElement *e, rt3::ParamSet *ps , string name );

/// Reads a single value from an XML element.
template <typename T>
std::optional<T> read_single_value( tinyxml2::XMLElement *p_element, const string& att_key );

//-------------------------------------------------------------------------------
}  // namespace rt3

#endif  // PARSER_H
