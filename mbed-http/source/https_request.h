/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _MBED_HTTPS_REQUEST_H_
#define _MBED_HTTPS_REQUEST_H_

#include <string>
#include <vector>
#include <map>
#include "http_request_base.h"
#include "TLSSocket.h"

#ifndef HTTP_RECEIVE_BUFFER_SIZE
#define HTTP_RECEIVE_BUFFER_SIZE 8 * 1024
#endif

/**
 * \brief HttpsRequest implements the logic for interacting with HTTPS servers.
 */
class HttpsRequest : public HttpRequestBase {
public:
    /**
     * HttpsRequest Constructor
     * Sets up event handlers and flags.
     *
     * @param[in] socket A connected TLSSocket
     * @param[in] method HTTP method to use
     * @param[in] url URL to the resource
     * @param[in] body_callback Callback on which to retrieve chunks of the response body.
                                If not set, the complete body will be allocated on the HttpResponse object,
                                which might use lots of memory.
     */
    HttpsRequest(TLSSocket* socket,
                 http_method method,
                 const char* url,
                 Callback<void(const char *at, uint32_t length)> body_callback = 0)
        : HttpRequestBase(socket, body_callback)
    {
        _parsed_url = new ParsedUrl(url);
        _body_callback = body_callback;
        _request_builder = new HttpRequestBuilder(method, _parsed_url);
        _response = NULL;
    }

    virtual ~HttpsRequest() {}

protected:
	virtual nsapi_error_t connect_socket(const SocketAddress &address) {
		return ((TLSSocket*)_socket)->connect(address);
	}	
};

#endif // _MBED_HTTPS_REQUEST_H_
