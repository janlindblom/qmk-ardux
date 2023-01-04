// Copyright (c) 2021 Mike "KemoNine" Crosson
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifndef KEYLOG_LEN
#    define KEYLOG_LEN 6
#endif
#ifndef KEYLOG_TIMEOUT
#    define KEYLOG_TIMEOUT 1000
#endif

bool process_record_user_oled(uint16_t keycode, keyrecord_t *record);

extern char keylog_str[KEYLOG_LEN];
