set(F1AP_RELEASE "16.3.1")
make_version(F1AP_VERSION 16 3 1)

if(F1AP_RELEASE VERSION_EQUAL "15.1.1")
  include(R15.1.1/f1ap-15.1.1.cmake)
elseif(F1AP_RELEASE VERSION_EQUAL "15.2.1")
  include(R15.2.1/f1ap-15.2.1.cmake)
elseif(F1AP_RELEASE VERSION_EQUAL "16.3.1")
  include(R16.3.1/f1ap-16.3.1.cmake)
else()
  message(FATAL_ERROR "No definitions for F1AP_RELEASE ${F1AP_RELEASE}")
endif()

add_custom_command(OUTPUT ${f1ap_source} ${f1ap_headers}
  COMMAND ASN1C_PREFIX=F1AP_ asn1c -gen-PER -no-gen-OER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps -D ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${F1AP_GRAMMAR}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${F1AP_GRAMMAR}
  COMMENT "Generating F1AP source files from ${CMAKE_CURRENT_SOURCE_DIR}/${F1AP_GRAMMAR}"
)

add_library(ASN1_F1AP_LIB ${f1ap_source})
target_include_directories(ASN1_F1AP_LIB PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_options(ASN1_F1AP_LIB PRIVATE -DASN_DISABLE_OER_SUPPORT -w)
