//
// CI Hello World
//
// Copyright (C) 2017 Assured Information Security, Inc.
// Author: Rian Quinn        <quinnr@ainfosec.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <hippomocks.h>
#include <catch/catch.hpp>

#include <consumer.h>

#include <dlfcn.h>

int* CreateLocal();
void DestroyLocal(int* target);

extern "C" __attribute__((visibility("default"))) int* CreatePlugin();
extern "C" __attribute__((visibility("default"))) void DestroyPlugin(int* target);

using create_t = int * ( * )();
using destroy_t = void( * )( int* );


#ifdef _HIPPOMOCKS__ENABLE_CFUNC_MOCKING_SUPPORT


TEST_CASE("consumer: failure")
{
    MockRepository mocks;
    auto p = mocks.Mock<producer>();

    mocks.OnCall(p, producer::print_msg).Throw(std::runtime_error("error"));

    CHECK_THROWS(consumer{p});
}

TEST_CASE("consumer: success")
{
    MockRepository mocks;
    auto p = mocks.Mock<producer>();

    mocks.OnCall(p, producer::print_msg);

    CHECK_NOTHROW(consumer{p});
}

TEST_CASE("c func local")
{
    REQUIRE(CreateLocal() == nullptr);
}

TEST_CASE("c func local mocked")
{
    MockRepository mocks;
    int i = 5;
    mocks.ExpectCallFunc( CreateLocal ).Return( &i );
    REQUIRE(CreateLocal() == &i );
}

TEST_CASE("c func plugin")
{
    void* handle = dlopen( nullptr, RTLD_LAZY );
    create_t create_func = reinterpret_cast<create_t>(dlsym( handle, "CreatePlugin" ));


    REQUIRE(create_func() == nullptr);
}

TEST_CASE("c func plugin mocked")
{
    MockRepository mocks;
    int i = 5;
    mocks.ExpectCallFunc( CreatePlugin ).Return( &i );

    void* handle = dlopen( nullptr, RTLD_LAZY );
    create_t create_func = reinterpret_cast<create_t>(dlsym( handle, "CreatePlugin" ));


    REQUIRE(create_func() == &i);
}


int* CreateLocal()
{
    return nullptr;
}

void DestroyLocal(int* target)
{

}


int* CreatePlugin()
{
    return nullptr;
}

void DestroyPlugin(int* target)
{

}

#endif
