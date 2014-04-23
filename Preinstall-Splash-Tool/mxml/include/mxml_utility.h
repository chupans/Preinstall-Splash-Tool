/* 
   Mini XML lib PLUS for C++

   Utilities

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_utility.h,v 1.1.1.1 2003/08/13 00:13:29 jonnymind Exp $
*/

#ifndef MXML_UTILITY_H
#define MXML_UTILITY_H

#include <ostream>
#include <string>

namespace MXML {

std::string escape( const std::string unescaped );
std::ostream & writeEscape( std::ostream &stream, const std::string &src );
char parseEntity( const std::string entity );

}

#endif

/* end of utility.h */
