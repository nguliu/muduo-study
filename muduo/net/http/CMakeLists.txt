set(http_SRCS
  HttpContext.cc
  HttpServer.cc
  HttpResponse.cc
  )

add_library(muduo_http ${http_SRCS})
target_link_libraries(muduo_http muduo_net)

install(TARGETS muduo_http DESTINATION lib)
set(HEADERS
  HttpRequest.h
  HttpResponse.h
  HttpServer.h
  )
install(FILES ${HEADERS} DESTINATION include/muduo/net/http)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
	add_executable(httpserver_test tests/HttpServer_test.cc)
	target_link_libraries(httpserver_test muduo_http)

	add_executable(http_send_html tests/Http_send_html.cc)
	target_link_libraries(http_send_html muduo_http)

	if(BOOSTTEST_LIBRARY)
		#add_executable(httprequest_unittest tests/HttpRequest_unittest.cc)
		#target_link_libraries(httprequest_unittest muduo_http boost_unit_test_framework)
	endif()
	
endif()

# add_subdirectory(tests)
