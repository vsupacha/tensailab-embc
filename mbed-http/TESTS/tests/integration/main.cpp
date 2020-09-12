/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2018 ARM Limited
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

#include "mbed.h"
#include "http_request.h"
#include "https_request.h"
#include "test_setup.h"
#include "utest/utest.h"
#include "unity/unity.h"
#include "greentea-client/test_env.h"

using namespace utest::v1;

static NetworkInterface *network;

static void setup_verify_network() {
    if (!network) network = connect_to_default_network_interface();
    TEST_ASSERT_NOT_NULL(network);
}

// verifies that the header is present and has a certain value
static void assert_header(HttpResponse *res, const char *header, const char *value) {
    bool headerPresent = false;
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        if (res->get_headers_fields()[ix]->compare(header)) {
            headerPresent = true;

            TEST_ASSERT(res->get_headers_values()[ix]->compare(value));
        }
    }
    TEST_ASSERT_EQUAL(true, headerPresent);
}

static control_t http_get(const size_t call_count) {
    setup_verify_network();

    HttpRequest *req = new HttpRequest(network, HTTP_GET, "http://httpbin.org/status/418");

    HttpResponse* res = req->send();
    TEST_ASSERT(res);
    TEST_ASSERT_EQUAL(418, res->get_status_code());

    delete req;

    return CaseNext;
}

static control_t http_post(const size_t call_count) {
    setup_verify_network();

    HttpRequest* req = new HttpRequest(network, HTTP_POST, "http://httpbin.org/post");
    req->set_header("Content-Type", "application/json");

    const char body[] = "{\"mykey\":\"mbedvalue\"}";

    HttpResponse* res = req->send(body, strlen(body));
    TEST_ASSERT(res);
    TEST_ASSERT_EQUAL(200, res->get_status_code());

    // verify that the Content-Type header is present, and set to application/json
    assert_header(res, "Content-Type", "application/json");

    // verify that both the key and value are present in the response
    TEST_ASSERT(res->get_body_length() > 0);
    TEST_ASSERT_NOT_EQUAL(res->get_body_as_string().find("mykey"), string::npos);
    TEST_ASSERT_NOT_EQUAL(res->get_body_as_string().find("mbedvalue"), string::npos);

    delete req;

    return CaseNext;
}

static control_t http_socket_reuse(const size_t call_count) {
    setup_verify_network();

    TCPSocket socket;
    nsapi_error_t open_result = socket.open(network);
    TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, open_result);

    nsapi_error_t connect_result = socket.connect("httpbin.org", 80);
    TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, connect_result);

    {
        HttpRequest *req = new HttpRequest(&socket, HTTP_GET, "http://httpbin.org/status/404");

        HttpResponse* res = req->send();
        TEST_ASSERT(res);
        TEST_ASSERT_EQUAL(404, res->get_status_code());

        delete req;
    }

    {
        HttpRequest *req = new HttpRequest(&socket, HTTP_GET, "http://httpbin.org/status/403");

        HttpResponse* res = req->send();
        TEST_ASSERT(res);
        TEST_ASSERT_EQUAL(403, res->get_status_code());

        delete req;
    }

    return CaseNext;
}

static control_t https_get(const size_t call_count) {
    setup_verify_network();

    HttpsRequest *req = new HttpsRequest(network, SSL_CA_PEM, HTTP_GET, "https://os.mbed.com/media/uploads/mbed_official/hello.txt");

    HttpResponse* res = req->send();
    TEST_ASSERT(res);
    TEST_ASSERT_EQUAL(200, res->get_status_code());
    TEST_ASSERT(res->get_body_length() > 0);
    TEST_ASSERT_NOT_EQUAL(res->get_body_as_string().find("Hello world!"), string::npos);

    delete req;

    return CaseNext;
}

