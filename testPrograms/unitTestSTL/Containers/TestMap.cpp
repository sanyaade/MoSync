/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

/**
 * @file TestMap.cpp
 * @author Gabriela Rata
 */

#include <string>
#include "../TestSTL.h"

/**
 * test_map: function for testing the map from STL
 * map - is an associative container. That means that the index doesn't have to be an integer.
 * An entry in the map is a combination of key - value.
 * std::map manages key/value std::pairs as elements ( pair<key_type, value_type>).
 * The key is used to identify an element in the map ( the index ). So you can write for example myMap[key] = value;
 * similar to vector: myVector[0] = 6; ( or x = myVector[2]; myValue = myMap[key]; ).
 * std::map sorts its elements automatically by key ( from lower to higher ). The default comparison criterion
 * is std::less, and it's provided as a default template parameter like this:
 * 		 	template < 	class Key ,
 * 						class T ,
 * 						class Cmp = less <Key >,
 * 						....
 * 				  	> class map;
 * Another ordering criterion, than the default one, can be provided as the third template parameter. It has
 * to be a class that defines an operator(), that takes two arguments of the key type and returns a bool. It can
 * be also just a function (with the same prototype as operator()). (See example bellow).
 * std::map allows only unique entries. That means that you can't have in a map two entries
 * with the same key. If you try to insert in map a entry with a certain key,
 * and inside the map exits already an element with that key, the element will be overridden.
 * std::map provides bidirectional iterators.
 * std::map is designed to be efficient in accessing its elements by their key.
 * std::map is defined in the <map> header.
 */
void TestSTL::test_map()
{
	//using std::map;

	/**
	 * default constructor
	 */
	//std::map<std::string, int> m1;

	//map provides ==, !=, <, >, <=, >=, and s w a p ()

}

void test_multimap()
{
	//using std::multimap;
}
