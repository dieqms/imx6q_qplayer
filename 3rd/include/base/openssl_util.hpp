/*
 * openssl_util.hpp
 *
 *  Created on: 2017年11月22日
 *      Author: LJT
 */

#ifndef BASE_OPENSSL_UTIL_H_
#define BASE_OPENSSL_UTIL_H_

#include <string>
#include <openssl/ssl.h>
#include <openssl/aes.h>

namespace Base {

class OpensslUtil {
public:
	OpensslUtil();
	virtual ~OpensslUtil();
	
	static const std::string AesEncrypt(const std::string & info, const std::string & key) {
		unsigned char key_adjust[16] = {0};
		int key_len = key.size();
		memcpy(key_adjust, key.data(), key_len < 16 ? key_len : 16);

	    int len = info.size() % AES_BLOCK_SIZE ? (info.size()/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE : info.size();
	    char * buffer_in = new char[len];
	    char * buffer_out = new char[len];
	    memset(buffer_in, 0, len);
	    memset(buffer_out, 0, len);

	    char * in = buffer_in;
	    char * out = buffer_out;
	    memcpy(in, info.c_str(), info.size());

	    AES_KEY aes;
	    if(AES_set_encrypt_key((unsigned char*)key_adjust, 128, &aes) < 0)
	    {
		    delete [] buffer_in;
		    delete [] buffer_out;
	        return "";
	    }

	    int en_len=0;
	    while(en_len < len)
	    {
	        AES_encrypt((unsigned char*)in, (unsigned char*)out, &aes);
	        in += AES_BLOCK_SIZE;
	        out += AES_BLOCK_SIZE;
	        en_len += AES_BLOCK_SIZE;
	    }

	    std::string encrypted;
	    encrypted.assign(buffer_out, len);

	    delete [] buffer_in;
	    delete [] buffer_out;

	    return encrypted;
	}

    static const std::string AesDecrypt(const std::string & info, const std::string & key) {
		unsigned char key_adjust[16] = {0};
		int key_len = key.size();
		memcpy(key_adjust, key.data(), key_len < 16 ? key_len : 16);

	    int len = info.size() % AES_BLOCK_SIZE ? (info.size()/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE : info.size();
		char * buffer_in = new(std::nothrow) char[len];
		if (buffer_in == NULL) {
			perror("AesDecrypt: new fail");
			return "";
		}
		char * buffer_out = new(std::nothrow) char[len];
		if (buffer_out == NULL) {
			perror("AesDecrypt: new fail");
			delete [] buffer_in;
			return "";
		}

	    memset(buffer_in, 0, len);
	    memset(buffer_out, 0, len);

	    char * in = buffer_in;
	    char * out = buffer_out;
	    memcpy(in, info.data(), info.size());

	    AES_KEY aes;
	    if(AES_set_decrypt_key((unsigned char*)key_adjust, 128, &aes) < 0)
	    {
		    delete [] buffer_in;
		    delete [] buffer_out;
	        return "";
	    }
	    int en_len = 0;
	    while(en_len < len)
	    {
	        AES_decrypt((unsigned char*)in, (unsigned char*)out, &aes);
	        in += AES_BLOCK_SIZE;
	        out += AES_BLOCK_SIZE;
	        en_len += AES_BLOCK_SIZE;
	    }

	    std::string encrypted;
	    encrypted.assign(buffer_out, len);

	    delete [] buffer_in;
	    delete [] buffer_out;

	    return encrypted;
	}
        
	static std::string AesCBCEncrypt(uint8_t * data, int data_len, const std::string & key, const std::string & iv) {
		char key_adjust[16] = {0};
		int key_len = key.size();
		memcpy(key_adjust, key.data(), key_len < 16 ? key_len : 16);
		
		unsigned char iv_adjust[16] = {0};
		int iv_len = iv.size();
		memcpy(iv_adjust, iv.data(), iv_len < 16 ? iv_len : 16);

		int len = data_len % AES_BLOCK_SIZE ? (data_len/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE : data_len;
		char * buffer_in = new(std::nothrow) char[len];
		if (buffer_in == NULL) {
			perror("AesCBCEncrypt: new fail");
			return "";
		}
		char * buffer_out = new(std::nothrow) char[len];
		if (buffer_out == NULL) {
			perror("AesCBCEncrypt: new fail");
			delete [] buffer_in;
			return "";
		}
		
		memset(buffer_in, 0, len);
		memset(buffer_out, 0, len);

		char * in = buffer_in;
		char * out = buffer_out;
		memcpy(in, data, data_len);
		
		AES_KEY aes;
		if(AES_set_decrypt_key((unsigned char*)key_adjust, 128, &aes) < 0)
		{
			delete [] buffer_in;
			delete [] buffer_out;
			return "";
		}
		    
		AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv_adjust, AES_ENCRYPT);
		std::string encrypted;
		encrypted.assign(buffer_out, len);

		delete [] buffer_in;
		delete [] buffer_out;

		return encrypted;
	}
	
	static const std::string AesCBCDecrypt(uint8_t * data, int data_len, const std::string & key, const std::string & iv) {
		char key_adjust[16] = {0};
		int key_len = key.size();
		memcpy(key_adjust, key.data(), key_len < 16 ? key_len : 16);
		
		unsigned char iv_adjust[16] = {0};
		int iv_len = iv.size();
		memcpy(iv_adjust, iv.data(), iv_len < 16 ? iv_len : 16);

		int len = data_len % AES_BLOCK_SIZE ? (data_len/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE : data_len;
		char * buffer_in = new(std::nothrow) char[len];
		char * buffer_out = new(std::nothrow) char[len];

		memset(buffer_in, 0, len);
		memset(buffer_out, 0, len);

		char * in = buffer_in;
		char * out = buffer_out;
		memcpy(in, data, data_len);
		
		AES_KEY aes;
		if(AES_set_decrypt_key((unsigned char*)key_adjust, 128, &aes) < 0)
		{
			delete [] buffer_in;
			delete [] buffer_out;
			return "";
		}
			
		AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv_adjust, AES_DECRYPT);
		std::string decrypted;
		decrypted.assign(buffer_out, len);

		delete [] buffer_in;
		delete [] buffer_out;

		return decrypted;
	}
        
	static const std::string Base64Encode(const char * input, int length, bool with_new_line = false) {
		BIO * bmem = NULL;
		BIO * b64 = NULL;
		BUF_MEM * bptr = NULL;

		b64 = BIO_new(BIO_f_base64());
		if(!with_new_line) {
			BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
		}
		bmem = BIO_new(BIO_s_mem());
		b64 = BIO_push(b64, bmem);
		BIO_write(b64, input, length);
		BIO_flush(b64);
		BIO_get_mem_ptr(b64, &bptr);

		std::string encode_str;
		encode_str.assign(bptr->data, bptr->length);

		BIO_free_all(b64);

		return encode_str;
	}

	static const std::string Base64Decode(char * input, size_t length, bool with_new_line = false) {
	    BIO * b64 = NULL;
	    BIO * bmem = NULL;
	    std::string decode_str;
	    
	    b64 = BIO_new(BIO_f_base64());
	    if(!with_new_line) {
	        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	    }
	    bmem = BIO_new_mem_buf(input, length);
	    bmem = BIO_push(b64, bmem);
	    
	    void * buffer = malloc(length);
	    if (buffer == NULL)
	    	return decode_str;
	    memset(buffer, 0, length);
	    
	    int new_len = BIO_read(bmem, buffer, length);
	    if (new_len > 0) {
	    	 decode_str.assign((const char*)buffer, new_len);
	    }
	    
	    free(buffer);
	    BIO_free_all(bmem);
	    
	    return decode_str;
	}
};

} 

#endif 
