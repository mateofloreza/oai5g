/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Author and copyright: Laurent Thomas, open-cells.com
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include <time.h>
#include <stdlib.h>
#include "e1ap.h"
#include "e1ap_common.h"

static e1ap_upcp_inst_t *e1ap_cp_inst[NUMBER_OF_gNB_MAX] = {0};
static e1ap_upcp_inst_t *e1ap_up_inst[NUMBER_OF_gNB_MAX] = {0};

e1ap_upcp_inst_t *getCxtE1(E1_t type, instance_t instance) {
  AssertFatal( instance < sizeofArray(e1ap_cp_inst), "instance exceeds limit\n");
  return type ? e1ap_up_inst[instance] : e1ap_cp_inst[instance];
}

int e1ap_assoc_id(bool isCu, instance_t instance) {
  return 0;
}

void createE1inst(E1_t type, instance_t instance, e1ap_setup_req_t *req) {
  if (type == CPtype) {
    AssertFatal(e1ap_cp_inst[instance] == NULL, "Double call to E1 CP instance %d\n", (int)instance);
    e1ap_cp_inst[instance] = (e1ap_upcp_inst_t *) calloc(1, sizeof(e1ap_upcp_inst_t));
  } else {
    AssertFatal(e1ap_up_inst[instance] == NULL, "Double call to E1 UP instance %d\n", (int)instance);
    e1ap_up_inst[instance] = (e1ap_upcp_inst_t *) calloc(1, sizeof(e1ap_upcp_inst_t));
    memcpy(&e1ap_up_inst[instance]->setupReq, req, sizeof(e1ap_setup_req_t));
  }
}

E1AP_TransactionID_t transacID[E1AP_MAX_NUM_TRANSAC_IDS] = {0}; 

void e1ap_common_init() {
  srand(time(NULL));
}

bool check_transac_id(E1AP_TransactionID_t id, int *freeIdx) {

  bool isFreeIdxSet = false;
  for (int i=0; i < E1AP_MAX_NUM_TRANSAC_IDS; i++) {
    if (id == transacID[i])
      return false;
    else if (!isFreeIdxSet && (transacID[i] == 0)) {
      *freeIdx = i;
      isFreeIdxSet = true;
    }
  }

  return true;
}

E1AP_TransactionID_t E1AP_get_next_transaction_identifier() {
  E1AP_TransactionID_t genTransacId;
  bool isTransacIdValid = false;
  int freeIdx;

  while (!isTransacIdValid) {
    genTransacId = rand();
    isTransacIdValid = check_transac_id(genTransacId, &freeIdx);
  }

  AssertFatal(freeIdx < E1AP_MAX_NUM_TRANSAC_IDS, "Free Index exceeds array length\n");
  transacID[freeIdx] = genTransacId;
  return genTransacId;
}

int e1ap_decode_initiating_message(E1AP_E1AP_PDU_t *pdu) {
  DevAssert(pdu != NULL);

  switch(pdu->choice.initiatingMessage->procedureCode) {
    case E1AP_ProcedureCode_id_gNB_CU_UP_E1Setup:
      break;

    case E1AP_ProcedureCode_id_gNB_CU_UP_ConfigurationUpdate:
      break;

    case E1AP_ProcedureCode_id_bearerContextSetup:
      break;

    default:
      LOG_E(E1AP, "Unsupported procedure code (%d) for initiating message\n",
            (int)pdu->choice.initiatingMessage->procedureCode);
      return -1;
  }
  return 0;
}

int e1ap_decode_successful_outcome(E1AP_E1AP_PDU_t *pdu) {
  DevAssert(pdu != NULL);
  switch(pdu->choice.successfulOutcome->procedureCode) {
    case E1AP_ProcedureCode_id_gNB_CU_UP_E1Setup:
      break;

    case E1AP_ProcedureCode_id_bearerContextSetup:
      break;

    default:
      LOG_E(E1AP, "Unsupported procedure code (%d) for successful message\n",
            (int)pdu->choice.successfulOutcome->procedureCode);
      return -1;
  }
  return 0;
}

