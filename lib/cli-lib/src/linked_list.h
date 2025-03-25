/*
 * @file linkedlist.h
 * @author Hisham D.
 * @date Oct 17, 2022
 * @brief brief description
 * @see www.aaaa.ccc
 *
 * detailed description
* @copyright Copyright (c) 2024
 * Confidentiality and Proprietary Rights Statement
 * The sample, Code and Hardware, provided at no cost to the customer,
 * contains confidential and proprietary information belonging exclusively
 * to Oxit LLC. All contents, including but not limited to concepts, ideas,
 * designs, methodologies, processes, technologies, and intellectual property,
 * are the sole property of Oxit LLC and are provided for evaluation purposes
 * only.
 *
 * Oxit LLC does not grant any intellectual property rights or permit any
 * other usage of the sample hardware and code beyond evaluation.
 *
 * Unauthorized use, disclosure, distribution, copying, or any form of
 * dissemination of the information contained in this sample is strictly
 * prohibited and may result in legal action.
 *
 * The recipient of this sample agrees to maintain the information's
 * confidentiality and use it only for the purposes explicitly permitted under
 * this agreement.
 *
 * Any exceptions to the proprietary rights and ownership as stated herein must
 * be explicitly acknowledged and agreed upon in writing by Oxit LLC.
 * Failure to comply with these terms may result in immediate termination of any
 * agreements and potential legal consequences.
 *
 * By accessing this sample, you acknowledge and agree to these terms:
 *
 * 1. Limited Use: You may use this Code and Hardware solely to evaluate the
 *    hardware specified by Oxit, LLC in a non-production environment.
 *    Any other use is strictly prohibited.
 *
 * 2. No Rights Granted: This Code and Hardware does not convey any rights,
 *    licenses, or permissions beyond limited evaluation use. Oxit, LLC
 *    retains all intellectual property rights in the Code and Hardware.
 *
 * 3. No Commercial Use: You do not have any rights to use this Code and
 *    Hardware for commercial purposes, incorporate it into any product or
 *    service, or otherwise exploit it commercially.
 *
 * 4. No Distribution: You may not distribute, share, sublicense, or transfer
 *    this Code and Hardware to any third parties without express written
 *    consent from Oxit, LLC.
 *
 * 5. Confidentiality: You agree to keep this Code and Hardware confidential
 *    and not disclose it to unauthorized parties.
 *
 * 6. No Warranty: This Code and Hardware is provided "AS IS" without any
 *    warranties, express or implied.
 *
 * 7. Termination: Your right to use this Code and Hardware terminates
 *    automatically if you breach any of these terms or upon request from
 *    Oxit, LLC.
 *
 * If you do not agree to these terms, you must immediately cease any use of
 * this Code and Hardware and return all copies to Oxit, LLC.
 */
#ifndef OXIT_SUBMOD_LINKEDLIST_INC_LINKEDLIST_H_
#define OXIT_SUBMOD_LINKEDLIST_INC_LINKEDLIST_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct node
{
    void *data;
    struct node *next;
    struct node *prev;
}node_t;

typedef struct
{
 node_t *head;
 node_t *tail;
}linked_list_t;

void linked_list_init(linked_list_t * list);
void linked_list_drop(linked_list_t * list);
void linked_list_append_node(linked_list_t * list, node_t *node);
void linked_list_remove_node(linked_list_t * list, node_t *node);
void linked_list_insert_node(linked_list_t * list, node_t *node, uint32_t idx);

void linked_list_create_and_append_node(linked_list_t * list, void *data);
void linked_list_delete_node(linked_list_t * list, node_t *node);

#endif /* OXIT_SUBMOD_LINKEDLIST_INC_LINKEDLIST_H_ */
