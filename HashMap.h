/*
||
|| @file HashMap.h
|| @version 1.0 Beta
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | This library provides a simple interface for storing data with an associate key
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#ifndef HASHMAP_H
#define HASHMAP_H

#include <WProgram.h>

/* Handle association */
template<typename hash,typename map>
class HashType {
public:
	HashType(){ reset(); }
	
	HashType(hash code,map value):hashCode(code),mappedValue(value){}
	
	void reset(){ hashCode = 0; mappedValue = 0; }
	hash getHash(){ return hashCode; }
	void setHash(hash code){ hashCode = code; }
	map getValue(){ return mappedValue; }
	void setValue(map value){ mappedValue = value; }
	
	HashType& operator()(hash code, map value){
		setHash( code );
		setValue( value );
	}
private:
	hash hashCode;
	map mappedValue;
};

/*
Handle indexing and searches
TODO - extend API
*/
template<typename hash,typename map>
class HashMap {
public:
	HashMap(HashType<hash,map>* newMap,byte newSize){
		hashMap = newMap;
		size = newSize;
		for (byte i=0; i<size; i++){
			hashMap[i].reset();
		}
	}
	
	HashType<hash,map>& operator[](int x){
		//TODO - bounds
		return hashMap[x];
	}
	
	byte getIndexOf( hash key ){
		for (byte i=0; i<size; i++){
			if (hashMap[i].getHash()==key){
				return i;
			}
		}
	}
	map getValueOf( hash key ){
		for (byte i=0; i<size; i++){
			if (hashMap[i].getHash()==key){
				return hashMap[i].getValue();
			}
		}
	}
	
	void debug(){
		for (byte i=0; i<size; i++){
			Serial.print(hashMap[i].getHash());
			Serial.print(" - ");
			Serial.println(hashMap[i].getValue());
		}
	}

private:
	HashType<hash,map>* hashMap;
	byte size;
};

#endif

/*
|| @changelog
|| | 1.0 2009-07-13 - Alexander Brevig : Initial Release
|| #
*/