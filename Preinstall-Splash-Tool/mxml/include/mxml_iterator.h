/* 
   Mini XML lib PLUS for C++

   Node::iterator class

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_iterator.h,v 1.5 2004/04/10 23:50:25 jonnymind Exp $
*/

//TO BE INCLUDED IN NODE.H

//namespace MXML {

template<class __Node >
__iterator<__Node> &__iterator<__Node>::__prev()
{
   assert( m_node != m_base );

   if (m_node == 0 ) {
      if ( m_base->parent() != 0 )
         m_node = m_base->parent()->lastChild();
      else {
         m_node = m_base;
         while ( m_node->next() != 0 )
            m_node = m_node->next();
      }
   }
   else
      m_node = m_node->prev();
   return *this;
}

template< class __Node >
__iterator<__Node> &__iterator<__Node>::operator+=( int count )
{
   while ( count != 0 && m_node != 0 )
      operator++();
   return *this;
}

template< class __Node >
__iterator<__Node> &__iterator<__Node>::operator-=( int count )
{
   while ( count != 0 && m_node != m_base )
      operator--();
   return *this;
}

template< class __Node >
__iterator<__Node> __iterator<__Node>::operator+( int count )
{
   __iterator tmp = *this;
   while ( count > 0 && tmp.m_node != 0 ) {
      operator++();
      count--;
   }
   return tmp;
}

template< class __Node >
__iterator<__Node> __iterator<__Node>::operator-( int count )
{
   __iterator tmp = *this;
   while ( count > 0 && tmp.m_node != m_base ) {
      operator--();
      count--;
   }
   return tmp;
}

/*********************************************
* Deep iterator
*********************************************/
template< class __Node >
__iterator<__Node> &__deep_iterator<__Node>::__next()
{
   assert( m_node != 0 );

   if ( m_node->child() != 0 ) {
      m_node = m_node->child();
   }
   else if ( m_node->next() != 0 ) {
      m_node = m_node->next();
   }
   else {
      while ( m_node->parent() != 0 ) {
         m_node = m_node->parent();
         if ( m_node->next() != 0 )
            break;
      }
      m_node = m_node->next(); // can be NULL
   }

   return *this;
}

template< class __Node >
__iterator<__Node> &__deep_iterator<__Node>::__prev()
{
return *this;
}

/***************************************************
* Find iterator
***************************************************/
template< class __Node >
__find_iterator<__Node>::__find_iterator( __Node *nd ):
            __deep_iterator< __Node>( nd )
{
   m_maxmatch = 0;
}

template< class __Node >
__find_iterator<__Node>::__find_iterator( __Node *nd, std::string name, std::string attr,
                  std::string valatt, std::string data):
            __deep_iterator< __Node>( nd )
{
   m_name = name;
   m_attr = attr;
   m_valattr = valatt;
   m_data = data;
   m_maxmatch = 0;
   if ( m_name != "" ) m_maxmatch++;
   if ( m_attr !=  "" ) m_maxmatch++;
   if ( m_valattr != "" ) m_maxmatch++;
   if ( m_data != "" ) m_maxmatch++;
   __find();
};


template< class __Node >
__iterator<__Node> &__find_iterator<__Node>::__next()
{
   __deep_iterator<__Node>::__next();
   return __find();
}

template< class __Node >
__iterator<__Node> &__find_iterator<__Node>::__find()
{
   int matches;
   while ( this->m_node != 0 ) {
      matches = 0;
      if ( m_name != "" && m_name == m_node->name() )
         matches++;

      if ( m_attr != "" && m_node->hasAttribute( m_attr ) ) {
         matches++;
         if ( m_valattr != "" && m_node->getAttribute( m_attr ) == m_valattr )
            matches++;
      }

      if ( m_data != "" && m_node->data().find( m_data ) != std::string::npos )
         matches++;

      if ( matches < m_maxmatch )
         __deep_iterator<__Node>::__next();
      else
         break;
   }

   return *this;
}

template< class __Node >
__iterator<__Node> &__find_iterator<__Node>::__prev()
{
return *this;
}


/*********************************************
* Path Iterator
*********************************************/
template< class __Node >
__path_iterator<__Node>::__path_iterator( __Node *nd, std::string path ):
            __iterator< __Node>( nd )
{
   m_path = path;
   __find();
}

template< class __Node >
__iterator<__Node> &__path_iterator<__Node>::__next()
{
   __Node *ptr = m_node;
   std::string name;
   std::string::size_type pos = m_path.rfind('/');
   
   if ( pos == std::string::npos ) {
      pos = 0;
      name = m_path;
   }
   else {
      pos++;
      name = m_path.substr( pos );
   }
   
   m_node = m_node->next();   
   // todo: this sucks, must re-do it better
   while ( m_node ) {
      if ( name == "*" || m_node->name() == name ) break;
      m_node = m_node->next();   
   }
   
   return *this;
}

template< class __Node >
__iterator<__Node> &__path_iterator<__Node>::__prev()
{
   assert( m_node != 0 );

   __Node *ptr = m_node;
   m_node = m_node->prev();
   // todo: this sucks, must re-do it better
   while ( m_node != 0 ) {
      if ( m_node->name() == ptr->name() ) break;
      m_node = m_node->prev();
   }

   return *this;
}


template< class __Node >
__Node *__path_iterator<__Node>::subfind( __Node *parent, 
            std::string::size_type begin )
{
   std::string::size_type end = m_path.find( '/', begin );
   std::string name = end == std::string::npos? m_path.substr( begin ) : m_path.substr( begin, end - begin );
   
   if ( name == "" ) return parent;
   
   parent = parent->child();
   while( parent != 0 ) {
      if ( name == "*"  || parent->name() == name ) {
         if ( end != std::string::npos ) {
            parent = subfind( parent, end +1 );
         }
         break;
      }
      parent = parent->next();
   }
   
   return parent;
}
            
template< class __Node >
__iterator<__Node> &__path_iterator<__Node>::__find()
{
   if ( m_node == 0 ) return *this;
   
   __Node *rootNode = m_node;
   std::string firstName;
   std::string::size_type pos;
   if ( rootNode->nodeType() == Node::typeDocument ) 
   {
      rootNode = rootNode->child();
      while( rootNode && rootNode->nodeType() != Node::typeTag )
         rootNode = rootNode->next();
      if ( ! rootNode )
      {
         m_node = 0;
         return *this;
      }
   }
   
   if ( m_path[0] == '/' ) 
   {         
      while( rootNode->parent() && rootNode->parent()->nodeType() != Node::typeDocument )
      {
         rootNode = rootNode->parent();
      }
      pos = m_path.find( '/', 1 );
      if( pos == std::string::npos ) 
         firstName = m_path.substr( 1 );
      else
         firstName = m_path.substr( 1, pos-1 );
   }
   else {
      // relative path, starting below us.
      rootNode = rootNode->child();
      pos = m_path.find( '/' );
      if( pos == std::string::npos ) 
         firstName = m_path;
      else
         firstName = m_path.substr( 0, pos );
   }
         
   while ( rootNode != 0 ) 
   {
      if ( firstName == "*" || firstName == rootNode->name() ) 
      {
         if ( pos == std::string::npos ) 
         {
            m_node = rootNode;
         }
         else {
            m_node = subfind( rootNode, pos+1 );
         }
         break;
      }
      rootNode = rootNode->next();
   }         

   return *this;
}

//}
/* end of mxml_iterator.h */