int e1ap_decode_unsuccessful_outcome(E1AP_E1AP_PDU_t *pdu) {
  DevAssert(pdu != NULL);
  switch(pdu->choice.unsuccessfulOutcome->procedureCode) {
    case E1AP_ProcedureCode_id_gNB_CU_UP_E1Setup:
      break;

    default:
      LOG_E(E1AP, "Unsupported procedure code (%d) for unsuccessful message\n",
            (int)pdu->choice.unsuccessfulOutcome->procedureCode);
      return -1;
  }
  return 0;
}

int asn1_xer_print_e1ap = 1;

int e1ap_decode_pdu(E1AP_E1AP_PDU_t *pdu, const uint8_t *const buffer, uint32_t length) {
  asn_dec_rval_t dec_ret;
  DevAssert(buffer != NULL);
  dec_ret = aper_decode(NULL,
                        &asn_DEF_F1AP_F1AP_PDU,
                        (void **)&pdu,
                        buffer,
                        length,
                        0,
                        0);

  if (asn1_xer_print_e1ap) {
    LOG_E(F1AP, "----------------- ASN1 DECODER PRINT START----------------- \n");
    xer_fprint(stdout, &asn_DEF_E1AP_E1AP_PDU, pdu);
    LOG_E(F1AP, "----------------- ASN1 DECODER PRINT END ----------------- \n");
  }

  if (dec_ret.code != RC_OK) {
    AssertFatal(1==0,"Failed to decode pdu\n");
    return -1;
  }

  switch(pdu->present) {
    case E1AP_E1AP_PDU_PR_initiatingMessage:
      return e1ap_decode_initiating_message(pdu);

    case E1AP_E1AP_PDU_PR_successfulOutcome:
      return e1ap_decode_successful_outcome(pdu);

    case E1AP_E1AP_PDU_PR_unsuccessfulOutcome:
      return e1ap_decode_unsuccessful_outcome(pdu);

    default:
      LOG_E(E1AP, "Unknown presence (%d) or not implemented\n", (int)pdu->present);
      break;
  }

  return -1;
}

int e1ap_encode_send(bool isCu, instance_t instance, E1AP_E1AP_PDU_t *pdu, uint16_t stream, const char *func) {
  DevAssert(pdu != NULL);

  if (asn1_xer_print_e1ap) {
    LOG_E(E1AP, "----------------- ASN1 ENCODER PRINT START ----------------- \n");
    xer_fprint(stdout, &asn_DEF_E1AP_E1AP_PDU, pdu);
    LOG_E(E1AP, "----------------- ASN1 ENCODER PRINT END----------------- \n");
  }

  char errbuf[2048]; /* Buffer for error message */
  size_t errlen = sizeof(errbuf); /* Size of the buffer */
  int ret = asn_check_constraints(&asn_DEF_E1AP_E1AP_PDU, pdu, errbuf, &errlen);

  if(ret) {
    LOG_E(E1AP, "%s: Constraint validation failed: %s\n", func, errbuf);
  }

  void *buffer = NULL;
  ssize_t encoded = aper_encode_to_new_buffer(&asn_DEF_E1AP_E1AP_PDU, 0, pdu, buffer);

  if (encoded < 0) {
    LOG_E(E1AP, "%s: Failed to encode E1AP message\n", func);
    return -1;
  } else {
    MessageDef *message = itti_alloc_new_message(isCu?TASK_CUCP_E1:TASK_CUUP_E1, 0, SCTP_DATA_REQ);
    sctp_data_req_t *s = &message->ittiMsg.sctp_data_req;
    s->assoc_id      = e1ap_assoc_id(isCu,instance);
    s->buffer        = buffer;
    s->buffer_length = encoded;
    s->stream        = stream;
    LOG_I(E1AP, "%s: Sending ITTI message to SCTP Task\n", func);
    itti_send_msg_to_task(TASK_SCTP, instance, message);
  }

  return encoded;
}

