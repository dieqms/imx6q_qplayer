/*
 * md5_util.h
 *
 *  Created on: 2017年9月11日
 *      Author: LJT
 */

#ifndef COMMON_MD5_UTIL_H_
#define COMMON_MD5_UTIL_H_

#include <string>
using std::string;

namespace Base {
/*!
 * Manage MD5.
 */
class Md5Util
{
private:
    struct md5_context
    {
        uint32_t total[2];
        uint32_t state[4];
        uint8_t buffer[64];
    };

    void md5_starts( struct md5_context *ctx );
    void md5_process( struct md5_context *ctx, uint8_t data[64] );
    void md5_update( struct md5_context *ctx, uint8_t *input, uint32_t length );
    void md5_finish( struct md5_context *ctx, uint8_t digest[16] );

public:
    //! construct a MD5 from any buffer
    void GenerateMD5(unsigned char* buffer, size_t bufferlen);

    //! construct a MD5
    Md5Util();

    //! construct a md5src from char *
    Md5Util(const char * md5src);

    //! construct a MD5 from a 16 bytes md5
    Md5Util(unsigned long* md5src);

    //! add a other md5
    Md5Util operator +(Md5Util adder);

    //! just if equal
    bool operator ==(Md5Util cmper);

    //! give the value from equer
    // void operator =(MD5 equer);

    //! to a string
    string ToString();

    unsigned long m_data[4];
};
}

#endif /* COMMON_MD5_UTIL_H_ */
