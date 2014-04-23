/* 
   Mini XML lib PLUS for C++

   Attribute class

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_attribute.h,v 1.2 2004/01/11 13:32:17 jonnymind Exp $
*/

#ifndef MXML_ATTRIBUTE_H
#define MXML_ATTRIBUTE_H

#include <string>
#include <iostream>
#include <mxml_element.h>

namespace MXML {

/** Encapsulates an XML attribute.
   This class represents the node attributes:
   &lt;node_name <b>attrib_name="attrib value"</b>&gt;.
   The class has support for serializationa and de-serialization.
*/
class Attribute: public Element
{
private:
   /** Name of the attribute */
   std::string m_name;
   /** Value of the attribute */
   std::string m_value;

public:
   /** Deserializes an attribute
      Reads an attribute from an XML file at current position.
      In case of error Throws an MXML::IOError or MXML::MalformedError.
      @param in the input sream
      @param style style bits; see MXML::Document::setStyle()
      @param line the current line in the stream
      @param pos the current position in line
      @throws MXML::MalformedError if the attribute is invalid
      @throws MXML::IOError in case of hard errors on the stream
   */
   Attribute( std::istream &in, int style=0, int line=1, int pos=0  );


   /* Creates a new attribute
      \todo chech for name validity and throw an error
      @param name the name of the newborn attribute
      @param value the value of the newborn attribute
   */
   Attribute( const std::string &name, const std::string &value ): Element()
   {
      m_name = name;
      m_value = value;
   }

   /* Copies an attribute. */
   Attribute( Attribute &src): Element( src )
   {
      m_name= src.m_name;
      m_value = src.m_value;
   };

   /** Returns current name of the attribute. */
   const std::string name() const { return m_name; }
   /** Returns the value stored in the attribute.
      Parsing the value or transforming it to a proper type (i.e. integer)
      is left to the caller.
   */
   const std::string value() const { return m_value; }

   /** Change name of the attribute
      \todo check validity of the name and throw a malformed error if wrong.
   */
   void name( const std::string &new_name ) { m_name = new_name; }

   /** Change value of the attribute
      \todo check validity of the name and throw a malformed error if wrong.
   */
   void value( const std::string &new_value ) { m_value = new_value; }

   /** Writes the attribute on a stream.
      This function is usually called by MXML::Document::write(), or by
      the << operator (in class MXML::Element), use the latter only
      for debug.
      @param stream the stream where the object will be written
      @param style the style of the serialization

   */
   virtual void write( std::ostream &out, const int style ) const;
};

} // namespace

#endif /* end of mxml_attribute.h */
