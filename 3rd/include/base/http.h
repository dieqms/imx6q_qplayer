#ifndef COMMON_HTTP_H_
#define COMMON_HTTP_H_

int HttpDownloadFile(const char *url,char *file_name, unsigned char *md5 = NULL);

#endif /* SYSCTL_HTTPDOWNLOAD_H_ */
