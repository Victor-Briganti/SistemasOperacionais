/*
 * Processo principal que irá manejar o produtor e consumidor
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#pragma once

#define QUEUE_NAME "utils.hpp"
#define QUEUE_ID 'A'

#define BUFFER_SIZE 512

struct MsgBuffer {
  long mtype;
  char buffer[BUFFER_SIZE];
};
