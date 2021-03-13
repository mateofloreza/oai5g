add_subdirectory(MESSAGES)

set(F1AP_RELEASE "16.3.1")
make_version(F1AP_VERSION 16 3 1)
add_definitions(-DF1AP_VERSION=${F1AP_VERSION})

add_library(F1AP
            f1ap_common.c
            f1ap_cu_interface_management.c
            f1ap_cu_paging.c
            f1ap_cu_rrc_message_transfer.c
            f1ap_cu_system_information.c
            f1ap_cu_task.c
            f1ap_cu_ue_context_management.c
            f1ap_cu_warning_message_transmission.c
            f1ap_decoder.c
            f1ap_du_interface_management.c
            f1ap_du_paging.c
            f1ap_du_rrc_message_transfer.c
            f1ap_du_system_information.c
            f1ap_du_task.c
            f1ap_du_ue_context_management.c
            f1ap_du_warning_message_transmission.c
            f1ap_encoder.c
            f1ap_handlers.c
            f1ap_itti_messaging.c
            f1ap_messaging.c
)
target_link_libraries(F1AP PUBLIC ASN1_F1AP_LIB)
target_include_directories(F1AP PRIVATE ASN1_F1AP_LIB)