static control_t https_post(const size_t call_count) {
    setup_verify_network();

    HttpsRequest* req = new HttpsRequest(network, SSL_CA_PEM, HTTP_POST, "https://httpbin.org/post");
    req->set_header("Content-Type", "application/json");

    const char body[] = "{\"myhttpskey\":\"janjanjan\"}";

    HttpResponse* res = req->send(body, strlen(body));
    TEST_ASSERT(res);
    TEST_ASSERT_EQUAL(200, res->get_status_code());

    // verify that the Content-Type header is present, and set to application/json
    assert_header(res, "Content-Type", "application/json");

    // verify that both the key and value are present in the response
    TEST_ASSERT(res->get_body_length() > 0);
    TEST_ASSERT_NOT_EQUAL(res->get_body_as_string().find("myhttpskey"), string::npos);
    TEST_ASSERT_NOT_EQUAL(res->get_body_as_string().find("janjanjan"), string::npos);

    delete req;

    return CaseNext;
}

static control_t https_socket_reuse(const size_t call_count) {
    setup_verify_network();

    TLSSocket socket;
    nsapi_error_t open_result = socket.open(network);
    TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, open_result);

    nsapi_error_t ca_result = socket.set_root_ca_cert(SSL_CA_PEM);
    TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, ca_result);

    nsapi_error_t connect_result = socket.connect("httpbin.org", 443);
    TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, connect_result);

    {
        HttpsRequest *req = new HttpsRequest(&socket, HTTP_GET, "http://httpbin.org/status/404");

        HttpResponse* res = req->send();
        TEST_ASSERT(res);
        TEST_ASSERT_EQUAL(404, res->get_status_code());

        delete req;
    }

    {
        HttpsRequest *req = new HttpsRequest(&socket, HTTP_GET, "http://httpbin.org/status/403");

        HttpResponse* res = req->send();
        TEST_ASSERT(res);
        TEST_ASSERT_EQUAL(403, res->get_status_code());

        delete req;
    }

    return CaseNext;
}

// Spread the message out over 3 different chunks
const char * chunks[] = {
    "{\"message\":",
    "\"this is an example",
    " of chunked encoding\"}"
};

int chunk_ix = 0;

// Callback function, grab the next chunk and return it
const void * get_chunk(uint32_t* out_size) {
    // If you don't have any data left, set out_size to 0 and return a null pointer
    if (chunk_ix == (sizeof(chunks) / sizeof(chunks[0]))) {
        *out_size = 0;
        return NULL;
    }
    const char *chunk = chunks[chunk_ix];
    *out_size = strlen(chunk);
    chunk_ix++;

    return chunk;
}

static control_t chunked_request(const size_t call_count) {
    setup_verify_network();

    HttpsRequest *req = new HttpsRequest(network, SSL_CA_PEM, HTTP_POST, "https://reqres.in/api/users");
    req->set_header("Content-Type", "application/json");

    HttpResponse* res = req->send(&get_chunk);
    TEST_ASSERT(res);
    TEST_ASSERT_EQUAL(201, res->get_status_code());
    TEST_ASSERT(res->get_body_length() > 0);
    TEST_ASSERT_NOT_EQUAL(res->get_body_as_string().find("message"), string::npos);
    TEST_ASSERT_NOT_EQUAL(res->get_body_as_string().find("this is an example of chunked encoding"), string::npos);
    TEST_ASSERT_NOT_EQUAL(res->get_body_as_string().find("createdAt"), string::npos);

    delete req;

    return CaseNext;
}

utest::v1::status_t greentea_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(1*60, "default_auto");
    return greentea_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("http get", http_get),
    Case("http post", http_post),
    Case("http socket reuse", http_socket_reuse),
    Case("https get", https_get),
    Case("https post", https_post),
    Case("https socket reuse", https_socket_reuse),
    Case("chunked request", chunked_request)
};

Specification specification(greentea_setup, cases);

void blink_led() {
    static DigitalOut led(LED1);
    led = !led;
}

int main() {
    Ticker t;
    t.attach(blink_led, 0.5);

    return !Harness::run(specification);
}
