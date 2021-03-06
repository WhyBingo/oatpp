/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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
 *
 ***************************************************************************/

#include "DTOMapperPerfTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/basic.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include "oatpp-test/Checker.hpp"

namespace oatpp { namespace test { namespace parser { namespace json { namespace mapping {
  
namespace {

typedef oatpp::parser::json::mapping::Serializer Serializer;
typedef oatpp::parser::json::mapping::Deserializer Deserializer;

#include OATPP_CODEGEN_BEGIN(DTO)
  
  class Test1 : public oatpp::data::mapping::type::Object {
    
    DTO_INIT(Test1, Object)
    
    DTO_FIELD(String, _string);
    DTO_FIELD(Int32, _int32);
    DTO_FIELD(List<Int32>::ObjectWrapper, _list);
    
    static ObjectWrapper createTestInstance(){
      auto result = Test1::createShared();
      result->_string = "String Field";
      result->_int32 = 5;
      result->_list = List<Int32>::createShared();
      result->_list->pushBack(1);
      result->_list->pushBack(2);
      result->_list->pushBack(3);
      return result;
    }
    
  };
  
#include OATPP_CODEGEN_END(DTO)
  
}
  
bool DTOMapperPerfTest::onRun() {
  
  v_int32 numIterations = 1000000;
  
  auto mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  
  auto test1 = Test1::createTestInstance();
  auto test1_Text = mapper->writeToString(test1);
  OATPP_LOGV(TAG, "json='%s'", (const char*) test1_Text->getData());
  {
    PerformanceChecker checker("Serializer");
    for(v_int32 i = 0; i < numIterations; i ++) {
      mapper->writeToString(test1);
    }
  }
  
  {
    PerformanceChecker checker("Deserializer");
    oatpp::parser::ParsingCaret caret(test1_Text);
    for(v_int32 i = 0; i < numIterations; i ++) {
      caret.setPosition(0);
      mapper->readFromCaret<Test1>(caret);
    }
  }
  return true;
}
  
}}}}}
