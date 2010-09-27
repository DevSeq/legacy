// Copyright (C) 2008, 2009 by Sirrix AG security technologies
// Copyright (C) 2010 by Philipp Deppenwiese
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

/**
 * @file        KeyronaTPM.hxx
 * @brief       Implementation of the Keyrona TPM class.
 * @version     $Revision: 779 $
 * @date        Last modification at $Date: 2009-07-31 15:20:20 +0200 (Fri, 31 Jul 2009) $ by $Author: selhorst $
 */

#ifndef _KEYRONATPM_HXX
#define	_KEYRONATPM_HXX

#include <Keyrona.hxx>

const std::string KEYRONA_TPM_DATA_EXTENSION            =".TPMDATA";
const std::string KEYRONA_TPM_KEY_EXTENSION             =".TPMKEY";

using namespace std;
using namespace utils;
using namespace keyrona;

namespace keyrona
{
    // required for cross-reference between KeyronaKey and KeyronaSubject
    class KeyronaTPM
    {
        public:
            /*!
             *  @brief  opens TPM
             */
            KeyronaTPM();

            ~KeyronaTPM();

            /*!
             *  @brief seals data with the TPM
	     *  @param dataToSeal, address of ByteVector, that contains the data to be sealed
	     *  @return vector of ByteVector, that contains the sealed data
             */
            vector<ByteVector> seal(ByteVector &dataToSeal, int &local, vector<int> &pcr);
	    
            /*!
             *  @brief unseals data with the TPM
	     *  @param dataToUnseal, address of vector of ByteVector, that contains the data to be unsealed
	     *  @return ByteVector, that contains the unsealed data
             */
	    ByteVector unseal(vector<ByteVector> &dataToUnseal);
	    
            /*!
             *  @brief  not implemented yet 
	     */
            ByteVector bind(ByteVector &dataToBind, UInt32 &bindkeynum);
           
            /*!
             *  @brief  not implemented yet 
	     */
	   ByteVector unbind(ByteVector &dataToUnbind, UInt32 &bindkeynum, string &password);
            
            /*!
             *  @brief returns random numbers, generated by the TPM
	     *  @param amountOfRandomBytes, UInt8, that contains the number of random bytes to receive
	     *  @return ByteVector, that contains the random numbers
	     */
	    ByteVector create_key(string &password, UInt32 &keynum, string &type);
	    
	    void remove_all_keys_by_uuid();
	    
	    void change_key_auth(string &password, string &password_old, UInt32 &keynum);
	    
	    void revokeek();
	    
	    void delete_key(UInt32 &keynum);
	   
	    
	    ByteVector getRandom(UInt8 amountOfRandomBytes);

        private:
            /*!
             *  @brief  disabled copy constructor
             */
            KeyronaTPM ( const KeyronaTPM& );

            /*!
             *  @brief  disabled assignment operator
             */
            KeyronaTPM& operator=( const KeyronaTPM& );
    };
};


#endif	/* _KEYRONATPM_HXX */
